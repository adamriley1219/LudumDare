#pragma once
#include "Engine/Math/IntVec2.hpp"

//--------------------------------------------------------------------------

class RenderContext;
struct AABB2;
struct IntVec2;
struct Vertex_LIT;
class Material;
class MeshGPU;

//--------------------------------------------------------------------------

struct MapTile
{
	int placeholder;
};

//--------------------------------------------------------------------------

class Map
{
public:
	Map( RenderContext* context );
	~Map();

public:
	bool Load( char const *filename );            
	bool Create( int tileWidth, int tileHeight ); 

	void Update(); 
	void Render() const; 

public:
	AABB2 GetXYBounds() const; 

private:
	void RenderTerrain( Material* matOverride = nullptr ) const; 															
	void GenerateTerrainMesh(); 

	int GetVertIndex( int x, int y );

private: 
	IntVec2 m_tileDimensions; 
	IntVec2 m_vertDimensions; 

	MapTile* m_tiles        = nullptr;  
	Vertex_LIT* m_vertices   = nullptr;  

	MeshGPU* m_terrainMesh; 
	Material* m_terrainMaterial; 

	RenderContext* m_renderContext = nullptr;
}; 