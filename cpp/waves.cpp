#include "vec2.hpp"
#include <GL/glut.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9 * t.tv_nsec;
}

int w = 512;
int h = 512;

void draw(const void *data, int n, GLenum mode){
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, data);
    glDrawArrays(mode, 0, n);
}
/*
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
*/

const float TWO_PI_F = 2.0 * 3.14159265358989;

void draw_circle(Vec2 center, float radius){
    const int n = 64;
    Vec2 points[n];
    for (int i = 0; i < n; i++){
        float angle = TWO_PI_F * i / n;
        points[i] = center + radius * polar(angle);
    }
    draw(points, n, GL_LINE_LOOP);
}

void on_frame(){
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    static double t_start = sec();

    double t = sec() - t_start;

    const int n = 100;
    float wave_height = 0.1f;
    float wave_frequency = 4.0f;
    Vec2 points[n];
    for (int i = 0; i <= n; i++){
        Vec2 p(4.0f * i / n - 2.0f, 0.0f);

        float angle = wave_frequency * i * TWO_PI_F / n - t;

        Vec2 q = p + wave_height * polar(angle);

        points[i] = q;

        draw_circle(q, 0.01f);
    }

    draw(points, n, GL_LINE_STRIP);

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

    glutDisplayFunc(on_frame);
    work(0);
    glutMainLoop();
    return 0;
}
