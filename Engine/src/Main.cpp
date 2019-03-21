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
	Game debug_game;
	return debug_game.Loop();
}