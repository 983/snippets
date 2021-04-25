#include <GL/glut.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <FreeImage.h>
#include "mat4.c"

#define WHERE printf("Line: %i\n", __LINE__);

#define UNUSED(x) ((void)x)

int window_width = 1920;
int window_height = 1080;
Vec2 mouse;
Vec2 mouse_old;
Mat4 view;
Mat4 projection;
int image_width;
int image_height;
GLuint image_texture;
char is_mouse_button_down[256];
char *current_path = NULL;
/*
#include <GL/glx.h>

typedef void (*MY_PFNGLGENERATEMIPMAPPROC) (GLenum target);

MY_PFNGLGENERATEMIPMAPPROC my_glGenerateMipmap;
*/
GLuint make_texture(const void *pixels, int width, int height){
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
/*
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
*/
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    //my_glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
}

typedef struct Vertex {
    float x, y;
    float u, v;
} Vertex;

void draw(const Vertex *vertices, size_t n, GLenum mode){
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &vertices->x);
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &vertices->u);
    glDrawArrays(mode, 0, n);
}

void draw_rect(float x0, float y0, float x1, float y1){
    Vertex vertices[4] = {
        (Vertex){x0, y0, 0.0f, 0.0f},
        (Vertex){x1, y0, 1.0f, 0.0f},
        (Vertex){x1, y1, 1.0f, 1.0f},
        (Vertex){x0, y1, 0.0f, 1.0f},
    };

    draw(vertices, 4, GL_QUADS);
}

void on_frame(){
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    Mat4 mvp = m4_mul(projection, view);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glMultMatrixf(mvp.data);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    draw_rect(0.0f, 0.0f, image_width, image_height);

    glutSwapBuffers();
}

void maximize_image(){
    projection = m4_ortho(0.0f, window_width, 0.0f, window_height);

    float scale = fmin((float)window_width / image_width, (float)window_height / image_height);

    float dx = 0.5f * (window_width - scale * image_width);
    float dy = 0.5f * (window_height - scale * image_height);
    view = m4_eye();
    view = m4_translate(view, dx, dy, 0.0f);
    view = m4_scale_xy(view, scale);

    glutPostRedisplay();
}

void on_reshape(int new_width, int new_height){
    window_width = new_width;
    window_height = new_height;

    glViewport(0, 0, new_width, new_height);
    projection = m4_ortho(0.0f, window_width, 0.0f, window_height);

    maximize_image();

    glutPostRedisplay();
}

void update_mouse(int x, int y){
    y = window_height - 1 - y;

    mouse_old = mouse;
    mouse = (Vec2){(float)x, (float)y};
}

#define GLUT_WHEEL_UP 3
#define GLUT_WHEEL_DOWN 4

void zoom(float scale){
    Vec2 pivot = m4_transform_v2(m4_inv(view), mouse);

    view = m4_translate(view, pivot.x, pivot.y, 0.0f);

    view = m4_scale_xy(view, scale);

    view = m4_translate(view, -pivot.x, -pivot.y, 0.0f);
}

void on_mouse_move(int x, int y){
    update_mouse(x, y);

    if (
        is_mouse_button_down[GLUT_LEFT_BUTTON] |
        is_mouse_button_down[GLUT_RIGHT_BUTTON] |
        is_mouse_button_down[GLUT_MIDDLE_BUTTON]
    ){
        Vec2 a = m4_transform_v2(m4_inv(view), mouse);
        Vec2 b = m4_transform_v2(m4_inv(view), mouse_old);

        float dx = a.x - b.x;
        float dy = a.y - b.y;

        view = m4_translate(view, dx, dy, 0.0f);

        glutPostRedisplay();
    }
}

void on_mouse_button(int button, int action, int x, int y){
    update_mouse(x, y);

    is_mouse_button_down[button] = action == GLUT_DOWN;

    float zoom_factor = 3.0f / 4.0f;

    if (action == GLUT_DOWN){
        if (button == GLUT_WHEEL_UP){
            zoom(1.0f / zoom_factor);

            glutPostRedisplay();
        }
        if (button == GLUT_WHEEL_DOWN){
            zoom(zoom_factor);

            glutPostRedisplay();
        }
    }
}

void load_image(const char *path){

    free(current_path);

    current_path = strdup(path);

    if (image_texture){
        glDeleteTextures(1, &image_texture);
    }

    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(path, 0);

    FIBITMAP *bitmap = FreeImage_Load(format, path, 0);

    if (!bitmap){
        fprintf(stderr, "ERROR: Failed to load image:\n%s\n", path);
        exit(-1);
    }

    // convert to RGBA if necessary
    if (FreeImage_GetBPP(bitmap) != 32){
        FIBITMAP *temp = bitmap;
        bitmap = FreeImage_ConvertTo32Bits(bitmap);
        FreeImage_Unload(temp);
    }

    int width = FreeImage_GetWidth(bitmap);
    int height = FreeImage_GetHeight(bitmap);

    char title[1024];
    snprintf(title, sizeof(title), "%i x %i %s", width, height, path);

    glutSetWindowTitle(title);

    image_width = width;
    image_height = height;

    maximize_image();

    uint8_t *data = FreeImage_GetBits(bitmap);

    for (int i = 0; i < width * height * 4; i += 4){
        uint8_t r = data[i + 0];
        uint8_t b = data[i + 2];

        data[i + 0] = b;
        data[i + 2] = r;
    }

    GLuint texture = make_texture(data, width, height);

    glBindTexture(GL_TEXTURE_2D, texture);

    FreeImage_Unload(bitmap);

    glutPostRedisplay();
}

