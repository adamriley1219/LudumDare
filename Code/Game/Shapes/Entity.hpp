#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Game/GameCommon.hpp"

class StopWatch;

class Entity
{
	friend class Map;
public:
	Entity( eAlignment alignment );
	virtual ~Entity();

	virtual void Die();
	bool IsAlive() const;
	bool IsGarbage() const;
	void DebugRenderCosmetic() const;
	void DebugRenderPhysics() const;
	virtual void Render() const;
	virtual void Update( float deltaSeconds ) = 0;

	Rgba GetTint() const;

	// Game play
	void TakeDamage( float damage );
	float GetCollisionDamage(); 

protected:
	bool m_isDead = false;
	bool m_isGarbage = false;

	// Gameplay
	eAlignment m_alignment = ALIGNMENT_NEUTRAL;
	float m_health = 1.0;
	float m_collisionDamage = 0.05f;
	float m_speed = 20.0f;

	StopWatch* m_preventInputTimer = nullptr;

	Rgba m_tint;
};

