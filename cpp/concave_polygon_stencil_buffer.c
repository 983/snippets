// Build on linux:
// sudo apt install freeglut3-dev
// gcc main.c -o main -lGL -lglut
#include <GL/glut.h>

void drawVertices(GLenum mode, const void *vertices, int n_vertices){
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glDrawArrays(mode, 0, n_vertices);
}

// Draw concave polygon using stencil buffer, technique from:
// "Drawing Filled, Concave Polygons Using the Stencil Buffer"
// OpenGl Programming Guide - The Official Guide to Learning OpenGL,
// Release 1, Eighth priting, May 1996
// Alternative techniques may use the color or depth buffer instead of
// the stencil buffer to accumulate polygon winding information.
// Make sure to request stencil buffer support on window creation!
void drawConcavePolygon(const void *vertices, int n_vertices){
    if (n_vertices == 0) return;
    
    // prepare stencil buffer
    glEnable(GL_STENCIL_TEST);
    glClear(GL_STENCIL_BUFFER_BIT);
    
    // set stencil buffer to invert value on draw, 0 to 1 and 1 to 0
    glStencilFunc(GL_ALWAYS, 0, 1);
    glStencilOp(GL_INVERT, GL_INVERT, GL_INVERT);
    
    // disable writing to color buffer
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    
    // draw polygon into stencil buffer
    drawVertices(GL_TRIANGLE_FAN, vertices, n_vertices);
    
    // set stencil buffer to only keep pixels when value in buffer is 1
    glStencilFunc(GL_EQUAL, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    
    // enable color again
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    
    // redraw polygon again, this time into color buffer.
    // alternatively, only draw bounding box of polygon
    drawVertices(GL_TRIANGLE_FAN, vertices, n_vertices);
    
    glDisable(GL_STENCIL_TEST);
}

int window_width = 640;
int window_height = 480;

typedef struct {
    float x, y;
} Vertex;

#define MAX_VERTICES 65536

Vertex vertices[MAX_VERTICES];
int n_vertices = 0;

void addVertex(float x, float y){
    Vertex v;
    
    v.x = x;
    v.y = y;
    
    vertices[n_vertices++] = v;
}

void mouseMove(int x, int y){
    if (n_vertices >= MAX_VERTICES) return;
    
    addVertex(x, y);
}

void mouseButton(int button, int state, int x, int y){
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        n_vertices = 0;
        
        addVertex(x, y);
    }
}

void redraw(){
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window_width, window_height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    drawConcavePolygon(vertices, n_vertices);
    
    glutSwapBuffers();
}

void mainLoop(int frame){
    glutPostRedisplay();
    glutTimerFunc(1000/60, mainLoop, frame + 1);
}

void resize(int width, int height){
    glViewport(0, 0, width, height);
    window_width = width;
    window_height = height;
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    // request stencil buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_STENCIL);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("Use mouse to draw");

    glutMotionFunc(mouseMove);
    glutMouseFunc(mouseButton);
    glutDisplayFunc(redraw);
    glutReshapeFunc(resize);
    mainLoop(0);
    glutMainLoop();
    return 0;
}
