#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include <vector>

#include <Math.h>
//--------------------------------------------------------------------------
/**
* Game
*/
Game::Game()
{
	ConstructGame();
}

//--------------------------------------------------------------------------
/**
* ~Game
*/
Game::~Game()
{
	DeconstructGame();
}

//--------------------------------------------------------------------------
/**
* GameStartup
*/
void Game::Startup()
{

}

//--------------------------------------------------------------------------
/**
* Shutdown
*/
void Game::Shutdown()
{

}

//--------------------------------------------------------------------------
/**
* ConstructGame
*/
void Game::ConstructGame()
{
}

//--------------------------------------------------------------------------
/**
* DeconstructGame
*/
void Game::DeconstructGame()
{
}
static int g_index = 0;

//--------------------------------------------------------------------------
/**
* HandleKeyPressed
*/
bool Game::HandleKeyPressed( unsigned char keyCode )
{
	UNUSED(keyCode);
	if( keyCode == 'O' )
	{
		g_index = ++g_index % ( 8 * 2 );
	}
	if( keyCode == 'W' )
	{
		EventArgs args;
		g_theEventSystem->FireEvent( "test" );
		g_theEventSystem->FireEvent( "test", args );
		g_theConsole->PrintString( args.GetValue( "test1", "NOT FOUND" ), DevConsole::CONSOLE_INFO );
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


BitmapFont* g_testBitmap = nullptr;
float g_pingPongTimer = 0.0f;
int g_printGlyphCount = 0;
float g_charTimer = 0.0f;

//--------------------------------------------------------------------------
/**
* GameRender
*/
void Game::GameRender() const
{

	static Texture* testTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	
 	std::vector<Vertex_PCU> verts;
	g_theRenderer->BindTexture( testTexture );
 	AddVertsForAABB2D( verts, AABB2( 10.f, 10.f, 100.f, 80.f ), Rgba( 1.f, 1.f, 1.f ), Vec2( 0.0f, 0.0f ), Vec2( 1.0f, 1.0f ) );
 	g_theRenderer->DrawVertexArray( (int) verts.size(), &verts[0] );
	verts.clear();

	g_theRenderer->BindTexture( nullptr );
	AddVertsForLine2D( verts, Vec2( 10.0f, 90.0f ), Vec2( 150.0f, 10.0f ), 1.0f, Rgba( 1.0f, 0.1f, 0.1f ) );
	AddVertsForDisc2D( verts, Vec2( 150.0f, 70.0f ), 20.0f, Rgba( 0.1f, 0.1f, 1.0f ) );
	AddVertsForRing2D( verts, Vec2( 120.0f, 30.0f ), 15.0f, 3.0f, Rgba( 1.0f, 1.0f, 1.0f ), 5 );
	AddVertsForRing2D( verts, Vec2( 145.0f, 30.0f ), 10.0f, 1.0f, Rgba( 0.4f, 1.0f, 0.4f ) );

 	g_theRenderer->DrawVertexArray( (int) verts.size(), &verts[0] );


	// Text of spriteSheets
	static Texture* testTexture2 = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_SpriteSheet8x2.png" );
	
	SpriteSheet spriteSheet( testTexture2, IntVec2( 8, 2 ) );
	Vec2 uvAtBottomLeft = Vec2(0.0f, 0.0f);
	Vec2 uvAtTopRight = Vec2(1.0f, 1.0f);
	SpriteDefinition sd = spriteSheet.GetSpriteDefinition( g_index );
	sd.GetUVs(uvAtBottomLeft, uvAtTopRight);
	std::vector<Vertex_PCU> ssVerts;
	AddVertsForAABB2D(ssVerts, AABB2( 85.f, 80.f, 90.f, 90 ), Rgba( 1.0f, 1.0f, 1.0f ), uvAtBottomLeft, uvAtTopRight );

	g_theRenderer->BindTexture( spriteSheet.GetTexture() );
	g_theRenderer->DrawVertexArray( ssVerts );

	// Bitmap Test
	if( !g_testBitmap )
	{
		g_testBitmap = g_theRenderer->CreateOrGetBitmapFromFile( "SquirrelFixedFont" );
	}

	std::vector<Vertex_PCU> bmVerts;
	g_testBitmap->AddVertsFor2DText( bmVerts, Vec2( 110.0f, 50.0f ), 5.0f, "HELLO, WORLD", .5f);

	g_theRenderer->BindTexture( g_testBitmap->GetTexture() );
	g_theRenderer->DrawVertexArray( bmVerts );


	SpriteAnimDefinition spriteAnimDef( spriteSheet, 0, 15, 30.0f, SPRITE_ANIM_PLAYBACK_PINGPONG );
	uvAtBottomLeft = Vec2(0.0f, 0.0f);
	uvAtTopRight = Vec2(1.0f, 1.0f);
	sd = spriteAnimDef.GetSpriteDefAtTime( g_theApp->GetGlobleTime() ); 
	sd.GetUVs(uvAtBottomLeft, uvAtTopRight);
	ssVerts.clear();
	AddVertsForAABB2D(ssVerts, AABB2( 95.f, 80.f, 100.f, 90 ), Rgba( 1.0f, 1.0f, 1.0f ), uvAtBottomLeft, uvAtTopRight );

	g_theRenderer->BindTexture( spriteSheet.GetTexture() );
	g_theRenderer->DrawVertexArray( ssVerts );

	float x = SinDegrees( g_theApp->GetGlobleTime() * 360.f / 3.7f + 17.0f ) + .50f;
	x = x > 0.0f ? x : 0.0f;
	x = x < 1.0f ? x : 1.0f;
	float y = CosDegrees( g_theApp->GetGlobleTime() * 360.0f / 4.0f ) + .50f;
	y = y > 0.0f ? y : 0.0f;
	y = y < 1.0f ? y : 1.0f;
	ssVerts.clear();
	AABB2 box( 25.0f + x * 30.0f, 8.0f + y * 10.0f , Vec2(WORLD_WIDTH/2.0f + 60.0f, WORLD_HEIGHT/2.0f - 20.0f) );
	AABB2 box2( 23.0f + x * 30.0f, 6.0f + y * 10.0f, Vec2(WORLD_WIDTH/2.0f + 60.0f, WORLD_HEIGHT/2.0f - 20.0f) );
	AddVertsForAABB2D( ssVerts, box, Rgba::CYAN, Vec2::ZERO, Vec2::ONE );
	AddVertsForAABB2D( ssVerts, box2, Rgba::BLUE, Vec2::ZERO, Vec2::ONE );
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawVertexArray(ssVerts);

	ssVerts.clear();
	Vec2 alignment( x , y );
	g_testBitmap->AddVertsFor2DTextAlignedInBox( ssVerts, g_pingPongTimer * 1.5f, "Hello!\ngood to go", box2, Vec2::ALIGN_CENTERED, UNCHANGED, 1.0f, Rgba( 1.0f, 0.0f, 0.0f, 0.5f ), g_printGlyphCount );
	g_testBitmap->AddVertsFor2DTextAlignedInBox( ssVerts, g_pingPongTimer * 1.5f, "Hello!\ngood to go", box2, Vec2::ALIGN_CENTERED, SHRINK_TO_FIT, 1.0f, Rgba::BLACK, g_printGlyphCount );
 	g_testBitmap->AddVertsFor2DTextAlignedInBox( ssVerts, 1.0f, "This is the\nTest for\nAlignment", box2, alignment, SHRINK_TO_FIT, 1.0f, Rgba::MAGENTA );
	g_theRenderer->BindTexture( g_testBitmap->GetTexture() );
	g_theRenderer->DrawVertexArray( ssVerts );

	// Debug
	if( g_isInDebug )
	{
		RenderDebug();
	}
}

//--------------------------------------------------------------------------
/**
* UpdateGame
*/
void Game::UpdateGame( float deltaSeconds )
{
	UpdateShakeReduction( deltaSeconds );
	static bool swapper = true;
	if( g_pingPongTimer < 0.0f || g_pingPongTimer > 7.0f )
	{
		swapper = !swapper;
		if( g_pingPongTimer < 0.0f )
			g_pingPongTimer = 0.0f;
		else
			g_pingPongTimer = 7.0f;
	}
	if( swapper )
		g_pingPongTimer += deltaSeconds;
	else
		g_pingPongTimer -= deltaSeconds;

	g_charTimer += deltaSeconds;
	g_printGlyphCount = (int) g_charTimer;
	if( g_charTimer > 30.0f )
	{
		g_charTimer = 0.0f;
	}
}


//--------------------------------------------------------------------------
/**
* GameRenderRenderDebug
*/
void Game::RenderDebug() const
{
	g_theRenderer->BindTexture( nullptr );
	RenderDebugCosmetics();
	RenderDebugPhysics();
}

//--------------------------------------------------------------------------
/**
* RenderDebugCosmetics
*/
void Game::RenderDebugCosmetics() const
{

}

//--------------------------------------------------------------------------
/**
* RenderDebugPhysics
*/
void Game::RenderDebugPhysics() const
{
	
}


//--------------------------------------------------------------------------
/**
* ShakeScreen
*/
void Game::ShakeScreen( float shake )
{
	m_screenShakeIntensity = shake;
}

//--------------------------------------------------------------------------
/**
* ResetGame
*/
void Game::ResetGame()
{
	DeconstructGame();
	ConstructGame();
}

//--------------------------------------------------------------------------
/**
* UpdateGameHandleShakeReduction
*/
void Game::UpdateShakeReduction( float deltaSeconds )
{
	// Shake Logic - NOTE: Shake will occur on death of player. No other logic after shake update.
	m_screenShakeIntensity -= m_shakeSpeedReduction * deltaSeconds;
	if( m_screenShakeIntensity < 0.0f )
		m_screenShakeIntensity = 0.0f;
}

