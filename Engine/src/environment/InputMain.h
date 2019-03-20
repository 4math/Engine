#pragma once

#include <map>
#include <string>

#include "GLFW/glfw3.h"

namespace environment
{

	class InputManager
	{
		// VARIABLES
	private:
		bool m_initialized = false;
		std::map<int, bool> m_key_map;

		GLFWwindow* m_window = nullptr;

		// CONSTRUCTORS/DESTRUCTORS
	public:
		InputManager(GLFWwindow* window_) : m_window(window_) { Initialize(); }
		~InputManager() { Shutdown(); }

		// METHODES
	private:
		void Initialize();
		void Shutdown();

		void SetKeyState(int key_id_, bool pressed_);

		static void KeyCallback(GLFWwindow* window_, int key_id_, int scancode_, int action_, int mods_);

	public:
		bool KeyPressed(int key_id_); // Returns the state of the key. True - pressed, False - not pressed
		std::string KeyName(int key_id_);
	};

}