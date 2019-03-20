#pragma once

#include <string>
#include <Windows.h>

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include "InputMain.h"

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

		int m_window_width = 0;
		int m_window_height = 0;
		std::string m_window_title;
		WindowType m_window_type = environment::NO_WINDOW;

		GLFWmonitor* m_monitor = nullptr;
		GLFWwindow* m_window = nullptr;

		InputManager* m_input_manager = nullptr;

		// CONSTRUCTORS/DESTRUCTORS
	public:
		EnvironmentManager() { Initialize(false); };
		EnvironmentManager(int window_width_, int window_height_, const std::string& window_title_, WindowType window_type_) : 
			m_window_width(window_width_), m_window_height(window_height_), 
			m_window_title(window_title_), m_window_type(window_type_) { Initialize(true); }
		~EnvironmentManager() { Shutdown(); };

		// METHODES
	private:
		void Initialize(bool create_window_);
		void Shutdown();

		int DestroyWindow();

		int CreateWindowedWindow();
		int CreateFullscreenWindow();
		int CreateBorderlessWindow();

	public:
		int WindowCreate(int width_, int height_, std::string title_, WindowType type_);
		void ProcessMessages();
		bool ShouldFinish();
		bool IsWindowCreated();

		InputManager* Input() const { return m_input_manager; }
	};

}
