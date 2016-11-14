#include "v2i.h"

#include <GL/glut.h>

#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>

v2i distance_transform_far_away(){
    return v2i_xy(INT_MIN, INT_MIN);
}

void distance_transform_rows(v2i *v, v2i *closest, int *regions, int w, int h){
    int y;
    v2i far_away = distance_transform_far_away();
    for (y = 0; y < h; y++, v += w){
        int region, x, k = 0;

        closest[0] = far_away;
        regions[0] = INT_MIN;

        for (x = 0; x < w; x++){
            if (v2i_eq(v[x], far_away)) continue;

            if (v2i_eq(closest[0], far_away)){
                closest[0] = v[x];
                continue;
            }

            while (1){
                v2i a = v[x], b = closest[k];
                a.y -= y;
                b.y -= y;
                region = (v2i_len2(a) - v2i_len2(b))/(2*(a.x - b.x));

                if (region > regions[k]){
                    k++;
                    closest[k] = v[x];
                    regions[k] = region;
                    break;
                }
                k--;
            }
        }
        regions[k + 1] = INT_MAX;

        k = 0;
        for (x = 0; x < w; x++){
            while (regions[k + 1] < x) k++;
            v[x] = closest[k];
        }
    }
}

void distance_transform_transpose(const v2i *src, v2i *dst, int w, int h){
    int i, x, y;
    for (i = 0, y = 0; y < h; y++)for (x = 0; x < w; x++, i++){
        v2i a = src[i];
        dst[y + x*h] = v2i_xy(a.y, a.x);
    }
}

void on_frame(){
    int t = glutGet(GLUT_ELAPSED_TIME);
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);
    int n = w > h ? w : h, i, x, y;

    int *regions  = (int*)malloc(sizeof(*regions )*n);
    v2i *closest  = (v2i*)malloc(sizeof(*closest )*n);
    v2i *closest0 = (v2i*)malloc(sizeof(*closest0)*w*h);
    v2i *closest1 = (v2i*)malloc(sizeof(*closest1)*w*h);
    float *distance = (float*)malloc(sizeof(*distance)*w*h);

    int radius = 100;
    v2i center = v2i_xy(w/2, h/2);
    v2i far_away = distance_transform_far_away();

    /* points are set to (x, y) if they are inside the circle */
    /* else they are set to far_away */
    for (i = 0, y = 0; y < h; y++) for (x = 0; x < w; x++, i++){
        v2i a = v2i_xy(x, y);
        closest0[i] = radius*radius > v2i_dist2(a, center) ? a : far_away;
    }

    /* separable distance transform */
    distance_transform_rows(closest0, closest, regions, w, h);
    distance_transform_transpose(closest0, closest1, w, h);
    distance_transform_rows(closest1, closest, regions, h, w);
    distance_transform_transpose(closest1, closest0, h, w);

    /* calcualte distance to closest point */
    for (i = 0, y = 0; y < h; y++) for (x = 0; x < w; x++, i++){
        v2i a = v2i_xy(x, y), b = closest0[i];

        if (v2i_eq(b, far_away)){
            /* not a single point set on the whole image */
            distance[i] = 1.0f/0.0f;
        }else{
            int dist2 = v2i_dist2(a, b);
            distance[i] = 0.01f*sqrtf(dist2);
        }
    }

    printf("%i milliseconds\n", glutGet(GLUT_ELAPSED_TIME) - t);

    glViewport(0, 0, w, h);
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawPixels(w, h, GL_LUMINANCE, GL_FLOAT, distance);

    free(distance);
    free(closest0);
    free(closest1);
    free(closest);
    free(regions);

    glutSwapBuffers();
}

void work(int frame){
    glutPostRedisplay();
    glutTimerFunc(20, work, frame + 1);
}

#define UNUSED(x) ((void)x)

void on_key(unsigned char key, int x, int y){
    int ESC = 27;
    UNUSED(x);
    UNUSED(y);
    if (key == 'q' || key == ESC) exit(0);
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitWindowSize(512, 256);
    glutCreateWindow("");
    glutDisplayFunc(on_frame);
    glutKeyboardFunc(on_key);
    work(0);
    glutMainLoop();
    return 0;
}

