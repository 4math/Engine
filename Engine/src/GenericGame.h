#pragma once

#include <string>

#include "EngineMain.h"

constexpr double DEFAULT_GAME_TICKRATE = 60.0; // 60 game updates per second

class GenericGame : public virtual engine::Engine
{
	// VARIABLES
protected:
	double m_game_tickrate; // Game updates per second

	// CONSTRUCTORS/DESTRUCTORS
public:
	GenericGame(double game_tickrate_ = DEFAULT_GAME_TICKRATE) : 
		m_game_tickrate(game_tickrate_) {}

	virtual ~GenericGame() {};
};