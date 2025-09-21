#include "Functions.h"
#include "GlobalVar.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

//void processInput(GLFWwindow* window)
//{
    //if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        //glfwSetWindowShouldClose(window, true);
//}


unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(shader, 512, NULL, log);
        std::cerr << "Shader compilation error:\n" << log << std::endl;
    }
    return shader;
}


bool initialize(GLFWwindow*& window) {
    if (!glfwInit()) return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 600, "Spinning Cube", NULL, NULL);
    if (!window) return false;

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return false;
    if (Pa_Initialize() != paNoError) return false;

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    return true;
}


unsigned int currAudioDevice = 0;
static int audioCallback(const void* inputBuffer, void*,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo*,
    PaStreamCallbackFlags,
    void*) {
    const float* in = (const float*)inputBuffer;
    float sum = 0.0f;

    if (in != nullptr) {
        for (unsigned long i = 0; i < framesPerBuffer; i++) {
            sum += in[i] * in[i];
        }
        gAmplitude = sqrt(sum / framesPerBuffer);
        std::cout << "Amplitude: " << gAmplitude << "\n";
    }


    return paContinue;
}
PaStream* stream;
int initAudio() {
    PaError err = Pa_Initialize();

    if (err != paNoError) return -1;

    PaStreamParameters inputParams;
    inputParams.device = currAudioDevice;
    inputParams.channelCount = 1;
    inputParams.sampleFormat = paFloat32;
    inputParams.suggestedLatency =
        Pa_GetDeviceInfo(inputParams.device)->defaultLowInputLatency;
    inputParams.hostApiSpecificStreamInfo = nullptr;

    err = Pa_OpenStream(&stream,
        &inputParams,
        nullptr,
        44100,
        256,
        paClipOff,
        audioCallback,
        nullptr);


    if (err != paNoError) return -1;
    Pa_StartStream(stream);
    return 0;
}

void processInputs(GLFWwindow* window,Cube& r, std::vector<int16_t> data) {
    //rotation speed; Might move it to a struct later 
    float rSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    //Rotate cube on X axis
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) r.rotX += rSpeed; 
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) r.rotX -= rSpeed;
        
    //Rotate cube on Y axis
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) r.rotY -= rSpeed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) r.rotY += rSpeed;
  
    //Rotate cube on Z axis
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) r.rotZ -= rSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) r.rotZ += rSpeed;
       
    //Change cube scale
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) r.scale -= 0.01f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) r.scale += 0.01f;
    
    //Reset cube's size and rotation
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) { r.rotX,r.rotY,r.rotZ,r.scale = 0; }

    
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        ListAudioDevice(); Sleep(1000);
    }
    if (glfwGetKey(window,GLFW_KEY_N) == GLFW_PRESS)
    {
        int nextDevice = currAudioDevice;
        nextDevice++;
        if (nextDevice >= Pa_GetDeviceCount()) {
            nextDevice = 0;
        }
        if (stream) {
            if (Pa_IsStreamActive(stream)) Pa_StopStream(stream);
            Pa_CloseStream(stream);
            stream = nullptr;
        }
            currAudioDevice = nextDevice;
            std::cout << "Current Device index: " << currAudioDevice << " name: " << Pa_GetDeviceInfo(currAudioDevice)->name << "\n";
            
            if (initAudio() != 0) {
                std::cerr << "Opening audio device failed \n";
            }
        
        Sleep(200);
    }

    if (glfwGetKey(window,GLFW_KEY_L == GLFW_PRESS))
    {
        for (size_t i = 0; i < data.size(); i++)
        {
            std::cout << data[i];
        }
    }
    
    //Cube scale boundaries
    if (r.scale < 0.1f) r.scale = 0.1f;
    if (r.scale > 3.0f) r.scale = 3.0f;
};  
float gAmplitude = 0.0f;


void ListAudioDevice() {
    int numDevices = Pa_GetDeviceCount();
    const PaDeviceInfo* deviceInfo;

    for (int i = 0; i < numDevices; i++) {
        deviceInfo = Pa_GetDeviceInfo(i);
        std::cout << "Device #" << i << ": " << deviceInfo->name << " (Max input channels: " << deviceInfo->maxInputChannels << ")\n";
    }
}

std::string openFileDialog(unsigned int MODE) {
    char filename[MAX_PATH] = "";
    OPENFILENAMEA ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    switch (MODE)
    {
        case 1: 
            ofn.lpstrFilter = "Image Files\0*.png;*.jpg;*.jpeg\0All Files\0*.*\0";
            break;
        case 2:
            ofn.lpstrFilter = "Audio Files\0*.Wav\0";
            break;
    default:
        break;
    }
    
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameA(&ofn)) {
        return filename;
    }
    return "";
}

int getFileSize(FILE* inFile) {
    int fileSize = 0;
    fseek(inFile, 0, SEEK_END);

    fileSize = ftell(inFile);

    fseek(inFile, 0, SEEK_SET);
    return fileSize;
}


