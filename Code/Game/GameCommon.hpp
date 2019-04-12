#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/GameUtils.hpp"

class RenderContext; // All may know about the renderer and the globe. You must include to use it.
extern RenderContext* g_theRenderer;

class App;
extern App* g_theApp;				// Created and owned by Main_Windows.cpp

class InputSystem;
extern InputSystem* g_theInputSystem;

class PhysicsSystem;
extern PhysicsSystem* g_thePhysicsSystem;

class AudioSystem;
extern AudioSystem* g_theAudioSystem;

class RNG;
extern RNG* g_theRNG;

class Game;
extern Game* g_theGame;

class WindowContext;
extern WindowContext* g_theWindowContext;

class GameController;
extern GameController* g_theGameController;

//--------------------------------------------------------------------------
// Constant global variables.
//--------------------------------------------------------------------------
constexpr float WORLD_WIDTH = 10.f;
constexpr float WORLD_HEIGHT = 5.f;
constexpr float WORLD_CENTER_X = WORLD_WIDTH / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_HEIGHT / 2.f;

constexpr float SCREEN_WIDTH = 200.0f;
constexpr float SCREEN_HEIGHT = 100.0f;
constexpr float SCREEN_HALF_WIDTH = 200.0f * .5f;
constexpr float SCREEN_HALF_HEIGHT = 100.0f * .5f;

enum eGameStates
{
	GAMESTATE_INIT,
	GAMESTATE_MAINMENU,
	GAMESTATE_LOADING,
	GAMESTATE_GAMEPLAY,
	GAMESTATE_EDITOR
};