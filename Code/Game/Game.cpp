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
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Core/Time/StopWatch.hpp"
#include "Engine/Core/MeshCPU.hpp"
#include "Engine/Renderer/MeshGPU.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"
#include "Engine/Core/Vertex/Vertex_LIT.hpp"
#include "Game/Map.hpp"
#include "Game/GameController.hpp"
#include "Engine/Renderer/Model.hpp"
#include "Engine/Renderer/Shaders/UniformBuffer.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"
#include "Engine/Physics/Collision2D.hpp"
#include "Game/Shapes/Shape.hpp"
#include "Game/Shapes/Pill.hpp"
#include "Game/Shapes/Cursor.hpp"

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
	g_theWindowContext->LockMouse();

	EventArgs args;
	g_theDebugRenderSystem->Command_Open( args );

	// Setup UI Camera
	m_UICamera.SetOrthographicProjection( Vec2( -SCREEN_HALF_WIDTH, -SCREEN_HALF_HEIGHT ), Vec2( SCREEN_HALF_WIDTH,  SCREEN_HALF_HEIGHT ) );
	m_UICamera.SetModelMatrix( Matrix44::IDENTITY );

	m_DevColsoleCamera.SetOrthographicProjection( Vec2( -100.0f, -50.0f ), Vec2( 100.0f,  50.0f ) );
	m_DevColsoleCamera.SetModelMatrix( Matrix44::IDENTITY );

	m_curCamera = &m_UICamera;

	m_fadeinStopwatch = new StopWatch( g_theApp->m_UIClock );
	m_fadeoutStopwatch = new StopWatch( g_theApp->m_UIClock );
}

