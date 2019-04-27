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
