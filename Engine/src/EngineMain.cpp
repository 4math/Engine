#include "EngineMain.h"

using namespace engine;

void Engine::Initiailize()
{
	m_environment_manager = std::make_shared<environment::EnvironmentManager>(800, 800, m_app_name, environment::WINDOWED);
	m_graphics_manager = std::make_shared<graphics::GraphicsManager>(m_environment_manager);
	m_sound_manager = std::make_shared<sound::SoundManager>();

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
		m_graphics_manager->BeginFrame();
		FrameAction();
		m_graphics_manager->EndFrame();
	}


	return EXIT_CODE_OK;
}
