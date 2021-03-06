#pragma once

#include <memory>

#include "environment/EnvironmentMain.h"
#include "environment/InputMain.h"
#include "graphics/GraphicsMain.h"
#include "sound/SoundMain.h"

namespace engine
{

	constexpr int EXIT_CODE_OK = 0;
	constexpr int EXIT_CODE_FAILURE = -1;

	class Engine
	{
		// VARIABLES
	private:
		bool m_initialized = false;
		std::shared_ptr<graphics::GraphicsManager> m_graphics_manager;
		std::shared_ptr<sound::SoundManager> m_sound_manager;
		std::shared_ptr<environment::EnvironmentManager> m_environment_manager;

		std::string m_app_name;
		bool m_should_finish = false;

		// CONSTRUCTORS/DESTRUCTORS
	public:
		Engine(const std::string& app_name_) : m_app_name(app_name_) { Initiailize(); }
		Engine() : m_app_name("default app") { Initiailize(); }
		virtual ~Engine() { /* Shutdown(); */ };

		// METHODES
	private:
		void Initiailize();
		void Shutdown();

	protected:
		virtual void FrameAction() = 0;

		std::shared_ptr<graphics::GraphicsManager> Graphics() { return m_graphics_manager; }
		std::shared_ptr<sound::SoundManager> Sound() { return m_sound_manager; }
		std::shared_ptr<environment::EnvironmentManager> Environment() { return m_environment_manager; }

		std::string AppName() { return m_app_name; }

		void SetShouldFinish(bool state_) { m_should_finish = state_; }

	public:
		int Loop();
	};

}