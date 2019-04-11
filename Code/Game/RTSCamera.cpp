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
//	g_theInputSystem
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
	m_distance = Clamp( zoom, m_minDistance, m_maxDistance );
}

//--------------------------------------------------------------------------
/**
* SetAngle
*/
void RTSCamera::SetAngle( float angle )
{
	m_angleOffset = Clamp( angle, -90.0f, 90.0f );
}

//--------------------------------------------------------------------------
/**
* SetTilt
*/
void RTSCamera::SetTilt( float tilt )
{
	m_tiltOffset = Clamp( tilt, 0.0f, 90.0f );
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
