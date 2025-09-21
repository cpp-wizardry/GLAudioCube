#include "AudioManager.h"
#include <fstream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <iostream>


static uint16_t read_u16_le(const char* p) { uint16_t v; std::memcpy(&v, p, sizeof(v)); return v; }

static uint32_t read_u32_le(const char* p) { uint32_t v; std::memcpy(&v, p, sizeof(v)); return v; }

AudioManager::AudioManager() : m_stream(nullptr), m_micAmplitude(0.0f), m_fileAmplitude(0.0f), m_mode(AudioMode::None){}

AudioManager::~AudioManager() { stop(); Pa_Terminate(); }

bool AudioManager::init() {
	PaError err = Pa_Initialize();
	return (err == paNoError);
}

bool AudioManager::scanWavFile(const std::string& Path)
{
	std::ifstream file(Path, std::ios::binary);
	if (!file) return false;

	char RIFF[4];
	uint32_t RIFFSize = 0;
	char wave[4];

	if (!file.read(RIFF, 4) || !file.read(reinterpret_cast<char*>(&RIFFSize), 4) || !file.read(wave, 4))
		return false;
	if (std::string(RIFF, 4) != "RIFF" || std::string(wave, 4) != "WAVE")
		return false;

	bool foundFmt = false;
	bool foundData = false;
	WAV_INFO localInfo{};

	while (file) {
		char chunkName[4];
		uint32_t chunkSize = 0;
		if (!file.read(chunkName, 4)) break;
		if (!file.read(reinterpret_cast<char*>(&chunkSize), 4)) return false;

		std::string name(chunkName, 4);

		if (name == "fmt ") {
			if (chunkSize < 16) return false;
			std::vector<char> fmtBuff(chunkSize);
			if (!file.read(fmtBuff.data(), chunkSize)) return false;

			localInfo.audioFormat = read_u16_le(fmtBuff.data() + 0);
			localInfo.nbrChannels = read_u16_le(fmtBuff.data() + 2);
			localInfo.sampleRate = read_u32_le(fmtBuff.data() + 4);
			localInfo.bitsPerSample = read_u16_le(fmtBuff.data() + 14);

			foundFmt = true;
			if (chunkSize % 2 == 1) file.seekg(1, std::ios::cur);
		}
		else if (name == "data") {
			if (!foundFmt) return false;
			if (chunkSize == 0) return false;
			if (localInfo.audioFormat != 1) return false;
			if (localInfo.bitsPerSample != 16) return false;
			if (localInfo.nbrChannels == 0) return false;

			size_t bytesPerSample = localInfo.bitsPerSample / 8;
			size_t sampleCount = chunkSize / bytesPerSample;
			m_fileSamples.resize(sampleCount);
			if (!file.read(reinterpret_cast<char*>(m_fileSamples.data()), chunkSize)) return false;
			if (chunkSize % 2 == 1) file.seekg(1, std::ios::cur);

			localInfo.dataSize = chunkSize;
			m_fileInfo = localInfo;
			foundData = true;
			break;
		}
		else {
			if (chunkSize > 0) {
				file.seekg(chunkSize, std::ios::cur);
				if (chunkSize % 2 == 1) file.seekg(1, std::ios::cur);
			}
		}
	}
	return foundData;
}


bool AudioManager::loadWavFile(std::string& Path) {
	std::lock_guard<std::mutex> lk(m_mtx);
	if (!scanWavFile(Path)) { return false; }
	m_audioData.samples = m_fileSamples.data();
	m_audioData.totalFrames = m_fileSamples.size();
	m_audioData.cursor = 0;
	m_audioData.channels = m_fileInfo.nbrChannels;
	m_mode = AudioMode::WavFile;
	return true;
}


bool AudioManager::openOutStream() {
	PaStreamParameters outParams;
	outParams.device = Pa_GetDefaultOutputDevice();
	if (outParams.device == paNoDevice) return false;
	outParams.channelCount = m_audioData.channels;
	outParams.sampleFormat = paInt16;
	outParams.suggestedLatency = Pa_GetDeviceInfo(outParams.device)->defaultLowOutputLatency;
	outParams.hostApiSpecificStreamInfo = nullptr;
	PaError err = Pa_OpenStream(&m_stream,nullptr,&outParams,m_fileInfo.sampleRate,paFramesPerBufferUnspecified,paNoFlag,outCallback,&m_audioData);
	if (err != paNoError)return false;
	err = Pa_StartStream(m_stream);
	return(err == paNoError);
}

