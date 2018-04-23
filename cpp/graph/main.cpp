#include "graph.h"

#include <GL/freeglut.h>
#include <GL/gl.h>
#include "mat.h"

#include <functional>

#define GL_CLAMP_TO_EDGE 0x812F

typedef Vector<uint8_t> Bytes;

Bytes load_file(const char *path){
    Bytes data;
    FILE *fp = fopen(path, "rb");
    assert(fp != NULL);
    fseek(fp, 0, SEEK_END);
    size_t n = ftell(fp);
    data.resize(n);
    rewind(fp);
    fread(data.data(), 1, n, fp);
    fclose(fp);
    return data;
}

template <typename T>
T my_min(const T &a, const T &b){
    return a < b ? a : b;
}

template <typename T>
T my_max(const T &a, const T &b){
    return a >= b ? a : b;
}

template <typename T>
void my_swap(T &a, T &b){
    T temp(a);
    a = b;
    b = temp;
}

int width = 512;
int height = 512;

Points points;

Vertices vertices;
Edges edges;

Matd modelview = Matd::eye(4);

double vertex_radius = 20.0;

Point mouse;
Point old_mouse;

const Vertex *selected_vertex = NULL;

int mouse_button_down[256];

Vec2d screen_to_world(Vec2d p){
    const double *m = modelview.data();
    
    double inv[4*4];
    inverse4(m, inv);
    
    double x = inv[0]*p.x + inv[4]*p.y + inv[12];
    double y = inv[1]*p.x + inv[5]*p.y + inv[13];
    
    return Point{x, y};
}

void set_color(Color color){
    float r = (color >> 0*8) & 0xff;
    float g = (color >> 1*8) & 0xff;
    float b = (color >> 2*8) & 0xff;
    float a = (color >> 3*8) & 0xff;
    
    r *= 1.0f/255.0f;
    g *= 1.0f/255.0f;
    b *= 1.0f/255.0f;
    a *= 1.0f/255.0f;
    
    glColor4f(r, g, b, a);
}

Points make_circle_points(Point center, double radius, int n = 128){
    Points points(n);
    
    for (int i = 0; i < n; i++){
        double angle = TWO_PI/n * i;
        double x = center.x + radius*cos(angle);
        double y = center.y + radius*sin(angle);
        
        points[i] = Point{x, y};
    }
    
    return points;
}

void draw_disk(Point center, double radius, Color color = COLOR_BLACK, int n = 128){
    set_color(color);
    
    Points points = make_circle_points(center, radius);
    
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(center.x, center.y);
    for (const Point &p : points){
        glVertex2f(p.x, p.y);
    }
    const Point &p = points[0];
    glVertex2f(p.x, p.y);
    glEnd();
}

void draw_circle(Point center, double radius, Color color = COLOR_BLACK, int n = 128){
    set_color(color);
    
    Points points = make_circle_points(center, radius);
    
    glBegin(GL_LINE_LOOP);
    for (const Point &p : points){
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

void draw_line(Point a, Point b, Color color = COLOR_BLACK){
    set_color(color);
    
    glBegin(GL_LINES);
    glVertex2f(a.x, a.y);
    glVertex2f(b.x, b.y);
    glEnd();
}

void draw_vertex(Point p){  
    draw_disk(p, vertex_radius, COLOR_WHITE);
    draw_circle(p, vertex_radius);
}

struct Font {
    GLuint texture;
    uint32_t image_width;
    uint32_t image_height;
    uint32_t dx;
    uint32_t dy;
    Bytes font_info;
    
    Font(const char *image_path, const char *info_path){
        Bytes font_image = load_file(image_path);
        
        font_info = load_file(info_path);
        
        assert(font_info.size() == 4*(4 + 256*2));
        
        uint32_t *info = (uint32_t*)font_info.data();
        
        image_width  = info[0];
        image_height = info[1];
        
        dx = info[2];
        dy = info[3];
        
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, font_image.data());
    }
    
    Font(const Font&);
    Font& operator = (const Font&);
    
    uint32_t get_char_width(char c) const {
        uint8_t idx = *(uint8_t*)&c;
        uint32_t *info = (uint32_t*)font_info.data();
        return info[4 + idx*2 + 0];
    }
    
    uint32_t get_char_height(char c) const {
        uint8_t idx = *(uint8_t*)&c;
        uint32_t *info = (uint32_t*)font_info.data();
        return info[4 + idx*2 + 1];
    }
    
    ~Font(){
        glDeleteTextures(1, &texture);
    }
};

Font *default_font = NULL;

Vec2d get_text_size(const char *text, const Font *font = NULL){
    if (!font) font = default_font;
    
    double x = 0.0;
    double y = 0.0;
    
    uint32_t dy = font->dy;
    
    double x1 = 0.0;
    double y1 = 0.0;
    
    for (const char *c = text; *c != '\0'; c++){
        uint32_t char_width  = font->get_char_width (*c);
        uint32_t char_height = font->get_char_height(*c);
        
        if (*c == '\n'){
            x = 0.0;
            y -= dy;
            
            y1 = my_min(y1, y - char_height);
        }else{
            x1 = my_max(x1, x + char_width);
            y1 = my_min(y1, y - char_height);
            
            x += char_width;
        }
    }
    
    return Vec2d{x1, y1};
}

void draw_text(
    const char *text,
    double x0,
    double y0,
    const Font *font,
    Color color = COLOR_BLACK
){
    if (!font) font = default_font;
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font->texture);
    
    double x = x0;
    double y = y0;
    
    uint32_t image_width = font->image_width;
    uint32_t image_height = font->image_height;
    uint32_t dx = font->dx;
    uint32_t dy = font->dy;

    set_color(color);
    
    for (const char *c = text; *c != '\0'; c++){
        if (*c == '\n'){
            x = x0;
            y -= dy;
            continue;
        }
        
        uint8_t idx = *(uint8_t*)c;
        
        uint8_t ix = idx % 16;
        uint8_t iy = idx / 16;
        
        uint32_t char_width  = font->get_char_width (*c);
        uint32_t char_height = font->get_char_height(*c);
        
        double u0 = (ix*dx              )*1.0/image_width;
        double v0 = (iy*dy              )*1.0/image_height;
        double u1 = (ix*dx + char_width )*1.0/image_width;
        double v1 = (iy*dy + char_height)*1.0/image_height;
        
        glBegin(GL_QUADS);
        glTexCoord2f(u0, v0); glVertex2f(x             , y              );
        glTexCoord2f(u1, v0); glVertex2f(x + char_width, y              );
        glTexCoord2f(u1, v1); glVertex2f(x + char_width, y - char_height);
        glTexCoord2f(u0, v1); glVertex2f(x             , y - char_height);
        glEnd();
        
        x += char_width;
    }
    
    glDisable(GL_TEXTURE_2D);
}

