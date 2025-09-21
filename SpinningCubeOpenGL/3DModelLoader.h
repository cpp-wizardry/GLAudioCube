#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>

struct OBJData {
    std::vector<std::vector<float>> positions;
    std::vector<std::vector<float>> texcoords;
    std::vector<std::vector<float>> normals;
    std::vector<unsigned int> indices;
};
struct OBJVertex {
    float x, y, z;
    float u, v;
    float nx, ny, nz;
};

std::vector<float> loadOBJ(const std::string Path);
void centerAndNormalizeOBJ(std::vector<float>& vertices);