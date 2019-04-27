#include "Game/Shapes/Entity.hpp"
#include "Game/GameCommon.hpp"

//--------------------------------------------------------------------------
/**
* Entity
*/
Entity::Entity()
{
	m_tint = Rgba( 1.0f, 1.0f, 1.0f );
}


//--------------------------------------------------------------------------
/**
* Entity
*/
Entity::~Entity()
{
}

//--------------------------------------------------------------------------
/**
* Kill
*/
void Entity::Die()
{
	m_isDead = true;
	m_isGarbage = true;
}

//--------------------------------------------------------------------------
/**
* isDying
*/
bool Entity::IsAlive() const
{
	return !m_isDead;
}

//--------------------------------------------------------------------------
/**
* DieisGarbage
*/
bool Entity::IsGarbage() const
{
	return m_isGarbage;
}

//--------------------------------------------------------------------------
/**
* PushTransformsToGame
*/
void Entity::Render() const
{

}

//--------------------------------------------------------------------------
/**
* DebugRenderDebugRenderCosmetic
*/
void Entity::DebugRenderCosmetic() const
{

}

//--------------------------------------------------------------------------
/**
* DebugRenderPhysics
*/
void Entity::DebugRenderPhysics() const
{

}


//--------------------------------------------------------------------------
/**
* isOffScreengetForwardVector
*/
Vec2 Entity::GetForwardVector() const
{
	Vec2 forwardVec = Vec2( CosDegrees( m_orientationDegrees ), SinDegrees( m_orientationDegrees ) ); 
	forwardVec.Normalize();
	return forwardVec;
}

//--------------------------------------------------------------------------
/**
* setAcceleration
*/
void Entity::SetAcceleration( bool on )
{
	m_isAccelerating = on;
}


//--------------------------------------------------------------------------
/**
* setRotationDirection
*/
void Entity::SetRotationDirection( int rotationDirection )
{
	m_rotateDirection = rotationDirection;
}

//--------------------------------------------------------------------------
/**
* SetRotation
*/
void Entity::SetRotation( float rotationDegrees )
{
	m_orientationDegrees = rotationDegrees;
}

//--------------------------------------------------------------------------
/**
* SetRotationGetRotationDegrees
*/
float Entity::GetRotationDegrees() const
{
	return m_orientationDegrees;
}

//--------------------------------------------------------------------------
/**
* EntityGetColor
*/
Rgba Entity::GetTint() const
{
	return m_tint;
}

//--------------------------------------------------------------------------
/**
* TakeDamage
*/
void Entity::TakeDamage( float damage )
{
	if( ( m_health -= damage ) <= 0.0f )
	{
		Die();
	}
}

//--------------------------------------------------------------------------
/**
* GetCollisionDamage
*/
float Entity::GetCollisionDamage()
{
	return m_collisionDamage;
}
