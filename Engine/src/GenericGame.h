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
};