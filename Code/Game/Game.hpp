#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Game/UIWidget.hpp"


class Shader;
class MeshGPU;
class Material;
class Map;



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

	// UI Setup
	void SetupMainMenuUI();
	void SetupEditorUI();

	//Update
	void UpdateCamera( float deltaSeconds );
	void UpdateMainMenu( float deltaSeconds ); 
	void UpdateMap( float deltaSec, unsigned int  index );
	void UpdateEditor( float deltaSec );
	void UpdateEditorUI( float deltaSec );

	void LMouseDown();
	void LMouseUp();
	void RMouseDown();
	void RMouseUp();

	void InisializeGame();
	void LoadLevel( unsigned int index );

private:
	static bool LoadToLevel( EventArgs& args );

private:
	void SwitchStates( eGameStates state );

private:
	// Getters
	Camera* GetCurrentCamera() { return m_curCamera; }

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
	
	// Camera
	Camera* m_curCamera;
	mutable Camera m_UICamera;
	mutable Camera m_DevColsoleCamera;

	// UI
	UICanvas m_mainMenuCanvis;
	UIRadioGroup* m_mainMenuRadGroup = nullptr;
	UICanvas m_editorCanvis;
	UIRadioGroup* m_editorRadGroup = nullptr;

	// Lighting
	float m_curAmbiant = 0.05f;
	float m_specFact = 1.0;
	float m_specPow = 20.0f;
	float m_emissiveFac = 0.1f;

	unsigned int m_actLights = 1;
	int shaderType = 0;

	float m_gameTime = 0.0f;
};