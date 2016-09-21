#include "graphics/shader.hpp"
#include "graphics/vertex.hpp"
#include "graphics/color.hpp"
#include "util/vector.hpp"
#include "util/util.hpp"
#include "math/mat.hpp"

int w = 512;
int h = 512;

Shader *shader;
GLuint texture;

GLuint make_texture(int width, int height, const void *pixels){
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    return texture;
}

Vertex vert(float x, float y, float u, float v, uint32_t color){
    Vertex w;
    memset(&w, 0, sizeof(w));
    w.data[0] = x;
    w.data[1] = y;
    w.data[2] = u;
    w.data[3] = v;
    Color::to_float(color, w.data + 4);
    return w;
}

void rect(
    Vector<Vertex> &vertices,
    float x0, float y0, float x1, float y1,
    float u0 = 0.0f,
    float v0 = 0.0f,
    float u1 = 1.0f,
    float v1 = 1.0f,
    uint32_t color = Color::white()
){
    vertices.push(vert(x0, y0, u0, v0, color));
    vertices.push(vert(x1, y0, u1, v0, color));
    vertices.push(vert(x1, y1, u1, v1, color));

    vertices.push(vert(x0, y0, u0, v0, color));
    vertices.push(vert(x1, y1, u1, v1, color));
    vertices.push(vert(x0, y1, u0, v1, color));
}

void on_frame(){
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader->use();

    Mat4f projection = ortho(0.0f, float(w), 0.0f, float(h));
    Mat4f mvp = projection;

    int location = glGetUniformLocation(shader->program, "mvp");
    assert(location != -1);
    glUniformMatrix4fv(location, 1, GL_FALSE, mvp.data());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    GLuint iChannel0 = glGetUniformLocation(shader->program, "iChannel0");
    glUniform1i(iChannel0, 0);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    int max_vertices = 1024*1024;
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*max_vertices, NULL, GL_STATIC_DRAW);

    Vector<Vertex> vertices;
    rect(vertices, 0.0f, 0.0f, float(w), float(h));

    for (int i = 0; i < MAX_ATTRIBUTES; i++){
        int location = shader->attributes[i];
        if (location != -1){
            glEnableVertexAttribArray(location);
            glVertexAttribPointer(location, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char*)(i*16));
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*vertices.size(), vertices.data());

    glDrawArrays(GL_TRIANGLES, 0, vertices.size());

    glDeleteBuffers(1, &vbo);

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

    glewInit();

    const char *vert_src = STR(
        attribute vec4 a_data0;
        attribute vec4 a_data1;
        attribute vec4 a_data2;
        attribute vec4 a_data3;

        varying vec4 v_data0;
        varying vec4 v_data1;
        varying vec4 v_data2;
        varying vec4 v_data3;

        uniform mat4 mvp;

        void main(){
            v_data0 = a_data0;
            v_data1 = a_data1;
            v_data2 = a_data2;
            v_data3 = a_data3;

            gl_Position = mvp*vec4(a_data0.xy, 0.0, 1.0);
        }
    );

    const char *frag_src = STR(
        varying vec4 v_data0;
        varying vec4 v_data1;
        varying vec4 v_data2;
        varying vec4 v_data3;

        uniform sampler2D iChannel0;

        void main(){
            gl_FragColor = texture2D(iChannel0, v_data0.zw)*v_data1;
        }
    );

    int width = 32;
    int height = 32;
    Vector<uint32_t> pixels(width*height, 0);
    for (int i = 0; i < width*height; i++){
        uint32_t c = rand() & 255;
        pixels[i] = 0xff000000 | (c << 16) | (c << 8) | c;
    }
    texture = make_texture(width, height, pixels.data());

    shader = new Shader(vert_src, frag_src);

    glutDisplayFunc(on_frame);
    work(0);
    glutMainLoop();
    return 0;
}
