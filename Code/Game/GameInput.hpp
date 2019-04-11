#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"

class GameInput
{
public:
	// Called each frame - translates raw input (keyboard/mouse/etc) to 
	// input used for the game.
	// 
	// Hotkey bindings will also live here; 
	void Update( float deltaSec ); 

	// A08
	Vec2 GetFramePan() const; 
	float GetFrameZoom() const; 

	float GetFrameRotation() const;     
	bool IsRotating() const;            // are we in a rotation mode (Control is Held)

										// A09
										// eGameAction DequeueNextAction(); 

public:
	// Configuration - Keyboard Input
	float m_keyboardPanSpeed         = 16.0f; 

	// Configuration - Mouse Input
	float m_edgePanSpeed             = 16.0f; // how quickly we pan when using edge-scroll
	float m_edgePanDistance          = 32.0f; // units away from the edge in 2D before scrolling starts

	float m_rotationSpeed            = g_PI; 
	float m_zoomSpeed                = 24.0f; 
}; 