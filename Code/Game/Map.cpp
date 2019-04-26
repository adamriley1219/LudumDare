#include "Game/Map.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/MeshCPU.hpp"
#include "Engine/Renderer/MeshGPU.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex/Vertex_LIT.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Shaders/Shader.hpp"
#include "Engine/Renderer/Model.hpp"
#include "Engine/Renderer/Debug/DebugRenderSystem.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/RTSCamera.hpp"
#include "Game/GameController.hpp"

//--------------------------------------------------------------------------
/**
* Map
*/
Map::Map( RenderContext* context )
{
	m_renderContext = context;
	m_terrainMaterial = context->CreateOrGetMaterialFromXML( "Data/Materials/default_lit.mat" );
	m_camera = new RTSCamera();
	m_camera->SetColorTargetView( context->GetColorTargetView() );
	m_camera->SetDepthTargetView( context->GetDepthTargetView() );
}

//--------------------------------------------------------------------------
/**
* ~Map
*/
Map::~Map()
{
	SAFE_DELETE( m_terrainMesh );
	SAFE_DELETE( m_camera );
}

//--------------------------------------------------------------------------
/**
* Load
*/
bool Map::Load( char const *filename )
{
	UNUSED(filename);
	Model model( g_theRenderer, "building/towncenter" );
	Model model2( g_theRenderer, "test/test" );
	Model model3( g_theRenderer, "SciFiFighterMk6/SciFi_Fighter-MK6-spec" );

	m_camera->SetFocalPoint( Vec3( 32.0f, 32.0f, 0.0f ) );
	m_hasLoaded = true;
	return Create( 64, 64 );
}

//--------------------------------------------------------------------------
/**
* Create
*/
bool Map::Create( int tileWidth, int tileHeight )
{
	m_tileDimensions = IntVec2( tileWidth, tileHeight );
	m_vertDimensions = IntVec2( 2 * tileWidth + 1, 2 * tileHeight + 1 );
	GenerateTerrainMesh();
	return true;
}

//--------------------------------------------------------------------------
/**
* Update
*/
void Map::Update( float deltaSec )
{
	UpdateCamera( deltaSec );
 	Vec3 mousePos = g_theGameController->GetWorldMousePos();
 	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, "Mouse World Pos: %f, %f, %f", mousePos.x, mousePos.y, mousePos.z );
}

//--------------------------------------------------------------------------
/**
* Render
*/
void Map::Render() const
{
	RenderTerrain();
	Model model( g_theRenderer, "building/towncenter" );
	Model model2( g_theRenderer, "test/test" );
	Model model3( g_theRenderer, "SciFiFighterMk6/SciFi_Fighter-MK6-spec" );

	Vec3 mousePos = g_theGameController->GetWorldMousePos();
	model.m_model = Matrix44::MakeTranslation3D( Vec3( m_tileDimensions.x * .5f, m_tileDimensions.y * .5f, 0.0f ) );
	model2.m_model = Matrix44::MakeTranslation3D( Vec3( mousePos.x, mousePos.y, 0.0f ) );
	model3.m_model = Matrix44::MakeTranslation3D( Vec3( m_tileDimensions.x * .5f - 10.f, m_tileDimensions.y * .5f, 0.0f ) ).Concatenate( Matrix44::MakeZRotationDegrees( (float) g_theApp->m_gameClock->GetTotalTime() * 10.0f ) );

	g_theRenderer->DrawModel( &model );
	g_theRenderer->DrawModel( &model2 );
	g_theRenderer->DrawModel( &model3 );

}

//--------------------------------------------------------------------------
/**
* IsLoaded
*/
bool Map::IsLoaded() const
{
	return m_hasLoaded;
}

//--------------------------------------------------------------------------
/**
* GetXYBounds
*/
AABB2 Map::GetXYBounds() const
{
	return AABB2( (float) m_tileDimensions.x, (float) m_tileDimensions.y, Vec2( (float) m_tileDimensions.x * .5f, (float) m_tileDimensions.y * .5f ) );
}

