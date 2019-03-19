#include "EngineMain.h"

using namespace engine;

void Engine::Initiailize()
{
	// TODO: Init sound -> Init input
	m_graphics_manager = new graphics::GraphicsManager(m_process_name);
	m_sound_manager = new sound::SoundManager();
#if _DEBUG
	m_graphics_manager->CreateGlfwWindow(environment::WINDOWED, 1280, 720);
#endif
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
	m_initialized = false;
}

int Engine::Loop()
{
	// If engine was not initialized successfully exit the loop
	if (!m_initialized)
		return EXIT_CODE_FAILURE;

	return EXIT_CODE_OK;
}
