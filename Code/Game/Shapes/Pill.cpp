#include "Game/Shapes/Pill.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/RNG.hpp"
#include "Engine/Physics/Transform2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/PillboxCollider2D.hpp"
#include "Engine/Math/AABB2.hpp"

//--------------------------------------------------------------------------
/**
* Box
*/
Pill::Pill( const Transform2D& spawnLoaction , ePhysicsSimulationType simType
	, float width /* = 1.0f */, float height /*= 1.0f*/, float radius /*= 1.0f*/
	, float mass /*= 1.0f*/, float restitution /*= 1.0f*/, float friction /*= 1.0f*/, float drag /*= 0.0f*/, float angularDrag /*= 0.0f*/ )
	: Shape( spawnLoaction, simType )
{
	// give it a shape 
	m_collider = m_rigidbody->SetCollider( new PillboxCollider2D( Pillbox2( Vec2( 0.0f, 0.0f ), radius, Vec2( width * .5f, height * .5f ) ) ) );  
	m_width = width;
	m_height = height; 
	m_rigidbody->SetMass( mass );
	m_rigidbody->SetPhyMaterial( restitution, friction, drag, angularDrag );
	m_radius = radius;
}

//--------------------------------------------------------------------------
// Helper
static void AddVertsForTrimmedLine2D( std::vector<Vertex_PCU>& addToVerts, const Vec2& start, const Vec2& end, float thickness, const Rgba& color )
{
	float halfThickness = thickness / 2.0f;
	Vec2 displacement = end - start;
	displacement.SetLength( halfThickness );
	Vec2 rotated90Displacement = displacement.GetRotated90Degrees();

	Vec2 frontLeft	= end	 + rotated90Displacement;
	Vec2 frontRight = end	 - rotated90Displacement;
	Vec2 backLeft	= start  + rotated90Displacement;
	Vec2 backRight	= start  - rotated90Displacement;

	Vec3 pointBL_pos( backLeft.x	, backLeft.y	, 0.0f );
	Vec3 pointBR_pos( backRight.x	, backRight.y	, 0.0f );
	Vec3 pointTR_pos( frontRight.x	, frontRight.y	, 0.0f );
	Vec3 pointTL_pos( frontLeft.x	, frontLeft.y	, 0.0f );

	Rgba pointBL_color = color;
	Rgba pointBR_color = color;
	Rgba pointTR_color = color;
	Rgba pointTL_color = color;

	Vec2 pointBL_UVs = Vec2( 0.0f, 0.0f );
	Vec2 pointBR_UVs = Vec2( 0.0f, 0.0f );
	Vec2 pointTR_UVs = Vec2( 0.0f, 0.0f );
	Vec2 pointTL_UVs = Vec2( 0.0f, 0.0f );

	Vertex_PCU pointBL( pointBL_pos, pointBL_color, pointBL_UVs );
	Vertex_PCU pointBR( pointBR_pos, pointBR_color, pointBR_UVs );
	Vertex_PCU pointTR( pointTR_pos, pointTR_color, pointTR_UVs );
	Vertex_PCU pointTL( pointTL_pos, pointTL_color, pointTL_UVs );

	addToVerts.push_back( pointBL );
	addToVerts.push_back( pointBR );
	addToVerts.push_back( pointTR );

	addToVerts.push_back( pointBL );
	addToVerts.push_back( pointTR );
	addToVerts.push_back( pointTL );
}

