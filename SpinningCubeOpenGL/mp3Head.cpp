#include "mp3Head.h"

std::vector<bool> flagCheck(uint8_t flags) {
    std::vector<bool> out(4);
    out[Unsync] = flags & 0x80;
    out[ExtHeader] = flags & 0x40;
    out[Experiment] = flags & 0x20;
    out[Footer] = flags & 0x10;
    return out;
};



int getFrameSize(uint32_t header) {
    int versionID = (header >> 19) & 0x3;
    int layer = (header >> 17) & 0x3;
    int bitrateID = (header >> 12) & 0xF;
    int freqID = (header >> 10) & 0x3;
    int padding = (header >> 9) & 0x1;

    static const int bitrates[2][16] = {
        // MPEG1 Layer III
        {0,32,40,48,56,64,80,96,112,128,160,192,224,256,320,0},
        // MPEG2/2.5 Layer III
        {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,0}
    };

    static const int sampleRates[3][4] = {
        {44100, 48000, 32000, 0}, // MPEG1
        {22050, 24000, 16000, 0}, // MPEG2
        {11025, 12000, 8000, 0}   // MPEG2.5
    };

    int versionIndex;
    if (versionID == 3) versionIndex = 0;     // MPEG1
    else if (versionID == 2) versionIndex = 1;// MPEG2
    else if (versionID == 0) versionIndex = 2;// MPEG2.5
    else return -1;

    if (layer != 1) return -1; // only Layer 3

    int bitrate = bitrates[(versionIndex == 0) ? 0 : 1][bitrateID] * 1000;
    int sampleRate = sampleRates[versionIndex][freqID];

    if (bitrate == 0 || sampleRate == 0) return -1;

    int frameSize;
    if (versionIndex == 0) { // MPEG1
        frameSize = 144 * bitrate / sampleRate + padding;
    }
    else {
        frameSize = 72 * bitrate / sampleRate + padding;
    }

    return frameSize;
}