bool AudioManager::playBack()
{
	if (m_mode != AudioMode::WavFile) return false;
	if (!openOutStream()) return false;
	return true;
}



bool AudioManager::openInStream(int deviceIndex, int channels, double sampleRate)
{
	PaStreamParameters inParams;
	inParams.device = (deviceIndex == paNoDevice) ? Pa_GetDefaultInputDevice() : deviceIndex;
	if (inParams.device == paNoDevice) return false;
	inParams.channelCount = channels;
	inParams.sampleFormat = paFloat32;
	inParams.suggestedLatency = Pa_GetDeviceInfo(deviceIndex)->defaultLowInputLatency;
	inParams.hostApiSpecificStreamInfo = nullptr;
	PaError err = Pa_OpenStream(&m_stream,&inParams,nullptr,sampleRate,256,paClipOff,inCallback,this);
	if (err != paNoError) return false;
	err = Pa_StartStream(m_stream);
	return (err == paNoError);
}


bool AudioManager::startMicrophone(int deviceIndex, int channels, double sampleRate)
{
	std::lock_guard<std::mutex> lk(m_mtx);
	m_mode = AudioMode::Micro;
	m_micAmplitude = 0.0f;
	return openInStream(deviceIndex, channels, sampleRate);
}

void AudioManager::stop()
{
	std::lock_guard<std::mutex> lk(m_mtx);
	if (m_stream)
	{
		Pa_StopStream(m_stream);
		Pa_CloseStream(m_stream);
		m_stream = nullptr;
	}
	m_mode = AudioMode::None;
}

float AudioManager::getAmplitude(size_t chunkSize)
{
	if (m_mode == AudioMode::Micro)
	{
		return m_micAmplitude.load();
	}
	else if (m_mode == AudioMode::WavFile)
	{
		std::lock_guard<std::mutex> lk(m_mtx);
		size_t cursor = m_audioData.cursor;
		float rms = normalizeData(cursor,chunkSize);
		return rms;
	}
	return 0.0f;
}



int AudioManager::outCallback(const void*, void* output, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData)
{
	AudioData* audio = reinterpret_cast<AudioData*>(userData);
	int16_t* out = reinterpret_cast<int16_t*>(output);
	size_t framesRequested = framesPerBuffer * audio->channels;
	size_t remaining = (audio->totalFrames > audio->cursor) ? (audio->totalFrames - audio->cursor) : 0;
	size_t toCopy = std::min(remaining, framesRequested);
	if (toCopy > 0) {
		std::memcpy(out, audio->samples + audio->cursor, toCopy * sizeof(int16_t));
	}
	if (toCopy < framesRequested) {
		std::memset(out + toCopy, 0, (framesRequested - toCopy) * sizeof(int16_t));
	}
	audio->cursor += toCopy;
	if (audio->cursor >= audio->totalFrames) return paComplete;
	return paContinue;
}

int AudioManager::inCallback(const void* inBuffer, void* outBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statFlag, void* userData)
{
	AudioManager* mgr = reinterpret_cast<AudioManager*>(userData);
	const float* in = reinterpret_cast<const float*>(inBuffer);
	if (!in) {
		mgr->m_micAmplitude.store(0.0f);
		return paContinue;
	}
	double sum = 0.0;
	for (unsigned long i = 0; i < framesPerBuffer; ++i) {
		double v = static_cast<double>(in[i]);
		sum += v * v;
	}
	float rms = static_cast<float>(std::sqrt(sum / framesPerBuffer));
	mgr->m_micAmplitude.store(rms);
	return paContinue;
}




float AudioManager::normalizeData(size_t offset, size_t chunkSize)
{
	if (m_fileSamples.empty() || offset >= m_fileSamples.size()) return 0.0f;
	size_t available = m_fileSamples.size() - offset;
	size_t processed = std::min(available, chunkSize);
	if (processed == 0) return 0.0f;
	double sumSq = 0.0;
	for (size_t i = 0; i < processed; ++i) {
		double normalized = static_cast<double>(m_fileSamples[offset + i]) / 32768.0;
		sumSq += normalized * normalized;
	}
	return static_cast<float>(std::sqrt(sumSq / static_cast<double>(processed)));
}




