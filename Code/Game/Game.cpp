#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Shaders/Shader.hpp"
#include "Engine/Renderer/Material.hpp"
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
	delete meshSquareGPU;
	meshSquareGPU = nullptr;
	delete meshSphereGPU;
	meshSphereGPU = nullptr;
	delete meshPlainGPU;
	meshPlainGPU = nullptr;

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
	if( keyCode == 'M' )
	{
		switch( m_state )
		{
		case GAMESTATE_INIT:
			SwitchStates( GAMESTATE_MAINMENU );
			break;
		case GAMESTATE_MAINMENU:
			SwitchStates( GAMESTATE_LOADING );
			break;
		case GAMESTATE_LOADING:
			SwitchStates( GAMESTATE_GAMEPLAY );
			break;
		case GAMESTATE_GAMEPLAY:
			SwitchStates( GAMESTATE_EDITOR );
			break;
		case GAMESTATE_EDITOR:
			SwitchStates( GAMESTATE_INIT );
			break;
		default:
			break;
		}
	}

	if( keyCode == 'Z' )
	{
		switch( m_state )
		{
		case GAMESTATE_MAINMENU:
			m_curMapIdx = 0;
			SwitchStates( GAMESTATE_LOADING );
			break;
		default:
			break;
		}
	}
	if( keyCode == 'X' )
	{
		switch( m_state )
		{
		case GAMESTATE_MAINMENU:
			m_curMapIdx = 1;
			SwitchStates( GAMESTATE_LOADING );
			break;
		default:
			break;
		}
	}
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
		break;
	case GAMESTATE_LOADING:
		LoadLevel( m_curMapIdx );
		break;
	case GAMESTATE_GAMEPLAY:
		UpdateMap( deltaSeconds, m_curMapIdx );
		break;
	case GAMESTATE_EDITOR:
		UpdateMap( deltaSeconds, 0 ); 
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
		g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_lit.mat" ) );
		DebugRenderScreenTextf( 0.0f, Vec2::ALIGN_BOTTOM, 10.0f, Rgba::WHITE, Rgba::WHITE, "Gameplay" );
		RenderMap( m_curMapIdx );
		break;
	case GAMESTATE_EDITOR:
		g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_lit.mat" ) );
		DebugRenderScreenTextf( 0.0f, Vec2::ALIGN_BOTTOM, 10.0f, Rgba::WHITE, Rgba::WHITE, "Editor" );
		RenderMap( 0 );
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
	DebugRenderScreenTextf( 0.0f, Vec2::ALIGN_CENTERED, 10.0f, Rgba::WHITE, Rgba::WHITE, "MainMenu" );
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
		m_curCamera = &(m_maps[m_curMapIdx]->GetCamera()->m_camera);
		m_curCamera->SetColorTargetView( g_theRenderer->GetColorTargetView() );
		m_curCamera->SetDepthTargetView( g_theRenderer->GetDepthTargetView() );
		g_theRenderer->BeginCamera( m_curCamera );
		break;
	case GAMESTATE_EDITOR:
		m_curCamera = &(m_maps[0]->GetCamera()->m_camera);
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
* UpdateMap
*/
void Game::UpdateMap( float deltaSec, unsigned int index )
{
	ASSERT_OR_DIE( index < m_maps.size() && index >= 0, Stringf( "Invalid index of: %u into the maps.", index ) );
	m_maps[index]->Update( deltaSec );
}

//--------------------------------------------------------------------------
/**
* InisializeGame
* initGame is to ensure that the game only inits once.
*/
static bool initGame = false;
void Game::InisializeGame()
{
	++m_loadingFramCount;
	if( m_loadingFramCount == 1 )
	{
		return;
	}
	if( initGame )
	{
		SwitchStates( GAMESTATE_MAINMENU );
		return;
	}
	MeshCPU meshCPU;
	CPUMeshAddCube( &meshCPU, AABB3( 2.0f, 2.0f, 2.0f ) );
	meshSquareGPU = new MeshGPU( g_theRenderer );
	meshSquareGPU->CreateFromCPUMesh<Vertex_LIT>( &meshCPU );

	meshCPU.Clear();
	CPUMeshAddUVSphere( &meshCPU, Vec3::ZERO, 1.5f );
	meshSphereGPU = new MeshGPU( g_theRenderer );
	meshSphereGPU->CreateFromCPUMesh<Vertex_LIT>( &meshCPU );

	meshCPU.Clear();
	CPUMeshAddPlain( &meshCPU, AABB2( 2.0f, 2.0f ) );
	meshPlainGPU = new MeshGPU( g_theRenderer );
	meshPlainGPU->CreateFromCPUMesh<Vertex_LIT>( &meshCPU );

	m_DevColsoleCamera.SetOrthographicProjection( Vec2( -100.0f, -50.0f ), Vec2( 100.0f,  50.0f ) );
	m_DevColsoleCamera.SetModelMatrix( Matrix44::IDENTITY );


	LightData light;
	light.is_direction = 1.0f;
	light.color = Rgba(1.0f, 1.0f, 1.0f, 1.0f);
	light.color.a = 1.0f;
	light.direction = Vec3( -1.0f, -1.0f, 1.0f ).GetNormalized();
	light.position = Vec3::ZERO;
	g_theRenderer->EnableLight( 0, light );

	g_theRenderer->SetSpecFactor( m_specFact );
	g_theRenderer->SetSpecPower( m_specPow );
	g_theRenderer->SetEmissiveFactor( m_emissiveFac );
	g_theRenderer->SetAmbientLight( Rgba::WHITE, m_curAmbiant );


	g_theEventSystem->SubscribeEventCallbackFunction( "setDirColor", Command_SetDirColor );

	m_couchMat = g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/couch.mat" );
	matStruct my_struct;
	my_struct.var = .7f;
	my_struct.padding = Vec3(0.0f, 0.0f, 0.0f); 
	m_couchMat->SetUniforms( &my_struct, sizeof(my_struct) );

	Map* editMap =  new Map( g_theRenderer );
	editMap->Load( "UNUNSED RIGHT NOW" );
	m_maps.push_back( editMap );
	editMap =  new Map( g_theRenderer );
	editMap->Load( "UNUNSED RIGHT NOW" );
	m_maps.push_back( editMap );

	SwitchStates( GAMESTATE_MAINMENU );
	initGame = true;
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

//--------------------------------------------------------------------------
/**
* Command_SetDirColor
*/
bool Game::Command_SetDirColor( EventArgs& args )
{
	Rgba color = args.GetValue("color", color );
	LightData light = g_theRenderer->GetLightAtSlot( 0 );
	light.color = color;
	g_theRenderer->EnableLight( 0, light );
	return true;
}
