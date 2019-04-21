#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"

//--------------------------------------------------------------------------

class RenderContext;

//--------------------------------------------------------------------------

class RTSCamera 
	: public Camera
{
public:
	RTSCamera();
	~RTSCamera();

public:
	// Called each frame to update the underlying camera with the RTSCamera's options; 
	void Update( float deltaSeconds ); 

	void SetFocalPoint( Vec3 const &pos ); 
	void SetZoom( float zoom ); 
	void SetAngle( float angle ); // really is setting an angle offset
	void SetTilt( float tilt ); // really is setting an angle offset

	void BindCamera( RenderContext* context );

public:
	Vec3 m_focusPoint = Vec3( 1.0f, 1.0f, 0.0f ); 
	float m_distance = 10.0f; 
	float m_zoom	 = 0.0f;

	// configuration - mess with these numbers to get a view you like; 
	float m_minDistance     = 2.0f;
	float m_maxDistance     = 16.0f;

	float m_defaultAngle    = -115.0f; 
	float m_defaultTilt     = 60.0f;
	float m_defaultDist		= 10.0f;
	float m_beginTiltDist	= 4.0f;

	// used for moving from the default
	float m_angleOffset     = 0.0f; 
	float m_tiltOffset		= 0.0f;

private:
	void UpdateMatrix();

}; 