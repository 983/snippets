#pragma once

#include <math.h>

template <typename T>
T clamp(const T &x, const T &a, const T &b){
    return
        x < a ? a :
        x > b ? b :
        x;
}

template <typename T>
struct Vec2 {
    T x, y;
};

template <typename T>
Vec2<T> operator + (const Vec2<T> &a, const Vec2<T> &b){
    return Vec2<T>{a.x + b.x, a.y + b.y};
}

template <typename T>
Vec2<T> operator - (const Vec2<T> &a, const Vec2<T> &b){
    return Vec2<T>{a.x - b.x, a.y - b.y};
}

template <typename T>
Vec2<T> operator * (const T &a, const Vec2<T> &b){
    return Vec2<T>{a * b.x, a * b.y};
}

template <typename T>
T dot(const Vec2<T> &a, const Vec2<T> &b){
    return a.x*b.x + a.y*b.y;
}

template <typename T>
T dist2(const Vec2<T> &a, const Vec2<T> &b){
    T dx = a.x - b.x;
    T dy = a.y - b.y;
    return dx*dx + dy*dy;
}

template <typename T>
T det(const Vec2<T> &a, const Vec2<T> &b){
    return a.x*b.y - a.y*b.x;
}
/*
bool operator == (const Vec2<T> &a, const Vec2<T> &b){
    return a.x == b.x && a.y == b.y;
}
*/
template <typename T>
T point_line_dist2(const Vec2<T> &p, const Vec2<T> &a, const Vec2<T> &b){
    Vec2<T> ba = b - a;
    T u = dot(p - a, ba)/dot(ba, ba);
    u = clamp(u, T(0), T(1));
    Vec2<T> q = a + u*ba;
    return dist2(p, q);
}

template <typename T>
int intersect(
    const Vec2<T> &a,
    const Vec2<T> &b,
    const Vec2<T> &c,
    const Vec2<T> &d,
    Vec2<T> *intersections
){
    //if (a == c || a == d || b == c || b == d) return 0;
    
    Vec2<T> ba = b - a;
    Vec2<T> dc = d - c;
    Vec2<T> ca = c - a;
    
    T ba_det_dc = det(ba, dc);
    T ca_det_dc = det(ca, dc);
    T ca_det_ba = det(ca, ba);
    
    // parallel segments
    if (ba_det_dc == 0){
        
        // parallel and on same line
        if (ca_det_dc == 0){
            T ba2 = dot(ba, ba);
            T dc2 = dot(dc, dc);
            
            int n = 0;
            
            T baca = dot(ba, c - a);
            T bada = dot(ba, d - a);
            T dcac = dot(dc, a - c);
            T dcbc = dot(dc, b - c);
            
            if (0 <= baca && baca <= ba2) intersections[n++] = c;
            if (0 <= bada && bada <= ba2) intersections[n++] = d;
            if (0 <= dcac && dcac <= dc2) intersections[n++] = a;
            if (0 <= dcbc && dcbc <= dc2) intersections[n++] = b;
            
            return n;
        }else{
            return 0;
        }
    }

    T t = ca_det_ba/ba_det_dc;
    T s = ca_det_dc/ba_det_dc;
    
    if (t >= T(0) && t <= T(1) && s >= T(0) && s <= T(1)){
        intersections[0] = a + s*ba;
        return 1;
    }
    
    return 0;
}

typedef Vec2<float> Vec2f;
typedef Vec2<double> Vec2d;
typedef Vec2<int> Vec2i;
