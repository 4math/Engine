#pragma once

#include <string>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace environment
{
#pragma region exceptions and EXIT_CODES
	struct window_creation_failure : public std::exception {
		const char * what() const throw () {
			return "Window creation failed";
		}
	};

	constexpr int EXIT_CODE_OK = 0;
	constexpr int EXIT_CODE_FAILURE = -1;
#pragma endregion

enum WindowType{ NO_WINDOW, WINDOWED, BORDERLESS, FULLSCREEN };

	class EnvironmentManager
	{
		// VARIABLES
	private:
		bool m_initialized = false;
		GLFWmonitor* m_monitor = nullptr;
		GLFWwindow* m_window = nullptr;
		WindowType m_windowType = environment::NO_WINDOW;
	public:
		// CONSTRUCTORS/DESTRUCTORS
	public:
		EnvironmentManager() { Initialize(); };
		~EnvironmentManager() { Shutdown(); };
		// METHODES
	private:
		void Initialize();
		void Shutdown();
	public:
		int CreateWindowedWindow(int width_, int height_, std::string title_);
		int CreateFullscreenWindow(int width_, int height_, std::string title_);
		int CreateBorderlessWindow(std::string title_);
		int DestroyWindow();
	};

}
