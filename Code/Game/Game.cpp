#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shaders/Shader.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/MeshCPU.hpp"
#include "Engine/Renderer/MeshGPU.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"
#include "Engine/Core/Vertex/Vertex_LIT.hpp"
#include "Game/Map.hpp"
#include "Game/GameController.hpp"
#include "Engine/Renderer/Model.hpp"
#include <vector>

#include <Math.h>
//--------------------------------------------------------------------------
/**
* Game
*/
Game::Game()
{

}

//--------------------------------------------------------------------------
/**
* ~Game
*/
Game::~Game()
{

}

//--------------------------------------------------------------------------
/**
* GameStartup
*/
void Game::Startup()
{
	m_shader = g_theRenderer->CreateOrGetShaderFromXML( "Data/Shaders/default_lit.xml" );
	g_theRenderer->BindShader( m_shader );
	
	g_theWindowContext->LockMouse();

	EventArgs args;
	g_theDebugRenderSystem->Command_Open( args );

	g_theRenderer->SetAmbientLight( Rgba::WHITE, m_curAmbiant );
	g_theRenderer->SetSpecFactor( m_specFact );
	g_theRenderer->SetSpecPower( m_specPow );

	// Setup UI Camera
	m_UICamera.SetOrthographicProjection( Vec2( -SCREEN_HALF_WIDTH, -SCREEN_HALF_HEIGHT ), Vec2( SCREEN_HALF_WIDTH,  SCREEN_HALF_HEIGHT ) );
	m_UICamera.SetModelMatrix( Matrix44::IDENTITY );

	m_curCamera = &m_UICamera;
}

//--------------------------------------------------------------------------
/**
* Shutdown
*/
void Game::Shutdown()
{
	SAFE_DELETE(m_mainMenuRadGroup);
	SAFE_DELETE(m_editorRadGroup);
	for( Map* map : m_maps )
	{
		delete map;
		map = nullptr;
	}
}

//--------------------------------------------------------------------------
/**
* HandleKeyPressed
*/
bool Game::HandleKeyPressed( unsigned char keyCode )
{

	//--------------------------------------------------------------------------
	if( keyCode == 'L' )
	{
		m_curAmbiant += 0.01f;
		m_curAmbiant = Clamp( m_curAmbiant, 0.0f, 1.0f );
		g_theRenderer->SetAmbientLight( Rgba::WHITE, m_curAmbiant );
	}
	if( keyCode == 'K' )
	{
		m_curAmbiant -= 0.01f;
		m_curAmbiant = Clamp( m_curAmbiant, 0.0f, 1.0f );
		g_theRenderer->SetAmbientLight( Rgba::WHITE, m_curAmbiant );
	}
	//--------------------------------------------------------------------------
	if( keyCode == 'G' )
	{
		m_emissiveFac += 0.01f;
		m_emissiveFac = Clamp( m_emissiveFac, 0.0f, 1.0f );
		g_theRenderer->SetEmissiveFactor( m_emissiveFac );
	}
	if( keyCode == 'F' )
	{
		m_emissiveFac -= 0.01f;
		m_emissiveFac = Clamp( m_emissiveFac, 0.0f, 1.0f );
		g_theRenderer->SetEmissiveFactor( m_emissiveFac );
	}

	if( keyCode == 'V' )
	{
		LightData light = g_theRenderer->GetLightAtSlot( 0 );
		light.direction = m_curCamera->GetForward();
		g_theRenderer->EnableLight( 0, light );
	}

	
	return false;
}

//--------------------------------------------------------------------------
/**
* HandleKeyReleased
*/
bool Game::HandleKeyReleased( unsigned char keyCode )
{
	UNUSED(keyCode);
	return false;
}

