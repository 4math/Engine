#include "GraphicsMain.h"

using namespace graphics;

void graphics::GraphicsManager::Initialize(std::string &process_name_)
{
#ifdef GLFW_INCLUDE_VULKAN
	m_env = new environment::EnvironmentManager();
	m_initialized = true;
#else
	throw bad_header();
#endif 
}

void graphics::GraphicsManager::Shutdown()
{
	if (m_env != nullptr)
	{
		delete m_env;
		m_env = nullptr;
	}
	m_initialized = false;
}

int graphics::GraphicsManager::CreateGlfwWindow(environment::WindowType type_, int width_, int height_)
{
	switch (type_)
	{
		case environment::WINDOWED:
			return m_env->CreateWindowedWindow(width_, height_, m_process_name);

		case environment::BORDERLESS:
			return m_env->CreateBorderlessWindow(m_process_name);

		case environment::FULLSCREEN:
			return m_env->CreateFullscreenWindow(width_, height_, m_process_name);

		default:
			return EXIT_CODE_FAILURE;
	}
}

int graphics::GraphicsManager::DestroyGlfwWindow()
{
	return m_env->DestroyWindow();
}
