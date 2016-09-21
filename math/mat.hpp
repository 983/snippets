#pragma once

#include "util.hpp"

//==============================================================================
// common member functions

#define MAT_COMMON(M, N) \
typedef T value_type;\
\
int      idx         (int i, int j) const { return i + j * M; }\
      T& at          (int i, int j)       { return data()[idx(i, j)]; }\
const T& at          (int i, int j) const { return data()[idx(i, j)]; }\
      T& operator () (int i, int j)       { return at(i, j); }\
const T& operator () (int i, int j) const { return at(i, j); }\
      T& at          (int i       )       { return data()[i]; }\
const T& at          (int i       ) const { return data()[i]; }\
      T& operator [] (int i       )       { return at(i); }\
const T& operator [] (int i       ) const { return at(i); }\
\
static int n_rows(){ return M; }\
static int n_cols(){ return N; }\
\
Mat<M, 1, T> col(int j) const {\
    Mat<M, 1, T> result;\
    for (int i = 0; i < M; i++) result[i] = at(i, j);\
    return result;\
}\
\
Mat<N, 1, T> row(int i) const {\
    Mat<N, 1, T> result;\
    for (int j = 0; j < N; j++) result[j] = at(i, j);\
    return result;\
}\
\
Mat& operator += (const Mat &a){ for (int k = 0; k < M * N; k++) at(k) += a[k]; return *this; }\
Mat& operator -= (const Mat &a){ for (int k = 0; k < M * N; k++) at(k) -= a[k]; return *this; }\
Mat& operator /= (const Mat &a){ for (int k = 0; k < M * N; k++) at(k) /= a[k]; return *this; }\
\
Mat& operator += (const T   &a){ for (int k = 0; k < M * N; k++) at(k) += a;    return *this; }\
Mat& operator -= (const T   &a){ for (int k = 0; k < M * N; k++) at(k) -= a;    return *this; }\
Mat& operator *= (const T   &a){ for (int k = 0; k < M * N; k++) at(k) *= a;    return *this; }\
Mat& operator /= (const T   &a){ for (int k = 0; k < M * N; k++) at(k) /= a;    return *this; }\
\
Mat(){}\
\
explicit Mat(const T &t){\
    for (int k = 0; k < M * N; k++) at(k) = t;\
}\
template <typename S>\
explicit Mat(const Mat<M, N, S> &b){\
    for (int i = 0; i < M*N; i++) at(i) = T(b[i]);\
}

//==============================================================================
// base template

template <int M, int N, typename T>
class Mat {
    T buf[M*N];
public:
    MAT_COMMON(M, N)

          T* data()       { return buf; }
    const T* data() const { return buf; }
};

//==============================================================================
// template spezialisations for vec2, vec3, vec4 with xyzw components

template <typename T>
class Mat <2, 1, T> {
public:
    MAT_COMMON(2, 1)
    T x, y;

    Mat(const T &x, const T &y): x(x), y(y){}

          T* data()       { return &x; }
    const T* data() const { return &x; }
};

template <typename T>
class Mat <3, 1, T> {
public:
    MAT_COMMON(3, 1)
    T x, y, z;

    Mat(const T &x, const T &y, const T &z): x(x), y(y), z(z){}

          T* data()       { return &x; }
    const T* data() const { return &x; }
};

template <typename T>
class Mat <4, 1, T> {
public:
    MAT_COMMON(4, 1)
    T x, y, z, w;

    Mat(const Mat<3, 1, T> &xyz, const T &w): x(xyz.x), y(xyz.y), z(xyz.z), w(w){}
    Mat(const T &x, const T &y, const T &z, const T &w): x(x), y(y), z(z), w(w){}

          T* data()       { return &x; }
    const T* data() const { return &x; }
};

//==============================================================================
// +-*/

template <int M, int N, typename T> Mat<M, N, T> operator + (const Mat<M, N, T> &a, const Mat<M, N, T> &b){ return Mat<M, N, T>(a) += b; }
template <int M, int N, typename T> Mat<M, N, T> operator - (const Mat<M, N, T> &a, const Mat<M, N, T> &b){ return Mat<M, N, T>(a) -= b; }
template <int M, int N, typename T> Mat<M, N, T> operator / (const Mat<M, N, T> &a, const Mat<M, N, T> &b){ return Mat<M, N, T>(a) /= b; }