//--------------------------------------------------------------------------
/**
* Shutdown
*/
void Game::Shutdown()
{
	SAFE_DELETE(m_cursor);
	SAFE_DELETE(m_mainMenuRadGroup);
	SAFE_DELETE(m_editorRadGroup);
	SAFE_DELETE(m_pauseMenuRadGroup);
	SAFE_DELETE(m_fadeinStopwatch);
	SAFE_DELETE(m_fadeoutStopwatch);
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
	if( m_state == GAMESTATE_EDITOR )
	{
		switch( keyCode )
		{
		case 76: // L
			m_curRadius += 0.05f;
			if( m_curRadius > 4.0f )
			{
				m_curRadius = 4.0f;
			}
			break;
		case 75: // K
			m_curRadius -= 0.05f;
			if( m_curRadius < 0.0f )
			{
				m_curRadius = 0.0f;
			}
			break;
		case 74: // J
			m_curThickness += 0.05f;
			if( m_curThickness > 4.0f )
			{
				m_curThickness = 4.0f;
			}
			break;
		case 72: // H
			m_curThickness -= 0.05f;
			if( m_curThickness < 0.0f )
			{
				m_curThickness = 0.0f;
			}
			break;
		case 222: // '
			m_spawnDynamic = !m_spawnDynamic;
			break;
		case 186: // ;
			break;
		case 86: // V
			m_friction -= 0.01f;
			if( m_friction < 0.0f )
			{
				m_friction = -0.0f;
			}
			break;
		case 66: // 'B'
			m_friction += 0.01f;
			if( m_friction > 8.0f )
			{
				m_friction = 8.0f;
			}
			break;

			// C
		case 'C':
			m_drag += 0.01f;
			break;

			// X
		case 'X':
			m_drag -= 0.01f;
			if( m_drag < 0.0f )
			{
				m_drag = 0.0f;
			}
			break;
		case 'Z':
			g_theApp->m_gameClock->ForceStep( 0.02 );
			break;

		case '1':
			m_xRestrcted =! m_xRestrcted;
			break;
		case '2':
			m_yRestrcted =! m_yRestrcted;
			break;
		case '3':
			m_rotRestrcted =! m_rotRestrcted;
			break;
		case '4':
			ToggleAlignment();
			break;
		case '5':
			SetEnd();
			break;

			// G
		case 'G':
			m_angularDrag += 0.01f;
			break;

			// F
		case 'F':
			m_angularDrag -= 0.01f;
			if( m_angularDrag < 0.0f )
			{
				m_angularDrag = 0.0f;
			}
			break;

		case 219:
			m_angularVel -= 2.f;
			break;
		case 221:
			m_angularVel += 2.f;
			break;

		case 78: // 'N'
			m_mass -= 0.15f;
			if( m_mass < 0.1f )
			{
				m_mass = 0.1f;
			}
			break;
		case 77: // 'M'
			m_mass += 0.15f;
			if( m_mass > 20.0f )
			{
				m_mass = 20.0f;
			}
			break;
		case 188: // '.'
			m_restitution -= 0.01f;
			if( m_restitution < 0.0f )
			{
				m_restitution = 0.0f;
			}
			break;
		case 190: // ','
			m_restitution += 0.01f;
			if( m_restitution > 1.0f )
			{
				m_restitution = 1.0f;
			}
			break;
		case 't': // F5 press
			return true;
			break;
		case 9: // Tab
			SelectShape();
			return true;
		case ' ': // Space
			DeselectShape();
			return true;
		case 46: // Delete
			m_deletingSelected = true;
			return true;

		

		default:
			break;
		}
	}
	else if( m_state == GAMESTATE_GAMEPLAY )
	{
		switch( keyCode )
		{
		case 187:
			LoadNextMap();
			break;
		default:
			break;
		}
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
		m_pauseMenuCanvis.ProcessInput( event );
		m_editorCanvis.ProcessInput( event );
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
	UpdateStates();
	m_gameTime += deltaSeconds;
	++m_stateFrameCount;
	if( m_toNextLevel )
	{
		m_toNextLevel = false;
		++m_curMapIdx;
	}
	switch( m_state )
	{
	case GAMESTATE_INIT:
		InisializeGame();
		break;
	case GAMESTATE_MAINMENU:
		UpdateMainMenu();
		break;
	case GAMESTATE_LOADING:
		LoadLevel( m_curMapIdx );
		break;
	case GAMESTATE_GAMEPLAY:
		if( g_theApp->IsPaused() )
		{
			UpdatePauseMenu();
		}
		UpdateMap( deltaSeconds, m_curMapIdx );
		break;
	case GAMESTATE_EDITOR:
		if( g_theApp->IsPaused() )
		{
			UpdatePauseMenu();
		}
		UpdateEditor( deltaSeconds ); 
		break;
	default:
		ERROR_AND_DIE("UNKNOWN STATE IN Game::UpdateGame");
		break;
	}

	if( m_deletingSelected )
	{
		DeleteShape();
	}

	float screenHeight = g_theDebugRenderSystem->GetScreenHeight() * .5f;
	float screenWidth = g_theDebugRenderSystem->GetScreenWidth() * .5f;
	Matrix44 camModle = m_curCamera->GetModelMatrix();
	camModle.InvertOrthonormal();
	DebugRenderScreenBasis( 0.0f, Vec2( screenWidth - 4.5f, -screenHeight + 4.5f ), Vec3( camModle.GetK() ), Vec3( camModle.GetJ() ), Vec3( camModle.GetI() ), 4.0f );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, "Camera: %.02f,%.02f,%.02f", m_curCamera->GetModelMatrix().m_values[Matrix44::Tx],  m_curCamera->GetModelMatrix().m_values[Matrix44::Ty],  m_curCamera->GetModelMatrix().m_values[Matrix44::Tz] );

	UpdatePlayerPosAndCamera( deltaSeconds );
}

//--------------------------------------------------------------------------
/**
* GetCurrentMap
*/
Map* Game::GetCurrentMap()
{
	if( GAMESTATE_GAMEPLAY == m_state || GAMESTATE_EDITOR == m_state )
	{
		return m_maps[m_curMapIdx];
	}
	return nullptr;
}

