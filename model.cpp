#include <fstream>
#include <sstream>
#include <iostream>
#include "model.h"

Model::Model(const std::string filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "can't open file " << filename << "\n";
        return;
    }
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("v ", 0) == 0) {
            std::istringstream iss(line.substr(2));
            vec3 v;
            iss >> v.x >> v.y >> v.z;
            verts.push_back(v);
        } else if (line.rfind("f ", 0) == 0) {
            std::istringstream iss(line.substr(2));
            std::array<int,3> f;
            std::string token;
            for (int i = 0; i < 3 && iss >> token; i++)
                f[i] = std::stoi(token) - 1;
            facet_vrt.push_back(f);
        }
    }
}

int Model::nverts() const { return verts.size(); }
int Model::nfaces() const { return facet_vrt.size(); }

vec3 Model::vert(const int i) const { return verts[i]; }
std::array<int,3> Model::face(const int i) const { return facet_vrt[i]; }
