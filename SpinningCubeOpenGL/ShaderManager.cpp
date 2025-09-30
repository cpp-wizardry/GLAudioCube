#include "ShaderManager.h"



ShaderManager::ShaderManager(std::string const& VertPath, std::string const& FragPath) : m_FragPath(FragPath), m_VertPath(VertPath){}

ShaderManager::~ShaderManager()
{
}

bool ShaderManager::loadShaders(std::vector<char*> vertShaders, std::vector<char*> fragShaders)
{
    //std::ifstream file(m_);









    return false;
}


unsigned int ShaderManager::compileShader(unsigned int type, std::vector<const char*> source) {
    for (size_t i = 0; i < source.size(); i++)
    {

    }
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

