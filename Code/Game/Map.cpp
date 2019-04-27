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
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Game/App.hpp"
#include "Game/Shapes/Shape.hpp"
#include "Game/Shapes/Pill.hpp"
#include "Game/GameCommon.hpp"
#include "Game/FollowCamera2D.hpp"
#include "Game/GameController.hpp"

//--------------------------------------------------------------------------
/**
* Map
*/
Map::Map( RenderContext* context )
{
	m_renderContext = context;
	m_terrainMaterial = context->CreateOrGetMaterialFromXML( "Data/Materials/default_lit.mat" );
	m_camera = new FollowCamera2D();
	m_camera->SetColorTargetView( context->GetColorTargetView() );
	m_camera->SetDepthTargetView( context->GetDepthTargetView() );
	m_player = new Pill( Transform2D( Vec2::ZERO ), PHYSICS_SIM_DYNAMIC, 0.0f, 0.0f, 1.0f );
	AddShape( m_player );
}

//--------------------------------------------------------------------------
/**
* ~Map
*/
Map::~Map()
{
	SAFE_DELETE( m_terrainMesh );
	SAFE_DELETE( m_camera );
	DeleteAllShapes();
}

//--------------------------------------------------------------------------
/**
* Load
*/
bool Map::Load( char const *filename )
{
	tinyxml2::XMLDocument config;
	config.LoadFile( filename );
	XmlElement* root = config.RootElement();

	if( root )
	{
		DeleteAllShapes();

		for( XmlElement* xmlShape = root->FirstChildElement( "shape" ); xmlShape != NULL; xmlShape = xmlShape->NextSiblingElement( "shape" ) )
		{
			XmlElement* rbEle = xmlShape->FirstChildElement( "rigidbody" );

			float restitution		= ParseXmlAttribute( *rbEle, "restitution", 0.f );
			float friction			= ParseXmlAttribute( *rbEle, "friction", 0.2f );
			float mass				= ParseXmlAttribute( *rbEle, "mass", 1.0f );
			float angularDrag		= ParseXmlAttribute( *rbEle, "angularDrag", 0.0f );
			float drag				= ParseXmlAttribute( *rbEle, "drag", 0.5f );


			std::string xRestrcted			= ParseXmlAttribute( *rbEle, "xRestrcted", "false" );
			std::string yRestrcted			= ParseXmlAttribute( *rbEle, "yRestrcted", "false" );
			std::string rotRestrcted		= ParseXmlAttribute( *rbEle, "rotRestrcted", "false" );

			std::string type		= ParseXmlAttribute( *rbEle, "type", "static" );

			XmlElement* colEle = xmlShape->FirstChildElement( "collider" );

			float radius	= ParseXmlAttribute( *colEle, "radius", 1.0f );
			Vec2 exstents	= ParseXmlAttribute( *colEle, "extents", Vec2::ONE );
			Vec2 locCanter	= ParseXmlAttribute( *colEle, "locCenter", Vec2::ZERO );
			Vec2 locRight	= ParseXmlAttribute( *colEle, "locRight", Vec2::RIGHT );

			XmlElement* transEle = xmlShape->FirstChildElement( "trans" );

			Vec2 pos	= ParseXmlAttribute( *transEle, "pos", Vec2::ZERO );
			Vec2 scale	= ParseXmlAttribute( *transEle, "scale", Vec2::ZERO );
			float rot	= ParseXmlAttribute( *transEle, "rot", 0.0f );

			Transform2D trans( pos, rot, scale );
			Shape* shape = new Pill( trans, type == "dynamic" ? PHYSICS_SIM_DYNAMIC : PHYSICS_SIM_STATIC, exstents.x * 2.0f, exstents.y * 2.0f, radius, mass, restitution, friction, drag, angularDrag );
			shape->m_rigidbody->SetRestrictions( xRestrcted == "true", yRestrcted == "true", rotRestrcted == "true" );
			AddShape( shape );

		}



	}
	m_camera->SetFocalPoint( Vec2( 32.0f, 32.0f ) );
	m_hasLoaded = true;
	return Create( 64, 64 );
}

