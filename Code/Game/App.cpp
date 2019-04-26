#include "Game/App.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/RNG.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameController.hpp"

//--------------------------------------------------------------------------
// Global Singletons
//--------------------------------------------------------------------------
RenderContext* g_theRenderer = nullptr;		// Created and owned by the App
InputSystem* g_theInputSystem = nullptr;
AudioSystem* g_theAudioSystem = nullptr;
App* g_theApp = nullptr;					// Created and owned by Main_Windows.cpp
RNG* g_theRNG = nullptr;
PhysicsSystem* g_thePhysicsSystem = nullptr;
Game* g_theGame = nullptr;
WindowContext* g_theWindowContext = nullptr;
GameController* g_theGameController = nullptr;


//--------------------------------------------------------------------------
/**
* Startup
*/
void App::Startup()
{
	g_theRNG = new RNG();
	g_theEventSystem = new EventSystem();
	g_theConsole = new DevConsole( "SquirrelFixedFont" );
	g_theRenderer = new RenderContext( g_theWindowContext );
	g_theDebugRenderSystem = new DebugRenderSystem( g_theRenderer, 50.0f, 100.0f, "SquirrelFixedFont" );
	g_theInputSystem = new InputSystem();
	g_theAudioSystem = new AudioSystem();
	g_thePhysicsSystem = new PhysicsSystem();
	g_theGame = new Game();
	g_theGameController = new GameController();

	ClockSystemStartup();
	m_gameClock = new Clock( &Clock::Master );
	m_UIClock = new Clock( &Clock::Master );

	g_theEventSystem->Startup();
	g_theRenderer->Startup();
	g_theDebugRenderSystem->Startup();
	g_theConsole->Startup();
	g_thePhysicsSystem->Startup();
	g_theGame->Startup();

	RegisterEvents();
}

//--------------------------------------------------------------------------
/**
* Shutdown
*/
void App::Shutdown()
{
	g_theGame->Shutdown();
	g_thePhysicsSystem->Shutdown();
	g_theConsole->Shutdown();
	g_theDebugRenderSystem->Startup();
	g_theRenderer->Shutdown();
	g_theEventSystem->Shutdown();

	SAFE_DELETE( m_gameClock );
	SAFE_DELETE( m_UIClock );

	delete g_theGameController;
	g_theGameController = nullptr;
	delete g_theGame;
	g_theGame = nullptr;
	delete g_theAudioSystem;
	g_theAudioSystem = nullptr;
	delete g_theInputSystem;
	g_theInputSystem = nullptr;
	delete g_theConsole;
	g_theConsole = nullptr;
	delete g_theDebugRenderSystem;
	g_theDebugRenderSystem = nullptr;
	delete g_theRenderer;
	g_theRenderer = nullptr;
	delete g_theRNG;
	g_theRNG = nullptr;
}

//--------------------------------------------------------------------------
/**
* RunFrame
*/
void App::RunFrame()
{
	if( m_isSlowMo )
	{
		m_gameClock->Dilate( 0.1f );
	}
	else if( m_isFastMo )
	{
		m_gameClock->Dilate( 4.0f );
	}
	else
	{
		m_gameClock->Dilate( 1.0f );
	}


	BeginFrame();
	Update( (float) m_gameClock->GetFrameTime() );
	Render();
	EndFrame();
}

//--------------------------------------------------------------------------
/**
* IsPaused
*/
bool App::IsPaused() const
{
	return m_gameClock->IsPaused(); 
}

//--------------------------------------------------------------------------
/**
* Unpause
*/
void App::Unpause()
{
	m_gameClock->Resume();
}

//--------------------------------------------------------------------------
/**
* Pause
*/
void App::Pause()
{
	m_UIClock->Pause();
}

//--------------------------------------------------------------------------
/**
* HandleKeyPressed
*/
bool App::HandleKeyPressed( unsigned char keyCode )
{
	if( g_theConsole->HandleKeyPress( keyCode ) )
	{
		return true;
	}
	switch( keyCode )
	{
	case 192: // '~' press
		
		break;
	case 'P':
		if( m_gameClock->IsPaused() )
			m_gameClock->Resume();
		else
			m_gameClock->Pause();
		return true;
		break;
	case 'T':
		m_isSlowMo = true;
		return true;
		break;
	case 'Y':
		m_isFastMo = true;
		return true;
		break;
	case 'w': // F8 press
		delete g_theGame;
		g_theGame = new Game();
		return true;
		break;
	default:
		return g_theGame->HandleKeyPressed( keyCode );
		break;
	}
	return false;
}

