#pragma once

#include <string>;
#include <exception>

#include "environment/EnvironmentMain.h"

namespace graphics
{
#pragma region exceptions and EXIT_CODES
	struct bad_header : public std::exception {
		const char * what() const throw () {
			return "Header file EnvironmentMain.h is missing or damaged";
		}
	};

	constexpr int EXIT_CODE_OK = 0;
	constexpr int EXIT_CODE_FAILURE = -1;
#pragma endregion

	class GraphicsManager
	{
		// VARIABLES
	private:
		bool m_initialized = false;
		std::string m_process_name;
		environment::EnvironmentManager* m_env = nullptr;
	public:


		// CONSTRUCTORS/DESTRUCTORS
	public:
		GraphicsManager(std::string &process_name_) : m_process_name(process_name_) { Initialize(process_name_); }
		~GraphicsManager() { Shutdow(); }

		// METHODES
	private:
		void Initialize(std::string &process_name_);
		void Shutdow();
	public:
		int CreateGlfwWindow(environment::WindowType type_, int width_ = 0, int height_ = 0);
		int DestroyGlfwWindow();
	};
}