//--------------------------------------------------------------------------
/**
* Save
*/
bool Map::Save( const char* filePath )
{
	tinyxml2::XMLDocument config;
	tinyxml2::XMLElement* root = config.NewElement("map");
	config.InsertFirstChild( root );

	root->SetAttribute( "mapDims", Stringf( "%f,%f", WORLD_WIDTH, WORLD_HEIGHT ).c_str() );

	for( unsigned int shapeIdx = 0; shapeIdx < ( unsigned int ) m_shapes.size(); ++shapeIdx )
	{
		Shape* shape = m_shapes[shapeIdx];
		if( shape )
		{
			tinyxml2::XMLElement* shapeEle = config.NewElement( "shape" );
			root->InsertFirstChild( shapeEle );


			tinyxml2::XMLElement* shapeRBEle = shape->m_rigidbody->GetAsXMLElement( &config );
			shapeEle->InsertFirstChild( shapeRBEle );

			tinyxml2::XMLElement* shapeColEle = shape->m_collider->GetAsXMLElemnt( &config );
			shapeEle->InsertFirstChild( shapeColEle );

			tinyxml2::XMLElement* shapeTransEle = config.NewElement( "trans" );
			shapeEle->InsertFirstChild( shapeTransEle );

			shapeTransEle->SetAttribute( "pos", Stringf( "%f,%f", shape->m_transform.m_position.x, shape->m_transform.m_position.y ).c_str() );
			shapeTransEle->SetAttribute( "scale", Stringf( "%f,%f", shape->m_transform.m_scale.x, shape->m_transform.m_scale.y ).c_str() );
			shapeTransEle->SetAttribute( "rot", Stringf( "%f", shape->m_transform.m_rotation ).c_str() );
		}
	}

	return config.SaveFile( filePath ) == tinyxml2::XML_SUCCESS;
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

//------------------------	--------------------------------------------------
/**
* Update
*/
void Map::Update( float deltaSec )
{
	UpdateCamera( deltaSec );
 	Vec3 mousePos = g_theGameController->GetWorldMousePos();
 	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, "Mouse World Pos: %f, %f, %f", mousePos.x, mousePos.y, mousePos.z );
	for( Shape* s : m_shapes )
	{
		if( s )
		{
			s->Update( deltaSec );
		}
	}
	GarbageCollection();
}

//--------------------------------------------------------------------------
/**
* Render
*/
void Map::Render() const
{
	RenderTerrain();
	for( Shape* s : m_shapes )
	{
		if( s )
		{
			s->Render();
		}
	}

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
			
			vertToAdd.position	= Vec3( -.5f + (float) wInt * .5f, -.5f + (float) hIdx * .5f, .1f );
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
* GarbageCollection
*/
void Map::GarbageCollection()
{
	for( Shape*& s : m_shapes )
	{
		if( s && s->m_isGarbage )
		{
			if( s == g_theGame->m_selectedShape )
			{
				g_theGame->DeselectShape();
			}
			RemoveShape( s );
		}
	}
}

//--------------------------------------------------------------------------
/**
* DeleteAllShapes
*/
void Map::DeleteAllShapes()
{
	for( Shape* s: m_shapes )
	{
		if( s )
		{
			SAFE_DELETE(s);
		}
	}
	m_shapes.clear();
}

//--------------------------------------------------------------------------
/**
* AddShape
*/
void Map::AddShape( Shape* shape )
{
	for( Shape*& s : m_shapes )
	{
		if( !s )
		{
			s = shape;
			return;
		}
	}
	m_shapes.push_back( shape );
}

//--------------------------------------------------------------------------
/**
* RemoveShape
*/
void Map::RemoveShape( Shape* shape )
{
	for( Shape*& s : m_shapes )
	{
		if( s && shape == s )
		{
			delete s;
			s = nullptr;
			return;
		}
	}
}

//--------------------------------------------------------------------------
/**
* GetNumShapes
*/
uint Map::GetNumShapes() const
{
	return (uint) m_shapes.size();
}

//--------------------------------------------------------------------------
/**
* UpdateCamera
*/
void Map::UpdateCamera( float deltaSec )
{
	// calc focus point
	Vec2 curPos = m_camera->m_focusPoint;
	Vec2 movement = g_theGameController->GetFramePan() * deltaSec;
	Vec3 right = m_camera->GetRight();
	Vec3 forward = m_camera->GetForward();
	Vec3 flatForward = Vec3( forward.x, forward.y, 0.0f );
	flatForward.Normalize();

	m_camera->SetFocalPoint( Vec2( curPos.x + movement.x, curPos.y + movement.y ) );
	
	m_camera->SetZoom( g_theGameController->GetFrameZoom() );

	m_camera->Update( deltaSec );
	m_camera->BindCamera( m_renderContext );
	//DebugRenderPoint( 0.0f, DEBUG_RENDER_ALWAYS, m_camera->m_focusPoint, Rgba::RED, Rgba::RED, 0.1f );
	DebugRenderMessage( 0.0f, Rgba::WHITE, Rgba::WHITE, "LookAt: %.02f,%.02f", m_camera->m_focusPoint.x,  m_camera->m_focusPoint.y );

}
