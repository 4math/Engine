#pragma once

#include <string>

#include "EngineMain.h"

constexpr double DEFAULT_GAME_TICKRATE = 60.0; // 60 game updates per second

class GenericGame : public virtual engine::Engine
{
	// VARIABLES
protected:
	double m_game_tickrate = DEFAULT_GAME_TICKRATE; // Game updates per second

	// CONSTRUCTORS/DESTRUCTORS
public:
	GenericGame() {}
	virtual ~GenericGame() {};

	// METHODES
public:
	virtual void FrameAction() = 0; // This method will be called every game tick
	void EngineAction() { FrameAction(); } // Implementation of virtual method of Engine class
};