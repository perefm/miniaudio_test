// SoundManager.h
// Spontz Demogroup

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>

#include "sound/Sound.h"

namespace Phoenix {

	class SoundManager final {

	public:
		SoundManager();
		~SoundManager();

	public:
		bool initEngine();
		void uninitEngine();
		bool setVolume(float volume);
		SP_Sound addSound(const std::string_view filePath);
		SP_Sound getSoundbyID(uint32_t id);
		void clearSounds(); // Clear all sounds
		std::string getVersion();

	private:
		ma_engine				m_engine;
		int32_t					m_LoadedSounds; // Loaded sounds
		bool					m_inited;
		
		// Resource manager
		ma_resource_manager_config	m_resourceManagerConfig;
		ma_resource_manager			m_resourceManager;

	public:
		std::vector<SP_Sound>	sound; // Sound list
		


	};
}