//--------------------------------------------------------------------------
/**
* SelectShape
*/
void Game::SelectShape()
{
	if( GetCurrentMap()->GetNumShapes() == 0 )
	{
		return;
	}
	if( m_selectedShape )
	{
		// Find where to start searching for next.
		int newIndex = 0;
		for( int shapeIndex = 0; shapeIndex < (int) GetCurrentMap()->m_shapes.size(); ++shapeIndex )
		{
			if( m_selectedShape == GetCurrentMap()->m_shapes[shapeIndex] )
			{
				newIndex = ++shapeIndex;
				break;
			}

		}

		// Don't go out of bounds
		if( newIndex >= (int) GetCurrentMap()->m_shapes.size() )
		{
			newIndex = 0;
		}

		// Find next item to select
		int toSelectIndex = newIndex;
		int counter = 300;
		while ( !GetCurrentMap()->m_shapes[toSelectIndex] )
		{
			toSelectIndex++;
			counter--;
			if( toSelectIndex >= (int) GetCurrentMap()->m_shapes.size() )
			{
				toSelectIndex = 0;
			}
			if( counter == 0 )
			{
				break;
			}
		}

		// Only select if found one that exists.
		if( GetCurrentMap()->m_shapes[toSelectIndex] )
		{
			DeselectShape();
			m_selectedShape = GetCurrentMap()->m_shapes[toSelectIndex];
			m_selectedShape->m_selected = true;
			m_selectedShape->m_rigidbody->SetSimulationType( ePhysicsSimulationType::PHYSICS_SIM_STATIC );
			m_cursor->m_trasform.m_position = m_selectedShape->GetPosition();

			m_xRestrcted	= m_selectedShape->m_rigidbody->IsXRestricted();
			m_yRestrcted	= m_selectedShape->m_rigidbody->IsYRestricted();
			m_rotRestrcted	= m_selectedShape->m_rigidbody->IsRotRestricted();

			m_angularVel = m_selectedShape->m_rigidbody->GetAngularVelocity();
			// 			m_restitution	= m_selectedShape->m_rigidbody->GetRestitution();
			// 			m_friction		= m_selectedShape->m_rigidbody->GetFriction();
			// 			m_mass			= m_selectedShape->m_rigidbody->GetMass();
			// 			m_angularDrag	= m_selectedShape->m_rigidbody->GetAngularDrag();
			// 			m_drag			= m_selectedShape->m_rigidbody->GetDrag();
		}
	}
	else
	{
		Shape* closestShape = nullptr;
		float closestDistance = 999999999999.0f;
		for( int shapeIndex = 0; shapeIndex < (int)  GetCurrentMap()->m_shapes.size(); ++shapeIndex )
		{
			if(  GetCurrentMap()->m_shapes[shapeIndex] )
			{
				Shape* testShape =  GetCurrentMap()->m_shapes[shapeIndex];
				float testDist = ( m_cursor->m_trasform.m_position - testShape->GetPosition() ).GetLengthSquared();
				if( testDist < closestDistance )
				{
					closestDistance = testDist;
					closestShape = testShape;
				}
			}
		}
		// Only select if found one that exists.
		if( closestShape )
		{
			m_selectedShape = closestShape;
			m_selectedShape->m_selected = true;
			m_selectedShape->m_rigidbody->SetSimulationType( ePhysicsSimulationType::PHYSICS_SIM_STATIC );
			m_cursor->m_trasform.m_position = m_selectedShape->GetPosition();

			m_xRestrcted	= m_selectedShape->m_rigidbody->IsXRestricted();
			m_yRestrcted	= m_selectedShape->m_rigidbody->IsYRestricted();
			m_rotRestrcted	= m_selectedShape->m_rigidbody->IsRotRestricted();

			m_angularVel = m_selectedShape->m_rigidbody->GetAngularVelocity();

			// 			m_restitution	= m_selectedShape->m_rigidbody->GetRestitution();
			// 			m_friction		= m_selectedShape->m_rigidbody->GetFriction();
			// 			m_mass			= m_selectedShape->m_rigidbody->GetMass();
			// 			m_angularDrag	= m_selectedShape->m_rigidbody->GetAngularDrag();
			// 			m_drag			= m_selectedShape->m_rigidbody->GetDrag();
		}
	}
}

//--------------------------------------------------------------------------
/**
* IsHovering
*/
bool Game::IsHovering( Shape* shape ) const
{
	Collision2D collisionInfo;
	DiscCollider2D cursorCollider( m_cursor->m_trasform.m_position, m_cursor->m_disc.m_radius );
	return shape->m_collider->IsTouching( &cursorCollider, &collisionInfo );
}

//--------------------------------------------------------------------------
/**
* DeselectShape
*/
void Game::DeselectShape()
{
	if( m_selectedShape )
	{
		m_selectedShape->m_rigidbody->ResetSimulationType();
		m_selectedShape->m_selected = false;
		m_selectedShape = nullptr;
	}
}

//--------------------------------------------------------------------------
/**
* DeleteShape
*/
void Game::DeleteShape()
{
	if( m_selectedShape )
	{
		m_selectedShape->m_isGarbage = true;
		m_selectedShape = nullptr;
	}
	m_deletingSelected = false;
}

