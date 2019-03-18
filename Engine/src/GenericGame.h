#pragma once

class GenericGame
{
	// VARIABLES
private:

public:

	// CONSTRUCTORS/DESTRUCTORS
public:
	GenericGame() { Initialize(); };
	~GenericGame() { Shutdown(); };

	// METHODES
private:
	virtual void Initialize() = 0;
	virtual void Shutdown() = 0;

public:

};