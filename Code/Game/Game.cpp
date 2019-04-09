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
	g_theWindowContext->HideMouse();
	g_theWindowContext->SetMouseMode( MOUSE_MODE_RELATIVE );

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
	if( keyCode == 'N' )
	{
		++shaderType;
		switch( shaderType )
		{
		case 0:
			m_shader = g_theRenderer->CreateOrGetShaderFromXML( "Data/Shaders/default_lit.xml" );
			break;
		case 1:
			m_shader = g_theRenderer->CreateOrGetShaderFromXML( "Data/Shaders/normal.xml" );
			break;
		case 2:
			m_shader = g_theRenderer->CreateOrGetShaderFromXML( "Data/Shaders/tangent.xml" );
			break;
		case 3:
			m_shader = g_theRenderer->CreateOrGetShaderFromXML( "Data/Shaders/bitangent.xml" );
			break;
		default:
			shaderType = 0;
			m_shader = g_theRenderer->CreateOrGetShaderFromXML( "Data/Shaders/default_lit.xml" );
			break;
		}
	}
	//--------------------------------------------------------------------------
	if( keyCode == 'J' )
	{
		m_specPow += 0.05f;
		m_specPow = Clamp( m_specPow, 0.0f, 99.0f );
		g_theRenderer->SetSpecPower( m_specPow );
	}
	if( keyCode == 'H' )
	{
		m_specPow -= 0.05f;
		m_specPow = Clamp( m_specPow, 0.0f, 99.0f );
		g_theRenderer->SetSpecPower( m_specPow );
	}	
	//--------------------------------------------------------------------------
	if( keyCode == 190 ) // '.'
	{
		m_specFact += 0.01f;
		m_specFact = Clamp( m_specFact, 0.0f, 1.0f );
		g_theRenderer->SetSpecFactor( m_specFact );
	}
	if( keyCode == 188 ) // ','
	{
		m_specFact -= 0.01f;
		m_specFact = Clamp( m_specFact, 0.0f, 1.0f );
		g_theRenderer->SetSpecFactor( m_specFact );
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

	//--------------------------------------------------------------------------
	if( keyCode == 'B' )
	{
		LightData light = g_theRenderer->GetLightAtSlot( 1 );
		light.position = m_camPos;
		g_theRenderer->EnableLight( 1, light );
	}
	if( keyCode == 'V' )
	{
		LightData light = g_theRenderer->GetLightAtSlot( 0 );
		light.direction = m_curentCamera.GetForward();
		g_theRenderer->EnableLight( 0, light );
	}
	if( keyCode == 'X' )
	{
		LightData light = g_theRenderer->GetLightAtSlot( 0 );
		light.color.a = 0.0f;
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
		m_camPos += m_curentCamera.GetForward() * 0.1f;
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
	m_camPos += m_curentCamera.GetForward() * input.z * deltaSeconds * speed;
	m_camPos += m_curentCamera.GetUp() * input.y * deltaSeconds * speed;
	m_camPos += m_curentCamera.GetRight() * input.x * deltaSeconds * speed;

	float screenHeight = g_theDebugRenderSystem->GetScreenHeight() * .5f;
	float screenWidth = g_theDebugRenderSystem->GetScreenWidth() * .5f;
	Matrix44 camModle = m_curentCamera.GetModelMatrix();
	camModle.InvertOrthonormal();
	DebugRenderScreenBasis( 0.0f, Vec2( screenWidth - 4.5f, -screenHeight + 4.5f ), Vec3( camModle.GetK() ), Vec3( camModle.GetJ() ), Vec3( camModle.GetI() ), 4.0f );


	Matrix44 mat = Matrix44::FromEuler( Vec3( 0.0f, g_theApp->GetGlobleTime() * 40.0f, 0.0f ), ROTATION_ORDER_ZXY );

	if( m_actLights > 1 )
	{
		LightData light = g_theRenderer->GetLightAtSlot( 1 );
		light.color.a =  1.0f;
		g_theRenderer->EnableLight( 1, light );
	}
	else
	{
		g_theRenderer->DisableLight( 1 );
	}
	if( m_actLights > 2 )
	{
		LightData light = g_theRenderer->GetLightAtSlot( 2 );
		light.position = mat.TransformPosition3D( Vec3( 0.0f, 0.0f, -1.0f ) * 5.0f );
		light.color.a =  1.0f;
		g_theRenderer->EnableLight( 2, light );
	}
	else
	{
		g_theRenderer->DisableLight( 2 );
	}
	if( m_actLights > 3 )
	{
		LightData light = g_theRenderer->GetLightAtSlot( 3 );
		light.position = mat.TransformPosition3D( Vec3( 1.0f, 0.0f, 0.0f ) * 5.0f );
		light.color.a =  1.0f;
		light.diffuse_attenuation = Vec3( 1.0f, 0.05f, 0.1f );
		g_theRenderer->EnableLight( 3, light );
	}
	else
	{
		g_theRenderer->DisableLight( 3 );
	}
	if( m_actLights > 4 )
	{
		LightData light = g_theRenderer->GetLightAtSlot( 4 );
		light.position = mat.TransformPosition3D( Vec3( -1.0f, 0.0f, 0.0f ) * 5.0f );
		light.color.a =  1.0f;
		//light.diffuse_attenuation = Vec3( .5f, 0.2f, 0.3f );
		g_theRenderer->EnableLight( 4, light );
	}
	else
	{
		g_theRenderer->DisableLight( 4 );
	}

	for( unsigned int lightIdx = 0; lightIdx < 8; ++lightIdx )
	{
		LightData light = g_theRenderer->GetLightAtSlot( lightIdx );
		if( g_theRenderer->GetLightAtSlot( lightIdx ).color.a > 0.0f )
		{
			DebugRenderPoint( 0.0f, DEBUG_RENDER_USE_DEPTH, light.position, light.color );
		}
		else
		{
			DebugRenderPoint( 0.0f, DEBUG_RENDER_USE_DEPTH, light.position, Rgba::RED );
		}
	}
	matStruct my_struct;
	my_struct.var = SinDegrees( (float) GetCurrentTimeSeconds() * 50.0f ) * .5f + .5f;
	my_struct.padding = Vec3(0.0f, 0.0f, 0.0f); 
	m_couchMat->SetUniforms( &my_struct, sizeof(my_struct) );



	UpdateMaps();
	UpdateCamera( deltaSeconds );
}

//--------------------------------------------------------------------------
/**
* GameRender
*/
void Game::GameRender() const
{
	
	RenderMaps();

	g_theDebugRenderSystem->RenderToCamera( g_theGame->GetCurrentCamera() );

}

//--------------------------------------------------------------------------
/**
* RenderMaps
*/
void Game::RenderMaps() const
{
	for( int mapIdx = 0; mapIdx < (int) m_maps.size(); ++mapIdx )
	{
		m_maps[mapIdx]->Render();
	}
}

//--------------------------------------------------------------------------
/**
* UpdateCamera
*/
void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	m_curentCamera.SetModelMatrix( g_theGame->m_camPos, g_theGame->m_camRot );
	m_curentCamera.SetPerspectiveProjection( 60.f, WORLD_WIDTH / WORLD_HEIGHT, 0.1f );
	m_curentCamera.SetColorTargetView( g_theRenderer->GetColorTargetView() );
	m_curentCamera.SetDepthTargetView( g_theRenderer->GetDepthTargetView() );
	g_theRenderer->BeginCamera( &m_curentCamera );
}


//--------------------------------------------------------------------------
/**
* UpdateMaps
*/
void Game::UpdateMaps()
{
	for( int mapIdx = 0; mapIdx < (int) m_maps.size(); ++mapIdx )
	{
		m_maps[mapIdx]->Update();
	}
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