//--------------------------------------------------------------------------
/**
* BeginShapeConstruction
*/
void Game::BeginShapeConstruction()
{
	if( !m_constructing )
	{
		m_constStart = m_cursor->m_trasform.m_position;
		m_constEnd = m_cursor->m_trasform.m_position;
		m_constructing = true;
	}
}

//--------------------------------------------------------------------------
/**
* EndShapeConstruction
*/
void Game::EndShapeConstruction()
{
	if( m_constructing )
	{
		m_constEnd =  m_cursor->m_trasform.m_position;
		Vec2 disp = m_constEnd - m_constStart;
		Transform2D trans( m_constStart + disp * 0.5f, disp.GetAngleDegrees() );
		Shape* shape = new Pill( trans , m_spawnDynamic ? PHYSICS_SIM_DYNAMIC : PHYSICS_SIM_STATIC, m_curAlignment, disp.GetLength(), m_curThickness, m_curRadius, m_mass, m_restitution, m_friction, m_drag, m_angularDrag );
		shape->m_rigidbody->SetRestrictions( m_xRestrcted, m_yRestrcted, m_rotRestrcted );
		shape->m_rigidbody->SetAngularVelocity( m_angularVel );
		m_maps[m_curMapIdx]->AddShape( shape );
		m_constructing = false;
	}
}

//--------------------------------------------------------------------------
/**
* ToggleAlignment
*/
void Game::ToggleAlignment()
{
	switch( m_curAlignment )
	{
	case ALIGNMENT_PLAYER:
		m_curAlignment = ALIGNMENT_NEUTRAL;
		break;
	case ALIGNMENT_NEUTRAL:
		m_curAlignment = ALIGNMENT_ALLY;
		break;
	case ALIGNMENT_ALLY:
		m_curAlignment = ALIGNMENT_ENEMY;
		break;
	case ALIGNMENT_ENEMY:
		m_curAlignment = ALIGNMENT_PLAYER;
		break;
	default:
		m_curAlignment = ALIGNMENT_NEUTRAL;
		break;
	}
}

//--------------------------------------------------------------------------
/**
* SetEnd
*/
void Game::SetEnd()
{
	if( m_state == GAMESTATE_EDITOR )	
	{
		m_maps[0]->m_endZone = Vec2( g_theGameController->GetWorldMousePos() );
	}
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

	if( g_theApp->IsPaused() && ( GAMESTATE_GAMEPLAY == m_state || GAMESTATE_EDITOR == m_state ) )
	{
		RenderPauseMenu();
	}

	RenderFade();

	g_theDebugRenderSystem->RenderToCamera( g_theGame->GetCurrentCamera() );
}

//--------------------------------------------------------------------------
/**
* RenderMap
*/
void Game::RenderMap( unsigned int index ) const
{
	ASSERT_OR_DIE( index < m_maps.size(), Stringf( "Invalid index of: %u into the maps.", index ) );
	g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
	m_maps[index]->Render();
}

//--------------------------------------------------------------------------
/**
* RenderLoadingScreen
*/
void Game::RenderLoadingScreen() const
{
	g_theRenderer->ClearScreen( Rgba::BLUE );


	UICanvas canvas;

	canvas.m_pivot = Vec2::ALIGN_CENTERED;
	canvas.m_virtualSize = Vec4( 1.0f, 1.0f, 0.f, 0.f );
	canvas.m_fillColor = Rgba::BLACK;
	canvas.m_boarderColor = Rgba::GRAY;
	canvas.m_boarderThickness = 1.0f;

	UILabel* textchild = canvas.CreateChild<UILabel>();
	textchild->m_pivot = Vec2::ALIGN_CENTERED;
	textchild->m_virtualPosition = Vec4( 0.5f, 0.5f, 0.0f, 0.0f );
	textchild->m_virtualSize = Vec4( .40f, .40f, 0.0f, 0.0f );
	textchild->m_text = "Loading";
	textchild->m_color = Rgba::WHITE;

	canvas.UpdateBounds( AABB2( SCREEN_WIDTH, SCREEN_HEIGHT ) );
	canvas.Render();
}


//--------------------------------------------------------------------------
/**
* RenderInit
*/
void Game::RenderInit() const
{
	g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
	RenderLoadingScreen();
}

