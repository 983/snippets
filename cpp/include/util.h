#pragma once

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#include <vector>
#include <algorithm>

#include "vec2.h"

#define Vector std::vector

#define PI 3.1415926535897932384626433832795
#define TWO_PI (2.0*PI)

typedef Vec2d Point;

typedef std::vector<Point> Points;

uint32_t state = 0x12345678;

uint32_t xorshift32(){
    state ^= state << 13;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

double randu(){
    return (xorshift32() - 1.0)/0xffffffff;
}

int randi(int n){
    return xorshift32() % n;
}

template <typename VALUES, typename VALUE>
bool contains(const VALUES &values, VALUE value){
    for (const auto &other : values){
        if (value == other) return true;
    }
    return false;
}

#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0xff000000
#define COLOR_GREEN 0xff00ff00

typedef uint32_t Color;