void draw_text_centered(
    const char *text,
    float x,
    float y,
    const Font *font = NULL,
    Color color = COLOR_BLACK
){
    if (!font) font = default_font;
    
    Vec2d size = get_text_size(text, font);
    
    draw_text(text, x - 0.5*size.x, y - 0.5*size.y, font, color);
}

void display(void){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixd(modelview.data());
    
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (Edge e : edges){
        Point p = points[e.a];
        Point q = points[e.b];
        draw_line(p, q);
    }
    
    for (size_t i = 0; i < points.size(); i++){
        Point p = points[i];
        
        draw_vertex(p);
        
        char label[128];
        snprintf(label, sizeof(label), "%i", (int)i);
        draw_text_centered(label, p.x, p.y + 3.0f);
    }
    
    Point p = screen_to_world(mouse);
    
    draw_circle(p, 5.0, COLOR_GREEN);
    
    glutSwapBuffers();
}

void reshape(int new_width, int new_height){
    width = new_width;
    height = new_height;
    
    glViewport(0, 0, width, height);
    
    glutPostRedisplay();
}

void onKey(unsigned char key, int, int){
    if (key == 27 || key == 'q'){
        printf("quit\n");
        exit(0);
    }
}

void updateMouse(int x, int y){
    old_mouse = mouse;
    y = height - 1 - y;
    mouse = Point{(double)x, (double)y};
}

void zoom(double s){
    Vec2d pivot = screen_to_world(mouse);
    
    modelview = modelview * translate(pivot.x, pivot.y);
    modelview = modelview * scale(s, s);
    modelview = modelview * translate(-pivot.x, -pivot.y);
}

void onMouseButton(int button, int state, int x, int y){
    updateMouse(x, y);
    
#define GLUT_WHEEL_UP 3
#define GLUT_WHEEL_DOWN 4

    const double MAGNIFICATION = 17.0/16.0;

    if (button == GLUT_WHEEL_UP){
        zoom(MAGNIFICATION);
    }
    
    if (button == GLUT_WHEEL_DOWN){
        zoom(1.0/MAGNIFICATION);
    }
    
    mouse_button_down[button] = state == GLUT_DOWN;
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
        Point p = screen_to_world(mouse);
        
        for (const Vertex &vertex : vertices){
            if (dist2(points[vertex], p) <= vertex_radius*vertex_radius){
                selected_vertex = &vertex;
            }
        }
    }
    
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){
        selected_vertex = NULL;
    }
    
    glutPostRedisplay();
}

void onMove(int x, int y){
    updateMouse(x, y);

    if (selected_vertex){
        points[*selected_vertex] = screen_to_world(mouse);
    }
    
    if (mouse_button_down[GLUT_RIGHT_BUTTON]){
        Vec2d delta = screen_to_world(mouse) - screen_to_world(old_mouse);
        
        modelview = modelview*translate(delta.x, delta.y);
    }
    
    glutPostRedisplay();
}

Point random_point(){
    double x = randu()*width;
    double y = randu()*height;
    return Point{x, y};
}

