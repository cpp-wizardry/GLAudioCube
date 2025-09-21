#pragma once
#include "stdafx.h"    





struct AudioData {
    const int16_t* samples; // pointer vers data stocké
    size_t totalFrames;     // nmbre de samples totaux
    size_t cursor;          // position dans l'array data
    int channels;           // nmbre de chaine 1 = mono 2 = stereo
};
