#pragma once
#include "Game/GameCommon.hpp"


class Game
{
public:
	Game();
	~Game();

	void Startup();
	void Shutdown();

	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleKeyReleased( unsigned char keyCode );

	void GameRender() const;
	void UpdateGame( float deltaSeconds );

	float GetScreenShakeIntensity() { return m_screenShakeIntensity; };
	void ShakeScreen( float shake );

private:
	//Render
	void RenderDebug() const;
	void RenderDebugCosmetics() const;
	void RenderDebugPhysics() const;

	//Update
	void UpdateShakeReduction( float deltaSeconds );


private:
	void ResetGame();

	// Helper Methods
	void ConstructGame();
	void DeconstructGame();
private:
	bool m_isQuitting = false;

	// Quality of life variables
	float m_screenShakeIntensity = 0.0f;
	float m_shakeSpeedReduction = 2.0f;
};