#pragma once

typedef struct v2i {
    int x, y;
} v2i;

inline v2i v2i_xy(int x, int y){
    v2i v;
    v.x = x;
    v.y = y;
    return v;
}

inline int v2i_eq(v2i a, v2i b){
    return a.x == b.x && a.y == b.y;
}

inline int v2i_len2(v2i a){
    return a.x*a.x + a.y*a.y;
}

inline int v2i_dist2(v2i a, v2i b){
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return dx*dx + dy*dy;
}
