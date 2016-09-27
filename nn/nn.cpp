#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../snippets/util/vector.hpp"
#include "../snippets/util/time.hpp"
#include "../snippets/math/mat.hpp"

#define STR(x) #x
#define WHERE printf("Line: %i\n", __LINE__);

// TODO optimize
double sigmoid(double x){
    return 1.0/(1.0 + exp(-x));
}

double sigmoid_derivative(double x){
    return x*(1.0 - x);
}

double lerp(double a, double b, double u){
    return (1.0 - u)*a + u*b;
}

double frand(double a, double b){
    double u = 1.0/RAND_MAX * rand();
    return lerp(a, b, u);
}

struct Matrix {
    Vector<double> values;
    int ny, nx;

    Matrix(): ny(0), nx(0){}

    Matrix(const char *text): ny(0), nx(-1){
        int n = strlen(text);
        int n_spaces = 1;
        char *ptr = (char*)text;
        for (int i = 0; i < n; i++){
            char c = text[i];
            if (c == ' ') n_spaces++;
            if (c == ';'){
                if (nx == -1){
                    nx = n_spaces;
                }else{
                    if (nx != n_spaces){
                        printf("row %i has different number of elements\n", ny);
                        exit(-1);
                    }
                }
                n_spaces = 0;
                ny++;
            }
            values.push(strtod(ptr, &ptr));
            ptr++;
        }
    }

    Matrix(int ny, int nx, double value = 0.0): values(nx*ny, value), ny(ny), nx(nx){}
    Matrix(const Matrix &other): values(other.values), ny(other.ny), nx(other.nx){}

    void resize(int new_ny, int new_nx){
        values.resize(new_nx*new_ny);
        nx = new_nx;
        ny = new_ny;
    }

    Matrix& operator = (const Matrix &other){
        resize(other.ny, other.nx);
        for (int i = 0; i < nx*ny; i++){
            values[i] = other.values[i];
        }
        return *this;
    }

    void swap(Matrix &other){
        my_swap(nx, other.nx);
        my_swap(ny, other.ny);
        values.swap(other.values);
    }

    int idx(int y, int x) const {
        assert(y >= 0);
        assert(x >= 0);
        assert(y < ny);
        assert(x < nx);
        return y*nx + x;
    }

    double at(int y, int x) const {
        return values[idx(y, x)];
    }

    double& at(int y, int x){
        return values[idx(y, x)];
    }

    double operator () (int y, int x) const {
        return at(y, x);
    }

    double& operator () (int y, int x){
        return at(y, x);
    }

    // TODO
    static Matrix rand(int ny, int nx, double a, double b){
        Matrix A(ny, nx);
        for (int i = 0; i < nx*ny; i++){
            A.values[i] = frand(a, b);
        }
        return A;
    }

    void print(){
        printf("%ix%i-matrix\n", ny, nx);
        for (int y = 0; y < ny; y++){
            for (int x = 0; x < nx; x++){
                printf("%3.6f ", at(y, x));
            }
            printf("\n");
        }
        printf("\n");
    }

    Matrix& operator += (const Matrix &B){
        assert(nx == B.nx);
        assert(ny == B.ny);
        for (int i = 0; i < nx*ny; i++) values[i] += B.values[i];
        return *this;
    }

    double sum() const {
        double result = 0.0;
        for (int i = 0; i < nx*ny; i++) result += values[i];
        return result;
    }

    double max2() const {
        double result = -1.0/0.0;
        for (int i = 0; i < nx*ny; i++){
            if (values[i] > result) result = values[i];
        }
        return result;
    }
};

Matrix& mul(Matrix &C, const Matrix &A, const Matrix &B){
    C.resize(A.ny, B.nx);
    assert(A.nx == B.ny);
    for (int y = 0; y < C.ny; y++){
        for (int x = 0; x < C.nx; x++){
            double sum = 0.0;
            for (int k = 0; k < A.nx; k++){
                sum += A(y, k)*B(k, x);
            }
            C(y, x) = sum;
        }
    }
    return C;
}

template <typename FUNC>
void apply(Matrix &C, const Matrix &A, FUNC func){
    C.resize(A.ny, A.nx);
    for (int i = 0; i < C.nx*C.ny; i++){
        C.values[i] = func(A.values[i]);
    }
}

Matrix& sub(Matrix &C, const Matrix &A, const Matrix &B){
    assert(A.nx == B.nx);
    assert(A.ny == B.ny);
    C.resize(A.ny, A.nx);
    for (int i = 0; i < C.nx*C.ny; i++){
        C.values[i] = A.values[i] - B.values[i];
    }
    return C;
}

Matrix operator + (const Matrix &A, const Matrix &B){
    Matrix C(A.ny, A.nx);
    assert(A.nx == B.nx);
    assert(A.ny == B.ny);
    for (int i = 0; i < C.nx*C.ny; i++){
        C.values[i] = A.values[i] + B.values[i];
    }
    return C;
}

Matrix operator - (const Matrix &A, const Matrix &B){
    Matrix C(A.ny, A.nx);
    assert(A.nx == B.nx);
    assert(A.ny == B.ny);
    for (int i = 0; i < C.nx*C.ny; i++){
        C.values[i] = A.values[i] - B.values[i];
    }
    return C;
}

