#pragma once

class GenericGame
{
	// VARIABLES
private:
	double m_fps_limit = 60.0;

	// CONSTRUCTORS/DESTRUCTORS
public:
	GenericGame() { Initialize(); };
	~GenericGame() { Shutdown(); };

	// METHODES
private:
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;

public:
	virtual void FrameAction() = 0; // This method will be called every game tick
};