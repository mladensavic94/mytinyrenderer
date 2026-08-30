#include <vector>
#include <string>
#include <array>


struct vec3 {
    float x, y, z;
    vec3 toScreen(int width, int height) const {
        return { (x+1.f)*width/2.f, (y+1.f)*height/2.f, z };
    }
};

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