template <int M, int N, typename T> Mat<M, N, T> operator + (const Mat<M, N, T> &a, const           T  &b){ return Mat<M, N, T>(a) += b; }
template <int M, int N, typename T> Mat<M, N, T> operator - (const Mat<M, N, T> &a, const           T  &b){ return Mat<M, N, T>(a) -= b; }
template <int M, int N, typename T> Mat<M, N, T> operator * (const Mat<M, N, T> &a, const           T  &b){ return Mat<M, N, T>(a) *= b; }
template <int M, int N, typename T> Mat<M, N, T> operator / (const Mat<M, N, T> &a, const           T  &b){ return Mat<M, N, T>(a) /= b; }

template <int M, int N, typename T> Mat<M, N, T> operator + (const           T  &a, const Mat<M, N, T> &b){ return Mat<M, N, T>(a) += b; }
template <int M, int N, typename T> Mat<M, N, T> operator - (const           T  &a, const Mat<M, N, T> &b){ return Mat<M, N, T>(a) -= b; }
template <int M, int N, typename T> Mat<M, N, T> operator * (const           T  &a, const Mat<M, N, T> &b){ return Mat<M, N, T>(b) *= a; }

template <int M, int N, typename T>
Mat<M, N, T> operator + (const Mat<M, N, T> &a){
    return a;
}

template <int M, int N, typename T>
Mat<M, N, T> operator - (const Mat<M, N, T> &a){
    Mat<M, N, T> result;
    for (int k = 0; k < M * N; k++) result[k] = -a[k];
    return result;
}

template <int M, int NM, int N, typename T>
Mat<M, N, T> operator * (const Mat<M, NM, T> &a, const Mat<NM, N, T> &b){
    Mat<M, N, T> result;
    for (int i = 0; i < M; i++) for (int j = 0; j < N; j++){
        result(i, j) = dot(a.row(i), b.col(j));
    }
    return result;
}

//==============================================================================
// boolean operations
template <int M, int N, typename T>
bool operator == (const Mat<M, N, T> &a, const Mat<M, N, T> &b){
    for (int k = 0; k < M * N; k++) if (a[k] != b[k]) return false;
    return true;
}

template <int M, int N, typename T>
bool operator != (const Mat<M, N, T> &a, const Mat<M, N, T> &b){
    return !(a == b);
}

//==============================================================================
// other matrix operations

template <int M, int N, typename T>
void identity(Mat<M, N, T> &a){
    for (int i = 0; i < M; i++) for (int j = 0; j < N; j++) a(i, j) = T(i == j);
}

template <int M, int N, typename T>
void values(Mat<M, N, T> &a, const T &value){
    for (int k = 0; k < M * N; k++) a[k] = value;
}

template <int M, int N, typename T>
void enumerate(Mat<M, N, T> &a){
    for (int k = 0; k < M * N; k++) a[k] = k;
}

template <int M, int N, typename T>
Mat<M, N, T> transpose(const Mat<M, N, T> &a){
    Mat<M, N, T> b;
    for (int i = 0; i < M; i++) for (int j = 0; j < N; j++) b(j, i) = a(i, j);
    return b;
}

template <int M, typename T>
T dot(const Mat<M, 1, T> &a, const Mat<M, 1, T> &b){
    T result(a[0] * b[0]);
    for (int i = 1; i < M; i++) result += a(i, 0) * b(i, 0);
    return result;
}

template <int M, int N, typename T>
T length_squared(const Mat<M, N, T> &a){
    return dot(a, a);
}

template <int M, int N, typename T>
T length(const Mat<M, N, T> &a){
    return std::sqrt(length_squared(a));
}

template <int M, int N, typename T>
T angle(const Mat<M, N, T> &a){
    return std::atan2(a.y, a.x);
}

template <int M, int N, typename T>
T distance_squared(const Mat<M, N, T> &a, const Mat<M, N, T> &b){
    return length_squared(a - b);
}

template <int M, int N, typename T>
T distance(const Mat<M, N, T> &a, const Mat<M, N, T> &b){
    return length(a - b);
}

template <int M, int N, typename T>
Mat<M, N, T> normalize(const Mat<M, N, T> &a){
    return a * inversesqrt(dot(a, a));
}

template <int M, int N, typename T>
Mat<M, N, T> reflect(const Mat<M, N, T> &v, const Mat<M, N, T> &normal){
    return v - T(2) * dot(v, normal) * normal;
}

template <typename T>
bool is_left_of(const Mat<2, 1, T> &p, const Mat<2, 1, T> &a, const Mat<2, 1, T> &b){
    return cross(b - a, p - a) > T(0);
}

template <typename T>
bool is_right_of(const Mat<2, 1, T> &p, const Mat<2, 1, T> &a, const Mat<2, 1, T> &b){
    return cross(b - a, p - a) < T(0);
}

template <typename T>
Mat<2, 1, T> left_vector(const Mat<2, 1, T> &a){
    return Mat<2, 1, T>(-a.y, a.x);
}

