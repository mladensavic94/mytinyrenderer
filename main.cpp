#include <algorithm>
#include <cstdlib>
#include <vector>
#include "tgaimage.h"
#include "model.h"
#include "matrix.h"

void triangle(const vec4 clip[3], const matrix4 &vp, TGAImage &framebuffer, std::vector<float> &dept, const TGAColor color) {
    vec4 ndc[3]    = { clip[0]/clip[0].w, clip[1]/clip[1].w, clip[2]/clip[2].w };                // normalized device coordinates
    vec2 screen[3] = { (vp*ndc[0]).xy(), (vp*ndc[1]).xy(), (vp*ndc[2]).xy() }; // screen coordinates

    matrix3 ABC = {{ {screen[0].x, screen[0].y, 1.}, {screen[1].x, screen[1].y, 1.}, {screen[2].x, screen[2].y, 1.} }};
    if (ABC.det()<1) return;

    auto [bbminx,bbmaxx] = std::minmax({screen[0].x, screen[1].x, screen[2].x});
    auto [bbminy,bbmaxy] = std::minmax({screen[0].y, screen[1].y, screen[2].y});
    auto M = ABC.invert_transpose();

    for (int x=std::max<int>(bbminx, 0); x<=std::min<int>(bbmaxx, framebuffer.width()-1); x++) { // clip the bounding box by the screen
        for (int y=std::max<int>(bbminy, 0); y<=std::min<int>(bbmaxy, framebuffer.height()-1); y++) {
            vec3 bc = M * vec3{float(x), float(y), 1.f}; // barycentric coordinates of {x,y} w.r.t the triangle
            if (bc.x<0 || bc.y<0 || bc.z<0) continue;                                                    // negative barycentric coordinate => the pixel is outside the triangle
            double z = bc * vec3{ ndc[0].z, ndc[1].z, ndc[2].z };
            if (z <= dept[x+y*framebuffer.width()]) continue;
            dept[x+y*framebuffer.width()] = z;
            framebuffer.set(x, y, color);
        }
    }
}

matrix4 viewport(const int x, const int y, const int w, const int h) {
    return {{ vec4{w/2.f,0,0,x+w/2.f}, vec4{0,h/2.f,0,y+h/2.f}, vec4{0,0,1,0}, vec4{0,0,0,1} }};
}

matrix4 perspective(const float f) {
    return {{ vec4{1,0,0,0}, vec4{0,1,0,0}, vec4{0,0,1,0}, vec4{0,0,-1.f/f,1} }};
}

matrix4 lookat(const vec3 eye, const vec3 center, const vec3 up) {
    vec3 n = normalized(eye-center);
    vec3 l = normalized(cross(up,n));
    vec3 m = normalized(cross(n, l));
    return matrix4{{{l.x,l.y,l.z,0}, {m.x,m.y,m.z,0}, {n.x,n.y,n.z,0}, {0,0,0,1}}} *
           matrix4{{{1,0,0,-center.x}, {0,1,0,-center.y}, {0,0,1,-center.z}, {0,0,0,1}}};
}

int main(int argc, char** argv) {
    constexpr int width  = 1200;    // output image size
    constexpr int height = 1200;
    constexpr vec3    eye{-1,0,100}; // camera position
    constexpr vec3 center{0,0,0};  // camera direction
    constexpr vec3     up{0,1,0};  // camera up vector
    TGAImage framebuffer(width, height, TGAImage::RGB);
    Model model("obj/diablo_pose.obj");


    const matrix4 ModelView   = lookat(eye, center, up);
    const matrix4 Perspective = perspective(norm(eye-center));
    const matrix4 Viewport    = viewport(width/16, height/16, width*7/8, height*7/8);
    const matrix4 Projection  = Perspective * ModelView;
    std::vector<float> dept(width*height, -1.f); // below any NDC z (range [-1,1])

    for (int i = 0; i < model.nfaces(); i++)
    {
        vec4 clip[3];
        auto faces = model.face(i);
        auto vec_a = model.vert(faces[0]);
        clip[0] = Projection * vec4{vec_a.x, vec_a.y, vec_a.z, 1};
        auto vec_b = model.vert(faces[1]);
        clip[1] = Projection * vec4{vec_b.x, vec_b.y, vec_b.z, 1};
        auto vec_c = model.vert(faces[2]);
        clip[2] = Projection * vec4{vec_c.x, vec_c.y, vec_c.z, 1};
        TGAColor rnd;
        for (int c=0; c<3; c++) rnd[c] = std::rand()%255;
        triangle(clip, Viewport, framebuffer, dept, rnd);
    }
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}