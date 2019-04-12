#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Camera.hpp"

class Shader;
class MeshGPU;
class Material;
class Map;



class Game
{
	friend App;
public:
	Game();
	~Game();

	void Startup();
	void Shutdown();

	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleKeyReleased( unsigned char keyCode );

	void GameRender() const;
	void UpdateGame( float deltaSeconds );

private:
	//Render
	void RenderMap( unsigned int index ) const;
	void RenderLoadingScreen() const;
	void RenderInit() const;
	void RenderLoading() const;
	void RenderMainMenu() const;

	//Update
	void UpdateCamera( float deltaSeconds );
	void UpdateMap( float deltaSec, unsigned int  index );

	void InisializeGame();
	void LoadLevel( unsigned int index );

private:
	void SwitchStates( eGameStates state );

private:
	// Getters
	Camera* GetCurrentCamera() { return m_curCamera; }
	static bool Command_SetDirColor( EventArgs& args );

private:
	eGameStates m_state = GAMESTATE_INIT;
	unsigned int  m_curMapIdx = 1;
	std::vector<Map*> m_maps;

private:
	unsigned int m_loadingFramCount = 0;

private:
	// Shader
	Material* m_couchMat = nullptr;

	Shader* m_shader = nullptr;
	MeshGPU* meshSquareGPU;
	MeshGPU* meshSphereGPU;
	MeshGPU* meshPlainGPU;
	
	// Camera
	Camera* m_curCamera;
	Camera m_UICamera;
	mutable Camera m_DevColsoleCamera;

	// Lighting
	float m_curAmbiant = 0.05f;
	float m_specFact = 1.0;
	float m_specPow = 20.0f;
	float m_emissiveFac = 0.1f;

	unsigned int m_actLights = 1;
	int shaderType = 0;

	float m_gameTime = 0.0f;
};