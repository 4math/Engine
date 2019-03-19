#include "EngineMain.h"

class Game : public GenericGame
{
	// CONSTRUCTORS/DESTRUCTORS
public:
	Game() {}
	~Game() {}

	// METHODES
public:
	void FrameAction()
	{
		// Some game update action here
	}
};

int main(int argc, char** argv)
{
	Game debug_game;
	engine::Engine engine(&debug_game);
	return engine.Loop();
}