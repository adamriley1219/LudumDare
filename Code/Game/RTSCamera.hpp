#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Renderer/Camera.hpp"

//--------------------------------------------------------------------------


//--------------------------------------------------------------------------

class RTSCamera
{
public:
	// Called each frame to update the underlying camera with the RTSCamera's options; 
	void Update( float deltaSeconds ); 

	void SetFocalPoint( Vec3 const &pos ); 
	void SetZoom( float zoom ); 
	void SetAngle( float angle ); // really is setting an angle offset


public:
	Camera m_camera;
	Vec3 m_focalPoint; 
	float m_distance; 

	// configuration - mess with these numbers to get a view you like; 
	float m_minDistance     = 2.0f;
	float m_maxDistance     = 16.0f;

	float m_defaultAngle    = -45.0f; 
	float m_defaultTilt     = 60.0f; 

	// used for moving from the default
	float m_angleOffset     = 0.0f; 

}; 