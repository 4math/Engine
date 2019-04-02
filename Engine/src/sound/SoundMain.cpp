#include "SoundMain.h"

void sound::SoundManager::Initialize()
{
	if (!BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL))
	{
		m_initialized = false;
		return;
	}
	
	m_initialized = true;
}

void sound::SoundManager::Shutdown()
{
	BASS_Stop();
	BASS_Free();
}

void sound::SoundManager::Play(Sound& sound_)
{
	if (!m_initialized)
		return;

	HSTREAM hstream = BASS_StreamCreateFile(FALSE, sound_.file_name.c_str(), 0, 0, 0);
	BASS_ChannelPlay(hstream, FALSE);
	sound_.stream_handle = hstream;
}

void sound::SoundManager::Stop(Sound& sound_)
{
	if (!m_initialized)
		return;

	BASS_ChannelStop(sound_.stream_handle);
}

void sound::SoundManager::SetVolume(float volume_)
{
	if (!m_initialized)
		return;

	BASS_SetVolume(volume_);
}
