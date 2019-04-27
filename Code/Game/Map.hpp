#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Game/FollowCamera2D.hpp"
#include <vector>

//--------------------------------------------------------------------------

class RenderContext;
struct AABB2;
struct IntVec2;
struct Vertex_LIT;
class Material;
class MeshGPU;
class FollowCamera2D;
class Shape;
class Game;

//--------------------------------------------------------------------------

struct MapTile
{
	int placeholder;
};

//--------------------------------------------------------------------------

class Map
{
	friend class Game;
	friend class Cursor;

public:
	Map( RenderContext* context );
	~Map();

public:
	bool Load( char const *filename );  
	bool Save( const char* filePath );
	bool Create( int tileWidth, int tileHeight ); 

	void Update( float deltaSec ); 
	void Render() const; 

public:
	bool IsLoaded() const;

public:
	AABB2 GetXYBounds() const; 
	FollowCamera2D* GetCamera() { return m_camera; }

private:
	void RenderTerrain( Material* matOverride = nullptr ) const; 															
	void GenerateTerrainMesh(); 
	int GetVertIndex( int x, int y );

private:
	void GarbageCollection();


private:
	void DeleteAllShapes();
	void AddShape( Shape* shape );
	void RemoveShape( Shape* shape );
	uint GetNumShapes() const;

private:
	void UpdateCamera( float deltaSec );

private:
	// Gameplay
	std::vector<Shape*> m_shapes;
	Shape* m_player;
	Shape* m_endZone;
	float m_camScale = 1.0f;

private: 
	IntVec2 m_tileDimensions; 
	IntVec2 m_vertDimensions; 

	MapTile* m_tiles        = nullptr;  
	Vertex_LIT* m_vertices   = nullptr;  

	MeshGPU* m_terrainMesh = nullptr; 
	Material* m_terrainMaterial = nullptr;  

	FollowCamera2D* m_camera = nullptr;

	RenderContext* m_renderContext = nullptr;

private:
	bool m_hasLoaded = false;

}; 