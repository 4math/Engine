#include "EngineMain.h"

using namespace engine;

void Engine::Initiailize()
{
	// TODO: Init graphics -> Init sound -> Init input

	m_sound_manager = new sound::SoundManager();

	m_initialized = true;
}

void Engine::Shutdown()
{
	// Free resources here
	if (m_sound_manager != nullptr)
	{
		delete m_sound_manager;
		m_sound_manager = nullptr;
	}
}

int Engine::Loop()
{
	// If engine was not initialized successfully exit the loop
	if (!m_initialized)
		return EXIT_CODE_FAILURE;

	return EXIT_CODE_OK;
}
