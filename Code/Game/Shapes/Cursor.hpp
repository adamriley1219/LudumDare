#pragma once
#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Physics/Transform2D.hpp"
#include "Engine/Math/Disc.hpp"

class Cursor
{
public:
	Cursor();
	~Cursor(){}

public:
	void Update( float deltaTime );
	void Render() const;

public:
	float m_moveRate = 10.0f;
	Vec2 m_moveDirection = Vec2::ZERO;
	Transform2D m_trasform;
	Disc m_disc;
};