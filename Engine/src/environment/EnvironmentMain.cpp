#include "EnvironmentMain.h"

void environment::EnvironmentManager::Initialize(bool create_window_)
{
	if (glfwInit() == GLFW_FALSE)
	{
		m_initialized = false;
		return;
	}
	
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	m_monitor = glfwGetPrimaryMonitor();

	m_initialized = true;

	if (create_window_)
		WindowCreate(m_window_width, m_window_height, m_window_title, m_window_type);
	glfwSetFramebufferSizeCallback(m_window, FramebufferResizeCallback);
}

void environment::EnvironmentManager::Shutdown()
{
	glfwPollEvents();
	DestroyWindow();
	glfwTerminate();
}

int environment::EnvironmentManager::CreateWindowedWindow()
{
	if ((m_window = glfwCreateWindow(m_window_width, m_window_height, m_window_title.c_str(), nullptr, nullptr)) != NULL)
	{
		m_input_manager = std::make_shared<InputManager>(m_window);
		return environment::EXIT_CODE_OK;
	}

	throw window_creation_failure();
	DestroyWindow();
	return environment::EXIT_CODE_FAILURE;
}

int environment::EnvironmentManager::CreateFullscreenWindow()
{
	if ((m_window = glfwCreateWindow(m_window_width, m_window_height, m_window_title.c_str(), m_monitor, nullptr)) != NULL)
	{
		m_input_manager = std::make_shared<InputManager>(m_window);
		return environment::EXIT_CODE_OK;
	}

	throw window_creation_failure();
	DestroyWindow();
	return environment::EXIT_CODE_FAILURE;
}

int environment::EnvironmentManager::CreateBorderlessWindow()
{
	const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);
	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	if ((m_window = glfwCreateWindow(mode->width, mode->height, m_window_title.c_str(), m_monitor, nullptr)) != NULL)
	{
		m_input_manager = std::make_shared<InputManager>(m_window);
		return environment::EXIT_CODE_OK;
	}

	throw window_creation_failure();
	DestroyWindow();
	return environment::EXIT_CODE_FAILURE;
}

int environment::EnvironmentManager::DestroyWindow()
{
	if (m_window_type == environment::NO_WINDOW)
		return environment::EXIT_CODE_FAILURE;

	glfwDestroyWindow(m_window);
	m_window_type = environment::NO_WINDOW;
	return environment::EXIT_CODE_OK;
}

void environment::EnvironmentManager::FramebufferResizeCallback(GLFWwindow* window_, int width_, int height_)
{
	auto app = reinterpret_cast<EnvironmentManager*>(glfwGetWindowUserPointer(window_));
	app->m_framebuffer_resized = true;
}

int environment::EnvironmentManager::WindowCreate(int width_, int height_, std::string title_, WindowType type_)
{
	if (!m_initialized || type_ == environment::NO_WINDOW)
		return EXIT_CODE_FAILURE;

	if (m_window != nullptr || m_window_type != environment::NO_WINDOW)
		DestroyWindow();

	m_window_width = width_;
	m_window_height = height_;
	m_window_title = title_;
	m_window_type = type_;

	switch (m_window_type)
	{
		case environment::WINDOWED:
			return CreateWindowedWindow();

		case environment::BORDERLESS:
			return CreateBorderlessWindow();

		case environment::FULLSCREEN:
			return CreateFullscreenWindow();

		default:
			return EXIT_CODE_FAILURE;
	}
}

void environment::EnvironmentManager::ProcessMessages()
{ 
	glfwPollEvents();
}

bool environment::EnvironmentManager::ShouldFinish()
{
	return glfwWindowShouldClose(m_window);
}

bool environment::EnvironmentManager::IsWindowCreated()
{
	return (m_window == nullptr || m_window_type == environment::NO_WINDOW) ? false : true;
}

HWND environment::EnvironmentManager::GetWindowHandle()
{
	return glfwGetWin32Window(m_window);
}