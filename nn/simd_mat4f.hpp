#pragma once

#include <xmmintrin.h>
#include <stdio.h>

struct Mat4f {
    union {
        __m128 cols[4];
        float data[4*4];
    };

    float operator [] (int i){
        return data[i];
    }

    int idx(int i, int j) const {
        return i + j*4;
    }

    Mat4f(){}

    Mat4f(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    ){
        data[idx(0, 0)] = m00; data[idx(0, 1)] = m01; data[idx(0, 2)] = m02; data[idx(0, 3)] = m03;
        data[idx(1, 0)] = m10; data[idx(1, 1)] = m11; data[idx(1, 2)] = m12; data[idx(1, 3)] = m13;
        data[idx(2, 0)] = m20; data[idx(2, 1)] = m21; data[idx(2, 2)] = m22; data[idx(2, 3)] = m23;
        data[idx(3, 0)] = m30; data[idx(3, 1)] = m31; data[idx(3, 2)] = m32; data[idx(3, 3)] = m33;
    }

    float at(int i, int j) const {
        return data[idx(i, j)];
    }

    float operator () (int i, int j) const {
        return data[idx(i, j)];
    }

    float& operator () (int i, int j){
        return data[idx(i, j)];
    }

    void print() const {
        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 4; j++){
                printf("%2.5f ", at(i, j));
            }
            printf("\n");
        }
        printf("\n");
    }

    void operator += (const Mat4f &other){
        cols[0] += other.cols[0];
        cols[1] += other.cols[1];
        cols[2] += other.cols[2];
        cols[3] += other.cols[3];
    }

    void operator -= (const Mat4f &other){
        cols[0] -= other.cols[0];
        cols[1] -= other.cols[1];
        cols[2] -= other.cols[2];
        cols[3] -= other.cols[3];
    }
};

struct Vec4f {
    union {
        __m128 col;
        struct { float x, y, z, w; };
        float data[4];
    };

    Vec4f(){}

    Vec4f(float x, float y, float z, float w): x(x), y(y), z(z), w(w){}

    void print() const {
        printf("Vec4f(%f, %f, %f, %f)\n", x, y, z, w);
    }
};

Vec4f operator * (Mat4f a, Vec4f b){
    Vec4f c;
    c.col = a.cols[0]*b.x + a.cols[1]*b.y + a.cols[2]*b.z + a.cols[3]*b.w;
    return c;
}

Mat4f operator * (Mat4f a, Mat4f b){
    Mat4f c;
    c.cols[0] = a.cols[0]*b(0, 0) + a.cols[1]*b(1, 0) + a.cols[2]*b(2, 0) + a.cols[3]*b(3, 0);
    c.cols[1] = a.cols[0]*b(0, 1) + a.cols[1]*b(1, 1) + a.cols[2]*b(2, 1) + a.cols[3]*b(3, 1);
    c.cols[2] = a.cols[0]*b(0, 2) + a.cols[1]*b(1, 2) + a.cols[2]*b(2, 2) + a.cols[3]*b(3, 2);
    c.cols[3] = a.cols[0]*b(0, 3) + a.cols[1]*b(1, 3) + a.cols[2]*b(2, 3) + a.cols[3]*b(3, 3);
    return c;
}

Mat4f matrixCompMult(Mat4f a, Mat4f b){
    Mat4f c;
    c.cols[0] = a.cols[0] * b.cols[0];
    c.cols[1] = a.cols[1] * b.cols[1];
    c.cols[2] = a.cols[2] * b.cols[2];
    c.cols[3] = a.cols[3] * b.cols[3];
    return c;
}

Mat4f operator + (Mat4f a, Mat4f b){
    Mat4f c;
    c.cols[0] = a.cols[0] + b.cols[0];
    c.cols[1] = a.cols[1] + b.cols[1];
    c.cols[2] = a.cols[2] + b.cols[2];
    c.cols[3] = a.cols[3] + b.cols[3];
    return c;
}

Mat4f operator - (Mat4f a, Mat4f b){
    Mat4f c;
    c.cols[0] = a.cols[0] - b.cols[0];
    c.cols[1] = a.cols[1] - b.cols[1];
    c.cols[2] = a.cols[2] - b.cols[2];
    c.cols[3] = a.cols[3] - b.cols[3];
    return c;
}

Mat4f operator * (float a, Mat4f b){
    Mat4f c;
    c.cols[0] = a*b.cols[0];
    c.cols[1] = a*b.cols[1];
    c.cols[2] = a*b.cols[2];
    c.cols[3] = a*b.cols[3];
    return c;
}

Vec4f operator - (Vec4f a, Vec4f b){
    Vec4f c;
    c.col = a.col - b.col;
    return c;
}

#if 0

int main(){

    Mat4f a(
        1, 3, 5, 7,
        11, 13, 17, 19,
        23, 29, 31, 37,
        41, 43, 47, 53
    );

    Mat4f b(
        59, 61, 67, 71,
        73, 79, 83, 89,
        97, 101, 103, 107,
        109, 113, 127, 131
    );

    Vec4f c(59, 61, 67, 71);

    (Mat4f(
        1526, 1594, 1720, 1790,
        5318, 5562, 5980, 6246,
        10514, 11006, 11840, 12378,
        15894, 16634, 17888, 18710
    ) - a*b).print();

    (a*c - Vec4f(1074, 3930, 7830, 11954)).print();

    return 0;
}

#endif
