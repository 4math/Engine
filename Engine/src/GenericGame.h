#pragma once

constexpr double DEFAULT_GAME_TICKRATE = 60.0; // 60 game updates per second

class GenericGame
{
	// VARIABLES
private:
	double m_game_tickrate = DEFAULT_GAME_TICKRATE; // Game updates per second

	// CONSTRUCTORS/DESTRUCTORS
public:
	GenericGame() { Initialize(); };
	virtual ~GenericGame() { Shutdown(); }; // Is virtual destructor needed here?

	// METHODES
private:
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;

public:
	virtual void FrameAction() = 0; // This method will be called every game tick

	double& GameTickrate() { return m_game_tickrate; }
};