Matd calculate_distances(const Vertices &vertices, const Edges &edges){
    size_t n = vertices.size();
    
    Matd dist(n, n, INFINITY);
    
    for (const Vertex &v : vertices){
        dist(v, v) = 0.0;
    }
    
    for (const Edge &e : edges){
        Vertex a = e.a;
        Vertex b = e.b;
        
        dist(a, b) = 1.0;
        dist(b, a) = 1.0;
    }
    
    for (size_t k = 0; k < n; k++){
        for (size_t i = 0; i < n; i++){
            for (size_t j = 0; j < n; j++){
                double d = dist(i, k) + dist(k, j);
                if (dist(i, j) > d){
                    dist(i, j) = d;
                }
            }
        }
    }
    
    return dist;
}

template <typename T>
void subsets(
    const Vector<T> &values,
    size_t k,
    Vector<T> &subset,
    std::function<void(void)> callback,
    size_t pos=0
){
    
    // found subset
    if (subset.size() == k){
        callback();
        return;
    }
    
    // no more values to put into subset
    if (pos >= values.size()) return;
    
    // either put value into subset and recur
    subset.push_back(values[pos]);
    subsets(values, k, subset, callback, pos + 1);
    subset.pop_back();
    
    // or don't put value into subset and recur
    subsets(values, k, subset, callback, pos + 1);
}

bool same_distances(
    Vertex v,
    Vertex w,
    const Vertices &subset,
    const Matd &dist
){
    for (const Vertex &s : subset){
        if (dist(s, v) != dist(s, w)) return false;
    }
    return true;
}

bool is_separating_set(
    const Vertices &vertices,
    const Edges &edges,
    const Vertices &subset,
    const Matd &dist
){
    for (size_t i = 0; i < vertices.size(); i++){
        Vertex v = vertices[i];
        
        if (contains(subset, v)) continue;
        
        for (size_t j = i + 1; j < vertices.size(); j++){
            Vertex w = vertices[j];
            
            if (contains(subset, w)) continue;
            
            // if two vertices can't be differentiated
            // via their distances to vertices in a set
            // then it's not a separating set
            if (same_distances(v, w, subset, dist)) return false;
        }
    }
    
    return true;
}

int main(int argc, char **argv){
    glutInit(&argc, argv);
    glutInitWindowSize(width, height);
    glutSetOption(GLUT_MULTISAMPLE, 8);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);
    glutCreateWindow("TODO window title");
    
    default_font = new Font("font", "font_info");
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    printf("GPU: %s\n", glGetString(GL_RENDERER));
    printf("\n");
    
    size_t n = 13;
    
    for (size_t i = 0; i < n; i++){
        vertices.push_back(i);
        points.push_back(random_point());
    }
    
    for (size_t i = 0; i < n; i++){
        while (1){
            size_t j = randi(n);
            
            if (i == j) continue;
            
            Edge edge{i, j};
            
            if (contains(edges, edge) || contains(edges, flip(edge))) continue;
            
            edges.push_back(edge);
            break;
        }
    }
    
    for (size_t k = 0; k < 5; k++){
        while (1){
            size_t i = randi(n);
            size_t j = randi(n);
            
            if (i == j) continue;
            
            Edge edge{i, j};
            
            if (contains(edges, edge) || contains(edges, flip(edge))) continue;
            
            edges.push_back(edge);
            break;
        }
    }
    
    size_t smallest_n = 12345678;
    
    for (size_t k = 0; k < 1000; k++){
        Points new_points(points);
        
        for (size_t i = 0; i < 3; i++){
            size_t j = randi(n);
            
            new_points[j] = random_point();
        }
        
        Points intersections = get_intersections(edges, new_points);
        
        if (smallest_n > intersections.size() || (
            smallest_n == intersections.size() && 
            min_vertex_edge_dist2(vertices, edges, points) <
            min_vertex_edge_dist2(vertices, edges, new_points)
            )
        ){
            smallest_n = intersections.size();
            points = new_points;
        }
    }
    
    Matd dist = calculate_distances(vertices, edges);
    
    for (size_t i = 0; i < dist.m; i++){           
        for (size_t j = 0; j < dist.n; j++){
            printf("%.f ", dist(i, j));
        }
        printf("\n");
    }
    printf("\n");
    
    bool found = false;
    for (size_t k = 1; k <= n; k++){
        Vertices subset;
        subsets(vertices, k, subset, [&](){
            if (found) return;
            
            if (is_separating_set(vertices, edges, subset, dist)){
                found = true;
                
                printf("Found separating set of size %i\n", (int)k);
                for (Vertex v : subset){
                    printf("%i ", (int)v);
                }
                printf("\n");
            }
        });
        
        if (found) break;
    }
    
    glutMouseFunc(onMouseButton);
    glutMotionFunc(onMove);
    glutPassiveMotionFunc(onMove);
    glutKeyboardFunc(onKey);
    glutReshapeFunc(reshape);
    glutDisplayFunc(display);
    
    glutMainLoop();
    
    return 0;
}

