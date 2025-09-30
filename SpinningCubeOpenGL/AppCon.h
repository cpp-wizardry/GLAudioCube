#pragma once
#include "AudioManager.h"
#include "3DModelLoader.h"

struct AppContext {
    std::vector<unsigned int> VBOs;//changed it so I can have multiple independent objects
    std::vector<unsigned int> VAOs;
    unsigned int texture;
    std::vector<size_t> vertexCounts;
    AudioManager* audio;
    std::string currentAudioPath;
    Manager3D Mng3D;
};
