#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Game/Game.hpp"

class Clock;

//--------------------------------------------------------------------------
class App
{
public:
	App() {};
	~App() {};
	void Startup();
	void Shutdown();
	void RunFrame();

	bool IsQuitting() const { return m_isQuitting; }
	bool IsPaused() const;
	void Unpause();
	void Pause();

	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleCharPressed( unsigned char keyCode );
	bool HandleKeyReleased( unsigned char keyCode );
	bool HandleQuitRequested();	

	static bool QuitEvent( EventArgs& args );
	static bool UnpauseEvent( EventArgs& args );
	static bool PauseEvent( EventArgs& args );

	float GetGlobleTime() const { return m_time; }
	int GetFrameCount() const { return m_frame; }
	float GetConsoleTextHeight() const { return m_consoleTextHeight; }

public:
	Clock* m_gameClock = nullptr;
	Clock* m_UIClock = nullptr;

private:
	void BeginFrame();
	void Update( float deltaSeconds );
	void Render() const;
	void EndFrame();
	void RegisterEvents();

private:
	bool m_isQuitting			= false;
	bool m_isSlowMo				= false;
	bool m_isFastMo				= false;
	float m_time				= 0.0f;
	int m_frame					= 0;
	float m_consoleTextHeight	= 2.0f;

};
