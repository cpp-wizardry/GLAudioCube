#include "Functions.h"
#include "GlobalVar.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

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


void processInputs(GLFWwindow* window,Cube& r) {
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
    
    //Cube scale boundaries
    if (r.scale < 0.1f) r.scale = 0.1f;
    if (r.scale > 3.0f) r.scale = 3.0f;
};  

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
        case 0:
            ofn.lpstrFilter = "3D Files\0*.obj*\0";
            break;
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



