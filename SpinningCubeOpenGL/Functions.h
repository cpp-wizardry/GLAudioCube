#pragma once
#include "stdafx.h"
#include "wavHead.h"
#include "AppCon.h"
using vertex = std::vector<std::vector<float>>;
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
	float rSpeed = 0.05f;

};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);


void processInputs(GLFWwindow* window, Cube& cube, AppContext& ctx);

void ListAudioDevice();
std::string openFileDialog(unsigned int MODE);

bool initialize(GLFWwindow*& window);


bool crashDumpWav(const WAV_HEADER& header, const std::string& filePath);

