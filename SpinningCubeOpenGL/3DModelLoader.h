#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <regex>


struct OBJData {
    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
};
struct MeshData {
    std::vector<float> vertices; 
};

class Manager3D
{

public:
    std::vector<MeshData> loadOBJ(const std::string& Path);
    std::vector<size_t> reloadModel(const std::string& path, std::vector<unsigned int>& VBOs, std::vector<unsigned int>& VAOs);
    unsigned int reloadTexture(const std::string& path, unsigned int oldTexture);

   //UINT getNumberObject() { return};



private:
    std::vector<OBJData> m_OBJData;
    std::string currOBJPath;



    void centerAndNormalizeOBJ(std::vector<float>& vertices);
};

unsigned int loadTexture(const char* filename);