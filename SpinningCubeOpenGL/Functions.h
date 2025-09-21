#pragma once
#include "stdafx.h"
#include "wavHead.h"
#include "audioData.h"

struct RGB {
	float red = 0.0f;
	float green = 0.0f;
	float blue = 0.0f;
};

struct Cube
{
	float rotX = 0.0f;
	float rotY = 0.0f;
	float rotZ = 0.0f;
	float scale = 0.5f;
	RGB color;
	unsigned int Tex;

};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

unsigned int compileShader(unsigned int type, const char* source);
void processInputs(GLFWwindow* window, Cube& r,std::vector<int16_t> data);

unsigned int loadTexture(const char* filename);

void ListAudioDevice();
std::string openFileDialog(unsigned int MODE);

bool initialize(GLFWwindow*& window);

bool loadAudio(const char* path, WAV_HEADER& header, std::vector<int16_t>& data);
float normalizeAudioData(const std::vector<int16_t>& samples, size_t offset, size_t chunkSize);
PaStream* playBack(const std::vector<int16_t>& data, const WAV_HEADER& header, AudioData* audioData);

bool crashDumpWav(const WAV_HEADER& header, const std::string& filePath);
