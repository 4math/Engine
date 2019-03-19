#include "EnvironmentMain.h"

void environment::EnvironmentManager::Initialize()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	m_monitor = glfwGetPrimaryMonitor();
	m_initialized = true;
}

void environment::EnvironmentManager::Shutdown()
{
	glfwPollEvents();
	DestroyWindow();
	glfwTerminate();
}

int environment::EnvironmentManager::CreateWindowedWindow(int width_, int height_, std::string title_)
{
	if ((m_window = glfwCreateWindow(width_, height_, title_.c_str(), nullptr, nullptr)) != NULL)
	{
		m_windowType = environment::WINDOWED;
		return environment::EXIT_CODE_OK;
	}
	else
	{
		throw window_creation_failure();
		DestroyWindow();
		return environment::EXIT_CODE_FAILURE;
	}
}

int environment::EnvironmentManager::CreateFullscreenWindow(int width_, int height_, std::string title_)
{
	if ((m_window = glfwCreateWindow(width_, height_, title_.c_str(), m_monitor, nullptr)) != NULL)
	{
		m_windowType = environment::FULLSCREEN;
		return environment::EXIT_CODE_OK;
	}
	else
	{
		DestroyWindow();
		return environment::EXIT_CODE_FAILURE;
	}
}

int environment::EnvironmentManager::CreateBorderlessWindow(std::string title_)
{
	const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	if ((m_window = glfwCreateWindow(mode->width, mode->height, title_.c_str(), m_monitor, NULL)) != NULL)
	{
		m_windowType = environment::BORDERLESS;
		return environment::EXIT_CODE_OK;
	}
	else
	{
		DestroyWindow();
		return environment::EXIT_CODE_FAILURE;
	}
}

int environment::EnvironmentManager::DestroyWindow()
{
	if (m_windowType != environment::NO_WINDOW)
	{
		glfwDestroyWindow(m_window);
		m_windowType = environment::NO_WINDOW;
		return environment::EXIT_CODE_OK;
	}
	else
	{
		return environment::EXIT_CODE_FAILURE;
	}
}
