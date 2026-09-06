#include <cmath>
#include <random>
#include "tgaimage.h"
#include "model.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};
constexpr TGAColor colors[] = { white, green, red, blue, yellow };

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

void drawTriangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color){
    drawLine(ax, ay, bx, by, framebuffer, color);
    drawLine(ax, ay, cx, cy, framebuffer, color);
    drawLine(cx, cy, bx, by, framebuffer, color);
}

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return .5*((by-ay)*(bx+ax) + (cy-by)*(cx+bx) + (ay-cy)*(ax+cx));
}

void fillTriangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color){
    int bbminx = std::min(std::min(ax, bx), cx); 
    int bbminy = std::min(std::min(ay, by), cy); 
    int bbmaxx = std::max(std::max(ax, bx), cx);
    int bbmaxy = std::max(std::max(ay, by), cy);
    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    if (total_area<1) return;

#pragma omp parallel for
    for (int x=bbminx; x<=bbmaxx; x++) {
        for (int y=bbminy; y<=bbmaxy; y++) {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta  = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha<0 || beta<0 || gamma<0) continue;
            framebuffer.set(x, y, color);
        }
    }
}

void triangle(vec3 a, vec3 b, vec3 c, TGAImage &framebuffer, std::vector<float> &dept) {
    int bbminx = std::max(0, (int)std::min(std::min(a.x, b.x), c.x)); // bounding box for the triangle
    int bbminy = std::max(0, (int)std::min(std::min(a.y, b.y), c.y)); // clamped to the framebuffer
    int bbmaxx = std::min(framebuffer.width()-1,  (int)std::max(std::max(a.x, b.x), c.x));
    int bbmaxy = std::min(framebuffer.height()-1, (int)std::max(std::max(a.y, b.y), c.y));
    double total_area = signed_triangle_area(a.x, a.y, b.x, b.y, c.x, c.y);
    if (total_area<1) return; // backface culling + discarding triangles that cover less than a pixel

#pragma omp parallel for
    for (int x=bbminx; x<=bbmaxx; x++) {
        for (int y=bbminy; y<=bbmaxy; y++) {
            double alpha = signed_triangle_area(x, y, b.x, b.y, c.x, c.y) / total_area;
            double beta  = signed_triangle_area(x, y, c.x, c.y, a.x, a.y) / total_area;
            double gamma = signed_triangle_area(x, y, a.x, a.y, b.x, b.y) / total_area;
            if (alpha<0 || beta<0 || gamma<0) continue; 
            double z = alpha * a.z + beta * b.z + gamma * c.z;
            int idx = x + y*framebuffer.width();
            if (z <= dept[idx]) continue; 
            dept[idx] = z;
            unsigned char gray = static_cast<unsigned char>(z);
            framebuffer.set(x, y, {0,0,gray});
        }
    }
}



int main(int argc, char** argv) {
    constexpr int width  = 640;
    constexpr int height = 640;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    std::vector<float> dept(width*height, -1.f); // lower than any toScreen() z (range [0,255])
    Model model("obj/diablo_pose.obj");

    for (int i = 0; i < model.nfaces(); i++)
    {
        auto faces = model.face(i);
        auto vec_a = model.vert(faces[0]).rotate(30.).perspective().toScreen(width, height);
        auto vec_b = model.vert(faces[1]).rotate(30.).perspective().toScreen(width, height);
        auto vec_c = model.vert(faces[2]).rotate(30.).perspective().toScreen(width, height);
        triangle(vec_a, vec_b, vec_c, framebuffer, dept);
    }
    
    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}