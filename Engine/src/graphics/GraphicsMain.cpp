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
		{
			if (m_env->CreateWindowedWindow(width_, height_, m_process_name) == environment::EXIT_CODE_OK)
				return EXIT_CODE_OK;
			else
				return EXIT_CODE_FAILURE;
		}
		case environment::BORDERLESS:
		{
			if (m_env->CreateBorderlessWindow(m_process_name) == environment::EXIT_CODE_OK)
				return EXIT_CODE_OK;
			else
				return EXIT_CODE_FAILURE;
		}
		case environment::FULLSCREEN:
		{
			if (m_env->CreateFullscreenWindow(width_, height_, m_process_name) == environment::EXIT_CODE_OK)
				return EXIT_CODE_OK;
			else
				return EXIT_CODE_FAILURE;
		}
		default:
			return EXIT_CODE_FAILURE;
	}
}

int graphics::GraphicsManager::DestroyGlfwWindow()
{
	if (m_env->DestroyWindow() == environment::EXIT_CODE_OK)
		return EXIT_CODE_OK;
	else
		return EXIT_CODE_FAILURE;
}
