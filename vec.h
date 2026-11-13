#pragma once
#include <cassert>
#include <cmath>
#include <ostream>
#include <type_traits>

template<int n, typename T> struct vec {
    T data[n] = {};
    T& operator[](int i)       { assert(i>=0 && i<n); return data[i]; }
    T  operator[](int i) const { assert(i>=0 && i<n); return data[i]; }
};

template<int n, typename T> T operator*(const vec<n,T> &lhs, const vec<n,T> &rhs) {
    T ret = T();
    for (int i = 0; i < n; i++) ret += lhs[i]*rhs[i];
    return ret;
}
template<int n, typename T> vec<n,T> operator+(vec<n,T> lhs, const vec<n,T> &rhs) {
    for (int i = 0; i < n; i++) lhs[i] += rhs[i];
    return lhs;
}
template<int n, typename T> vec<n,T> operator-(vec<n,T> lhs, const vec<n,T> &rhs) {
    for (int i = 0; i < n; i++) lhs[i] -= rhs[i];
    return lhs;
}
template<int n, typename T> vec<n,T> operator*(vec<n,T> lhs, std::type_identity_t<T> rhs) {
    for (int i = 0; i < n; i++) lhs[i] *= rhs;
    return lhs;
}

template<int n, typename T> vec<n,T> operator/(vec<n,T> lhs, std::type_identity_t<T> rhs) {
    for (int i = 0; i < n; i++) lhs[i] /= rhs;
    return lhs;
}

template<int n, typename T> std::ostream& operator<<(std::ostream& out, const vec<n, T>& v) {
    for (int i=0; i<n; i++) out << v[i] << " ";
    return out;
}

template<int n, typename T> T norm(const vec<n,T> &v) { return std::sqrt(v*v); }

template<int n, typename T> vec<n,T> normalized(const vec<n,T>& v) {
    return v / norm(v);
}

template<typename T> struct vec<2,T> {
    T x{}, y{};
    T& operator[](int i)       { return i ? y : x; }
    T  operator[](int i) const { return i ? y : x; }
};

template<typename T> struct vec<3,T> {
    T x{}, y{}, z{};
    T& operator[](int i)       { assert(i>=0 && i<3); return i==0 ? x : i==1 ? y : z; }
    T  operator[](int i) const { assert(i>=0 && i<3); return i==0 ? x : i==1 ? y : z; }
    vec<2,T> xy()  const { return {x, y};    }
};
template<typename T> vec<3,T> cross(const vec<3,T> &a, const vec<3,T> &b) {
    return { a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x };
}

template<typename T> struct vec<4,T> {
    T x{}, y{}, z{}, w{};
    T& operator[](int i)       { assert(i>=0 && i<4); return i==0 ? x : i==1 ? y : i==2 ? z : w; }
    T  operator[](int i) const { assert(i>=0 && i<4); return i==0 ? x : i==1 ? y : i==2 ? z : w; }
    vec<2,T> xy()  const { return {x, y};    }
    vec<3,T> xyz() const { return {x, y, z}; }
};

using vec2 = vec<2,float>;
using vec3 = vec<3,float>;
using vec4 = vec<4,float>;
