#pragma once
#include "Engine/Math/Vec3.hpp"

struct Vertex_PCU;
class Entity;
class Camera;

float GetRandomlyChosenFloat( float a, float b );


struct matStruct
{
	float var;
	Vec3 padding;
};

enum eAlignment
{
	ALIGNMENT_PLAYER,
	ALIGNMENT_NURTAL,
	ALIGNMENT_ALLY,
	ALIGNMENT_ENEMY,
	
};