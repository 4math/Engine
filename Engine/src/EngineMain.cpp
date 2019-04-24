#include "EngineMain.h"

using namespace engine;

void Engine::Initiailize()
{
	m_environment_manager = std::make_shared<environment::EnvironmentManager>
		(environment::EnvironmentManager(500, 500, m_app_name, environment::WINDOWED));

	m_graphics_manager = std::make_shared<graphics::GraphicsManager>
		(graphics::GraphicsManager(m_environment_manager));

	m_sound_manager = std::make_shared<sound::SoundManager>
		(sound::SoundManager());

	m_initialized = true;
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

		FrameAction();
	}

	return EXIT_CODE_OK;
}
