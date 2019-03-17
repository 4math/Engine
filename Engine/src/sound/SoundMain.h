#pragma once

#include <string>
#include <list>
#include <thread>

#pragma comment(lib, "Winmm.lib")
#include <Windows.h>

namespace sound
{
	struct Sound
	{
		std::string file_name;
		float volume;
	};

	class SoundManager
	{
		// VARIABLES
	private:
		bool m_initialized = false;
		std::list<std::thread*> m_sound_threads;

		// CONSTRUCTORS/DESTRUCTORS
	public:
		SoundManager() { Initialize(); }
		~SoundManager() { Shutdown(); }

		// METHODES
	private:
		void Initialize();
		void Shutdown();
		void PlaySoundThread(Sound sound_);

	public:
		void Play(const std::string& sound_file_name_);
	};
}