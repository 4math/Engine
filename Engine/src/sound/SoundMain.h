#pragma once

#include <string>

#include "bass.h"

namespace sound
{
	struct Sound
	{
		std::string file_name;
		HSTREAM stream_handle;
	};

	class SoundManager
	{
		// VARIABLES
	private:
		bool m_initialized = false;

		// CONSTRUCTORS/DESTRUCTORS
	public:
		SoundManager() { Initialize(); }
		~SoundManager() { Shutdown(); }

		// METHODES
	private:
		void Initialize();
		void Shutdown();

	public:
		void Play(Sound& sound_);
		void Stop(Sound& sound_);
		void SetVolume(float volume_); // Volume range from 0 (no sound) to 1 (maximum)
	};
}