template <typename T>
Mat<2, 1, T> right_vector(const Mat<2, 1, T> &a){
    return Mat<2, 1, T>(a.y, -a.x);
}

template <int M, int N, typename T>
Mat<M, N, T> line_project(const Mat<M, N, T> &p, const Mat<M, N, T> &a, const Mat<M, N, T> &b){
    Mat<M, N, T> ba = b - a;
    T u = dot(p - a, ba)/dot(ba, ba);
    return a + u * ba;
}

template <int M, int N, typename T>
Mat<M, N, T> line_segment_closest_point(const Mat<M, N, T> &p, const Mat<M, N, T> &a, const Mat<M, N, T> &b){
    Mat<M, N, T> ba = b - a;
    T u = dot(p - a, ba)/dot(ba, ba);
    u = clamp(u, T(0), T(1));
    return a + u * ba;
}

template <int M, int N, typename T>
T line_distance_squared(const Mat<M, N, T> &p, const Mat<M, N, T> &a, const Mat<M, N, T> &b){
    Mat<M, N, T> q = line_project(p, a, b);
    return distance_squared(p, q);
}

template <int M, int N, typename T>
T line_segment_distance_squared(const Mat<M, N, T> &p, const Mat<M, N, T> &a, const Mat<M, N, T> &b){
    Mat<M, N, T> q = line_segment_closest_point(p, a, b);
    return distance_squared(p, q);
}

template <int M, int N, typename T>
T line_distance(const Mat<M, N, T> &p, const Mat<M, N, T> &a, const Mat<M, N, T> &b){
    return std::sqrt(line_distance_squared(p, a, b));
}

template <int M, int N, typename T>
T line_segment_distance(const Mat<M, N, T> &p, const Mat<M, N, T> &a, const Mat<M, N, T> &b){
    return std::sqrt(line_segment_distance_squared(p, a, b));
}

template <typename T>
T det(const Mat<2, 1, T> &a, const Mat<2, 1, T> &b){
    return a.x * b.y - a.y * b.x;
}

template <typename T>
Mat<3, 1, T> cross(const Mat<3, 1, T> &a, const Mat<3, 1, T> &b){
    Mat<3, 1, T> c;
    c[0] = a[1] * b[2] - a[2] * b[1];
    c[1] = a[2] * b[0] - a[0] * b[2];
    c[2] = a[0] * b[1] - a[1] * b[0];
    return c;
}

template <typename T>
Mat<2, 1, T> polar(const T &angle){
    return Mat<2, 1, T>(std::cos(angle), std::sin(angle));
}

template <typename T>
Mat<4, 1, T> hue_to_rgba(const T &h, const T &a = T(1)){
    T h6 = h * T(6);
    T r = std::abs(h6 - T(3)) - T(1);
    T g = T(2) - std::abs(h6 - T(2));
    T b = T(2) - std::abs(h6 - T(4));
    return clamp(Mat<4, 1, T>(r, g, b, a), T(0), T(1));
}

template <typename T>
Mat<4, 1, T> hsv_to_rgba(const T &h, const T &s, const T &v, const T &a = T(1)){
    return ((hue_to_rgba(h, a) - T(1)) * s + T(1)) * v;
}
//==============================================================================
// componentwise operations

#define MAT_COMPONENTWISE1(name, func) \
template <int M, int N, typename T>\
Mat<M, N, T> name(const Mat<M, N, T> &a){\
    Mat<M, N, T> result;\
    for (int i = 0; i < M * N; i++) result[i] = func(a[i]);\
    return result;\
}

#define MAT_COMPONENTWISE2(name, func) \
template <int M, int N, typename T>\
Mat<M, N, T> name(const Mat<M, N, T> &a, const Mat<M, N, T> &b){\
    Mat<M, N, T> result;\
    for (int i = 0; i < M * N; i++) result[i] = func(a[i], b[i]);\
    return result;\
}

#define MAT_COMPONENTWISE3(name, func) \
template <int M, int N, typename T>\
Mat<M, N, T> name(const Mat<M, N, T> &a, const Mat<M, N, T> &b, const Mat<M, N, T> &c){\
    Mat<M, N, T> result;\
    for (int i = 0; i < M * N; i++) result[i] = func(a[i], b[i], c[i]);\
    return result;\
}

MAT_COMPONENTWISE1(abs, std::abs)
MAT_COMPONENTWISE1(ceil, std::ceil)
MAT_COMPONENTWISE1(floor, std::floor)
MAT_COMPONENTWISE1(fract, fract)
MAT_COMPONENTWISE1(sqrt, std::sqrt)
MAT_COMPONENTWISE1(log, std::log)
MAT_COMPONENTWISE1(exp, std::exp)

