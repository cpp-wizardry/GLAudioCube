#include "3DModelLoader.h"

std::vector<float> loadOBJ(const std::string Path) {
    std::ifstream file(Path);
    if (!file) {
        std::cerr << "Failed to open OBJ file: " << Path << "\n";
        return {};
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<float> finalData;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::istringstream ss(line);
        std::string prefix;
        ss >> prefix;

        if (prefix == "v") {
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            positions.push_back(pos);
        }
        else if (prefix == "vt") {
            glm::vec2 uv;
            ss >> uv.x >> uv.y;
            uvs.push_back(uv);
        }
        else if (prefix == "vn") {
            glm::vec3 normal;
            ss >> normal.x >> normal.y >> normal.z;
            normals.push_back(normal);
        }
        else if (prefix == "f") {
            std::string vertStr;
            while (ss >> vertStr) {
                int vi = 0, ti = 0, ni = 0;
                sscanf(vertStr.c_str(), "%d/%d/%d", &vi, &ti, &ni);

                glm::vec3 pos = positions[vi - 1];
                glm::vec2 uv = (ti > 0 ? uvs[ti - 1] : glm::vec2(0.0f));
                glm::vec3 normal = (ni > 0 ? normals[ni - 1] : glm::vec3(0.0f));

                finalData.push_back(pos.x);
                finalData.push_back(pos.y);
                finalData.push_back(pos.z);

                finalData.push_back(uv.x);
                finalData.push_back(uv.y);

                finalData.push_back(normal.x);
                finalData.push_back(normal.y);
                finalData.push_back(normal.z);
            }
        }
    }

    return finalData;
}

void centerAndNormalizeOBJ(std::vector<float>& vertices) {
    if (vertices.empty()) return;

    glm::vec3 min(FLT_MAX);
    glm::vec3 max(-FLT_MAX);

    for (size_t i = 0; i < vertices.size(); i += 8) {
        glm::vec3 pos(vertices[i], vertices[i + 1], vertices[i + 2]);

        min.x = std::min(min.x, pos.x);
        min.y = std::min(min.y, pos.y);
        min.z = std::min(min.z, pos.z);

        max.x = std::max(max.x, pos.x);
        max.y = std::max(max.y, pos.y);
        max.z = std::max(max.z, pos.z);
    }

    glm::vec3 center = (min + max) / 2.0f;
    float maxExtent = std::max({ max.x - min.x, max.y - min.y, max.z - min.z });


    for (size_t i = 0; i < vertices.size(); i += 8) {
        vertices[i + 0] = (vertices[i + 0] - center.x) / maxExtent;
        vertices[i + 1] = (vertices[i + 1] - center.y) / maxExtent;
        vertices[i + 2] = (vertices[i + 2] - center.z) / maxExtent;
    }
}

