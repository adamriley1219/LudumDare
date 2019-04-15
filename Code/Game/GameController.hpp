#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/IntVec2.hpp"

class GameController
{
public:
	GameController();
	~GameController();

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
	Vec2 GetScreenMousePos();
	Vec3 GetWorldMousePos();

	void LMousePress();
	void RMousePress();
	void LMouseRelease();
	void RMouseRelease();
	void WheelMovement( float offset );
public:
	// Configuration - Keyboard Input
	float m_keyboardPanSpeed         = 32.0f; 

	// Configuration - Mouse Input
	float m_edgePanSpeed             = 16.0f; // how quickly we pan when using edge-scroll
	float m_edgePanDistance          = 32.0f; // units away from the edge in 2D before scrolling starts

	float m_rotationSpeed            = g_PI; 
	float m_zoomSpeed                = 24.0f; 

	float m_frameZoom				 = 0.0f;
	float m_frameRotation			 = 0.0f;

	float m_wheelOffset				 = 0.0f;

	IntVec2 m_mousePos;

	KeyButtonState A_Key;
	KeyButtonState S_Key;
	KeyButtonState D_Key;
	KeyButtonState W_Key;

	KeyButtonState Shift_Button;
}; 