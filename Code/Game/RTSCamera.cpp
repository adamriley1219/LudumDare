#include "Game/RTSCamera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"

//--------------------------------------------------------------------------
/**
* RTSCamera
*/
RTSCamera::RTSCamera()
{
	m_camera.SetPerspectiveProjection( 90.f, WORLD_WIDTH / WORLD_HEIGHT, 0.000000001f );
}

//--------------------------------------------------------------------------
/**
* ~RTSCamera
*/
RTSCamera::~RTSCamera()
{

}

//--------------------------------------------------------------------------
/**
* Update
*/
void RTSCamera::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	UpdateMatrix();
}

//--------------------------------------------------------------------------
/**
* SetFocalPoint
*/
void RTSCamera::SetFocalPoint( Vec3 const &pos )
{
	m_focalPoint = pos;
}

//--------------------------------------------------------------------------
/**
* SetZoom
*/
void RTSCamera::SetZoom( float zoom )
{
	m_zoom += zoom;
	m_zoom = Clamp( m_zoom, m_minDistance - m_defaultDist, m_maxDistance - m_defaultDist );
	m_distance = m_zoom + m_defaultDist;
	if( m_distance < m_beginTiltDist )
	{
		SetTilt( ( m_beginTiltDist / m_distance  ) * -20.0f );
	}
	else
	{
		SetTilt( 0.0f );
	}
}

//--------------------------------------------------------------------------
/**
* SetAngle
*/
void RTSCamera::SetAngle( float angle )
{
	m_angleOffset = Clamp( angle, -180.0f, 180.0f );
}

//--------------------------------------------------------------------------
/**
* SetTilt
*/
void RTSCamera::SetTilt( float tilt )
{
	m_tiltOffset = Clamp( tilt, -90.0f, 90.0f );
}

//--------------------------------------------------------------------------
/**
* BindCamera
*/
void RTSCamera::BindCamera( RenderContext* context )
{
	m_camera.SetColorTargetView( context->GetColorTargetView() );
	m_camera.SetDepthTargetView( context->GetDepthTargetView() );
	context->BeginCamera( &m_camera );
}

//--------------------------------------------------------------------------
/**
* UpdateMatrix
*/
void RTSCamera::UpdateMatrix()
{
	float zDist = SinDegrees( m_tiltOffset + m_defaultTilt ) * m_distance;
	float xyDist = CosDegrees( m_tiltOffset + m_defaultTilt ) * m_distance;
	Vec3 camOffset = Vec3( CosDegrees( m_angleOffset + m_defaultAngle ), SinDegrees( m_angleOffset + m_defaultAngle ), 0.0f ) * xyDist + Vec3( 0.0f, 0.0f, -zDist );
	m_camera.SetModelMatrix( Matrix44::LookAt( camOffset + m_focalPoint, m_focalPoint, Vec3( 0.0f, 0.0f, -1.0f ) ) );
}