//--------------------------------------------------------------------------
/**
* RenderTerrain
*/
void Map::RenderTerrain( Material* matOverride /*= nullptr */ ) const
{
	if( matOverride )
	{
		m_renderContext->BindMaterial( matOverride );
	}
	else
	{
		m_renderContext->BindMaterial( m_terrainMaterial );
	}
	m_renderContext->DrawMesh( m_terrainMesh );
}

//--------------------------------------------------------------------------
/**
* GenerateTerrainMesh
*/
void Map::GenerateTerrainMesh()
{
	MeshCPU plane;
	AABB2 bounds = GetXYBounds();

	// Push all verts into plane.
	float v = (float) m_vertDimensions.y;
	for( unsigned int hIdx = 0; hIdx < (unsigned int) m_vertDimensions.y; ++hIdx )
	{
		float u = 0.0f;
		for( unsigned int wInt = 0; wInt < (unsigned int) m_vertDimensions.x; ++wInt )
		{
			VertexMaster vertToAdd;
			
			vertToAdd.position	= Vec3( -.5f + (float) wInt * .5f, -.5f + (float) hIdx * .5f, 0.0f );
			vertToAdd.normal	= Vec3( 0.0f, 0.0f, -1.0f );
			vertToAdd.tangent	= Vec4( 1.0f, 0.0f, 0.0f, 1.0f );			
			vertToAdd.uv		= Vec2( u, v );

			plane.AddVertex( vertToAdd );
			u += .5f;
		}
		v -= .5f;
	}

	for( unsigned int hIdx = 0; hIdx < (unsigned int) m_tileDimensions.y * 2; ++hIdx )
	{
		for( unsigned int wInt = 0; wInt < (unsigned int) m_tileDimensions.x * 2; ++wInt )
		{
			plane.AddIndexedTriangle( GetVertIndex( wInt, hIdx ), GetVertIndex( wInt + 1, hIdx ), GetVertIndex( wInt + 1, hIdx + 1 ) );
			plane.AddIndexedTriangle( GetVertIndex( wInt, hIdx ), GetVertIndex( wInt + 1, hIdx + 1 ), GetVertIndex( wInt, hIdx + 1 ) );
		}
	}

	m_terrainMesh = new MeshGPU( g_theRenderer );
	m_terrainMesh->CreateFromCPUMesh<Vertex_LIT>( &plane );
}

//--------------------------------------------------------------------------
/**
* GetVertIndex
*/
int Map::GetVertIndex( int x, int y )
{
	return x + y * m_vertDimensions.x;
}

//--------------------------------------------------------------------------
/**
* UpdateCamera
*/
void Map::UpdateCamera( float deltaSec )
{
	// calc focus point
	Vec3 curPos = m_camera->m_focusPoint;
	Vec2 movement = g_theGameController->GetFramePan() * deltaSec;
	Vec3 right = m_camera->GetRight();
	Vec3 forward = m_camera->GetForward();
	Vec3 flatForward = Vec3( forward.x, forward.y, 0.0f );
	flatForward.Normalize();

	m_camera->SetFocalPoint( Clamp( flatForward * movement.y + right * movement.x + curPos, Vec3( -.5f, -.5f, 0.0f ), Vec3( m_tileDimensions.x - .5f, m_tileDimensions.y - .5f, 0.0f ) ) );
	

	// calc rotation
	m_camera->SetAngle( g_theGameController->GetFrameRotation() );

	// calc height
	m_camera->SetZoom( g_theGameController->GetFrameZoom() );

	m_camera->Update( deltaSec );
	m_camera->BindCamera( m_renderContext );
	//DebugRenderPoint( 0.0f, DEBUG_RENDER_ALWAYS, m_camera->m_focusPoint, Rgba::RED, Rgba::RED, 0.1f );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, "LookAt: %.02f,%.02f,%.02f", m_camera->m_focusPoint.x,  m_camera->m_focusPoint.y,  m_camera->m_focusPoint.z );

}
