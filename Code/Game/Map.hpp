#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Game/RTSCamera.hpp"

//--------------------------------------------------------------------------

class RenderContext;
struct AABB2;
struct IntVec2;
struct Vertex_LIT;
class Material;
class MeshGPU;
class RTSCamera;

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

	void Update( float deltaSec ); 
	void Render() const; 

public:
	bool IsLoaded() const;

public:
	AABB2 GetXYBounds() const; 
	RTSCamera* GetCamera() { return m_camera; }

private:
	void RenderTerrain( Material* matOverride = nullptr ) const; 															
	void GenerateTerrainMesh(); 

	int GetVertIndex( int x, int y );

private:
	void UpdateCamera( float deltaSec );

private: 
	IntVec2 m_tileDimensions; 
	IntVec2 m_vertDimensions; 

	MapTile* m_tiles        = nullptr;  
	Vertex_LIT* m_vertices   = nullptr;  

	MeshGPU* m_terrainMesh = nullptr; 
	Material* m_terrainMaterial = nullptr;  

	RTSCamera* m_camera = nullptr;

	RenderContext* m_renderContext = nullptr;

private:
	bool m_hasLoaded = false;

}; 