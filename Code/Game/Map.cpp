#include "Game/Map.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/MeshCPU.hpp"
#include "Engine/Renderer/MeshGPU.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex/Vertex_LIT.hpp"

//--------------------------------------------------------------------------
/**
* Map
*/
Map::Map( RenderContext* context )
{
	m_renderContext = context;
	m_terrainMaterial = context->CreateOrGetMaterialFromXML( "Data/Materials/default_lit.mat" );
}

//--------------------------------------------------------------------------
/**
* ~Map
*/
Map::~Map()
{
	SAFE_DELETE( m_terrainMesh );
}

//--------------------------------------------------------------------------
/**
* Load
*/
bool Map::Load( char const *filename )
{
	UNUSED(filename);
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
void Map::Update()
{

}

//--------------------------------------------------------------------------
/**
* Render
*/
void Map::Render() const
{
	RenderTerrain();
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
