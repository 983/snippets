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

    uint32_t *pixels = (uint32_t*)malloc(sizeof(*pixels)*w*h);
    for (int y = 0; y < h; y++) for (int x = 0; x < w; x++){
        float dx = x - (w - 1)*0.5f;
        float dy = y - (h - 1)*0.5f;
        float r = 100.0f;

        pixels[x + y*w] = dx*dx + dy*dy < r*r ? 0xffffffff : 0;
    }
    glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    free(pixels);

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
