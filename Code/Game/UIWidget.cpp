#include "Game/UIWidget.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/MeshCPU.hpp"
#include "Engine/Renderer/MeshGPU.hpp"
#include "Engine/Renderer/Textures/Texture2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/BitmapFont.hpp"


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
	m_radioGroup->m_widgets.push_back( this );
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
	for( int idx = (int) m_children.size() - 1; idx >= 0; --idx )
	{
		m_children[idx]->ProcessInput( evt );
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
	AddVertsForAABB2D( verts, m_worldBounds, m_boarderColor );
	AddVertsForAABB2D( verts, m_boarderBounds, m_fillColor );
	g_theRenderer->BindTextureView( TEXTURE_SLOT_ALBEDO, nullptr );
	g_theRenderer->DrawVertexArray(	verts );

	RenderChildren();
}

//--------------------------------------------------------------------------
/**
* UpdateBounds
*/
void UICanvas::UpdateBounds( const AABB2& container )
{
	float scaledBounds = m_boarderScaleWithParent ? m_boarderThickness * ( m_virtualSize.x + m_virtualSize.y ) * 0.5f + m_boarderThickness * ( m_virtualSize.z + m_virtualSize.w ) * 0.5f : m_boarderThickness;
	UIWidget::UpdateBounds( container );
	m_boarderBounds = m_worldBounds;
	m_boarderBounds.CarveBoxOffBottom( 0.f, scaledBounds );
	m_boarderBounds.CarveBoxOffTop( 0.f, scaledBounds );
	m_boarderBounds.CarveBoxOffLeft( 0.f, scaledBounds );
	m_boarderBounds.CarveBoxOffRight( 0.f, scaledBounds );
}


//--------------------------------------------------------------------------
/**
* UILabel
*/
UILabel::UILabel()
{

}

//--------------------------------------------------------------------------
/**
* ~UILabel
*/
UILabel::~UILabel()
{

}

//--------------------------------------------------------------------------
/**
* Render
*/
void UILabel::Render() const
{
	std::vector<Vertex_PCU> verts;
	BitmapFont* font = g_theRenderer->CreateOrGetBitmapFromFile( m_font.c_str() );
	if( font )
	{
		font->AddVertsFor2DTextAlignedInBox( verts, m_worldBounds.GetHeight(), m_text.c_str(), m_worldBounds, m_pivot, BITMAP_MODE_SHRINK_TO_FIT, .75f, m_color );
		g_theRenderer->BindTextureViewWithSampler( TEXTURE_SLOT_ALBEDO, font->GetTextureView() );
		g_theRenderer->DrawVertexArray(	verts );
	}

	RenderChildren();
}


//--------------------------------------------------------------------------
/**
* UIButton
*/
UIButton::UIButton()
{

}

//--------------------------------------------------------------------------
/**
* ~UIButton
*/
UIButton::~UIButton()
{

}

