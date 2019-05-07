#include "GL/glew.h"

#include <utility>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>

#include "graphics/Vertex.h"

// Reads only pos and normal
std::pair<std::vector<VertexN>, std::vector<GLushort>> ReadMesh(const std::string &path)
{
    std::fstream file;
    std::vector<VertexN> V;
    std::vector<GLushort> I;
    try {
        file.open(path);
        std::stringstream ss;
        ss << file.rdbuf();
        unsigned n;
        ss >> n;
        for (unsigned i = 0; i < n; ++i) {
            glm::vec3 p;
            glm::vec3 normal;
            ss >> p.x >> p.y >> p.z >> normal.x >> normal.y >> normal.z;
            V.push_back({p, {0.0, 0.0}, normal});
        }
        ss >> n;
        for (unsigned i = 0; i < n; ++i) {
            GLushort ind;
            ss >> ind;
            I.push_back(ind);
        }
    } catch (std::fstream::failure &e) {
        std::cerr << "Could not open mesh file: " << path << std::endl;
        std::cerr << e.what() << std::endl;
    }
    return {V, I};
}
