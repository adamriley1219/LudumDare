#include "Game/UIWidget.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/MeshCPU.hpp"
#include "Engine/Renderer/MeshGPU.hpp"
#include "Engine/Renderer/Textures/Texture2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"



//--------------------------------------------------------------------------
/**
* Click
*/
void UIButton::Click()
{
	Event evt( m_eventOnClick ); 
	g_theEventSystem->FireEvent( evt.m_name );  
	
}

//--------------------------------------------------------------------------
/**
* UIWidget
*/
UIWidget::UIWidget()
{

}

//--------------------------------------------------------------------------
/**
* ~UIWidget
*/
UIWidget::~UIWidget()
{
	DestroyChildren();
}

//--------------------------------------------------------------------------
/**
* UpdateBounds
*/
void UIWidget::UpdateBounds( const AABB2& container )
{
	float dimX = m_virtualSize.x * container.GetWidth() + m_virtualSize.z;
	float dimY = m_virtualSize.y * container.GetHeight() + m_virtualSize.w;

	float posX = m_virtualPosition.x * container.GetWidth() + m_virtualPosition.z + container.GetBottomLeft().x;
	float posY = m_virtualPosition.y * container.GetHeight() + m_virtualPosition.w + container.GetBottomLeft().y;

	float localMinX = -m_pivot.x * dimX;
	float localMinY = -m_pivot.y * dimY;

	Vec2 worldMin( posX + localMinX, posY + localMinY );
	Vec2 worldMax = worldMin + Vec2( dimX, dimY );

	m_worldBounds = AABB2( worldMin, worldMax );
	m_position = worldMin + Vec2( dimX * -localMinX, dimY * -localMinY );

	UpdateChildrenBounds();
}

//--------------------------------------------------------------------------
/**
* ProcessInput
*/
void UIWidget::ProcessInput( Event& evt )
{
	ProcessChildrenInput( evt );
}


//--------------------------------------------------------------------------
/**
* Render
*/
void UIWidget::Render() const
{
	RenderChildren();
}

//--------------------------------------------------------------------------
/**
* AddChild
*/
UIWidget* UIWidget::AddChild( UIWidget *widget )
{
	widget->m_parent = this;
	m_children.push_back( widget );
	return widget;
}

//--------------------------------------------------------------------------
/**
* RemoveChild
*/
void UIWidget::RemoveChild( UIWidget *widget )
{
	for( int idx = 0; idx < (int) m_children.size(); ++idx )
	{
		if( m_children[idx] == widget )
		{
			delete widget;
			widget = nullptr;
			m_children.erase( m_children.begin() + idx );
			return;
		}
	}
}

//--------------------------------------------------------------------------
/**
* SetRadioGroup
*/
void UIWidget::SetRadioGroup( UIRadioGroup* group )
{
	m_radioGroup = group;
}

//--------------------------------------------------------------------------
/**
* UpdateChildrenBounds
*/
void UIWidget::UpdateChildrenBounds()
{
	for( UIWidget* child : m_children )
	{
		child->UpdateBounds( m_worldBounds );
	}
}

//--------------------------------------------------------------------------
/**
* ProcessChildrenInput
*/
void UIWidget::ProcessChildrenInput( Event &evt )
{
	if( !evt.m_consumed )
	{
		for( int idx = (int) m_children.size() - 1; idx >= 0; --idx )
		{
			m_children[idx]->ProcessInput( evt );
		}
	}
}

//--------------------------------------------------------------------------
/**
* RenderChildren
*/
void UIWidget::RenderChildren() const
{
	for( int idx = (int) m_children.size() - 1; idx >= 0; --idx )
	{
		m_children[idx]->Render();
	}
}

//--------------------------------------------------------------------------
/**
* DestroyChildren
*/
void UIWidget::DestroyChildren()
{
	for( UIWidget* child : m_children )
	{
		delete child;
		child = nullptr;
	}
	m_children.clear();
}

//--------------------------------------------------------------------------
/**
* UICanvas
*/
UICanvas::UICanvas()
	: UIWidget()
{

}

//--------------------------------------------------------------------------
/**
* ~UICanvas
*/
UICanvas::~UICanvas()
{

}

//--------------------------------------------------------------------------
/**
* Render
*/
void UICanvas::Render() const
{
	std::vector<Vertex_PCU> verts;
	AddVertsForAABB2D( verts, m_worldBounds, m_color );
	g_theRenderer->BindTextureView( TEXTURE_SLOT_ALBEDO, nullptr );
	g_theRenderer->DrawVertexArray(	verts );

	RenderChildren();
}