template <typename FUNC>
Matrix apply(const Matrix &A, FUNC func){
    Matrix C(A.ny, A.nx);
    apply(C, A, func);
    return C;
}

Matrix& elementwise_mul(Matrix &C, const Matrix &A, const Matrix &B){
    assert(A.nx == B.nx);
    assert(A.ny == B.ny);
    C.resize(A.ny, A.nx);
    for (int i = 0; i < C.nx*C.ny; i++){
        C.values[i] = A.values[i] * B.values[i];
    }
    return C;
}

Matrix elementwise_mul(const Matrix &A, const Matrix &B){
    Matrix C;
    elementwise_mul(C, A, B);
    return C;
}

Matrix& T(Matrix &C, const Matrix &A){
    C.resize(A.nx, A.ny);
    for (int y = 0; y < A.ny; y++){
        for (int x = 0; x < A.nx; x++){
            C(x, y) = A(y, x);
        }
    }
    return C;
}

Matrix T(const Matrix &A){
    Matrix C;
    T(C, A);
    return C;
}

Matrix operator * (const Matrix &A, const Matrix &B){
    Matrix C;
    mul(C, A, B);
    return C;
}

Matrix operator * (double a, const Matrix &B){
    Matrix C(B.ny, B.nx);
    for (int i = 0; i < C.nx*C.ny; i++){
        C.values[i] = a*B.values[i];
    }
    return C;
}

int n_iterations = 2000;

Vector<double> error_l1(n_iterations, 0.0);
Vector<double> error_l2(n_iterations, 0.0);
Vector<double> error_linf(n_iterations, 0.0);

void init(){
    double t = Time::sec();

    int n = 3;
    Matrix layers[1 + n];
    Matrix layer_deltas[n];
    Matrix weights[n];

    layers[0] = Matrix(STR(
        0 0 1;
        0 1 1;
        1 0 1;
        1 1 1;
    ));

    Matrix expected_output = STR(
        0;
        1;
        1;
        0;
    );

    weights[0] = Matrix::rand(3, 4, -1.0, +1.0);
    weights[1] = Matrix::rand(4, 3, -1.0, +1.0);
    weights[2] = Matrix::rand(3, 1, -1.0, +1.0);

    Matrix temps[2];

    Matrix difference;

    for (int j = 0; j < n_iterations; j++){
        for (int i = 0; i < n; i++){
            mul(layers[i + 1], layers[i], weights[i]);
            apply(layers[i + 1], layers[i + 1], sigmoid);
        }

        sub(temps[0], expected_output, layers[n]);
        apply(temps[1], layers[n], sigmoid_derivative);
        elementwise_mul(layer_deltas[n - 1], temps[0], temps[1]);

        for (int i = n - 1; i >= 1; i--){
            T(temps[1], weights[i]);
            mul(temps[0], layer_deltas[i], temps[1]);
            apply(temps[1], layers[i], sigmoid_derivative);
            elementwise_mul(layer_deltas[i - 1], temps[0], temps[1]);
        }

        for (int i = 0; i < n; i++){
            T(temps[0], layers[i]);
            mul(temps[1], temps[0], layer_deltas[i]);
            weights[i] += temps[1];
        }

        apply(difference, layer_deltas[2], fabs);
        elementwise_mul(temps[0], difference, difference);

        error_l1[j] = difference.sum();
        error_l2[j] = temps[0].sum();
        error_linf[j] = difference.max2();
    }

    double dt = Time::sec() - t;
    printf("%f\n", dt);
}

#include <GL/glut.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int w = 512;
int h = 512;

void draw(const void *data, int n, GLenum mode){
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, data);
    glDrawArrays(mode, 0, n);
}

void draw_line(float ax, float bx, float ay, float by){
    float xy[2*2] = {ax, ay, bx, by};
    draw(xy, 2, GL_LINES);
}

void draw_circle(float x, float y, float r){
    size_t i, n = 100;
    float xy[2*100];
    for (i = 0; i < n; i++){
        float angle = 2.0f*3.14159f*i/n;
        xy[i*2 + 0] = x + cos(angle)*r;
        xy[i*2 + 1] = y + sin(angle)*r;
    }
    draw(xy, n, GL_LINE_LOOP);
}

void on_frame(){
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, n_iterations, 0, 1, -1, +1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(1, 0, 0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < n_iterations; i++){
        float x = i;
        float y = error_l1[i];
        glVertex2f(x, y);
    }
    glEnd();

    glColor3f(0, 1, 0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < n_iterations; i++){
        float x = i;
        float y = error_l2[i];
        glVertex2f(x, y);
    }
    glEnd();

    glColor3f(0, 0, 1);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < n_iterations; i++){
        float x = i;
        float y = error_linf[i];
        glVertex2f(x, y);
    }
    glEnd();

    glutSwapBuffers();
}

void work(int frame){
    glutPostRedisplay();
    glutTimerFunc(20, work, frame + 1);
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutCreateWindow("");

    for (int i = 0; i < 5; i++) init();

    glutDisplayFunc(on_frame);
    work(0);
    glutMainLoop();
    return 0;
}
