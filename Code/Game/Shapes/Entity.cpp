#include "Game/Shapes/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time/StopWatch.hpp"
#include "Game/App.hpp"

//--------------------------------------------------------------------------
/**
* Entity
*/
Entity::Entity( eAlignment alignment )
	: m_alignment( alignment )
{
	m_tint = Rgba( 1.0f, 1.0f, 1.0f );
	m_preventInputTimer = new StopWatch( g_theApp->m_gameClock );
	m_preventInputTimer->SetAndReset( 0.1f );
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
