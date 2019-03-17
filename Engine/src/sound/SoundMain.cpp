#include "SoundMain.h"

using namespace sound;

void SoundManager::Initialize()
{
	m_initialized = true;
}

void SoundManager::Shutdown()
{
	for (auto sound_thread : m_sound_threads)
		sound_thread->join();
}

void SoundManager::PlaySoundThread(Sound sound_)
{
	DWORD sound_playing_flags = 0;
	sound_playing_flags |= SND_FILENAME;
	sound_playing_flags |= SND_ASYNC;

	PlaySoundA(sound_.file_name.c_str(), NULL, sound_playing_flags);
}

void SoundManager::Play(const std::string& sound_file_name_)
{
	Sound temp_sound = { sound_file_name_, 1 };
	std::thread* sound_thread = new std::thread(&SoundManager::PlaySoundThread, this, temp_sound);
	m_sound_threads.push_back(sound_thread);
}
