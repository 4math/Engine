#pragma once

#include <string>
#include <exception>

#include "../environment/EnvironmentMain.h"

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
		environment::EnvironmentManager* m_environment_manager = nullptr;

		// CONSTRUCTORS/DESTRUCTORS
	public:
		GraphicsManager(environment::EnvironmentManager* environment_manager_) : 
			m_environment_manager(environment_manager_) { Initialize(); }
		~GraphicsManager() { Shutdown(); }

		// METHODES
	private:
		void Initialize();
		void Shutdown();
	};
}
