#pragma once
#include "Game/GameCommon.hpp"

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

	//Update
	void UpdateCamera( float deltaSeconds );
	void UpdateMap( unsigned int  index );

private:
	// Getters
	Camera* GetCurrentCamera() { return &m_curentCamera; }
	static bool Command_SetDirColor( EventArgs& args );

private:
	eGameStates m_state = GAMESTATE_INIT;
	unsigned int  m_curMapIdx = 1;
	std::vector<Map*> m_maps;

private:
	// Shader
	Material* m_couchMat = nullptr;

	Shader* m_shader = nullptr;
	MeshGPU* meshSquareGPU;
	MeshGPU* meshSphereGPU;
	MeshGPU* meshPlainGPU;
	
	// Camera
	Camera m_UICamera;
	Camera m_curentCamera;
	mutable Camera m_DevColsoleCamera;
	Vec3 m_camPos = Vec3( 0.0f, 0.0f, -10.0f);
	Vec3 m_camRot = Vec3( 0.0f, 0.0f, 0.0f);

	// Lighting
	float m_curAmbiant = 0.05f;
	float m_specFact = 1.0;
	float m_specPow = 20.0f;
	float m_emissiveFac = 0.1f;

	unsigned int m_actLights = 1;
	int shaderType = 0;

	Vec3 input = Vec3::ZERO;
	float m_gameTime = 0.0f;
};