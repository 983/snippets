#pragma once

#include <math.h>

struct Vec2 {
    float x, y;

    Vec2(){}
    Vec2(float x, float y): x(x), y(y){}
};

static inline Vec2 polar(float angle){
    return Vec2(cos(angle), sin(angle));
}

static inline Vec2 operator + (const Vec2 &a, const Vec2 &b){
    return Vec2(a.x + b.x, a.y + b.y);
}

static inline Vec2 operator - (const Vec2 &a, const Vec2 &b){
    return Vec2(a.x - b.x, a.y - b.y);
}

static inline Vec2 operator * (float a, const Vec2 &b){
    return Vec2(a * b.x, a * b.y);
}