MAT_COMPONENTWISE2(min, std::min)
MAT_COMPONENTWISE2(max, std::max)
MAT_COMPONENTWISE2(pow, std::pow)
MAT_COMPONENTWISE2(step, step)
MAT_COMPONENTWISE2(mod, mod)

MAT_COMPONENTWISE3(mix, mix)
MAT_COMPONENTWISE3(clamp, clamp)
MAT_COMPONENTWISE3(smoothstep, smoothstep)


//==============================================================================
// mixed types (Mat and T)

template <int M, int N, typename T>
Mat<M, N, T> step(const Mat<M, N, T> &edge, const T &x){
    return step(edge, Mat<M, N, T>(x));
}

template <int M, int N, typename T>
Mat<M, N, T> clamp(const Mat<M, N, T> &x, const T &a, const T &b){
    return clamp(x, Mat<M, N, T>(a), Mat<M, N, T>(b));
}

template <int M, int N, typename T>
Mat<M, N, T> mix(const Mat<M, N, T> &x, const Mat<M, N, T> &y, const T &u){
    return mix(x, y, Mat<M, N, T>(u));
}

template <int M, int N, typename T>
Mat<M, N, T> smoothstep(const Mat<M, N, T> &x, const Mat<M, N, T> &y, const T &u){
    return smoothstep(x, y, Mat<M, N, T>(u));
}

//==============================================================================
// opengl stuff

template <typename T>
Mat<4, 4, T> ortho(T left, T right, T bottom, T top){
    Mat<4, 4, T> m;
    identity(m);
    assert(right != left);
    assert(top != bottom);
    m(0, 0) = T(2) / (right - left);
    m(1, 1) = T(2) / (top - bottom);
    m(0, 3) = (right + left) / (left - right);
    m(1, 3) = (top + bottom) / (bottom - top);
    return m;
}

template <typename T>
Mat<4, 4, T> scale(T sx, T sy, T sz){
    Mat<4, 4, T> m;
    identity(m);
    m(0, 0) = sx;
    m(1, 1) = sy;
    m(2, 2) = sz;
    return m;
}
/*
TODO
template <typename T>
Mat<2, 2, T> rotation2(T radians){
    T c = std::cos(radians);
    T s = std::sin(radians);
    Mat<2, 2, T> m;
    m(0, 0) = +c; m(0, 1) = s;
    m(1, 0) = -s; m(1, 1) = c;
    return m;
}

template <typename T>
Mat<4, 4, T> rotate(T radians){
    T c = std::cos(radians);
    T s = std::sin(radians);
    Mat<4, 4, T> m;
    identity(m);
    m(0, 0) = +c; m(0, 1) = s;
    m(1, 0) = -s; m(1, 1) = c;
    return m;
}
*/
template <typename T>
Mat<4, 4, T> translate(T tx = 0.0f, T ty = 0.0f, T tz = 0.0f){
    Mat<4, 4, T> m;
    identity(m);
    m(0, 3) = tx;
    m(1, 3) = ty;
    m(2, 3) = tz;
    return m;
}

//==============================================================================
// shorthands

template <typename T>
Mat<2, 1, T> v2(const T &x, const T &y){
    Mat<2, 1, T> v;
    v[0] = x;
    v[1] = y;
    return v;
}

template <typename T>
Mat<3, 1, T> v3(const T &x, const T &y, const T &z){
    Mat<3, 1, T> v;
    v[0] = x;
    v[1] = y;
    v[2] = z;
    return v;
}

template <typename T>
Mat<4, 1, T> v4(const T &x, const T &y, const T &z, const T &w){
    Mat<4, 1, T> v;
    v[0] = x;
    v[1] = y;
    v[2] = z;
    v[3] = w;
    return v;
}

typedef Mat<1, 1, unsigned char> Vec1u8;
typedef Mat<2, 1, unsigned char> Vec2u8;
typedef Mat<3, 1, unsigned char> Vec3u8;
typedef Mat<4, 1, unsigned char> Vec4u8;

typedef Mat<1, 1, int   > Vec1i;
typedef Mat<2, 1, int   > Vec2i;
typedef Mat<3, 1, int   > Vec3i;
typedef Mat<4, 1, int   > Vec4i;

typedef Mat<1, 1, float > Vec1f;
typedef Mat<2, 1, float > Vec2f;
typedef Mat<3, 1, float > Vec3f;
typedef Mat<4, 1, float > Vec4f;

