#include "EngineMain.h"

using namespace engine;

void Engine::Initiailize()
{
	// TODO: Init graphics -> Init sound -> Init input

	m_initialized = true;
}

void Engine::Shutdown()
{
	// Free resources here
}

int Engine::Loop()
{
	// If engine was not initialized successfully exit the loop
	if (!m_initialized)
		return EXIT_CODE_FAILURE;

	return EXIT_CODE_OK;
}
