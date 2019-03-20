#include "EngineMain.h"

using namespace engine;

void Engine::Initiailize()
{
	// TODO: Init input
	m_environment_manager = new environment::EnvironmentManager();

#if _DEBUG
	m_environment_manager->WindowCreate(500, 500, "debug window", environment::WINDOWED);
#endif

	m_graphics_manager = new graphics::GraphicsManager(m_environment_manager);
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
	if (m_graphics_manager != nullptr)
	{
		delete m_graphics_manager;
		m_graphics_manager = nullptr;
	}
	if (m_environment_manager != nullptr)
	{
		delete m_environment_manager;
		m_environment_manager = nullptr;
	}
	m_initialized = false;
}

int Engine::Loop()
{
	// If engine was not initialized successfully exit the loop
	if (!m_initialized)
		return EXIT_CODE_FAILURE;

	if (!m_environment_manager->IsWindowCreated())
		return EXIT_CODE_OK;

	bool loop_should_finish = false; // Local flag for exiting loop

	while (!m_environment_manager->ShouldFinish() && !loop_should_finish)
	{
		// Get key press events, mouse moves, window size changes etc.
		m_environment_manager->ProcessMessages();

		// Check if escape key was pressed
		if (m_environment_manager->Input()->KeyPressed(256))
			loop_should_finish = true;
	}

	return EXIT_CODE_OK;
}
