#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../snippets/util/vector.hpp"
#include "../snippets/util/time.hpp"
#include "simd_mat4f.hpp"

#define STR(x) #x
#define WHERE printf("Line: %i\n", __LINE__);

#if 1
static const float rate = 0.5f;

float activation(float x){
    return 1.0f/(1.0f + powf(2.0f, x));
}

#define LOG2_F 0.6931471805599453f

float activation_derivative(float x){
    return LOG2_F*x*(1.0f - x);
}

#elif 0
static const float rate = 0.03f;

float activation(float x){
    return tanhf(x);
}

float activation_derivative(float x){
    return 1.0f/cosh(x)/cosh(x);

    //return 1.0f - x*x;
}
#else
static const float rate = 0.01f;
static const float alpha = 0.1f;

float activation(float x){
    return x < 0.0f ? alpha * x : x;
}

float activation_derivative(float x){
    return x < 0.0f ? alpha : 1.0f;
}
#endif

float frandf(float a, float b){
    float u = 1.0f/RAND_MAX * rand();
    return a + (b - a)*u;
}

int n_runs = 7;
int n_iterations = 5000;

Vector<double> error_l1  (n_iterations*n_runs, 0.0);
Vector<double> error_l2  (n_iterations*n_runs, 0.0);
Vector<double> error_linf(n_iterations*n_runs, 0.0);

template <typename FUNC>
Mat4f apply(Mat4f a, FUNC func){
    Mat4f c;
    for (int k = 0; k < 4*4; k++) c.data[k] = func(a.data[k]);
    return c;
}

Mat4f rand4x4(){
    Mat4f c;
    for (int k = 0; k < 4*4; k++){
        c.data[k] = frandf(-1.0f, +1.0f);
    }
    return c;
}

float sum(Mat4f a){
    float sum = 0.0f;
    for (int i = 0; i < 4*4; i++){
        sum += a.data[i];
    }
    return sum;
}

float max2(Mat4f a){
    float result = -1.0f/0.0f;
    for (int i = 0; i < 4*4; i++){
        if (result < a.data[i]) result = a.data[i];
    }
    return result;
}

Mat4f transpose(Mat4f a){
    Mat4f c;
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++){
        c(j, i) = a(i, j);
    }
    return c;
}

void init(){
    double t = Time::sec();

    Mat4f layer0 = {
        0, 0, 1, 0,
        0, 1, 1, 0,
        1, 0, 1, 0,
        1, 1, 1, 0,
    };

    Mat4f expected_output = {
        0, 0, 0, 0,
        1, 1, 1, 1,
        1, 1, 1, 1,
        0, 0, 0, 0
    };

    Mat4f weights0 = rand4x4();
    Mat4f weights1 = rand4x4();
    Mat4f weights2 = rand4x4();

    for (int j = 0; j < n_iterations; j++){
        auto layer1 = apply(layer0 * weights0, activation);
        auto layer2 = apply(layer1 * weights1, activation);
        auto layer3 = apply(layer2 * weights2, activation);

#if 0
        // calculate derivative directly
        auto layer3_delta = matrixCompMult(expected_output - layer3          , apply(layer2 * weights2, activation_derivative));
        auto layer2_delta = matrixCompMult(layer3_delta * transpose(weights2), apply(layer1 * weights1, activation_derivative));
        auto layer1_delta = matrixCompMult(layer2_delta * transpose(weights1), apply(layer0 * weights0, activation_derivative));
#else
        // calculate derivative from forward result
        auto layer3_delta = matrixCompMult(expected_output - layer3          , apply(layer3, activation_derivative));
        auto layer2_delta = matrixCompMult(layer3_delta * transpose(weights2), apply(layer2, activation_derivative));
        auto layer1_delta = matrixCompMult(layer2_delta * transpose(weights1), apply(layer1, activation_derivative));
#endif

        weights0 += rate * transpose(layer0) * layer1_delta;
        weights1 += rate * transpose(layer1) * layer2_delta;
        weights2 += rate * transpose(layer2) * layer3_delta;

        auto difference = apply(layer3_delta, fabsf);

        error_l1.push(sum(difference));
        error_l2.push(sum(matrixCompMult(difference, difference)));
        error_linf.push(max2(difference));
    }

    double dt = Time::sec() - t;
    printf("%f %f %f %f\n", dt, error_l1.back(), error_l2.back(), error_linf.back());
}

#include <GL/glut.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int w = 256*n_runs;
int h = 256;

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
    glOrtho(0, n_iterations*n_runs, 0, 1, -1, +1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glColor3f(1, 0, 0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < n_iterations*n_runs; i++){
        float x = i;
        float y = error_l1[i];
        glVertex2f(x, y);
    }
    glEnd();

    glColor3f(0, 1, 0);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < n_iterations*n_runs; i++){
        float x = i;
        float y = error_l2[i];
        glVertex2f(x, y);
    }
    glEnd();

    glColor3f(0, 0, 1);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < n_iterations*n_runs; i++){
        float x = i;
        float y = error_linf[i];
        glVertex2f(x, y);
    }
    glEnd();

    glColor3f(1, 1, 1);
    glBegin(GL_LINES);
    for (int i = 0; i < n_runs; i++){
        float x = i*n_iterations;
        glVertex2f(x, 0.0f);
        glVertex2f(x, 1.0f);
        glVertex2f(x + 1.0f, 0.0f);
        glVertex2f(x + 1.0f, 1.0f);
    }
    glEnd();

    glutSwapBuffers();
}

void work(int frame){
    glutPostRedisplay();
    glutTimerFunc(20, work, frame + 1);
}

int main(int argc, char **argv){

    int n = 10;
    float eps = 1e-3f;
    for (int i = 0; i <= n; i++){
        float u = i*1.0f/n;
        float x = -5.0f + u*10.0f;
        float a = (activation(x - eps) - activation(x + eps)) * (0.5f/eps);
        float b = activation_derivative(activation(x));
        printf("%f, %f %f\n", a - b, a, b);
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutCreateWindow("");

    error_l1.clear();
    error_l2.clear();
    error_linf.clear();

    for (int i = 0; i < n_runs; i++) init();

    glutDisplayFunc(on_frame);
    work(0);
    glutMainLoop();

    return 0;
}
