#include <math.h>

typedef struct {
    float x, y;
} Vec2;

Vec2 v2(float x, float y){
    Vec2 v;
    v.x = x;
    v.y = y;
    return v;
}

#define MY_SWAP(T, a, b) do { T _tmp = (a); (a) = (b); (b) = _tmp; } while (0)

typedef struct {
    float data[4 * 4];
} Mat4;

#define m4_at(A, i, j) ((A).data[(i) + (j) * 4])

Mat4 m4_zeros(){
    Mat4 A;

    for (int i = 0; i < 4 * 4; i++) A.data[i] = 0.0f;

    return A;
}

Mat4 m4_eye(){
    Mat4 A = m4_zeros();

    for (int i = 0; i < 4; i++){
        m4_at(A, i, i) = 1.0f;
    }

    return A;
}

Mat4 m4_ortho(float left, float right, float bottom, float top){
    Mat4 A = m4_eye();

    m4_at(A, 0, 0) = 2.0f / (right - left);
    m4_at(A, 1, 1) = 2.0f / (top - bottom);
    m4_at(A, 0, 3) = (right + left) / (left - right);
    m4_at(A, 1, 3) = (top + bottom) / (bottom - top);

    return A;
}

Mat4 m4_mul(Mat4 A, Mat4 B){
    Mat4 C;

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            m4_at(C, i, j) =
                m4_at(A, i, 0)*m4_at(B, 0, j) +
                m4_at(A, i, 1)*m4_at(B, 1, j) +
                m4_at(A, i, 2)*m4_at(B, 2, j) +
                m4_at(A, i, 3)*m4_at(B, 3, j);
        }
    }

    return C;
}

Mat4 m4_inv(Mat4 A){
    Mat4 B = m4_eye();

    int N = 4;

    for (int i = 0; i < N; i++){
        // find absolute biggest element in column i in row <= i (called pivot)
        int pivot = i;
        for (int k = i + 1; k < N; k++){
            if (fabs(m4_at(A, pivot, i)) < fabs(m4_at(A, k, i))) pivot = k;
        }
        // swap pivot row with row i
        if (pivot != i){
            for (int j = 0; j < N; j++){
                MY_SWAP(float, m4_at(A, i, j), m4_at(A, pivot, j));
                MY_SWAP(float, m4_at(B, i, j), m4_at(B, pivot, j));
            }
        }
        // make all rows below i have value 0 in column i
        for (int k = i + 1; k < N; k++){
            float c = m4_at(A, k, i)/m4_at(A, i, i);
            for (int j = 0; j < N; j++){
                m4_at(A, k, j) -= c*m4_at(A, i, j);
                m4_at(B, k, j) -= c*m4_at(B, i, j);
            }
        }
    }

    for (int i = N - 1; i >= 0; i--){
        // normalize row i so there is a 1 on the diagonal
        float c = 1.0f/m4_at(A, i, i);
        for (int j = N - 1; j >= 0; j--){
            m4_at(A, i, j) *= c;
            m4_at(B, i, j) *= c;
        }
        // make all rows above row i have value 0 in column i
        for (int k = i - 1; k >= 0; k--){
            float d = m4_at(A, k, i);
            for (int j = 0; j < N; j++){
                m4_at(A, k, j) -= d*m4_at(A, i, j);
                m4_at(B, k, j) -= d*m4_at(B, i, j);
            }
        }
    }

    return B;
}

Vec2 m4_transform_v2(Mat4 A, Vec2 v){
    float x = m4_at(A, 0, 0) * v.x + m4_at(A, 0, 1) * v.y + m4_at(A, 0, 3);
    float y = m4_at(A, 1, 0) * v.x + m4_at(A, 1, 1) * v.y + m4_at(A, 1, 3);
    return (Vec2){x, y};
}

Mat4 m4_translate(Mat4 A, float dx, float dy, float dz){
    for (int i = 0; i < 4; i++){
        m4_at(A, i, 3) += m4_at(A, i, 0)*dx + m4_at(A, i, 1)*dy + m4_at(A, i, 2)*dz;
    }
    return A;
}

Mat4 m4_scale(Mat4 A, float sx, float sy, float sz){
    for (int i = 0; i < 4; i++){
        m4_at(A, i, 0) *= sx;
        m4_at(A, i, 1) *= sy;
        m4_at(A, i, 2) *= sz;
    }
    return A;
}

Mat4 m4_rotate(Mat4 A, float radians){
    float c = cos(radians);
    float s = sin(radians);

    for (int i = 0; i < 4; i++){
        float a = m4_at(A, i, 0);
        float b = m4_at(A, i, 1);

        m4_at(A, i, 0) = a*c + b*s;
        m4_at(A, i, 1) = b*c - a*s;
    }

    return A;
}

Mat4 m4_scale_xy(Mat4 A, float scale){
    return m4_scale(A, scale, scale, scale);
}