//--------------------------------------------------------------------------
/**
* Render
*/
void UIButton::Render() const
{
	if( m_useText )
	{
		BitmapFont* font = g_theRenderer->CreateOrGetBitmapFromFile( m_font.c_str() );
		if( font )
		{
			std::vector<Vertex_PCU> verts;
			switch( m_state )
			{
			case BUTTON_STATE_NUTRAL:
				font->AddVertsFor2DTextAlignedInBox( verts, m_worldBounds.GetHeight(), m_text.c_str(), m_worldBounds, m_pivot, BITMAP_MODE_UNCHANGED, m_worldBounds.GetWidth() / m_text.length() / m_worldBounds.GetHeight(), m_nutralColor );
				break;
			case BUTTON_STATE_SELECTED:
				font->AddVertsFor2DTextAlignedInBox( verts, m_worldBounds.GetHeight(), m_text.c_str(), m_worldBounds, m_pivot, BITMAP_MODE_UNCHANGED, m_worldBounds.GetWidth() / m_text.length() / m_worldBounds.GetHeight(), m_selectedColor );
				break;
			case BUTTON_STATE_HOVERED:
				font->AddVertsFor2DTextAlignedInBox( verts, m_worldBounds.GetHeight(), m_text.c_str(), m_worldBounds, m_pivot, BITMAP_MODE_UNCHANGED, m_worldBounds.GetWidth() / m_text.length() / m_worldBounds.GetHeight(), m_hoveredColor );
				break;
			default:
				break;
			}
			g_theRenderer->BindTextureViewWithSampler( TEXTURE_SLOT_ALBEDO, font->GetTextureView() );
			g_theRenderer->DrawVertexArray(	verts );
		}

		RenderChildren();
	}
	else
	{
		std::vector<Vertex_PCU> verts;
		switch( m_state )
		{
		case BUTTON_STATE_NUTRAL:
			AddVertsForAABB2D( verts, m_worldBounds, m_nutralBoarderColor );
			AddVertsForAABB2D( verts, m_boarderBounds, m_nutralColor );
			break;
		case BUTTON_STATE_SELECTED:
			AddVertsForAABB2D( verts, m_worldBounds, m_selectedBoarderColor );
			AddVertsForAABB2D( verts, m_boarderBounds, m_selectedColor );
			break;
		case BUTTON_STATE_HOVERED:
			AddVertsForAABB2D( verts, m_worldBounds, m_hoveredBoarderColor );
			AddVertsForAABB2D( verts, m_boarderBounds, m_hoveredColor );
			break;
		default:
			break;
		}

		if( m_texturePath == "" )
		{
			g_theRenderer->BindTextureView( TEXTURE_SLOT_ALBEDO, nullptr );
		}
		else
		{
			TextureView* texture = g_theRenderer->CreateOrGetTextureViewFromFile( m_texturePath.c_str() );
			g_theRenderer->BindTextureViewWithSampler( TEXTURE_SLOT_ALBEDO, texture );
		}
		g_theRenderer->DrawVertexArray(	verts );
	}

	RenderChildren();
}

//--------------------------------------------------------------------------
/**
* UpdateBounds
*/
void UIButton::UpdateBounds( const AABB2& container )
{
	float scaledBounds = m_boarderScaleWithParent ? m_boarderThickness * ( m_virtualSize.x + m_virtualSize.y ) * 0.5f : m_boarderThickness;
	UIWidget::UpdateBounds( container );
	m_boarderBounds = m_worldBounds;
	m_boarderBounds.CarveBoxOffBottom( 0.f, scaledBounds );
	m_boarderBounds.CarveBoxOffTop( 0.f, scaledBounds );
	m_boarderBounds.CarveBoxOffLeft( 0.f, scaledBounds );
	m_boarderBounds.CarveBoxOffRight( 0.f, scaledBounds );
}

//--------------------------------------------------------------------------
/**
* ProcessInput
*/
void UIButton::ProcessInput( Event& evt )
{
	ProcessChildrenInput( evt );

	if( evt.m_name == "unselect_all" )
	{
		m_state = BUTTON_STATE_NUTRAL;
	}
	if( evt.m_name == "hover" && m_state != BUTTON_STATE_SELECTED )
	{
		float x = evt.m_args.GetValue( "x", -1.0f );
		float y = evt.m_args.GetValue( "y", -1.0f );
		if( !evt.m_consumed && IsPointOnOBB2( Vec2( x, y ), OBB2( m_worldBounds ) ) )
		{
			m_state = BUTTON_STATE_HOVERED;
			evt.m_consumed = true;
		}
		else
		{
			m_state = BUTTON_STATE_NUTRAL;
		}
		return;
	}
	if( evt.m_name == "click" )
	{
		float x = evt.m_args.GetValue( "x", -1.0f );
		float y = evt.m_args.GetValue( "y", -1.0f );
		if( !evt.m_consumed && IsPointOnOBB2( Vec2( x, y ), OBB2( m_worldBounds ) ) )
		{
			evt.m_consumed = true;
			Click();
			return;
		}
	}
}

//--------------------------------------------------------------------------
/**
* Click
*/
void UIButton::Click()
{
	if( m_radioGroup )
	{
		for( UIWidget* widget : m_radioGroup->m_widgets )
		{
			Event event( "unselect_all" );
			widget->ProcessInput( event );
		}
	}
	
	m_state = BUTTON_STATE_SELECTED;

	Event evt( m_eventOnClick ); 
	g_theEventSystem->FireEvent( evt.m_name, evt.m_args );  
}
