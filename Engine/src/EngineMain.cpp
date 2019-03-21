#include "EngineMain.h"

using namespace engine;

void Engine::Initiailize()
{
	m_environment_manager = new environment::EnvironmentManager(500, 500, m_app_name, environment::WINDOWED);
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

	while (!m_environment_manager->ShouldFinish() && !m_should_finish)
	{
		// Get key press events, mouse move events, window size change events etc.
		m_environment_manager->ProcessMessages();

		EngineAction();
	}

	return EXIT_CODE_OK;
}
