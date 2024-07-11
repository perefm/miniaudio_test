// SoundManager.cpp
// Spontz Demogroup

#include "main.h"
#include "sound/SoundManager.h"

namespace Phoenix {
	SoundManager::SoundManager()
	{
		ma_result result;

		sound.clear();
		m_LoadedSounds = 0;
		m_inited = false;

		// Resource manager initialization
		m_resourceManagerConfig = ma_resource_manager_config_init();
		m_resourceManagerConfig.decodedFormat = ma_format_f32;	// ma_format_f32 should almost always be used as that's what the engine (and most everything else) uses for mixing
		m_resourceManagerConfig.decodedChannels = 0;			// Setting the channel count to 0 will cause sounds to use their native channel count
		m_resourceManagerConfig.decodedSampleRate = 48000;		// Using a consistent sample rate is useful for avoiding expensive resampling in the audio thread. This will result in resampling being performed by the loading thread(s)

		result = ma_resource_manager_init(&m_resourceManagerConfig, &m_resourceManager);
		if (result != MA_SUCCESS) {
			printf("\nFailed to initialize the resource manager.");
		}
	}

	SoundManager::~SoundManager()
	{
		clearSounds();
		ma_resource_manager_uninit(&m_resourceManager);
	}
	
	bool SoundManager::initEngine()
	{
		if (!m_inited) {
			ma_result result;
			// Sound engine Init
			result = ma_engine_init(NULL, &m_engine); // TODO: Can we init engione before or after the Resource manager? or is not even needed?
			if (result != MA_SUCCESS) {
				m_inited = false;
			}
			else
				m_inited = true;
		}
		return m_inited;
	}

	void SoundManager::uninitEngine()
	{
		ma_engine_uninit(&m_engine);
	}

	bool SoundManager::setVolume(float volume)
	{
		if (m_inited) {
			ma_result result;
			result = ma_engine_set_volume(&m_engine, volume);
			if (result != MA_SUCCESS)
				return false;
		}
		return true;
	}

	SP_Sound SoundManager::addSound(const std::string_view filePath)
	{
		SP_Sound p_sound;

		// check if sound is already loaded, then we just retrieve the ID of our sound
		for (auto const& m_sound : sound) {
			if (m_sound->filePath.compare(filePath) == 0) {
				p_sound = m_sound;
			}
		}

		if (p_sound == nullptr) {
			SP_Sound new_sound = std::make_shared<Sound>();
			if (new_sound->loadSoundFile(filePath, m_engine)) {
				sound.push_back(new_sound);
				printf("\nSound %s loaded OK", filePath.data());
				m_LoadedSounds++;
				p_sound = new_sound;
			}
			else {
				printf("\nCould not load song: %s", filePath.data());
			}
		}
		return p_sound;
		
	}

	SP_Sound SoundManager::getSoundbyID(uint32_t id)
	{
		if (id >= sound.size())
			return nullptr;
		else
			return sound[id];
	}

	void SoundManager::clearSounds()
	{
		sound.clear();
		m_LoadedSounds = 0;
	}

	std::string SoundManager::getVersion()
	{
		std::string ma_version;
		ma_version = MA_VERSION_STRING;
		return ma_version;
	}

}
