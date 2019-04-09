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
	m_terrainMaterial = context->CreateOrGetMaterialFromXML( "Data/Materials/couch.mat" );
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
	CPUMeshAddPlain( &plane, GetXYBounds() );
	m_terrainMesh = new MeshGPU( g_theRenderer );
	m_terrainMesh->CreateFromCPUMesh<Vertex_LIT>( &plane );
}
