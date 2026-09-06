#pragma once
#include <cassert>
#include <cmath>
#include <ostream>

template<int n, typename T> struct vec {
    T data[n] = {};
    T& operator[](int i)       { assert(i>=0 && i<n); return data[i]; }
    T  operator[](int i) const { assert(i>=0 && i<n); return data[i]; }
};

template<int n, typename T> T operator*(const vec<n,T> &lhs, const vec<n,T> &rhs) {
    T ret = T();
    #pragma omp parallel for
    for (int i = 0; i < n; i++) ret += lhs[i]*rhs[i];
    return ret;
}
template<int n, typename T> vec<n,T> operator+(vec<n,T> lhs, const vec<n,T> &rhs) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) lhs[i] += rhs[i];
    return lhs;
}
template<int n, typename T> vec<n,T> operator-(vec<n,T> lhs, const vec<n,T> &rhs) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) lhs[i] -= rhs[i];
    return lhs;
}
template<int n, typename T> vec<n,T> operator*(vec<n,T> lhs, const T &rhs) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) lhs[i] *= rhs;
    return lhs;
}

template<int n, typename T> std::ostream& operator<<(std::ostream& out, const vec<n, T>& v) {
    for (int i=0; i<n; i++) out << v[i] << " ";
    return out;
}

template<int n, typename T> T norm(const vec<n,T> &v) { return std::sqrt(v*v); }

template<typename T> struct vec<2,T> {
    T x{}, y{};
    T& operator[](int i)       { return i ? y : x; }
    T  operator[](int i) const { return i ? y : x; }
};

template<typename T> struct vec<3,T> {
    T x{}, y{}, z{};
    T& operator[](int i)       { assert(i>=0 && i<3); return i==0 ? x : i==1 ? y : z; }
    T  operator[](int i) const { assert(i>=0 && i<3); return i==0 ? x : i==1 ? y : z; }
    vec<3,T> toScreen(int width, int height) const {
        return { (x+1.f)*width/2.f, (y+1.f)*height/2.f, (z+1.f)*255/2.f };
    }
    vec<3,T> rotate(double degree) const {
        double rad = degree * M_PI / 180.0;
        double c = std::cos(rad), s = std::sin(rad);
        return { T(x*c + z*s), y, T(-x*s + z*c) };
    }
    vec<3,T> perspective() const {
        double denom = (1-z/3.);
        return {T(x*denom), T(y*denom), T(z*denom)};
    }
};
template<typename T> vec<3,T> cross(const vec<3,T> &a, const vec<3,T> &b) {
    return { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
}

template<typename T> struct vec<4,T> {
    T x{}, y{}, z{}, w{};
    T& operator[](int i)       { assert(i>=0 && i<4); return i==0 ? x : i==1 ? y : i==2 ? z : w; }
    T  operator[](int i) const { assert(i>=0 && i<4); return i==0 ? x : i==1 ? y : i==2 ? z : w; }
};

using vec2 = vec<2,float>;
using vec3 = vec<3,float>;
using vec4 = vec<4,float>;
