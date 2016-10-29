#pragma once

#include "../math/util.hpp"
#include "../math/mat.hpp"
#include "../util/int.hpp"

struct Color {
    static Vec4f white(){
        return Vec4f(1.0f, 1.0f, 1.0f, 1.0f);
    }

    static Vec4f green(){
        return Vec4f(0.0f, 1.0f, 0.0f, 1.0f);
    }

    static Vec4f rgba(float r, float g, float b, float a = 1.0f){
        return Vec4f(r, g, b, a);
    }

    static u32 to_u32(Vec4f color){
        u32 r = clamp(u32(color.x*256.0f), u32(0), u32(255));
        u32 g = clamp(u32(color.y*256.0f), u32(0), u32(255));
        u32 b = clamp(u32(color.z*256.0f), u32(0), u32(255));
        u32 a = clamp(u32(color.w*256.0f), u32(0), u32(255));
        return (a << 24) | (b << 16) | (g << 8) | r;
    }
/*
    static u32 from_u8(u32 r, u32 g, u32 b, u32 a){
        r = clamp(r, u32(0), u32(255));
        g = clamp(g, u32(0), u32(255));
        b = clamp(b, u32(0), u32(255));
        a = clamp(a, u32(0), u32(255));
        return (a << 24) | (b << 16) | (g << 8) | r;
    }

    static u32 rgba(float r, float g, float b, float a = 1.0f){
        return from_u8(r*256, g*256, b*256, a*256);
    }
    */
};