//--------------------------------------------------------------------------
/**
* HandleQuitRequest
*/
bool Game::HandleQuitRequest()
{
	if( m_state == GAMESTATE_GAMEPLAY || m_state == GAMESTATE_EDITOR )
	{
		SwitchStates( GAMESTATE_MAINMENU );
		Event event("unselect_all");
		m_mainMenuCanvis.ProcessInput( event );
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
/**
* UpdateGame
*/
void Game::UpdateGame( float deltaSeconds )
{
	m_gameTime += deltaSeconds;
	switch( m_state )
	{
	case GAMESTATE_INIT:
		InisializeGame();
		break;
	case GAMESTATE_MAINMENU:
		UpdateMainMenu( deltaSeconds );
		break;
	case GAMESTATE_LOADING:
		LoadLevel( m_curMapIdx );
		break;
	case GAMESTATE_GAMEPLAY:
		UpdateMap( deltaSeconds, m_curMapIdx );
		break;
	case GAMESTATE_EDITOR:
		UpdateEditor( deltaSeconds ); 
		break;
	default:
		ERROR_AND_DIE("UNKNOWN STATE IN Game::UpdateGame");
		break;
	}

	float screenHeight = g_theDebugRenderSystem->GetScreenHeight() * .5f;
	float screenWidth = g_theDebugRenderSystem->GetScreenWidth() * .5f;
	Matrix44 camModle = m_curCamera->GetModelMatrix();
	camModle.InvertOrthonormal();
	DebugRenderScreenBasis( 0.0f, Vec2( screenWidth - 4.5f, -screenHeight + 4.5f ), Vec3( camModle.GetK() ), Vec3( camModle.GetJ() ), Vec3( camModle.GetI() ), 4.0f );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, "Camera: %.02f,%.02f,%.02f", m_curCamera->GetModelMatrix().m_values[Matrix44::Tx],  m_curCamera->GetModelMatrix().m_values[Matrix44::Ty],  m_curCamera->GetModelMatrix().m_values[Matrix44::Tz] );

	UpdateCamera( deltaSeconds );
}

//--------------------------------------------------------------------------
/**
* GameRender
*/
void Game::GameRender() const
{
	switch( m_state )
	{
	case GAMESTATE_INIT:
		RenderInit();
		break;
	case GAMESTATE_MAINMENU:
		RenderMainMenu();
		break;
	case GAMESTATE_LOADING:
		RenderLoading();
		break;
	case GAMESTATE_GAMEPLAY:
		RenderMap( m_curMapIdx );
		break;
	case GAMESTATE_EDITOR:
		RenderEditor();
		
		break;
	default:
		ERROR_AND_DIE("UNKNOWN STATE IN Game::GameRender");
		break;
	}

	g_theRenderer->EndCamera();
	g_theDebugRenderSystem->RenderToCamera( g_theGame->GetCurrentCamera() );
}

//--------------------------------------------------------------------------
/**
* RenderMap
*/
void Game::RenderMap( unsigned int index ) const
{
	ASSERT_OR_DIE( index < m_maps.size(), Stringf( "Invalid index of: %u into the maps.", index ) );
	g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_lit.mat" ) );
	m_maps[index]->Render();
}

//--------------------------------------------------------------------------
/**
* RenderLoadingScreen
*/
void Game::RenderLoadingScreen() const
{
	g_theRenderer->ClearScreen( Rgba::LIGHT_BLUE );
	DebugRenderScreenTextf( 0.0f, Vec2::ALIGN_CENTERED, 10.0f, Rgba::WHITE, Rgba::WHITE, "Loading..." );
}


//--------------------------------------------------------------------------
/**
* RenderInit
*/
void Game::RenderInit() const
{
	g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
	DebugRenderScreenTextf( 0.0f, Vec2::ALIGN_BOTTOM, 10.0f, Rgba::WHITE, Rgba::WHITE, "INIT" );
	RenderLoadingScreen();
}

//--------------------------------------------------------------------------
/**
* RenderLoading
*/
void Game::RenderLoading() const
{
	g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
	DebugRenderScreenTextf( 0.0f, Vec2::ALIGN_BOTTOM, 10.0f, Rgba::WHITE, Rgba::WHITE, "GameLoding" );
	RenderLoadingScreen();
}

//--------------------------------------------------------------------------
/**
* RenderMainMenu
*/
void Game::RenderMainMenu() const
{
	g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
	g_theRenderer->ClearScreen( Rgba::BLUE );

	m_mainMenuCanvis.Render();
}


//--------------------------------------------------------------------------
/**
* RenderEditor
*/
void Game::RenderEditor() const
{
	g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
	m_maps[0]->Render();
	g_theRenderer->EndCamera();
	g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
	m_UICamera.SetColorTargetView( g_theRenderer->GetColorTargetView() );
	m_UICamera.SetDepthTargetView( g_theRenderer->GetDepthTargetView() );
	g_theRenderer->BeginCamera( &m_UICamera );
	m_editorCanvis.Render();
}

//--------------------------------------------------------------------------
/**
* SetupMainMenuUI
*/
void Game::SetupMainMenuUI()
{
	m_mainMenuRadGroup = new UIRadioGroup();

	m_mainMenuCanvis.m_pivot = Vec2::ALIGN_CENTERED;
	m_mainMenuCanvis.m_virtualSize = Vec4( 1.0f, 1.0f, 0.f, 0.f );
	m_mainMenuCanvis.m_fillColor = Rgba( 0.2f, .7f, 0.1f );
	m_mainMenuCanvis.m_boarderColor = Rgba::DARK_GREEN;
	m_mainMenuCanvis.m_boarderThickness = ( SinDegrees( m_gameTime * 90.f ) + 2.0f ) * .5f;

	UILabel* textchild = m_mainMenuCanvis.CreateChild<UILabel>();
	textchild->m_pivot = Vec2::ALIGN_CENTERED;
	textchild->m_virtualPosition = Vec4( .5f, .9f, 0, 0 );
	textchild->m_virtualSize = Vec4( .40f, .40f, 0.0f, 0.0f );
	textchild->m_text = "--RTS TITLE--";
	textchild->m_color = Rgba::BLACK;

	UIButton* button = m_mainMenuCanvis.CreateChild<UIButton>();
	button->m_pivot = Vec2::ALIGN_CENTER_LEFT;
	button->m_virtualPosition = Vec4( .2f, .5f, 0, 0 );
	button->m_virtualSize = Vec4( .0f, .0f, 25.f, 10.0f );
	button->m_hoveredColor = Rgba::YELLOW;
	button->m_nutralColor = Rgba::WHITE;
	button->m_selectedColor = Rgba::BLUE;
	button->m_useText = true;
	button->m_text = "Play";
	button->m_eventOnClick = "play level=1";
	button->SetRadioGroup( m_mainMenuRadGroup );

	button = m_mainMenuCanvis.CreateChild<UIButton>();
	button->m_pivot = Vec2::ALIGN_CENTERED;
	button->m_virtualPosition = Vec4( .5f, .5f, 0, 0 );
	button->m_virtualSize = Vec4( .0f, .0f, 25.f, 10.0f );
	button->m_hoveredColor = Rgba::YELLOW;
	button->m_nutralColor = Rgba::WHITE;
	button->m_selectedColor = Rgba::BLUE;
	button->m_useText = true;
	button->m_text = "Editor";
	button->m_eventOnClick = "play level=0";
	button->SetRadioGroup( m_mainMenuRadGroup );

	button = m_mainMenuCanvis.CreateChild<UIButton>();
	button->m_pivot = Vec2::ALIGN_CENTER_RIGHT;
	button->m_virtualPosition = Vec4( .8f, .5f, 0, 0 );
	button->m_virtualSize = Vec4( .0f, .0f, 25.f, 10.0f );
	button->m_hoveredColor = Rgba::YELLOW;
	button->m_nutralColor = Rgba::WHITE;
	button->m_selectedColor = Rgba::BLUE;
	button->m_useText = true;
	button->m_text = "Quit";
	button->m_eventOnClick = "quit";
	button->SetRadioGroup( m_mainMenuRadGroup );
}

//--------------------------------------------------------------------------
/**
* SetupEditorUI
*/
void Game::SetupEditorUI()
{
	m_editorRadGroup = new UIRadioGroup();

	m_editorCanvis.m_pivot = Vec2::ALIGN_TOP_LEFT;
	m_editorCanvis.m_virtualSize = Vec4( 0.0f, 0.0f, 60.0f, 15.f );
	m_editorCanvis.m_virtualPosition = Vec4( 0.025f, 0.95f, 0.0f, 0.0f );
	m_editorCanvis.m_fillColor = Rgba::FADED_BLACK;
	m_editorCanvis.m_boarderColor = Rgba::FADED_GRAY;
	m_editorCanvis.m_boarderThickness = .01f;
	
	float size = 10;

	UIButton* button = m_editorCanvis.CreateChild<UIButton>();
	button->m_pivot = Vec2::ALIGN_CENTERED;
	button->m_virtualPosition = Vec4( .1f, .5f, 0, 0 );
	button->m_virtualSize = Vec4( 0.0f, 0.0f, size, size );
	button->m_hoveredColor = Rgba::YELLOW;
	button->m_nutralColor = Rgba::WHITE;
	button->m_selectedColor = Rgba::BLUE;
	button->m_useText = false;
	button->m_texturePath = "Data/Images/Terrain/Dirt_DIFFU.png";
	button->SetRadioGroup( m_editorRadGroup );

	button = m_editorCanvis.CreateChild<UIButton>();
	button->m_pivot = Vec2::ALIGN_CENTERED;
	button->m_virtualPosition = Vec4( .3f, .5f, 0, 0 );
	button->m_virtualSize = Vec4( 0.0f, 0.0f, size, size );
	button->m_hoveredColor = Rgba::YELLOW;
	button->m_nutralColor = Rgba::WHITE;
	button->m_selectedColor = Rgba::BLUE;
	button->m_useText = false;
	button->m_texturePath = "Data/Images/Terrain/Dirt_Stone_DIFFU.png";
	button->SetRadioGroup( m_editorRadGroup );

	button = m_editorCanvis.CreateChild<UIButton>();
	button->m_pivot = Vec2::ALIGN_CENTERED;
	button->m_virtualPosition = Vec4( .5f, .5f, 0, 0 );
	button->m_virtualSize = Vec4( 0.0f, 0.0f, size, size );
	button->m_hoveredColor = Rgba::YELLOW;
	button->m_nutralColor = Rgba::WHITE;
	button->m_selectedColor = Rgba::BLUE;
	button->m_useText = false;
	button->m_texturePath = "Data/Images/Terrain/Grass_DIFFU.png";
	button->SetRadioGroup( m_editorRadGroup );

	button = m_editorCanvis.CreateChild<UIButton>();
	button->m_pivot = Vec2::ALIGN_CENTERED;
	button->m_virtualPosition = Vec4( .7f, .5f, 0, 0 );
	button->m_virtualSize = Vec4( 0.0f, 0.0f, size, size );
	button->m_hoveredColor = Rgba::YELLOW;
	button->m_nutralColor = Rgba::WHITE;
	button->m_selectedColor = Rgba::BLUE;
	button->m_useText = false;
	button->m_texturePath = "Data/Images/Terrain/GrassRubble_DIFFU.png";
	button->SetRadioGroup( m_editorRadGroup );

	button = m_editorCanvis.CreateChild<UIButton>();
	button->m_pivot = Vec2::ALIGN_CENTERED;
	button->m_virtualPosition = Vec4( .9f, .5f, 0, 0 );
	button->m_virtualSize = Vec4( 0.0f, 0.0f, size, size );
	button->m_hoveredColor = Rgba::YELLOW;
	button->m_nutralColor = Rgba::WHITE;
	button->m_selectedColor = Rgba::BLUE;
	button->m_useText = false;
	button->m_texturePath = "Data/Images/Terrain/Stone_DIFFU1.png";
	button->SetRadioGroup( m_editorRadGroup );

}


//--------------------------------------------------------------------------
/**
* UpdateCamera
*/
void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );

	switch( m_state )
	{
	case GAMESTATE_INIT:
	case GAMESTATE_MAINMENU:
	case GAMESTATE_LOADING:
		m_UICamera.SetColorTargetView( g_theRenderer->GetColorTargetView() );
		m_UICamera.SetDepthTargetView( g_theRenderer->GetDepthTargetView() );
		g_theRenderer->BeginCamera( &m_UICamera );
		m_curCamera = &m_UICamera;
		break;
	case GAMESTATE_GAMEPLAY:
		m_curCamera = (m_maps[m_curMapIdx]->GetCamera());
		m_curCamera->SetColorTargetView( g_theRenderer->GetColorTargetView() );
		m_curCamera->SetDepthTargetView( g_theRenderer->GetDepthTargetView() );
		g_theRenderer->BeginCamera( m_curCamera );
		break;
	case GAMESTATE_EDITOR:
		m_curCamera = (m_maps[0]->GetCamera());
		m_curCamera->SetColorTargetView( g_theRenderer->GetColorTargetView() );
		m_curCamera->SetDepthTargetView( g_theRenderer->GetDepthTargetView() );
		g_theRenderer->BeginCamera( m_curCamera );		
		break;
	default:
		ERROR_AND_DIE("UNKNOWN STATE IN Game::UpdateGame");
		break;
	}
}