//--------------------------------------------------------------------------
/**
* RenderLoading
*/
void Game::RenderLoading() const
{
	g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
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
	m_cursor->Render();
	m_maps[0]->Render();

	if( m_constructing )
	{
		if( m_constStart != m_cursor->m_trasform.m_position )
		{
			std::vector<Vertex_PCU> verts;
			AddVertsForLine2D( verts, m_constStart, m_cursor->m_trasform.m_position, 0.05f, Rgba::DARK_RED );
			g_theRenderer->DrawVertexArray( verts );
		}
	}

// 	g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
// 	m_UICamera.SetColorTargetView( g_theRenderer->GetColorTargetView() );
// 	m_UICamera.SetDepthTargetView( g_theRenderer->GetDepthTargetView() );
// 	g_theRenderer->BeginCamera( &m_UICamera );
// 	m_editorCanvis.Render();
}

//--------------------------------------------------------------------------
/**
* RenderPauseMenu
*/
void Game::RenderPauseMenu() const
{
	singleEffect matVals;
	matVals.STRENGTH = 1.0f;
	Material* mat = g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/grayscale.mat" );
	mat->SetUniforms( &matVals, sizeof( matVals ) );
	g_theRenderer->ApplyEffect( g_theRenderer->GetScratchColorTargetView(), g_theRenderer->GetRenderTargetTextureView(), mat );
	g_theRenderer->CopyTexture( g_theRenderer->GetBufferTexture(), g_theRenderer->GetScratchBuffer() );

	g_theRenderer->BindMaterial( g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/default_unlit.mat" ) );
	m_UICamera.SetColorTargetView( g_theRenderer->GetColorTargetView() );
	m_UICamera.SetDepthTargetView( g_theRenderer->GetDepthTargetView() );
	g_theRenderer->BeginCamera( &m_UICamera );
	m_pauseMenuCanvis.Render();
}


//--------------------------------------------------------------------------
/**
* DrawEditorValues
*/
void Game::DrawEditorValues()
{
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Set EndPos [5]" ).c_str() );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Number of Objects:	   %d", GetCurrentMap()->m_shapes.size() ).c_str() );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Objects Mass[n,m]:       %.2f", m_mass ).c_str() );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Objects Restitution[<,>]:%.2f", m_restitution ).c_str() );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Objects Friction[V,B]:   %.2f", m_friction ).c_str() );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Objects Drag[X,C]:	   %.2f", m_drag ).c_str() );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Objects AngularDrag[F,G]:%.2f", m_angularDrag ).c_str() );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Objects AngularVel[[,]]: %.2f", m_angularVel ).c_str() );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Objects Radius[K,L]:	   %.2f", m_curRadius ).c_str() );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Objects Thickness[H,J]:  %.2f", m_curThickness ).c_str() );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Objects Type[']: %s", m_spawnDynamic ? "DYNAMIC" : "STATIC" ).c_str() );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Restrictions :  x:%s, y:%s Rot: %s", m_xRestrcted ? "True " : "False", m_yRestrcted ? "True " : "False", m_rotRestrcted ? "True " : "False" ).c_str() );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, Stringf( "Alignment[4] : %s", GetStringFromAlignment( m_curAlignment ).c_str() ).c_str() );
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
	m_mainMenuCanvis.m_fillColor = Rgba( 0.3f, .3f, 0.4f );
	m_mainMenuCanvis.m_boarderColor = Rgba::LIGHT_BLUE;
	m_mainMenuCanvis.m_boarderThickness = ( SinDegrees( m_gameTime * 90.f ) + 2.0f ) * .5f;

	UILabel* textchild = m_mainMenuCanvis.CreateChild<UILabel>();
	textchild->m_pivot = Vec2::ALIGN_CENTERED;
	textchild->m_virtualPosition = Vec4( .5f, .9f, 0, 0 );
	textchild->m_virtualSize = Vec4( .40f, .40f, 0.0f, 0.0f );
	textchild->m_text = "Shape Wars";
	textchild->m_color = Rgba::LIGHT_RED;

	textchild = m_mainMenuCanvis.CreateChild<UILabel>();
	textchild->m_pivot = Vec2::ALIGN_CENTERED;
	textchild->m_virtualPosition = Vec4( .5f, .1f, 0, 0 );
	textchild->m_virtualSize = Vec4( .40f, .40f, 0.0f, 0.0f );
	textchild->m_text = "Movement Controls: W,A,S,D";
	textchild->m_color = Rgba::GREEN;

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
	
	float size = 10.0f;

	UIButton* button = m_editorCanvis.CreateChild<UIButton>();
	button->m_pivot = Vec2::ALIGN_CENTERED;
	button->m_virtualPosition = Vec4( .1f, .5f, 0, 0 );
	button->m_virtualSize = Vec4( 0.0f, 0.0f, size, size );
	button->m_hoveredColor = Rgba::YELLOW;
	button->m_nutralColor = Rgba::WHITE;
	button->m_selectedColor = Rgba::BLUE;
	button->m_useText = false;
	button->m_texturePath = "Data/Images/Terrain/Dirt_DIFFU.png";
	button->m_eventOnClick = "select";
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
	button->m_eventOnClick = "select";
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
	button->m_eventOnClick = "select";
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
	button->m_eventOnClick = "select";
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
	button->m_eventOnClick = "select";
	button->SetRadioGroup( m_editorRadGroup );

}


