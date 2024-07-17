// SoundManager.h
// Spontz Demogroup

#pragma once

#include <string>
#include <string_view>
#include <vector>
#include <memory>

#include <kiss_fft.h>
#include <kiss_fftr.h>

#include "sound/Sound.h"

namespace Phoenix {

	#define FFT_SIZE 1024
	#define CHANNEL_COUNT 2
	#define SAMPLE_RATE 44100
	#define SAMPLE_FORMAT ma_format_f32

	class SoundManager final {

	public:
		SoundManager();
		~SoundManager();

	public:
		bool initEngine();
		void uninitEngine();
		bool setMasterVolume(float volume);
		SP_Sound addSound(const std::string_view filePath);
		SP_Sound getSoundbyID(uint32_t id);
		void clearSounds(); // Clear all sounds
		std::string getVersion();

		void enumerateDevices();

	private:
		static void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

	public:
		bool performFFT();

	private:
		ma_engine					m_engine;
		int32_t						m_LoadedSounds; // Loaded sounds
		bool						m_inited;

		// Context
		ma_context					m_context;
		// Devices
		ma_device_info*				m_pPlaybackDeviceInfos;
		ma_uint32					m_playbackDeviceCount;
		ma_device					m_device;

		// FFT capture and analysis
		kiss_fftr_cfg				m_fftcfg;
		float						m_sampleBuf[FFT_SIZE * 2];
		float						m_fAmplification = 1.0f;

		// Group magnitudes into low, mid, and high frequency bands
		float			m_lowFreqMax = 400.0f;	// Low frequency max value
		float			m_midFreqMax = 2000.0f;	// Mid frequency max value

	public:
		float			m_fftBuffer[FFT_SIZE];		// FFT magnitues
		float			m_fftFrequencies[FFT_SIZE];	// FFT frequencies analyzed
		float			m_lowFreqSum = 0.0f;
		float			m_midFreqSum = 0.0f;
		float			m_highFreqSum = 0.0f;

	public:
		std::vector<SP_Sound>	sound; // Sound list

	};
}