//--------------------------------------------------------------------------
/**
* UpdateMainMenu
*/
void Game::UpdateMainMenu( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	m_mainMenuCanvis.m_boarderThickness = ( SinDegrees( m_gameTime * 90.f ) + 2.0f ) * .5f;
	m_mainMenuCanvis.UpdateBounds( AABB2( SCREEN_WIDTH, SCREEN_HEIGHT ) );
	Vec2 mousePos = g_theGameController->GetScreenMousePos();
	Event event( Stringf("hover x=%f y=%f", mousePos.x, mousePos.y ) );
	m_mainMenuCanvis.ProcessInput( event );
}

//--------------------------------------------------------------------------
/**
* UpdateMap
*/
void Game::UpdateMap( float deltaSec, unsigned int index )
{
	ASSERT_OR_DIE( index < m_maps.size() && index >= 0, Stringf( "Invalid index of: %u into the maps.", index ) );
	m_maps[index]->Update( deltaSec );
}

//--------------------------------------------------------------------------
/**
* UpdateEditor
*/
void Game::UpdateEditor( float deltaSec )
{
	UpdateEditorUI( deltaSec );

	m_maps[0]->Update( deltaSec );
}

//--------------------------------------------------------------------------
/**
* UpdateEditorUI
*/
void Game::UpdateEditorUI( float deltaSec )
{
	UNUSED(deltaSec);

	m_editorCanvis.UpdateBounds( AABB2( SCREEN_WIDTH, SCREEN_HEIGHT ) );
	Vec2 mousePos = g_theGameController->GetScreenMousePos();
	Event event( Stringf("hover x=%f y=%f", mousePos.x, mousePos.y ) );
	m_editorCanvis.ProcessInput( event );

}