//--------------------------------------------------------------------------
/**
* Render
*/
void Pill::Render() const
{
	g_theRenderer->BindTextureView( 0, nullptr );

	Rgba color = m_color;

	std::vector<Vertex_PCU> verts;
	const Pillbox2 pill( Vec2( 0.0f, 0.0f ), m_radius, Vec2( m_width * .5f, m_height * 0.5f ), m_transform.m_rotation );

	Vec2 BL = pill.m_obb.m_center - pill.m_obb.m_extents.x * pill.m_obb.GetRight() - pill.m_obb.GetUp() * pill.m_obb.m_extents.y + m_transform.m_position;
	Vec2 TR = pill.m_obb.m_center + pill.m_obb.m_extents.x * pill.m_obb.GetRight() + pill.m_obb.GetUp() * pill.m_obb.m_extents.y + m_transform.m_position;
	Vec2 TL = pill.m_obb.m_center + pill.m_obb.GetUp() * pill.m_obb.m_extents.y - pill.m_obb.GetRight() * pill.m_obb.m_extents.x + m_transform.m_position;
	Vec2 BR = pill.m_obb.m_center - pill.m_obb.GetUp() * pill.m_obb.m_extents.y + pill.m_obb.GetRight() * pill.m_obb.m_extents.x + m_transform.m_position;

	// Disc
	if( BL == TL && BL == BR )
	{
		AddVertsForDisc2D( verts, BL, pill.m_radius, color, 24 );
		AddVertsForRing2D( verts, BL, pill.m_radius, 0.05f, Rgba::BLACK, 24 );
		g_theRenderer->DrawVertexArray( verts );
		return;
	}

	// Check if there is length along the height but none on width
	if( BL != TL && BL == BR )
	{
		Vec2 alongHeight = TR - BR;
		alongHeight.SetLength( pill.m_radius );
		alongHeight.Rotate90Degrees();
		AddVertsForTrimmedLine2D( verts, BL, TL, pill.m_radius * 2.0f, color );
		AddVertsForLine2D( verts, BR - alongHeight, TR - alongHeight, 0.1f, m_boarderColor );
		AddVertsForLine2D( verts, TL + alongHeight, BL + alongHeight, 0.1f, m_boarderColor );
		AddVertsForDisc2D( verts, BL, pill.m_radius, color, 24 );
		AddVertsForDisc2D( verts, TL, pill.m_radius, color, 24 );
		g_theRenderer->DrawVertexArray( verts );
		return;
	}

	// Check to see if there is length along the width but not height
	if( BL != BR && BL == TL )
	{
		Vec2 alongWidth = BR - BL;
		alongWidth.SetLength( pill.m_radius );
		alongWidth.RotateMinus90Degrees();
		AddVertsForTrimmedLine2D( verts, BL, BR, pill.m_radius * 2.0f, color );
		AddVertsForLine2D( verts, TR - alongWidth, TL - alongWidth, 0.1f, m_boarderColor );
		AddVertsForLine2D( verts, BL + alongWidth, BR + alongWidth, 0.1f, m_boarderColor );
		AddVertsForDisc2D( verts, BL, pill.m_radius, color, 24 );
		AddVertsForDisc2D( verts, BR, pill.m_radius, color, 24 );
		g_theRenderer->DrawVertexArray( verts );
		return;
	}


	Vec2 alongHeight = TR - BR;
	float thickness = alongHeight.GetLength();
	alongHeight.SetLength( pill.m_radius );
	alongHeight.Rotate90Degrees();
	AddVertsForTrimmedLine2D( verts, ( BL + TL ) * .5f, ( BR + TR ) * .5f, pill.m_radius * 2.0f + thickness, color );
	AddVertsForLine2D( verts, BR - alongHeight, TR - alongHeight, 0.1f, m_boarderColor );
	AddVertsForLine2D( verts, TL + alongHeight, BL + alongHeight, 0.1f, m_boarderColor );

	Vec2 alongWidth = BR - BL;
	thickness = alongWidth.GetLength();
	alongWidth.SetLength( pill.m_radius );
	alongWidth.RotateMinus90Degrees();
	AddVertsForTrimmedLine2D( verts, ( BL + BR ) * .5f, ( TL + TR ) * .5f, pill.m_radius * 2.0f + thickness, color );
	AddVertsForLine2D( verts, TR - alongWidth, TL - alongWidth, 0.1f, m_boarderColor );
	AddVertsForLine2D( verts, BL + alongWidth, BR + alongWidth, 0.1f, m_boarderColor );

	AddVertsForDisc2D( verts, BL, pill.m_radius, color, 24 );
	AddVertsForDisc2D( verts, BR, pill.m_radius, color, 24 );
	AddVertsForDisc2D( verts, TR, pill.m_radius, color, 24 );
	AddVertsForDisc2D( verts, TL, pill.m_radius, color, 24 );

	g_theRenderer->DrawVertexArray( verts );
}

//--------------------------------------------------------------------------
/**
* IsOutOfBounds
*/
bool Pill::IsOutOfBounds( const AABB2& bounds ) const
{
	Pillbox2 boundingPill = Pillbox2( Vec2::ZERO, 0.0f, Vec2( bounds.GetWidth(), bounds.GetHeight() ) * .5f );
	return !DoesPillboxOverlapPillbox( boundingPill, static_cast<PillboxCollider2D*>( m_collider )->GetWorldShape() );
}
