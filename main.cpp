#include <cmath>
#include "tgaimage.h"
#include "model.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void drawLine(int ax, int ay, int bx, int by, TGAImage &framebuffer, TGAColor color){
    bool steep = std::abs(ax-bx) < std::abs(ay-by);
    if (steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (ax > bx) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int y = ay;
    int ierror = 0;
    for (int x=ax; x <= bx; x++)
    {
        if (steep){
            framebuffer.set(y,x, color);    
        }
        else {
            framebuffer.set(x, y, color);
        }
        ierror += 2 * std::abs(by-ay);
        y += (by > ay ? 1 : -1) * (ierror > bx - ax);
        ierror -= 2 * (bx-ax)   * (ierror > bx - ax);
    }
    
}

int main(int argc, char** argv) {
    constexpr int width  = 640;
    constexpr int height = 640;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    Model model("obj/diablo_pose.obj");

    for (int i = 0; i < model.nfaces(); i++)
    {
        auto faces = model.face(i);
        auto vec_a = model.vert(faces[0]).toScreen(width, height);
        auto vec_b = model.vert(faces[1]).toScreen(width, height);
        auto vec_c = model.vert(faces[2]).toScreen(width, height);
        drawLine(vec_a.x, vec_a.y, vec_b.x, vec_b.y, framebuffer, red);
        drawLine(vec_a.x, vec_a.y, vec_c.x, vec_c.y, framebuffer, red);
        drawLine(vec_c.x, vec_c.y, vec_b.x, vec_b.y, framebuffer, red);
    }
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}