//--------------------------------------------------------------------------
/**
* SetUpPauseMenu
*/
void Game::SetUpPauseMenu()
{
	m_pauseMenuRadGroup = new UIRadioGroup();

	m_pauseMenuCanvis.m_pivot = Vec2::ALIGN_CENTERED;
	m_pauseMenuCanvis.m_virtualSize = Vec4( .0f, .0f, 70.f, 80.f );
	m_pauseMenuCanvis.m_virtualPosition = Vec4( .5f, .5f, 0.0f, 0.0f );
	m_pauseMenuCanvis.m_fillColor = Rgba( 0.3f, .3f, 0.4f );
	m_pauseMenuCanvis.m_boarderColor = Rgba::LIGHT_BLUE;
	m_pauseMenuCanvis.m_boarderThickness = .01f;

	UILabel* textchild = m_pauseMenuCanvis.CreateChild<UILabel>();
	textchild->m_pivot = Vec2::ALIGN_CENTERED;
	textchild->m_virtualPosition = Vec4( .5f, .9f, 0, 0 );
	textchild->m_virtualSize = Vec4( .40f, .40f, 0.0f, 0.0f );
	textchild->m_text = "PAUSED";
	textchild->m_color = Rgba::LIGHT_RED;

	UIButton* button = m_pauseMenuCanvis.CreateChild<UIButton>();
	button->m_pivot = Vec2::ALIGN_CENTERED;
	button->m_virtualPosition = Vec4( .5f, .6f, 0, 0 );
	button->m_virtualSize = Vec4( .0f, .0f, 25.f, 10.0f );
	button->m_hoveredColor = Rgba::YELLOW;
	button->m_nutralColor = Rgba::WHITE;
	button->m_selectedColor = Rgba::BLUE;
	button->m_useText = true;
	button->m_text = "Resume";
	button->m_eventOnClick = "unpause";
	button->SetRadioGroup( m_pauseMenuRadGroup );

	button = m_pauseMenuCanvis.CreateChild<UIButton>();
	button->m_pivot = Vec2::ALIGN_CENTERED;
	button->m_virtualPosition = Vec4( .5f, .4f, 0, 0 );
	button->m_virtualSize = Vec4( .0f, .0f, 25.f, 10.0f );
	button->m_hoveredColor = Rgba::YELLOW;
	button->m_nutralColor = Rgba::WHITE;
	button->m_selectedColor = Rgba::BLUE;
	button->m_useText = true;
	button->m_text = "Quit";
	button->m_eventOnClick = "quit";
	button->SetRadioGroup( m_pauseMenuRadGroup );

}

//--------------------------------------------------------------------------
/**
* UpdatePlayerPosAndCamera
*/
void Game::UpdatePlayerPosAndCamera( float deltaSeconds )
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
void Game::UpdateMainMenu()
{
	if( m_stateFrameCount == 1 )
	{
		FadeIn();
	}
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
	if( m_stateFrameCount == 1 )
	{
		FadeIn();
	}
	ASSERT_OR_DIE( index < m_maps.size() && index >= 0, Stringf( "Invalid index of: %u into the maps.", index ) );
	m_maps[index]->Update( deltaSec );
}

