#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/UIWidget.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vec3.hpp"


class Shader;
class MeshGPU;
class Material;
class Map;
class StopWatch;
class UniformBuffer;

struct singleEffect
{
	float STRENGTH = .5f; 
	Vec3 tonemap_pad00;
};

struct matrixEffect
{
	Matrix44 mat = Matrix44::IDENTITY;
	float STRENGTH = .5f; 
	Vec3 tonemap_pad00;
};

class Game
{
	friend App;
	friend GameController;
public:
	Game();
	~Game();

	void Startup();
	void Shutdown();

	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleKeyReleased( unsigned char keyCode );
	bool HandleQuitRequest();

	void GameRender() const;
	void UpdateGame( float deltaSeconds );

private:
	//Render
	void RenderMap( unsigned int index ) const;
	void RenderLoadingScreen() const;
	void RenderInit() const;
	void RenderLoading() const;
	void RenderMainMenu() const;
	void RenderEditor() const;
	void RenderPauseMenu() const;

	// UI Setup
	void SetupMainMenuUI();
	void SetupEditorUI();
	void SetUpPauseMenu();

	//Update
	void UpdateCamera( float deltaSeconds );
	void UpdateMainMenu(); 
	void UpdateMap( float deltaSec, unsigned int  index );
	void UpdateEditor( float deltaSec );
	void UpdateEditorUI( float deltaSec );
	void UpdatePauseMenu();

	void LMouseDown();
	void LMouseUp();
	void RMouseDown();
	void RMouseUp();

	void InisializeGame();
	void LoadLevel( unsigned int index );

private:
	static bool LoadToLevel( EventArgs& args );

private:
	void UpdateStates();
	void SwitchStates( eGameStates state );

public:
	bool FadeIn();
	bool FadeOut();

private:
	void RenderFade() const;

private:
	// Getters
	Camera* GetCurrentCamera() { return m_curCamera; }

private:
	eGameStates m_state = GAMESTATE_INIT;
	eGameStates m_switchToState = m_state;
	unsigned int  m_curMapIdx = 1;
	std::vector<Map*> m_maps;

private:
	unsigned int m_stateFrameCount = 0;

private:
	Shader* m_shader = nullptr;
	StopWatch* m_fadeinStopwatch = nullptr;
	StopWatch* m_fadeoutStopwatch = nullptr;


	// Camera
	Camera* m_curCamera;
	mutable Camera m_UICamera;
	mutable Camera m_DevColsoleCamera;

	// UI
	UICanvas m_mainMenuCanvis;
	UIRadioGroup* m_mainMenuRadGroup = nullptr;
	UICanvas m_editorCanvis;
	UIRadioGroup* m_editorRadGroup = nullptr;
	UICanvas m_pauseMenuCanvis;
	UIRadioGroup* m_pauseMenuRadGroup = nullptr;

	// Lighting
	float m_curAmbiant = 0.7f;
	float m_specFact = .3f;
	float m_specPow = 30.0f;
	float m_emissiveFac = 0.1f;

	unsigned int m_actLights = 1;
	int shaderType = 0;

	float m_gameTime = 0.0f;
};