bool crashDumpWav(const WAV_HEADER& header, const std::string& filePath) {

    std::ofstream note("wav_info.txt");
    if (!note) {
        std::cerr << "Failed to open wav_info.txt for writing\n";
        return false;
    }

    note << "=== WAV FILE INFO ===\n";
    note << "File: " << filePath << "\n\n";

    note << "RIFF: " << std::string(header.RIFF, 4) << "\n";
    note << "File size: " << header.fileSize << " bytes\n";
    note << "WAVE: " << std::string(header.WAVE, 4) << "\n";

    note << "Format chunk: " << std::string(header.fmt, 4) << "\n";
    note << "Format length: " << header.dataLength << "\n";
    note << "Format type: " << header.Info.audioFormat
        << (header.Info.audioFormat == 1 ? " (PCM)" : header.Info.audioFormat == 3 ? " (IEEE float)" : " (other)") << "\n";
    note << "Channels: " << header.Info.nbrChannels << "\n";
    note << "Sample rate: " << header.Info.sampleRate << " Hz\n";
    note << "Byte rate: " << header.byteRate << " B/s\n";
    note << "Block align: " << header.bytePerBloc << " bytes\n";
    note << "Bits per sample: " << header.Info.bitsPerSample << "\n";

    note << "Data subchunk: " << std::string(reinterpret_cast<const char*>(&header.dataSubChunk), 4) << "\n";
    note << "Data size: " << header.Info.dataSize << " bytes\n";

    note.close();

    return true;
}

bool loadAudio(const char* Path, WAV_HEADER& header, std::vector<int16_t>& data) {
    std::ifstream file(Path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << Path << "\n";
        return false;
    }

    char riff[4];
    uint32_t riffSize = 0;
    char wave[4];
    if (!file.read(riff, 4) || !file.read(reinterpret_cast<char*>(&riffSize), 4) || !file.read(wave, 4)) {
        std::cerr << "failed to read RIFF header\n";
        return false;
    }

    if (std::string(riff, 4) != "RIFF" || std::string(wave, 4) != "WAVE") {
        std::cerr << "Invalid WAV file\n";
        return false;
    }

    bool foundFmt = false;
    bool foundData = false;
    WAV_INFO localInfo;

    while (file) {
        char chunkName[4];
        uint32_t chunkSize = 0;
        if (!file.read(chunkName, 4)) break;
        if (!file.read(reinterpret_cast<char*>(&chunkSize), 4)) return false;

        std::string name(chunkName, 4);
        std::cout << "found chunk " << name << " size: " << chunkSize << "\n";

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
            if (bytesPerSample == 0) return false;
            if (chunkSize % bytesPerSample != 0) return false;

            size_t sampleCount = chunkSize / bytesPerSample;
            data.resize(sampleCount);
            if (!file.read(reinterpret_cast<char*>(data.data()), chunkSize)) return false;
            if (chunkSize % 2 == 1) file.seekg(1, std::ios::cur);

            localInfo.dataSize = chunkSize;
            header.Info = localInfo;
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

    if (!foundData) {
        std::cerr << "No data chunk has been found\n";
        return false;
    }

    return true;
}

float normalizeAudioData(const std::vector<int16_t>& samples, size_t offset, size_t chunkSize) {
    if (samples.empty() || offset >= samples.size()) return 0.0f;
    size_t available = samples.size() - offset;
    size_t processed = min(available, chunkSize);
    if (processed == 0) return 0.0f;

    double sumSq = 0.0;
    for (size_t i = 0; i < processed; ++i) {
        double normalized = static_cast<double>(samples[offset + i]) / 32768.0;
        sumSq += normalized * normalized;
    }
    double rms = std::sqrt(sumSq / static_cast<double>(processed));
    return static_cast<float>(rms);
}




static int outAudioCallback(const void* inBuffer, void* outBuffer, ULONG framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statFlag, void* userData) {

    AudioData* audio = reinterpret_cast<AudioData*>(userData);
    int16_t* out = reinterpret_cast<int16_t*>(outBuffer);

    size_t framesToPlay = std::min<size_t>(framesPerBuffer * audio->channels, audio->totalFrames - audio->cursor);

    for (size_t i = 0; i < framesToPlay; i++)
    {
        out[i] = audio->samples[audio->cursor + i];
    }

    for (size_t i = framesToPlay; i < framesPerBuffer*audio->channels; i++)
    {
        //au cas ou il y a pas assez de donné pour remplir le buffer
        //Pu la merde à changer plus tard
        out[i] = 0;
    }
    audio->cursor += framesToPlay;

    if (audio->cursor >= audio->totalFrames)
    {
        return paComplete;
    }

    return paContinue;

}


PaStream* playBack(const std::vector<int16_t>& data, const WAV_HEADER& header, AudioData* audioData){
    
    if (!audioData) return nullptr;
    
    audioData->samples = data.data();
    audioData->totalFrames = data.size();
    audioData->cursor = 0;
    audioData->channels = header.Info.nbrChannels;

    PaStream* stream = nullptr; 
    PaStreamParameters outParams;

    outParams.device = Pa_GetDefaultOutputDevice();
    outParams.channelCount = audioData->channels;
    outParams.sampleFormat = paInt16;
    outParams.suggestedLatency = Pa_GetDeviceInfo(outParams.device)->defaultLowOutputLatency;
    outParams.hostApiSpecificStreamInfo = nullptr;


    PaError err = Pa_OpenStream(&stream,
        nullptr,
        &outParams,
        header.Info.sampleRate,
        paFramesPerBufferUnspecified,
        paNoFlag,
        outAudioCallback,
        audioData);

    if (err != paNoError)
    {
        std::cerr << "Failed to open playBack stream";
        return nullptr;
    }
    Pa_StartStream(stream);
    return stream;
}