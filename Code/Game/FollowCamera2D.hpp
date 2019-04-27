#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"

//--------------------------------------------------------------------------

class RenderContext;

//--------------------------------------------------------------------------

class FollowCamera2D 
	: public Camera
{
public:
	FollowCamera2D();
	~FollowCamera2D();

public:
	// Called each frame to update the underlying camera with the FollowCamera2D's options; 
	void Update( float deltaSeconds ); 

	void SetFocalPoint( Vec2 const &pos ); 
	void SetZoom( float zoom ); 

	void BindCamera( RenderContext* context );

public:
	Vec2 m_focusPoint = Vec2( 1.0f, 1.0f ); 
	float m_zoom	 = 1.0f;




}; 