char* my_substr(const char *a, const char *b){
    // creates a new string [a, b)

    if (!a && !b) return NULL;

    if (!a){
        a = b;
        b = NULL;
    }

    if (!b){
        b = a + strlen(a);
    }

    size_t n = b - a;

    char *s = (char*)malloc(n + 1);
    memcpy(s, a, n);
    s[n] = '\0';

    return s;
}

int endswith(const char *s, const char *suffix){
    size_t n = strlen(s);
    size_t n_suffix = strlen(suffix);

    if (n < n_suffix) return 0;

    return 0 == memcmp(s + (n - n_suffix), suffix, n_suffix);
}

int compare_natural(const void *p, const void *q){
    const char *a = *(const char**)p;
    const char *b = *(const char**)q;

    while (*a != '\0' && *b != '\0'){
        if (!isdigit(*a) || !isdigit(*b)){
            if (*a < *b) return -1;
            if (*a > *b) return +1;
            a++;
            b++;
            continue;
        }

        long x = strtol(a, (char**)&a, 10);
        long y = strtol(b, (char**)&b, 10);

        if (x < y) return -1;
        if (x > y) return +1;
    }

    if (*a == '\0' && *b != '\0') return -1;
    if (*a != '\0' && *b == '\0') return +1;

    return 0;
}

void natsort(const char **values, size_t n){
    qsort(values, n, sizeof(*values), compare_natural);
}

int might_be_image_file(const char *name){
    static const char *extensions[] = {
        ".png",
        ".jpg",
        ".jpeg",
        ".gif",
        ".tif",
        ".bmp",
        ".ppm",
        ".pgm",
    };

    for (size_t i = 0; i < sizeof(extensions) / sizeof(*extensions); i++){
        if (endswith(name, extensions[i])) return 1;
    }
    return 0;
}

void advance_image(int step){
    char *slash = strrchr((char*)current_path, '/');
    char *name;
    char *directory;

    if (slash){
        name = strdup(slash + 1);
        directory = my_substr(current_path, slash);
    }else{
        name = strdup(current_path);
        directory = strdup(".");
    }

    DIR *d = opendir(directory);

    char *names[10000];
    int n_names = 0;

    while (1){
        struct dirent *entry = readdir(d);

        if (!entry) break;

        if (might_be_image_file(entry->d_name)){
            names[n_names++] = strdup(entry->d_name);
        }

        // TODO grow names
        if (n_names >= 10000) break;
    }

    closedir(d);

    natsort((const char**)names, n_names);

    int index = -1;

    for (int i = 0; i < n_names; i++){
        if (0 == strcmp(names[i], name)){
            index = i;
        }
    }

    if (index == -1){
        fprintf(stderr, "ERROR: Failed to find images in directory:\n%s\n", directory);
        exit(-1);
    }

    index = (index + n_names + step) % n_names;

    char new_path[65536];
    snprintf(new_path, sizeof(new_path), "%s/%s", directory, names[index]);

    load_image(new_path);

    for (int i = 0; i < n_names; i++){
        free(names[i]);
    }

    free(name);
    free(directory);
}

void on_special_key_down(int key, int x, int y){
    update_mouse(x, y);

    switch (key){
        case GLUT_KEY_F5:
            advance_image(0);
        break;

        case GLUT_KEY_LEFT:
            advance_image(-1);
        break;

        case GLUT_KEY_RIGHT:
            advance_image(1);
        break;
    }
}

void on_key_down(unsigned char key, int x, int y){
    update_mouse(x, y);

    switch (key){
        case 'q':
        // Escape key
        case 27:
        // Ctrl + q (ctrl + a = 1, ctrl + b = 2, ctrl + c = 3, ...)
        case 17:
            exit(0);
        break;

        case 32:
            maximize_image();
        break;
    }
}

int main(int argc, char **argv){
    const char *path = "/home/user/projects/viewer/test.png";

    if (argc >= 2){
        path = argv[1];
    }

    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(path, 0);

    if (format == -1){
        printf("ERROR: Failed to load image:\n%s\n", path);
        return -1;
    }

    view = m4_eye();
    projection = m4_ortho(0.0f, window_width, 0.0f, window_height);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("");
    glutKeyboardFunc(on_key_down);
    glutReshapeFunc(on_reshape);
    glutMouseFunc(on_mouse_button);
    glutMotionFunc(on_mouse_move);
    glutPassiveMotionFunc(on_mouse_move);
    glutSpecialFunc(on_special_key_down);

    //my_glGenerateMipmap = (MY_PFNGLGENERATEMIPMAPPROC)glXGetProcAddress((const GLubyte*)"glGenerateMipmap");

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    load_image(path);

    glutDisplayFunc(on_frame);

    glutMainLoop();
    return 0;
}
