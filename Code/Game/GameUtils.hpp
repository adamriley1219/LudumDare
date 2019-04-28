#pragma once
#include "Engine/Math/Vec3.hpp"
#include <string>

struct Vertex_PCU;
class Entity;
class Camera;

//--------------------------------------------------------------------------
enum eAlignment
{
	ALIGNMENT_PLAYER,
	ALIGNMENT_NEUTRAL,
	ALIGNMENT_ALLY,
	ALIGNMENT_ENEMY,

};

//--------------------------------------------------------------------------
float GetRandomlyChosenFloat( float a, float b );
eAlignment GetAlignmentFromString( const std::string& string );
std::string GetStringFromAlignment( eAlignment alignment );

struct matStruct
{
	float var;
	Vec3 padding;
};

