#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Game/Shapes/Entity.hpp"

class Collider2D;
class Rigidbody2D;


class Shape
	: public Entity
{
public:
	explicit Shape( const Transform2D& spawnLoaction, ePhysicsSimulationType simType, eAlignment alignment );
	~Shape();

	virtual void Render() const = 0;
	virtual void Update( float deltaSec );
	virtual bool IsOutOfBounds( const AABB2& bounds ) const = 0;
	Vec2 GetPosition() const;
	void SetTransform( Transform2D trasform );
	void SetPosition( const Vec2& pos );


protected:
	Rgba DeterminColor() const;
	Rgba m_color = Rgba::BLUE;
	Rgba m_dyingColor = Rgba::DARK_RED;
	Rgba m_boarderColor = Rgba::BLACK;
	Rgba m_hitColor = Rgba::RED;

public:
	Rigidbody2D* m_rigidbody;
	Collider2D* m_collider;
	Transform2D m_transform;
	bool m_selected = false;
	bool m_isGarbage = false;
};

