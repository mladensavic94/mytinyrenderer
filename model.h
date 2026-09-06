#include <vector>
#include <string>
#include <array>
#include "vec.h"


class Model {
    std::vector<vec3> verts = {};
    std::vector<std::array<int,3>> facet_vrt = {};
public:
    Model(const std::string filename);
    int nverts() const;
    int nfaces() const; 
    vec3 vert(const int i) const;
    std::array<int,3> face(const int i) const;                     
};