#include "InputMain.h"

void environment::InputManager::Initialize()
{
	if (m_window == nullptr)
		return;

	glfwSetWindowUserPointer(m_window, this);
	glfwSetKeyCallback(m_window, KeyCallback);

	m_initialized = true;
}

void environment::InputManager::Shutdown()
{
	glfwSetWindowUserPointer(m_window, nullptr);
	m_initialized = false;
}

bool environment::InputManager::KeyPressed(int key_id_)
{
	return m_key_map[key_id_];
}

void environment::InputManager::KeyCallback(GLFWwindow* window_, int key_id_, int scancode_, int action_, int mods_)
{
	InputManager* input_manager = (InputManager*)glfwGetWindowUserPointer(window_);

	if (action_ == GLFW_PRESS)
		input_manager->SetKeyState(key_id_, true);
	else if (action_ == GLFW_RELEASE)
		input_manager->SetKeyState(key_id_, false);
}

void environment::InputManager::SetKeyState(int key_id_, bool pressed_)
{
	m_key_map[key_id_] = pressed_;
}

std::string environment::InputManager::KeyName(int key_id_)
{
	return std::string(glfwGetKeyName(key_id_, 0));
}