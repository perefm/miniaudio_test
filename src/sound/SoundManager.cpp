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

		// Setup FFT variables
		memset(m_sampleBuf, 0, sizeof(float) * FFT_SIZE * 2);
		m_fftcfg = kiss_fftr_alloc(FFT_SIZE * 2, false, NULL, NULL);
		memset(m_fftBuffer, 0, sizeof(float) * FFT_SIZE);
		for (int32_t i = 0; i < FFT_SIZE; i++) {
			m_fftFrequencies[i] = static_cast<float>(i) * (SAMPLE_RATE / FFT_SIZE);
		}

	}

	SoundManager::~SoundManager()
	{
		clearSounds();
	}
	
	bool SoundManager::initEngine()
	{
		ma_result result;
		

		if (!m_inited) {

			// We're going to want a context so we can enumerate our playback devices
			result = ma_context_init(NULL, 0, NULL, &m_context);
			if (result != MA_SUCCESS) {
				printf("Failed to initialize context.");
				return false;
			}

			// Now that we have a context we will want to enumerate over each device so we can display them to the user and give
			// them a chance to select the output devices they want to use.
			result = ma_context_get_devices(&m_context, &m_pPlaybackDeviceInfos, &m_playbackDeviceCount, NULL, NULL);
			if (result != MA_SUCCESS) {
				printf("Failed to enumerate playback devices.");
				ma_context_uninit(&m_context);
				return false;
			}

			// Setup Device
			ma_device_config deviceConfig;
			/*
			Create the device first before the engine. We'll specify the device in the engine's config. This is optional. When a device is
			not pre-initialized the engine will create one for you internally. The device does not need to be started here - the engine will
			do that for us in ma_engine_start(). The device's format is derived from the resource manager, but can be whatever you want.
			It's useful to keep the format consistent with the resource manager to avoid data conversions costs in the audio callback. In
			this example we're using the resource manager's sample format and sample rate, but leaving the channel count set to the device's
			native channels. You can use whatever format/channels/rate you like.
			*/
			deviceConfig = ma_device_config_init(ma_device_type_playback);
			deviceConfig.playback.pDeviceID = &m_pPlaybackDeviceInfos[0].id;
			deviceConfig.playback.format = SAMPLE_FORMAT;
			deviceConfig.playback.channels = CHANNEL_COUNT;
			deviceConfig.sampleRate = SAMPLE_RATE;
			deviceConfig.dataCallback = dataCallback;
			deviceConfig.pUserData = this;

			result = ma_device_init(&m_context, &deviceConfig, &m_device);
			if (result != MA_SUCCESS) {
				printf("Failed to initialize device.\n");
				return false;
			}

			// Setup Engine
			ma_engine_config engineConfig;
			// Now that we have the device we can initialize the engine. The device is passed into the engine's config
			engineConfig = ma_engine_config_init();
			engineConfig.pDevice = &m_device;
			engineConfig.noAutoStart = MA_TRUE;    // Don't start the engine by default - we'll do that manually below

			// Sound engine Init
			result = ma_engine_init(&engineConfig, &m_engine); // TODO: Can we init engione before or after the Resource manager? or is not even needed?
			if (result != MA_SUCCESS) {
				m_inited = false;
			}
			else
				m_inited = true;

			result = ma_engine_start(&m_engine);
			if (result != MA_SUCCESS) {
				printf("WARNING: Failed to start engine.\n");
			}

		}
		return m_inited;
	}

	void SoundManager::uninitEngine()
	{
		ma_engine_uninit(&m_engine);
		ma_device_uninit(&m_device);
		ma_context_uninit(&m_context);
	}

	bool SoundManager::setMasterVolume(float volume)
	{
		if (m_inited) {
			ma_result result;
			result = ma_device_set_master_volume(&m_device, volume);
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

	void SoundManager::enumerateDevices()
	{
		ma_result result;
		ma_context context;
		ma_device_info* pPlaybackDeviceInfos;
		ma_uint32 playbackDeviceCount;
		ma_device_info* pCaptureDeviceInfos;
		ma_uint32 captureDeviceCount;
		ma_uint32 iDevice;

		if (ma_context_init(NULL, 0, NULL, &context) != MA_SUCCESS) {
			printf("Failed to initialize context.\n");
			return;
		}

		result = ma_context_get_devices(&context, &pPlaybackDeviceInfos, &playbackDeviceCount, &pCaptureDeviceInfos, &captureDeviceCount);
		if (result != MA_SUCCESS) {
			printf("Failed to retrieve device information.\n");
			return;
		}

		printf("Playback Devices\n");
		for (iDevice = 0; iDevice < playbackDeviceCount; ++iDevice) {
			printf("    %u: %s\n", iDevice, pPlaybackDeviceInfos[iDevice].name);
		}

		printf("\n");

		printf("Capture Devices\n");
		for (iDevice = 0; iDevice < captureDeviceCount; ++iDevice) {
			printf("    %u: %s\n", iDevice, pCaptureDeviceInfos[iDevice].name);
		}


		ma_context_uninit(&context);

	}

	void SoundManager::dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
	{
		//(void)pInput;
		//ma_engine_read_pcm_frames((ma_engine*)pDevice->pUserData, pOutput, frameCount, NULL);

		(void)pInput;

		float* pOutputF32 = (float*)pOutput;
		SoundManager* p_sm = (SoundManager*)pDevice->pUserData;

		ma_engine_read_pcm_frames(&(p_sm->m_engine), pOutput, frameCount, NULL);
		
		// Fill the sampleBuffer for the FFT analysis
		frameCount = frameCount < FFT_SIZE * 2 ? frameCount : FFT_SIZE * 2;

		// Just rotate the buffer; copy existing, append new - https://github.com/Gargaj/Bonzomatic/blob/master/src/platform_common/FFT.cpp
		const float* samples = (const float*)pOutputF32;
		if (samples) {
			float* p_sample = p_sm->m_sampleBuf;
			for (uint32_t i = frameCount; i < (FFT_SIZE * 2); i++) {
				*(p_sample++) = p_sm->m_sampleBuf[i];
			}
			for (uint32_t i = 0; i < frameCount; i++) {
				*(p_sample++) = (samples[i * 2] + samples[i * 2 + 1]) / 2.0f * p_sm->m_fAmplification;
			}
		}
	}

	bool SoundManager::performFFT()
	{
		if (!m_inited) {
			return false;
		}

		kiss_fft_cpx out[FFT_SIZE + 1];			// FFT complex output
		kiss_fftr(m_fftcfg, m_sampleBuf, out);


		m_lowFreqSum = 0.0f;
		m_midFreqSum = 0.0f;
		m_highFreqSum = 0.0f;

		for (uint32_t i = 0; i < FFT_SIZE; i++)
		{
			// Calculate the FFT buffer
			static const float scaling = 1.0f / (float)FFT_SIZE;
			m_fftBuffer[i] = 2.0f * sqrtf(out[i].r * out[i].r + out[i].i * out[i].i) * scaling;

			// Calculate the maximum value of the Low, Medium and High frequencies
			if (m_fftFrequencies[i] <= m_lowFreqMax) {
				m_lowFreqSum += m_fftBuffer[i];
			}
			else if (m_fftFrequencies[i] <= m_midFreqMax) {
				m_midFreqSum += m_fftBuffer[i];
			}
			else {
				m_highFreqSum += m_fftBuffer[i];
			}
		}

		return true;
	}

}
