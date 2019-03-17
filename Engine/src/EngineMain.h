#pragma once

class Engine
{
// VARIABLES
private:

public:


// CONSTRUCTORS/DESTRUCTORS
public:
	Engine() { Initiailize(); };
	~Engine() { Shutdown(); };

// METHODES
private:
	void Initiailize();
	void Shutdown();

public:

};