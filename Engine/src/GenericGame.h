#pragma once

#include <string>

constexpr double DEFAULT_GAME_TICKRATE = 60.0; // 60 game updates per second

class GenericGame
{
	// VARIABLES
protected:
	double m_game_tickrate = DEFAULT_GAME_TICKRATE; // Game updates per second
	std::string m_game_name;
	
	// CONSTRUCTORS/DESTRUCTORS
public:
	GenericGame() {};
	virtual ~GenericGame() {};

	// METHODES
public:
	virtual void FrameAction() = 0; // This method will be called every game tick

	double& GameTickrate() { return m_game_tickrate; }
	std::string& GameName() { return m_game_name; }
};