//--------------------------------------------------------------------------
/**
* UpdateEditor
*/
void Game::UpdateEditor( float deltaSec )
{
	if( m_stateFrameCount == 1 )
	{
		FadeIn();
	}
	if( !g_theApp->IsPaused() )
	{
		UpdateEditorUI( deltaSec );
	}

	if( m_selectedShape && m_selectedShape->IsAlive() )
	{
		m_selectedShape->m_rigidbody->SetMass( m_mass );
		m_selectedShape->m_rigidbody->SetPhyMaterial( m_restitution, m_friction, m_drag, m_angularDrag );
		m_selectedShape->m_rigidbody->SetRestrictions( m_xRestrcted, m_yRestrcted, m_rotRestrcted );
		m_selectedShape->m_rigidbody->SetAngularVelocity( m_angularVel );
	}

	
	m_cursor->Update( deltaSec );
	m_maps[0]->Update( deltaSec );
	DrawEditorValues();

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
* UpdatePauseMenu
*/
void Game::UpdatePauseMenu()
{
	m_pauseMenuCanvis.UpdateBounds( AABB2( SCREEN_WIDTH, SCREEN_HEIGHT ) );
	Vec2 mousePos = g_theGameController->GetScreenMousePos();
	Event event( Stringf("hover x=%f y=%f", mousePos.x, mousePos.y ) );
	m_pauseMenuCanvis.ProcessInput( event );
}

//--------------------------------------------------------------------------
/**
* LMouseDown
*/
void Game::LMouseDown()
{
	if(  m_state == GAMESTATE_EDITOR  )
	{
		if( !g_theApp->IsPaused() )
		{
			BeginShapeConstruction();
		}
	}

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
		if( g_theApp->IsPaused() )
		{
			m_pauseMenuCanvis.ProcessInput( event );
		}
		else
		{
			m_editorCanvis.ProcessInput( event );
			EndShapeConstruction();

		}
	}
	if( m_state == GAMESTATE_GAMEPLAY )
	{
		if( g_theApp->IsPaused() )
		{
			m_pauseMenuCanvis.ProcessInput( event );
		}
		else
		{
			// Game input
		}
	}
}

//--------------------------------------------------------------------------
/**
* RMouseDown
*/
void Game::RMouseDown()
{
	SelectShape();
}

//--------------------------------------------------------------------------
/**
* RMouseUp
*/
void Game::RMouseUp()
{
	DeselectShape();
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

	if( m_stateFrameCount == 1 )
	{
		return;
	}	

	g_theEventSystem->SubscribeEventCallbackFunction( "play", LoadToLevel );
	g_theEventSystem->SubscribeEventCallbackFunction( "save", Save );
	g_theEventSystem->SubscribeEventCallbackFunction( "load", LoadMap );


	for( uint mapIdx = 0; mapIdx < m_numLevels + 1; ++mapIdx )
	{
		m_maps.push_back( new Map( g_theRenderer ) );
	}

	initGame = true;

	m_cursor = new Cursor();

	SetupMainMenuUI();
	SetupEditorUI();
	SetUpPauseMenu();

	SwitchStates( GAMESTATE_MAINMENU );
}

//--------------------------------------------------------------------------
/**
* LoadLevel
*/
void Game::LoadLevel( unsigned int index )
{
	if( m_stateFrameCount == 1 )
	{
		return;
	}
	m_curMapIdx = index;
	ASSERT_OR_DIE( (unsigned int) m_maps.size() > index, "Bad level index" );
	
	for( Map* m : m_maps )
	{
		m->DeleteAllShapes();
	}

	if( !m_maps[index]->IsLoaded() )
	{
		m_maps[index]->Load( Stringf("Data/Saved/map%u.map", index ).c_str() );
	}
	SwitchStates( m_curMapIdx == 0 ? GAMESTATE_EDITOR : GAMESTATE_GAMEPLAY );
}

