#include "GenericGame.h"

class Game : public virtual GenericGame
{
	// CONSTRUCTORS/DESTRUCTORS
public:
	Game() {}
	~Game() {}

	// METHODES
public:
	void FrameAction()
	{
		// If escape key is pressed - finish the program
		if (Environment()->Input()->KeyPressed(256))
			SetShouldFinish(true);
	}
};

int main(int argc, char** argv)
{
	
#ifdef _DEBUG
	Game* debug_game = new Game;
	int return_value = debug_game->Loop();
	delete debug_game;
	std::cin.get();
	return return_value;
#else
	Game debug_game;
	return debug_game.Loop();
#endif
}