#include "Game/GameController.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Game/Game.hpp"
#include "Game/FollowCamera2D.hpp"
#include "Engine/Core/DevConsole.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

//--------------------------------------------------------------------------
/**
* GameController
*/
GameController::GameController()
{
	
}

//--------------------------------------------------------------------------
/**
* ~GameController
*/
GameController::~GameController()
{

}

//--------------------------------------------------------------------------
/**
* Update
*/
void GameController::Update( float deltaSec )
{
	Shift_Button.UpdateStatus( g_theInputSystem->IsShiftPressed() );

	if( Shift_Button.WasJustPressed() )
	{
		g_theWindowContext->SetMouseMode( MOUSE_MODE_RELATIVE );
		g_theWindowContext->HideMouse();
		g_theWindowContext->UnlockMouse();
	}
	if( Shift_Button.WasJustReleased() )
	{
		g_theWindowContext->SetMouseMode( MOUSE_MODE_ABSOLUTE );
		g_theWindowContext->SetClientMousePosition( m_mousePos );
		g_theWindowContext->ShowMouse();
		g_theWindowContext->LockMouse();
	}

	if( IsRotating() )
	{
		// Get mouse displacement
		Vec2 disp = Vec2( g_theWindowContext->GetClientMouseRelativeMovement() );
		if( disp.x > 0 )
		{
			m_frameRotation += disp.GetLength() * m_rotationSpeed * deltaSec;
		}
		if( disp.x < 0 )
		{
			m_frameRotation += disp.GetLength() * -m_rotationSpeed * deltaSec;
		}
	}
	else
	{
		m_frameRotation = 0.0f;
	}

	if( !Shift_Button.IsPressed() )
	{
		m_mousePos = g_theWindowContext->GetClientMousePosition();
	}

	m_frameZoom = m_wheelOffset * m_zoomSpeed * deltaSec;
	m_wheelOffset = 0.0f;
}


//--------------------------------------------------------------------------
/**
* GetFramePan
*/
Vec2 GameController::GetFramePan() const
{
	bool consoleOpen = g_theConsole->IsOpen();
	Vec2 ret = Vec2::ZERO;
	ret += Vec2( g_theInputSystem->KeyIsDown( KEY_A ) && !consoleOpen ? -1.0f : 0.0f, 0.0f );
	ret += Vec2( g_theInputSystem->KeyIsDown( KEY_D ) && !consoleOpen ? 1.0f : 0.0f, 0.0f );
	ret += Vec2( 0.0f, g_theInputSystem->KeyIsDown( KEY_S ) && !consoleOpen ? -1.0f : 0.0f );
	ret += Vec2( 0.0f, g_theInputSystem->KeyIsDown( KEY_W ) && !consoleOpen ? 1.0f : 0.0f );

	IntVec2 pos = g_theWindowContext->GetClientMousePosition();
	AABB2 screen = g_theWindowContext->GetClientScreen();

	ret += Vec2( pos.x >= screen.GetTopRight().x - 1 && !consoleOpen ? 1.0f : 0.0f, 0.0f );
	ret += Vec2( pos.x <= screen.GetBottomLeft().x && !consoleOpen ? -1.0f : 0.0f, 0.0f );
	ret += Vec2( 0.0f, pos.y >= screen.GetBottomLeft().y - 1 && !consoleOpen ? -1.0f : 0.0f );
	ret += Vec2( 0.0f, pos.y <= screen.GetTopRight().y && !consoleOpen ? 1.0f : 0.0f );

	ret.Normalize();
	ret *= m_keyboardPanSpeed;
	return ret;
}

//--------------------------------------------------------------------------
/**
* GetFrameZoom
*/
float GameController::GetFrameZoom() const
{
	return m_frameZoom;
}

//--------------------------------------------------------------------------
/**
* GetFrameRotation
*/
float GameController::GetFrameRotation() const
{
	return m_frameRotation;
}

//--------------------------------------------------------------------------
/**
* IsRotating
*/
bool GameController::IsRotating() const
{
	return g_theInputSystem->IsShiftPressed();
}

//--------------------------------------------------------------------------
/**
* GetScreenMousePos
*/
Vec2 GameController::GetScreenMousePos()
{
	IntVec2 rawMouseMovement = g_theWindowContext->GetClientMousePosition();

	Vec3 worldCamPos = g_theGame->m_UICamera.GetClientToWorld( rawMouseMovement );
	return Vec2( worldCamPos.x, worldCamPos.y );
}

//--------------------------------------------------------------------------
/**
* GetWorldMousePos
*/
Vec3 GameController::GetWorldMousePos()
{
	IntVec2 rawMouseMovement = g_theWindowContext->GetClientMousePosition();

	return g_theGame->m_curCamera->GetClientToWorld( rawMouseMovement );
}

//--------------------------------------------------------------------------
/**
* LMousePress
*/
void GameController::LMousePress()
{
	g_theGame->LMouseDown();
}

//--------------------------------------------------------------------------
/**
* RMousePress
*/
void GameController::RMousePress()
{
	g_theGame->RMouseDown();
}

//--------------------------------------------------------------------------
/**
* LMouseRelease
*/
void GameController::LMouseRelease()
{
	g_theGame->LMouseUp();
}

//--------------------------------------------------------------------------
/**
* RMouseRelease
*/
void GameController::RMouseRelease()
{
	g_theGame->RMouseUp();
}

//--------------------------------------------------------------------------
/**
* WheelMovement
*/
void GameController::WheelMovement( float offset )
{
	m_wheelOffset = offset;
}
