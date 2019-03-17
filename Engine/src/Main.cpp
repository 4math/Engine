#include "EngineMain.h"

int main(int argc, char** argv)
{
	engine::Engine engine("Debug game");
	return engine.Loop();
}