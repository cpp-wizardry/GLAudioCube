#pragma once
#include <string>
#include <vector>
#include <atomic>
#include <cstdint>
#include <mutex>
#include <portaudio.h>



struct WAV_INFO
{
	uint16_t audioFormat = 0;
	uint16_t nbrChannels = 0;
	uint32_t sampleRate = 0;
	uint16_t bitsPerSample = 0;
	uint32_t dataSize = 0;
};

struct AudioData {
	const int16_t* samples = nullptr; // pointer vers data stocké
	size_t totalFrames;     // nmbre de samples totaux
	size_t cursor;          // position dans l'array data
	int channels;           // nmbre de chaine 1 = mono 2 = stereo
};

enum AudioMode { None, Micro,WavFile };
enum AudioFileFormat { Wav, Mp3, Ma3, };



class AudioManager
{
public:
	AudioManager();
	~AudioManager();
	bool init();
	bool loadWavFile(std::string &Path);
	bool playBack();
	bool startMicrophone(int deviceIndex = paNoDevice, int channels = 1, double sampleRate = 44100);
	void stop();
	float getAmplitude(size_t chunkSize);
	AudioMode getMode() const { return m_mode; }
	const WAV_INFO& fileInfo()const { return m_fileInfo; }

private:
	std::vector<int16_t> m_fileSamples;
	WAV_INFO m_fileInfo;
	AudioData m_audioData;
	PaStream* m_stream;
	std::atomic<float> m_micAmplitude;
	std::atomic<float> m_fileAmplitude;
	std::mutex m_mtx;
	AudioMode m_mode;

	bool openOutStream();
	bool openInStream(int deviceIndex, int channels, double sampleRate);
	
	static int outCallback(const void*, void* output, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void* userData);
	static int inCallback(const void* inBuffer, void* outBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statFlag, void* userData);
	
	bool scanWavFile(const std::string& Path);


	float normalizeData(size_t offset, size_t chunkSize);
};
