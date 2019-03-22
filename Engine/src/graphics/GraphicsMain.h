#pragma once

#include "../environment/EnvironmentMain.h"

namespace graphics
{

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

	public:
		void BeginFrame();
		void EndFrame();
	};

}
