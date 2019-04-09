#pragma once
#include "Engine/Math/Vec3.hpp"

struct Vertex_PCU;
class Entity;
class Camera;

float GetDistanceBetween( const Entity* entityA, const Entity* entiryB );
float GetRandomlyChosenFloat( float a, float b );


struct matStruct
{
	float var;
	Vec3 padding;
};