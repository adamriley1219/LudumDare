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
	
	m_UICamera.SetOrthographicProjection( Vec2( -100.0f, -50.0f ), Vec2( 100.0f,  50.0f ) );
	m_UICamera.SetModelMatrix( Matrix44::IDENTITY );

	EventArgs args;
	g_theDebugRenderSystem->Command_Open( args );

	g_theRenderer->SetAmbientLight( Rgba::WHITE, m_curAmbiant );
	g_theRenderer->SetSpecFactor( m_specFact );
	g_theRenderer->SetSpecPower( m_specPow );

	LightData light;
	light.is_direction = 1.0f;
	light.color = Rgba(1.0f, 1.0f, 1.0f, 1.0f);
	light.color.a = 1.0f;
	light.direction = Vec3( -1.0f, -1.0f, 1.0f ).GetNormalized();
	light.position = Vec3::ZERO;
	g_theRenderer->EnableLight( 0, light );

	light.is_direction = 0.0f;
	light.color = Rgba::CYAN;
	light.color.a = 0.0f;
	g_theRenderer->EnableLight( 1, light );
	
	light.is_direction = 0.0f;
	light.color = Rgba::BLUE;
	light.color.a = 0.0f;
	g_theRenderer->EnableLight( 2, light );

	light.is_direction = 0.0f;
	light.color = Rgba::MAGENTA;
	light.color.a = 0.0f;
	g_theRenderer->EnableLight( 3, light );

	light.is_direction = 0.0f;
	light.color = Rgba::GREEN;
	light.color.a = 0.0f;
	g_theRenderer->EnableLight( 4, light );

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

	// Setup UI Camera
	m_UICamera.SetOrthographicProjection( Vec2( -50.0f, -100.0f ), Vec2( 50.0f,  100.0f ) );
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
			m_state = GAMESTATE_MAINMENU;
			break;
		case GAMESTATE_MAINMENU:
			m_state = GAMESTATE_LOADING;
			break;
		case GAMESTATE_LOADING:
			m_state = GAMESTATE_GAMEPLAY;
			break;
		case GAMESTATE_GAMEPLAY:
			m_state = GAMESTATE_EDITOR;
			break;
		case GAMESTATE_EDITOR:
			m_state = GAMESTATE_INIT;
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

	//--------------------------------------------------------------------------
	if( keyCode == 222 ) // '''
	{
		++m_actLights;
		m_actLights = Clamp( (int) m_actLights, 0, 8 );
	}
	if( keyCode == 186 ) // ';'
	{
		--m_actLights;
		m_actLights = Clamp( (int) m_actLights, 0, 8 );
	}

	//--------------------------------------------------------------------------
	if( keyCode == 'W' )
	{
		input.z = 1.0f;
		return true;
	}
	if( keyCode == 'S' )
	{
		input.z = -1.0f;
		return true;
	}
	if( keyCode == 'E' )
	{
		input.y = 1.0f;
		return true;
	}
	if( keyCode == 'Q' )
	{
		input.y = -1.0f;
		return true;
	}
	if( keyCode == 'A' )
	{
		input.x = -1.0f;
		return true;
	}
	if( keyCode == 'D' )
	{
		input.x = 1.0f;
		return true;
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
	if( keyCode == 'W' )
	{
		input.z = 0;
		m_camPos += m_curCamera->GetForward() * 0.1f;
		return true;
	}
	if( keyCode == 'S' )
	{
		input.z = 0;
		return true;
	}
	if( keyCode == 'E' )
	{
		input.y = 0;
		return true;
	}
	if( keyCode == 'Q' )
	{
		input.y = 0;
		return true;
	}
	if( keyCode == 'A' )
	{
		input.x = 0;
		return true;
	}
	if( keyCode == 'D' )
	{
		input.x = 0;
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
	switch( m_state )
	{
	case GAMESTATE_INIT:
		m_UICamera.SetColorTargetView( g_theRenderer->GetColorTargetView() );
		m_UICamera.SetDepthTargetView( g_theRenderer->GetDepthTargetView() );
		g_theRenderer->BeginCamera( &m_UICamera );
		break;
	case GAMESTATE_MAINMENU:
		break;
	case GAMESTATE_LOADING:
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

	m_gameTime += deltaSeconds;
	IntVec2 rawMouseMovement = g_theWindowContext->GetClientMouseRelativeMovement();
	float rotSpeed = 35.0f;
	m_camRot.y += 0.005f * rawMouseMovement.x * rotSpeed;
	m_camRot.x += 0.005f * rawMouseMovement.y * rotSpeed;
	if( m_camRot.x > 90.0f )
	{
		m_camRot.x = 90.0f;
	}
	if( m_camRot.x < -90.0f )
	{
		m_camRot.x = -90.0f;
	}

	float speed = 10.0f;
	m_camPos += m_curCamera->GetForward() * input.z * deltaSeconds * speed;
	m_camPos += m_curCamera->GetUp() * input.y * deltaSeconds * speed;
	m_camPos += m_curCamera->GetRight() * input.x * deltaSeconds * speed;

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
		g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
		DebugRenderScreenTextf( 0.0f, Vec2::ALIGN_BOTTOM, 10.0f, Rgba::WHITE, Rgba::WHITE, "INIT" );
		RenderLoadingScreen();
		break;
	case GAMESTATE_MAINMENU:
		g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
		g_theRenderer->ClearScreen( Rgba::BLUE );
		DebugRenderScreenTextf( 0.0f, Vec2::ALIGN_CENTERED, 10.0f, Rgba::WHITE, Rgba::WHITE, "MainMenu" );
		break;
	case GAMESTATE_LOADING:
		g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
		DebugRenderScreenTextf( 0.0f, Vec2::ALIGN_BOTTOM, 10.0f, Rgba::WHITE, Rgba::WHITE, "GameLoding" );
		RenderLoadingScreen();
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
		g_theRenderer->BeginCamera( m_curCamera );
		break;
	case GAMESTATE_EDITOR:
		m_curCamera = &(m_maps[0]->GetCamera()->m_camera);
		g_theRenderer->BeginCamera( m_curCamera );		
		break;
	default:
		ERROR_AND_DIE("UNKNOWN STATE IN Game::UpdateGame");
		break;
	}
// 	m_curentCamera.SetModelMatrix( g_theGame->m_camPos, g_theGame->m_camRot );
// 	m_curentCamera.SetPerspectiveProjection( 90.f, WORLD_WIDTH / WORLD_HEIGHT, 0.000000001f );
// 	m_curentCamera.SetColorTargetView( g_theRenderer->GetColorTargetView() );
// 	m_curentCamera.SetDepthTargetView( g_theRenderer->GetDepthTargetView() );
// 	g_theRenderer->BeginCamera( &m_curentCamera );
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
