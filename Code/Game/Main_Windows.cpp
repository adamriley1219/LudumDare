#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include "Engine/Core/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"



//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move these constants to GameCommon.hpp or elsewhere
// 
constexpr float CLIENT_ASPECT = 2.0f; // We are requesting a 1:1 aspect (square) window area


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move each of these items to its proper place, once that place is established
// 
void* g_hWnd = nullptr;							// ...becomes WindowContext::m_windowHandle
HDC g_displayDeviceContext = nullptr;			// ...becomes WindowContext::m_displayContext
HGLRC g_openGLRenderingContext = nullptr;		// ...becomes RenderContext::m_apiRenderingContext
const char* APP_NAME = "RTS";					// ...becomes ???


//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
//
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	switch( wmMessageCode )
	{
		case WM_CREATE: 
		{
			// Window creation happened.
		} break;
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
		case WM_CLOSE:		
		{
			g_theEventSystem->FireEvent( "quit" );
			return 0; // "Consumes" this message (tells Windows "okay, we handled it")
		}

		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_KEYDOWN:
		{
			unsigned char asKey = (unsigned char) wParam;
			g_theApp->HandleKeyPressed( asKey );

			if( asKey == VK_ESCAPE )
			{
				if( !g_theConsole->HandleESCPress() )
				{
					g_theEventSystem->FireEvent( "quit" );
				}
				return 0; // "Consumes" this message (tells Windows "okay, we handled it")
			}
			break;
		}

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{
			unsigned char asKey = (unsigned char) wParam;
			g_theApp->HandleKeyReleased( asKey );
			break;
		}

		case WM_MOUSEMOVE:
			// mouse has moved.
			// wparam - mouse keys
			// lparam - mouse positions

			// Positions are relative to client area.
			// int mouse_x = LOWORD(lparam);
			// int mouse_y = HIWORD(lparam);
			break;
		case WM_MOVE:
		case WM_SIZE: 
		{
			// Called when the window changes size or is moved.
			break;
		} 
		case WM_CHAR: 
		{
			// User has typed a character
			// wparam - char code
			// lparam - additional information (like ALT state)
			unsigned char asKey = (unsigned char) wParam;
			g_theApp->HandleCharPressed( asKey );

			//TODO: Figure out how to access info
// 			if( IsBitFlagSet( (unsigned char)lParam, VK_SHIFT ) )
// 			{
// 				int x = 0;
// 			}
			break;
		} 
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
void CreateD3D11Window( HINSTANCE applicationInstanceHandle, float clientAspect )
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	RegisterClassEx( &windowClassDescription );

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)( desktopRect.right - desktopRect.left );
	float desktopHeight = (float)( desktopRect.bottom - desktopRect.top );
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int) clientMarginX;
	clientRect.right = clientRect.left + (int) clientWidth;
	clientRect.top = (int) clientMarginY;
	clientRect.bottom = clientRect.top + (int) clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, APP_NAME, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	g_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL );

	ShowWindow( (HWND)g_hWnd, SW_SHOW );
	SetForegroundWindow( (HWND)g_hWnd );
	SetFocus( (HWND)g_hWnd );

	g_displayDeviceContext = GetDC( (HWND)g_hWnd );

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );
}


//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
void RunMessagePump()
{
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}




//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
//
void RunFrame( float timeFrameBeganSec )
{
	RunMessagePump();

	g_theApp->RunFrame( timeFrameBeganSec );
}

void SetupXMLInfo()
{
	tinyxml2::XMLDocument gameConfig;
	gameConfig.LoadFile("Data/GameConfig.xml");

	XmlElement* root = gameConfig.RootElement();
	g_gameConfigBlackboard.PopulateFromXmlElementAttributes( *root );
}


void UnregisterWindow()
{
	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, APP_NAME, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	UnregisterClass( windowTitle, GetModuleHandle(NULL) );
}

static HINSTANCE GetCurrentHINSTANCE()
{
	return ::GetModuleHandle(NULL);
}

bool RegisterWindow()
{
	WNDCLASSEX wc;
	memset( &wc, 0, sizeof(wc) );

	// Setup the definition for this window class
	wc.cbSize = sizeof(WNDCLASSEX);

	// This sets that it will redraw for vertical or horizontal changes
	// and it also owns its own device context handle (more effecient if we're
	// drawing to this window a lot - which we will be.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	// The Windows Messaeg Pump - handles messages for this window type.
	wc.lpfnWndProc = WindowsMessageHandlingProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetCurrentHINSTANCE();

	// Default icon for this window (can be set later)
	wc.hIcon = NULL;
	wc.hIconSm = NULL;

	// Cursor to use (can be set later if needed)
	wc.hCursor = ::LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH) 0;
	wc.lpszMenuName = NULL;

	// Name to use when creating windows so it knows to use this class
	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, APP_NAME, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	wc.lpszClassName = windowTitle;

	ATOM result = RegisterClassEx( &wc );
	if (NULL == result) {
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------------------------
void Startup( HINSTANCE applicationInstanceHandle )
{
	RegisterWindow();
	CreateD3D11Window( applicationInstanceHandle, CLIENT_ASPECT );

	SetupXMLInfo();
	if(g_theApp != nullptr)
		delete g_theApp;
	g_theApp = new App();
	g_theApp->Startup();
}



//-----------------------------------------------------------------------------------------------
void Shutdown()
{
	// Destroy the global App instance
	g_theApp->Shutdown();
	delete g_theApp;
	g_theApp = nullptr;

	UnregisterWindow();
}

//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE applicationInstanceHandle, HINSTANCE, LPSTR commandLineString, int )
{
	UNUSED( commandLineString );
	Startup( applicationInstanceHandle );

	// Program main loop; keep running frames until it's time to quit
	while( !g_theApp->IsQuitting() ) // #SD1ToDo: ...becomes:  !g_theApp->IsQuitting()
	{
		RunFrame( (float) GetCurrentTimeSeconds() );
		Sleep( 0 );
	}

	Shutdown();
	return 0;
}


