#include "Game/FollowCamera2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------
/**
* FollowCamera2D
*/
FollowCamera2D::FollowCamera2D()
{
	SetOrthographicProjection( Vec2( -MAP_SCREEN_HALF_WIDTH * 0.5f, -MAP_SCREEN_HALF_HEIGHT * 0.5f ) * m_zoom, Vec2( MAP_SCREEN_HALF_WIDTH * 0.5f, MAP_SCREEN_HALF_HEIGHT * 0.5f ) * m_zoom );
	SetModelMatrix( Vec3( m_focusPoint.x, m_focusPoint.y, 0.0f ) );
}

//--------------------------------------------------------------------------
/**
* ~FollowCamera2D
*/
FollowCamera2D::~FollowCamera2D()
{

}

//--------------------------------------------------------------------------
/**
* Update
*/
void FollowCamera2D::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	SetModelMatrix( Vec3( m_focusPoint, 0.0f ) );
}

//--------------------------------------------------------------------------
/**
* SetFocalPoint
*/
void FollowCamera2D::SetFocalPoint( Vec2 const &pos )
{
	m_focusPoint = pos;
}

//--------------------------------------------------------------------------
/**
* SetZoom
*/
void FollowCamera2D::SetZoom( float zoom )
{
	m_zoom += zoom * -0.2f;
	m_zoom = Clamp( m_zoom, 0.2f, 4.0f );

	SetOrthographicProjection( Vec2( -MAP_SCREEN_HALF_WIDTH * 0.5f, -MAP_SCREEN_HALF_HEIGHT * 0.5f ) * m_zoom, Vec2( MAP_SCREEN_HALF_WIDTH * 0.5f, MAP_SCREEN_HALF_HEIGHT * 0.5f ) * m_zoom );
}


//--------------------------------------------------------------------------
/**
* BindCamera
*/
void FollowCamera2D::BindCamera( RenderContext* context )
{
	SetColorTargetView( context->GetColorTargetView() );
	SetDepthTargetView( context->GetDepthTargetView() );
	context->BeginCamera( this );
}


