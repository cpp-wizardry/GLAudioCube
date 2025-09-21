#pragma once
#include "AudioManager.h"


struct AppContext {
    unsigned int VBO;
    unsigned int VAO;
    unsigned int texture;
    size_t vertexCount;
    AudioManager* audio;
    std::string currentWavPath;
};
