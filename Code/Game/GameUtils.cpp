#include "Game/GameUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RNG.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/Shapes/Entity.hpp"
#include "Game/App.hpp"


float GetRandomlyChosenFloat( float a, float b )
{
	int randomf = g_theRNG->GetRandomIntInRange(0, 1);
	if( randomf == 0 )
	{
		return a;
	}
	return b;
}



eAlignment GetAlignmentFromString( const std::string& string )
{
	if( string == "neutral" )
	{
		return ALIGNMENT_NEUTRAL;
	}
	if( string == "enemy" )
	{
		return ALIGNMENT_ENEMY;
	}
	if( string == "ally" )
	{
		return ALIGNMENT_ALLY;
	}
	if( string == "player" )
	{
		return ALIGNMENT_PLAYER;
	}
	return ALIGNMENT_NEUTRAL;
}

std::string GetStringFromAlignment( eAlignment alignment )
{
	switch( alignment )
	{
	case ALIGNMENT_PLAYER:
		return "player";
		break;
	case ALIGNMENT_NEUTRAL:
		return "neutral";
		break;
	case ALIGNMENT_ALLY:
		return "ally";
		break;
	case ALIGNMENT_ENEMY:
		return "enemy";
		break;
	default:
		return "neutral";
		break;
	}
}