//--------------------------------------------------------------------------
/**
* LMouseDown
*/
void Game::LMouseDown()
{

}

//--------------------------------------------------------------------------
/**
* LMouseUp
*/
void Game::LMouseUp()
{
	Vec2 mousePos = g_theGameController->GetScreenMousePos();
	Event event( Stringf("click x=%f y=%f", mousePos.x, mousePos.y ) );
	if( m_state == GAMESTATE_MAINMENU )
	{
		m_mainMenuCanvis.ProcessInput( event );
	}
	if( m_state == GAMESTATE_EDITOR )
	{
		m_editorCanvis.ProcessInput( event );
	}
}

//--------------------------------------------------------------------------
/**
* RMouseDown
*/
void Game::RMouseDown()
{
	
}

//--------------------------------------------------------------------------
/**
* RMouseUp
*/
void Game::RMouseUp()
{

}

//--------------------------------------------------------------------------
/**
* InisializeGame
* initGame is to ensure that the game only inits once.
*/
static bool initGame = false;
void Game::InisializeGame()
{
	if( initGame )
	{
		SwitchStates( GAMESTATE_MAINMENU );
		return;
	}
	++m_loadingFramCount;
	if( m_loadingFramCount == 1 )
	{
		return;
	}
	
	m_DevColsoleCamera.SetOrthographicProjection( Vec2( -100.0f, -50.0f ), Vec2( 100.0f,  50.0f ) );
	m_DevColsoleCamera.SetModelMatrix( Matrix44::IDENTITY );


	LightData light;
	light.is_direction = 1.0f;
	light.color = Rgba(1.0f, 1.0f, 1.0f, 1.0f);
	light.color.a = 1.0f;
	light.direction = Vec3( -1.0f, 1.0f, 1.0f ).GetNormalized();
	light.position = Vec3::ZERO;
	g_theRenderer->EnableLight( 0, light );

	g_theRenderer->SetSpecFactor( m_specFact );
	g_theRenderer->SetSpecPower( m_specPow );
	g_theRenderer->SetEmissiveFactor( m_emissiveFac );
	g_theRenderer->SetAmbientLight( Rgba::WHITE, m_curAmbiant );

	g_theEventSystem->SubscribeEventCallbackFunction( "play", LoadToLevel );

	
	Map* editMap =  new Map( g_theRenderer );
	editMap->Load( "UNUNSED RIGHT NOW" );
	m_maps.push_back( editMap );
	editMap =  new Map( g_theRenderer );
	editMap->Load( "UNUNSED RIGHT NOW" );
	m_maps.push_back( editMap );

	SwitchStates( GAMESTATE_MAINMENU );
	initGame = true;

	//Model model( g_theRenderer, "building/towncenter");

	SetupMainMenuUI();
	SetupEditorUI();
}

