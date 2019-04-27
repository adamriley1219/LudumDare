#include "Game/Shapes/Cursor.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/Shapes/Shape.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/WindowContext.hpp"

//--------------------------------------------------------------------------
/**
* Cursor
*/
Cursor::Cursor()
{
	
}

//--------------------------------------------------------------------------
/**
* Update
*/
void Cursor::Update( float deltaTime )
{
	UNUSED(deltaTime);
	IntVec2 rawMouseMovement = g_theWindowContext->GetClientMousePosition();

	Vec3 worldCamPos = g_theGame->GetCurrentMap()->m_camera->GetClientToWorld( rawMouseMovement );
	Vec2 camPos = Vec2( g_theGame->GetCurrentMap()->m_camera->m_focusPoint );
	m_trasform.m_position.x = worldCamPos.x;
	m_trasform.m_position.y = worldCamPos.y;
	m_trasform.m_position = Vec2::ClampBetween( m_trasform.m_position, Vec2( -SCREEN_WIDTH * 0.5f, -SCREEN_HEIGHT *0.5f ) * g_theGame->GetCurrentMap()->m_camScale + camPos, Vec2( SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT *0.5f ) * g_theGame->GetCurrentMap()->m_camScale + camPos );
	if( g_theGame->m_selectedShape )
	{
		g_theGame->m_selectedShape->SetPosition( m_trasform.m_position );
	}
}

//--------------------------------------------------------------------------
/**
* Render
*/
void Cursor::Render() const
{
	g_theRenderer->BindTextureView( 0, nullptr );

	std::vector<Vertex_PCU> verts;
	AddVertsForRing2D( verts, m_trasform.m_position, m_disc.m_radius, .1f, Rgba::WHITE );
	AddVertsForRing2D( verts, m_trasform.m_position, m_disc.m_radius * .70f, .05f, Rgba::WHITE );
	AddVertsForLine2D( verts, Vec2( 0.0f, 0.5f ) + m_trasform.m_position,  Vec2( 0.0f, -0.5f ) + m_trasform.m_position, 0.05f, Rgba::YELLOW );
	AddVertsForLine2D( verts, Vec2( 0.5f, 0.0f ) + m_trasform.m_position,  Vec2( -0.5f, 0.0f ) + m_trasform.m_position, 0.05f, Rgba::YELLOW );
	g_theRenderer->DrawVertexArray( (int) verts.size(), &verts[0] );

}
