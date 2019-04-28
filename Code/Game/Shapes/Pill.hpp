#pragma once
#include "Game/Shapes/Shape.hpp"



class Pill
	: public Shape
{
public:
	explicit Pill( const Transform2D& spawnLoaction, ePhysicsSimulationType simType, eAlignment alignment
		, float width = 1.0f, float height = 1.0f, float radius = 1.0f
		, float mass = 1.0f, float restitution = 1.0f, float friction = 0.0f, float drag = 0.0f, float angularDrag = 0.0f );

public:
	void Render() const;

	bool IsOutOfBounds( const AABB2& bounds ) const;

private:
	float m_width = 1.0f;
	float m_height = 1.0f;
	float m_radius = 1.0f;
};