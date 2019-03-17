#pragma once

#include <string>

namespace engine
{

#include "graphics/GraphicsMain.h"
#include "input/InputMain.h"
#include "sound/SoundMain.h"

	constexpr int EXIT_CODE_OK = 0;
	constexpr int EXIT_CODE_FAILURE = -1;

	class Engine
	{
		// VARIABLES
	private:
		bool m_initialized = false;
		std::string m_game_name;


		// CONSTRUCTORS/DESTRUCTORS
	public:
		Engine(const std::string& game_name_) : m_game_name(game_name_) { Initiailize(); };
		~Engine() { Shutdown(); };


		// METHODES
	private:
		void Initiailize();
		void Shutdown();

	public:
		int Loop();

	};

}