/*
void m4_zeros(mat4 *A){
    for (int i = 0; i < 4*4; i++){
        A->data[i] = 0.0f;
    }
}

void m4_eye(mat4 *A){
    m4_zeros(A);
    for (int i = 0; i < 4; i++){
        m4_at(A, i, i) = 1.0f;
    }
}

void m4_ortho(mat4 *A, float left, float right, float bottom, float top){
    m4_eye(A);

    m4_at(A, 0, 0) = 2.0f / (right - left);
    m4_at(A, 1, 1) = 2.0f / (top - bottom);
    m4_at(A, 0, 3) = (right + left) / (left - right);
    m4_at(A, 1, 3) = (top + bottom) / (bottom - top);
}

void m4_inv(mat4 *B, const mat4 *A0){
    mat4 A[1];
    *A = *A0;
    m4_eye(B);

    int N = 4;

    for (int i = 0; i < N; i++){
        // find absolute biggest element in column i in row <= i (called pivot)
        int pivot = i;
        for (int k = i + 1; k < N; k++){
            if (my_absf(m4_at(A, pivot, i)) < my_absf(m4_at(A, k, i))) pivot = k;
        }
        // swap pivot row with row i
        if (pivot != i){
            for (int j = 0; j < N; j++){
                MY_SWAP(float, m4_at(A, i, j), m4_at(A, pivot, j));
                MY_SWAP(float, m4_at(B, i, j), m4_at(B, pivot, j));
            }
        }
        // make all rows below i have value 0 in column i
        for (int k = i + 1; k < N; k++){
            float c = m4_at(A, k, i)/m4_at(A, i, i);
            for (int j = 0; j < N; j++){
                m4_at(A, k, j) -= c*m4_at(A, i, j);
                m4_at(B, k, j) -= c*m4_at(B, i, j);
            }
        }
    }

    for (int i = N - 1; i >= 0; i--){
        // normalize row i so there is a 1 on the diagonal
        float c = 1.0f/m4_at(A, i, i);
        for (int j = N - 1; j >= 0; j--){
            m4_at(A, i, j) *= c;
            m4_at(B, i, j) *= c;
        }
        // make all rows above row i have value 0 in column i
        for (int k = i - 1; k >= 0; k--){
            float d = m4_at(A, k, i);
            for (int j = 0; j < N; j++){
                m4_at(A, k, j) -= d*m4_at(A, i, j);
                m4_at(B, k, j) -= d*m4_at(B, i, j);
            }
        }
    }
}

static inline void m4_rand(mat4 *A, RNG *rng){
    for (int i = 0; i < 4*4; i++) A->data[i] = rng_randf(rng);
}

void m4_sub(mat4 *C, const mat4 *A, const mat4 *B){
    for (int i = 0; i < 4*4; i++) C->data[i] = A->data[i] - B->data[i];
}

void m4_abs(mat4 *B, const mat4 *A){
    for (int i = 0; i < 4*4; i++) B->data[i] = my_absf(A->data[i]);
}

void m4_mul(mat4 *C, const mat4 *A, const mat4 *B){
    // TODO
    if (A == B) FAIL("m4_mul with A == B");
    if (B == C) FAIL("m4_mul with B == C");

    for (int i = 0; i < 4; i++){
        for (int j = 0; j < 4; j++){
            m4_at(C, i, j) =
                m4_at(A, i, 0)*m4_at(B, 0, j) +
                m4_at(A, i, 1)*m4_at(B, 1, j) +
                m4_at(A, i, 2)*m4_at(B, 2, j) +
                m4_at(A, i, 3)*m4_at(B, 3, j);
        }
    }
}

float m4_max(const mat4 *A){
    float result = A->data[0];

    for (int i = 1; i < 4*4; i++){
        result = my_maxf(result, A->data[i]);
    }

    return result;
}

void m4_test(){
    RNG rng[1];
    rng_init(rng);

    for (int i = 0; i < 100; i++){
        mat4 A[1];
        m4_rand(A, rng);

        mat4 B[1];
        m4_inv(B, A);

        mat4 I[1];
        m4_eye(I);

        mat4 AB[1];
        m4_mul(AB, A, B);

        mat4 D[1];
        m4_sub(D, I, AB);
        m4_abs(D, D);

        assert(m4_max(D) < 2e-4f);
    }
}

void m4_translate(mat4 *A, float dx, float dy, float dz){
    for (int i = 0; i < 4; i++){
        m4_at(A, i, 3) += m4_at(A, i, 0)*dx + m4_at(A, i, 1)*dy + m4_at(A, i, 2)*dz;
    }
}

void m4_scale(mat4 *A, float sx, float sy, float sz){
    for (int i = 0; i < 4; i++){
        m4_at(A, i, 0) *= sx;
        m4_at(A, i, 1) *= sy;
        m4_at(A, i, 2) *= sz;
    }
}

void m4_rotate(mat4 *A, float radians){
    float c = cos(radians);
    float s = sin(radians);

    for (int i = 0; i < 4; i++){
        float a = m4_at(A, i, 0);
        float b = m4_at(A, i, 1);

        m4_at(A, i, 0) = a*c + b*s;
        m4_at(A, i, 1) = b*c - a*s;
    }
}

void m4_transform(vec2 *w, const mat4 *A, const vec2 *v){
    w->x = m4_at(A, 0, 0)*v->x + m4_at(A, 0, 1)*v->y + m4_at(A, 0, 3);
    w->y = m4_at(A, 1, 0)*v->x + m4_at(A, 1, 1)*v->y + m4_at(A, 1, 3);
}
*/
