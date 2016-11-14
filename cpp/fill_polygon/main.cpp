#include <GL/glut.h>
#include <stdint.h>
#include <vector>
#include <algorithm>

int window_width = 512;
int window_height = 512;

struct Point {
    int x, y;

    Point(int x, int y): x(x), y(y){}
};

typedef std::vector<Point> Points;

Points poly;

// draw a line from (x0, y0) to (x1, y1), include endpoints
void line(int x0, int y0, int x1, int y1, Points &pts){
    int dx = +std::abs(x1 - x0);
    int dy = -std::abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while (1){
        pts.push_back(Point(x0, y0));

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2*err;
        if (e2 > dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}

// combine consecutive points with same y value
// only keep the one with smallest x value
Points combine_same_y(const Points &pts){
    Points result;

    if (pts.empty()) return result;

    result.push_back(pts.back());

    for (size_t i = 0; i < pts.size(); i++){
        Point curr = pts[i];
        Point &prev = result.back();
        if (prev.y == curr.y){
            // we only want the point with the smaller x coordinate
            prev.x = std::min(prev.x, curr.x);
        }else{
            result.push_back(curr);
        }
    }

    // handle endpoints
    if (result.front().y == result.back().y){
        result.front().x = std::min(result.front().x, result.back().x);
        result.pop_back();
    }

    return result;
}

// if pixels meet in a v shape, the lowest point of the v does not flip inout
Points discard_v_corners(const Points &pts){
    Points result;

    if (pts.size() < 2) return result;

    Point a = pts[pts.size() - 2];
    Point b = pts[pts.size() - 1];
    for (size_t i = 0; i < pts.size(); i++){
        Point c = pts[i];

        if ((a.y < b.y) == (b.y < c.y)){
            result.push_back(b);
        }

        a = b;
        b = c;
    }

    return result;
}

void draw_poly(
    std::vector<uint32_t> &image,
    int nx, int ny,
    const Points &poly,
    uint32_t fill_color,
    uint32_t outline_color
){
    if (poly.empty()) return;

    Points pts;

    // lines to line pixel positions
    Point prev = poly.back();
    for (size_t i = 0; i < poly.size(); i++){
        Point curr = poly[i];

        line(prev.x, prev.y, curr.x, curr.y, pts);

        prev = curr;
    }

    typedef std::vector<int> Ints;
    std::vector<Ints> rows(ny);
    Points tmp = discard_v_corners(combine_same_y(pts));
    for (size_t i = 0; i < tmp.size(); i++){
        Point p = tmp[i];
        if (p.y >= 0 && p.y < ny){
            rows[p.y].push_back(p.x);
        }
    }

    // fill polygon
    for (int y = 0; y < ny; y++){
        Ints &x_values = rows[y];
        std::sort(x_values.begin(), x_values.end());
        for (size_t i = 0; i + 1 < x_values.size(); i += 2){
            int x0 = x_values[i + 0];
            int x1 = x_values[i + 1];
            for (int x = x0; x <= x1; x++){
                if (x >= 0 && x < nx){
                    image[x + y*nx] = fill_color;
                }
            }
        }
    }

    // draw polygon outline
    for (size_t i = 0; i < pts.size(); i++){
        Point p = pts[i];
        if (p.x >= 0 && p.x < nx && p.y >= 0 && p.y < ny){
            image[p.x + p.y*nx] = outline_color;
        }
    }
}

void on_frame() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    window_width  = glutGet(GLUT_WINDOW_WIDTH);
    window_height = glutGet(GLUT_WINDOW_HEIGHT);

    uint32_t dark_blue = 0xff996633;
    uint32_t green     = 0xff00ff00;
    uint32_t black     = 0xff000000;

    std::vector<uint32_t> image(window_width*window_height, dark_blue);
    draw_poly(image, window_width, window_height, poly, green, black);
    glDrawPixels(window_width, window_height, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

    glutSwapBuffers();
}

void on_drag(int x, int y){
    poly.push_back(Point(x, window_height - 1 - y));
    glutPostRedisplay();
}

void on_button(int button, int action, int x, int y){
    on_drag(x, y);
    if (button == GLUT_LEFT_BUTTON){
        if (action == GLUT_UP){
            poly.clear();
        }
    }
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("");
    glutMotionFunc(on_drag);
    glutMouseFunc(on_button);

    glutDisplayFunc(on_frame);
    glutMainLoop();
    return 0;
}
