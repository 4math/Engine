#pragma once

#include "GenericGame.h"
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
		GenericGame* m_game = nullptr;
		graphics::GraphicsManager* m_graphics_manager = nullptr;
		sound::SoundManager* m_sound_manager = nullptr;
		environment::EnvironmentManager* m_environment_manager = nullptr;

		// CONSTRUCTORS/DESTRUCTORS
	public:
		Engine(GenericGame* game_) : m_game(game_) { Initiailize(); };
		~Engine() { Shutdown(); };

		// METHODES
	private:
		void Initiailize();
		void Shutdown();

	public:
		int Loop();
	};

}