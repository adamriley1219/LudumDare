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
class Cursor;
class Shape;

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
	friend class App;
	friend class Map;
	friend class Cursor;
	friend class GameController;
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

public:
	// Gameplay
	Map* GetCurrentMap();

	// Editor
	void SelectShape();
	bool IsHovering( Shape* shape ) const;
	void DeselectShape();
	void DeleteShape();

	void BeginShapeConstruction();
	void EndShapeConstruction();

	void ToggleAlignment();
	void SetEnd();

private:
	//Render
	void RenderMap( unsigned int index ) const;
	void RenderLoadingScreen() const;
	void RenderInit() const;
	void RenderLoading() const;
	void RenderMainMenu() const;
	void RenderEditor() const;
	void RenderPauseMenu() const;
	void DrawEditorValues();

	// UI Setup
	void SetupMainMenuUI();
	void SetupEditorUI();
	void SetUpPauseMenu();

	//Update
	void UpdatePlayerPosAndCamera( float deltaSeconds );
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
	void LoadNextMap();

private:
	static bool LoadToLevel( EventArgs& args );
	static bool LoadMap( EventArgs& args );
	static bool Save( EventArgs& args );

private:
	void UpdateStates();
	void SwitchStates( eGameStates state );

public:
	bool FadeIn();
	bool FadeOut();

private:
	void RenderFade() const;

private:
	// Editor
	float m_restitution = 0.9f;
	float m_friction	= 0.2f;
	float m_mass		= 1.0f;
	float m_angularDrag = 0.0f;
	float m_drag		= 0.5f;
	float m_angularVel	= 0.0f;

	bool m_xRestrcted = false;
	bool m_yRestrcted = false;
	bool m_rotRestrcted = false;

	float m_curRadius = 0.0f;
	float m_curThickness = 1.5f;
	bool m_spawnDynamic = true;
	eAlignment m_curAlignment = ALIGNMENT_NEUTRAL;
		
		// Selecting
		bool m_deletingSelected = false;
		Vec2 m_constStart = Vec2::ZERO;
		Vec2 m_constEnd = Vec2::ZERO;
		bool m_constructing  = false;
		Shape* m_selectedShape = nullptr;
		Cursor* m_cursor;


private:
	// Getters
	Camera* GetCurrentCamera() { return m_curCamera; }

private:
	eGameStates m_state = GAMESTATE_INIT;
	eGameStates m_switchToState = m_state;
	unsigned int  m_curMapIdx = 1;
	bool		  m_toNextLevel = false;
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