//--------------------------------------------------------------------------
/**
* LoadNextMap
*/
void Game::LoadNextMap()
{
	if( m_curMapIdx < (uint) m_maps.size() - 1 )
	{
		m_toNextLevel = true;
		SwitchStates( GAMESTATE_LOADING );
		return;
	}
	HandleQuitRequest();
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
* LoadMap
*/
bool Game::LoadMap( EventArgs& args )
{
	std::string fileName = args.GetValue( "fileName", "COULD_NOT_FIND_FILENAME" );
	std::string filePath = Stringf( "Data/Saved/%s.map", fileName.c_str() );

	return g_theGame->GetCurrentMap()->Load( filePath.c_str() );
}

//--------------------------------------------------------------------------
/**
* Save
*/
bool Game::Save( EventArgs& args )
{
	std::string fileName = args.GetValue( "fileName", "COULD_NOT_FIND_FILENAME" );
	std::string filePath = Stringf( "Data/Saved/%s.map", fileName.c_str() );

	if( fileName == "COULD_NOT_FIND_FILENAME" )
	{
		return false;
	}

	return g_theGame->GetCurrentMap()->Save( filePath.c_str() );
}

//--------------------------------------------------------------------------
/**
* UpdateStates
*/
void Game::UpdateStates()
{
	if( m_state != m_switchToState )
	{
		m_state = m_switchToState;
		m_stateFrameCount = 0;

		if( g_theApp->IsPaused() )
		{
			g_theApp->Unpause();
		}
	}
}

//--------------------------------------------------------------------------
/**
* SwitchStates
*/
void Game::SwitchStates( eGameStates state )
{
	m_switchToState = state;
}

//--------------------------------------------------------------------------
/**
* FadeIn
*/
bool Game::FadeIn()
{
	m_fadeoutStopwatch->Stop();
	if( m_fadeinStopwatch->IsStopped() )
	{
		m_fadeinStopwatch->SetAndReset( 1.0f );
	}

	if( m_fadeinStopwatch->HasElapsed() )
	{
		m_fadeinStopwatch->Stop();
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
/**
* FadeOut
*/
bool Game::FadeOut()
{
	if( !m_fadeinStopwatch->IsStopped() )
	{
		return false;
	}
	if( m_fadeoutStopwatch->IsStopped() )
	{
		m_fadeoutStopwatch->SetAndReset( 1.0f );
	}
	
	if( m_fadeoutStopwatch->HasElapsed() )
	{
		m_fadeoutStopwatch->Stop();
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
/**
* RenderFade
*/
void Game::RenderFade() const
{
	if( !m_fadeoutStopwatch->IsStopped() )
	{
		float fadeVal = Clamp( m_fadeoutStopwatch->GetNormalizedElapsedTime(), 0.0f, 1.0f );
		matrixEffect matVals;
		matVals.STRENGTH = fadeVal;
		matVals.mat.m_values[Matrix44::Ix] = 0.0f;
		matVals.mat.m_values[Matrix44::Jy] = 0.0f;
		matVals.mat.m_values[Matrix44::Kz] = 0.0f;
		Material* mat = g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/tonemap.mat" );
		mat->SetUniforms( &matVals, sizeof( matVals ) );
		g_theRenderer->ApplyEffect( g_theRenderer->GetScratchColorTargetView(), g_theRenderer->GetRenderTargetTextureView(), mat );
		g_theRenderer->CopyTexture( g_theRenderer->GetBufferTexture(), g_theRenderer->GetScratchBuffer() );
		if( m_fadeoutStopwatch->HasElapsed() )
		{
			m_fadeoutStopwatch->Stop();
		}
	}
	if( !m_fadeinStopwatch->IsStopped() )
	{
		float fadeVal = Clamp( 1.0f - m_fadeinStopwatch->GetNormalizedElapsedTime(), 0.0f, 1.0f );
		matrixEffect matVals;
		matVals.STRENGTH = fadeVal;
		matVals.mat.m_values[Matrix44::Ix] = 0.0f;
		matVals.mat.m_values[Matrix44::Jy] = 0.0f;
		matVals.mat.m_values[Matrix44::Kz] = 0.0f;
		Material* mat = g_theRenderer->CreateOrGetMaterialFromXML( "Data/Materials/tonemap.mat" );
		mat->SetUniforms( &matVals, sizeof( matVals ) );
		g_theRenderer->ApplyEffect( g_theRenderer->GetScratchColorTargetView(), g_theRenderer->GetRenderTargetTextureView(), mat );
		g_theRenderer->CopyTexture( g_theRenderer->GetBufferTexture(), g_theRenderer->GetScratchBuffer() );
		if( m_fadeinStopwatch->HasElapsed() )
		{
			m_fadeinStopwatch->Stop();
		}
	}
	
}
