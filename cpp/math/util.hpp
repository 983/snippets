#pragma once

#include <assert.h>
#include <cmath>
#include <algorithm>

template <typename T>
T sign(const T &t){
    if (t < T(0)) return T(-1);
    if (t > T(0)) return T(+1);
    return T(0);
}

template <typename T>
T mod(const T &x, const T &y){
    return x - y * std::floor(x / y);
}

template <typename T>
T inversesqrt(const T &a){
    return T(1)/std::sqrt(a);
}

template <typename T>
T clamp(const T &x, const T &a, const T &b){
    return std::max(a, std::min(b, x));
}

template <typename T>
T fract(const T &x){
    return x - std::floor(x);
}

template <typename T, typename U>
T lerp(const T &a, const T &b, const U &u){
    return (U(1) - u)*a + u*b;
}

template <typename T>
T mix(const T &a, const T &b, const T &u){
    return lerp(a, b, u);
}

template <typename T>
T step(const T &edge, const T &x){
    return x < edge ? T(0) : T(1);
}

template <typename T>
T smoothstep(const T &edge0, const T &edge1, const T &x){
    T t = clamp((x - edge0) / (edge1 - edge0), T(0), T(1));
    return t * t * (T(3) - T(2) * t);
}

template <typename T> const T& my_min(const T &a, const T &b){ return a < b ? a : b; }
template <typename T> const T& my_max(const T &a, const T &b){ return a > b ? a : b; }

#define PI_F 3.1415926535897932384626433832795f
#define PI_D 3.1415926535897932384626433832795
