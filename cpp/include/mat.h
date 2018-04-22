#pragma once

#include "util.h"

template <typename T>
struct Mat {
    // Matrix layout is column-major
    // Memory layout of a 4x4 column-major matrix:
    // 0 4  8 12
    // 1 5  9 13
    // 2 6 10 14
    // 3 7 11 14
    
    const size_t idx(size_t i, size_t j) const {
        return i + j*m;
    }
    
    size_t m, n;
    Vector<T> values;
    
    Mat(size_t m, size_t n, const T &default_value = T(0)):
        m(m),
        n(n),
        values(m*n, default_value){}

    const T& operator () (size_t i, size_t j) const {
        return values[idx(i, j)];
    }
    
    T& operator () (size_t i, size_t j){
        return values[idx(i, j)];
    }
    
    static Mat<T> eye(size_t n){
        Mat<T> A(n, n);
        for (size_t i = 0; i < n; i++){
            A(i, i) = T(1);
        }
        return A;
    }
    
    const T* data() const {
        return values.data();
    }
    
    T* data(){
        return values.data();
    }
};

typedef Mat<double> Matd;

template <typename T>
Mat<T> operator * (const Mat<T> &A, const Mat<T> &B){
    assert(A.n == B.m);
    
    Mat<T> C(A.m, B.n);
    for (size_t i = 0; i < C.m; i++){
        for (size_t j = 0; j < C.n; j++){
            T sum = A(i, 0)*B(0, j);
            for (size_t k = 1; k < A.n; k++){
                sum += A(i, k)*B(k, j);
            }
            C(i, j) = sum;
        }
    }
    return C;
}

Matd translate(
    double dx = 0.0,
    double dy = 0.0,
    double dz = 0.0
){
    Matd A = Matd::eye(4);
    A(0, 3) = dx;
    A(1, 3) = dy;
    A(2, 3) = dz;
    return A;
}

Matd scale(
    double sx = 1.0,
    double sy = 1.0,
    double sz = 1.0
){
    Matd S = Matd::eye(4);
    S(0, 0) = sx;
    S(1, 1) = sy;
    S(2, 2) = sz;
    return S;
}

int inverse4(
    const double *m,
    double invOut[16]
){
    double inv[16];

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

    double det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0){
        return -1;
    }

    det = 1.0 / det;

    for (int i = 0; i < 16; i++){
        invOut[i] = inv[i] * det;
    }

    return 0;
}