typedef Mat<1, 1, double> Vec1d;
typedef Mat<2, 1, double> Vec2d;
typedef Mat<3, 1, double> Vec3d;
typedef Mat<4, 1, double> Vec4d;

typedef Mat<2, 2, float > Mat2f;
typedef Mat<3, 3, float > Mat3f;
typedef Mat<4, 4, float > Mat4f;

typedef Mat<2, 2, double> Mat2d;
typedef Mat<3, 3, double> Mat3d;
typedef Mat<4, 4, double> Mat4d;

//==============================================================================
/*
TODO
template <typename T>
Mat<2, 2, T> rotation(const Mat<2, 1, T> &axis){
    Mat<2, 2, T> b;
    b(0, 0) = axis.x; b(0, 1) = -axis.y;
    b(1, 0) = axis.y; b(1, 1) = axis.x;
    return b;
}

template <typename T>
Mat<2, 2, T> rotation(const T &angle){
    T c = std::cos(angle);
    T s = std::sin(angle);
    Mat<2, 2, T> b;
    b(0, 0) = c; b(0, 1) = -s;
    b(1, 0) = s; b(1, 1) = c;
    return b;
}
*/
#if 0
TODO
template <typename T>
Mat<2, 2, T> inverse(const Mat<2, 2, T> &a){
    Mat<2, 2, T> b;
    T det = a(0, 0)*a(1, 1) - a(1, 0)*a(0, 1);
    T inv_det = T(1)/det;
    b(0, 0) = inv_det * +a(1, 1); b(0, 1) = inv_det * -a(0, 1);
    b(1, 0) = inv_det * -a(1, 0); b(1, 1) = inv_det * +a(0, 0);
    return b;
}

template <typename T>
Mat<3, 3, T> inverse(const Mat<3, 3, T> &a){
    Mat<3, 3, T> b;

    b(0, 0) = a(1, 1)*a(2, 2) - a(2, 1)*a(1, 2);
    b(1, 0) = a(2, 0)*a(1, 2) - a(1, 0)*a(2, 2);
    b(2, 0) = a(1, 0)*a(2, 1) - a(2, 0)*a(1, 1);

    b(0, 1) = a(0, 2)*a(2, 1) - a(2, 2)*a(0, 1);
    b(1, 1) = a(0, 0)*a(2, 2) - a(2, 0)*a(0, 2);
    b(2, 1) = a(0, 1)*a(2, 0) - a(0, 0)*a(2, 1);

    b(0, 2) = a(0, 1)*a(1, 2) - a(1, 1)*a(0, 2);
    b(1, 2) = a(1, 0)*a(0, 2) - a(0, 0)*a(1, 2);
    b(2, 2) = a(0, 0)*a(1, 1) - a(0, 1)*a(1, 0);

    T det = b(0, 0)*a(0, 0) + b(0, 1)*a(1, 0) + b(0, 2)*a(2, 0);

    return b * (T(1)/det);
}

template <typename T>
Mat<4, 4, T> inverse(const Mat<4, 4, T> &a){
    Mat<4, 4, T> b;

    const T *m = a.data();
    T *inv = b.data();

    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    T det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    return b * (T(1)/det);
}
#endif
template <typename T, int N>
Mat<N, N, T> inverse(const Mat<N, N, T> &a0){
    Mat<N, N, T> a(a0), b;
    identity(b);

    for (int i = 0; i < N; i++){
        // find absolute biggest element in column i in row <= i (called pivot)
        int pivot = i;
        for (int k = i + 1; k < N; k++){
            if (std::abs(a(pivot, i)) < std::abs(a(k, i))) pivot = k;
        }
        // swap pivot row with row i
        if (pivot != i){
            for (int j = 0; j < N; j++){
                std::swap(a(i, j), a(pivot, j));
                std::swap(b(i, j), b(pivot, j));
            }
        }
        // make all rows below i have value 0 in column i
        for (int k = i + 1; k < N; k++){
            T c = a(k, i)/a(i, i);
            for (int j = 0; j < N; j++){
                a(k, j) -= c*a(i, j);
                b(k, j) -= c*b(i, j);
            }
        }
    }
    for (int i = N - 1; i >= 0; i--){
        // normalize row i so there is a 1 on the diagonal
        T c = T(1)/a(i, i);
        for (int j = N - 1; j >= 0; j--){
            a(i, j) *= c;
            b(i, j) *= c;
        }
        // make all rows above row i have value 0 in column i
        for (int k = i - 1; k >= 0; k--){
            T d = a(k, i);
            for (int j = 0; j < N; j++){
                a(k, j) -= d*a(i, j);
                b(k, j) -= d*b(i, j);
            }
        }
    }
    return b;
}
