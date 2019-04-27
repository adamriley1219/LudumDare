#include "Game/Shapes/Shape.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/RNG.hpp"
#include "Engine/Physics/Transform2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/AABB2Collider2D.hpp"
#include "Engine/Physics/DiscCollider2D.hpp"



//--------------------------------------------------------------------------
/**
* Shape
*/
Shape::Shape( const Transform2D& spawnLoaction, ePhysicsSimulationType simType )
{
	m_transform = spawnLoaction;
	m_rigidbody = g_thePhysicsSystem->CreateRigidbody( 1.0f );
	m_rigidbody->SetOriginalSimulationType( simType ); 
	
	// give it a for the physics system to affect our object
	m_rigidbody->SetObject( this, &m_transform ); 
}



//--------------------------------------------------------------------------
/**
* ~Shape
*/
Shape::~Shape()
{
	g_thePhysicsSystem->RemoveRigidbody( m_rigidbody );
	m_rigidbody = nullptr;
	m_collider = nullptr;
}

//--------------------------------------------------------------------------
/**
* Update
*/
void Shape::Update( float deltaSec )
{
	UNUSED(deltaSec);
}

//--------------------------------------------------------------------------
/**
* GetPosition
*/
Vec2 Shape::GetPosition() const
{
	return m_transform.m_position;
}

//--------------------------------------------------------------------------
/**
* SetRigidbodyTransform
*/
void Shape::SetTransform( Transform2D trasform )
{
	m_transform = trasform;
}


//--------------------------------------------------------------------------
/**
* SetPosition
*/
void Shape::SetPosition( const Vec2& pos )
{
	m_transform.m_position = pos;
}

//--------------------------------------------------------------------------
/**
* DeterminColor
*/
Rgba Shape::DeterminColor() const
{
	if( m_selected )
	{
		return Rgba::WHITE;
	}
	Rgba color = Rgba::CYAN;
	
	{
		switch( m_rigidbody->GetSimulationType() )
		{
		case ePhysicsSimulationType::PHYSICS_SIM_DYNAMIC:
			if( m_collider->IsColliding() )
				color = Rgba::RED;
			else
				color = Rgba::BLUE;
			break;
		case ePhysicsSimulationType::PHYSICS_SIM_STATIC:
			if( m_collider->IsColliding() )
				color = Rgba::MAGENTA;
			else
				color = Rgba::YELLOW;
			break;
		}
	}
	return color;
}