//--------------------------------------------------------------------------
/**
* LoadLevel
*/
void Game::LoadLevel( unsigned int index )
{
	++m_loadingFramCount;
	if( m_loadingFramCount == 1 )
	{
		return;
	}
	m_curMapIdx = index;
	if( (unsigned int) m_maps.size() > index )
	{
		SwitchStates( m_curMapIdx == 0 ? GAMESTATE_EDITOR : GAMESTATE_GAMEPLAY );
		return;
	}
	if( !m_maps[index]->IsLoaded() )
	{
		m_maps[index]->Load( "UNUSED RIGHT NOW" );
		SwitchStates( m_curMapIdx == 0 ? GAMESTATE_EDITOR : GAMESTATE_GAMEPLAY );
	}
}

//--------------------------------------------------------------------------
/**
* LoadToLevel
*/
bool Game::LoadToLevel( EventArgs& args )
{
	int level = args.GetValue( "level", 0 );
	g_theGame->SwitchStates( GAMESTATE_LOADING );
	g_theGame->m_curMapIdx = level;
	return true;
}

//--------------------------------------------------------------------------
/**
* SwitchStates
*/
void Game::SwitchStates( eGameStates state )
{
	if( state == GAMESTATE_INIT || state == GAMESTATE_LOADING )
	{
		m_loadingFramCount = 0;
	}
	m_state = state;
}