//--------------------------------------------------------------------------
/**
* HandleCharPressed
*/
bool App::HandleCharPressed( unsigned char keyCode )
{
	if( g_theConsole->HandleCharPress( keyCode ) )
	{
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
/**
* HandleKeyReleased
*/
bool App::HandleKeyReleased( unsigned char keyCode )
{
	if( g_theConsole->HandleKeyReleased( keyCode ) )
	{
		return true;
	}
	switch( keyCode )
	{
	case 'T':
		m_isSlowMo = false;
		break;
	case 'Y':
		m_isFastMo = false;
		break;
	case 'H':
		g_theEventSystem->FireEvent( "help" );
		break;
	default:
		return g_theGame->HandleKeyReleased( keyCode );
		break;
	}
	return true;
}

//--------------------------------------------------------------------------
/**
* HandleQuitRequested
*/
bool App::HandleQuitRequested()
{
	if( !g_theGame->HandleQuitRequest() )
	{
		m_isQuitting = true;
	}
	return true;
}

//--------------------------------------------------------------------------
/**
* QuitEvent
*/
bool App::QuitEvent( EventArgs& args )
{
	UNUSED( args );
	g_theApp->HandleQuitRequested();
	return true;
}

//--------------------------------------------------------------------------
/**
* UnpauseEvent
*/
bool App::UnpauseEvent( EventArgs& args )
{
	UNUSED( args );
	g_theApp->Unpause();
	Event event("unselect_all");
	g_theGame->m_pauseMenuCanvis.ProcessInput( event );
	return true;
}

//--------------------------------------------------------------------------
/**
* PauseEvent
*/
bool App::PauseEvent( EventArgs& args )
{
	UNUSED( args );
	g_theApp->Pause();
	return true;
}

//--------------------------------------------------------------------------
/**
* BeginFrame
*/
void App::BeginFrame()
{
	g_theEventSystem->		BeginFrame();
	g_theRenderer->			BeginFrame();
	g_theConsole->			BeginFrame();
	g_theInputSystem->		BeginFrame();
	g_theAudioSystem->		BeginFrame();
	g_thePhysicsSystem->	BeginFrame();
	g_theDebugRenderSystem->BeginFrame();
	ClockSystemBeginFrame();
	++m_frame;
}


//--------------------------------------------------------------------------
/**
* Update
*/
void App::Update( float deltaSeconds )
{
	g_theConsole->			Update();
	g_theGameController->	Update( deltaSeconds );
	g_theGame->				UpdateGame( deltaSeconds );
	g_theDebugRenderSystem->Update();
}

//--------------------------------------------------------------------------
/**
* Render
*/
void App::Render() const
{
	g_theRenderer->ClearScreen( Rgba::DARK_GRAY );
	g_theGame->GameRender();

	if( g_theConsole->IsOpen() )
	{
		g_theConsole->Render( g_theRenderer, g_theGame->m_DevColsoleCamera, m_consoleTextHeight );
	}
	else
	{
		g_theDebugRenderSystem->RenderToScreen();
	}
}

//--------------------------------------------------------------------------
/**
* EndFrame
*/
void App::EndFrame()
{
	g_theRenderer->EndCamera();
	g_theDebugRenderSystem->EndFrame();
	g_thePhysicsSystem->	EndFrame();
	g_theConsole->			EndFrame();
	g_theAudioSystem->		EndFrame();
	g_theInputSystem->		EndFrame();
	g_theRenderer->			EndFrame();
	g_theEventSystem->		EndFrame();
}

//--------------------------------------------------------------------------
/**
* RegisterEvents
*/
void App::RegisterEvents()
{
	g_theEventSystem->SubscribeEventCallbackFunction( "quit", QuitEvent );
	g_theEventSystem->SubscribeEventCallbackFunction( "pause", PauseEvent );
	g_theEventSystem->SubscribeEventCallbackFunction( "unpause", UnpauseEvent );
}

