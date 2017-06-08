#include <GL/glut.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <algorithm>
#include <vector>
#include <assert.h>
#include <time.h>
#include <set>

double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9*t.tv_nsec;
}

int window_width = 1024;
int window_height = 1024;

const uint32_t nx = 256;
const uint32_t ny = 256;

std::vector<uint8_t> blocked(nx*ny, 0);
std::vector<uint8_t> nodeOpened(nx*ny, 0);
std::vector<uint8_t> nodeClosed(nx*ny, 0);
std::vector<double> f(nx*ny);
std::vector<double> g(nx*ny);
std::vector<uint32_t> prev(nx*ny);
std::vector<size_t> heapIndex(nx*ny);
std::vector<uint32_t> openSet;

void ON_MOVE(uint32_t node, int i){
    heapIndex[node] = i;
}

bool is_less(uint32_t a, uint32_t b){
    return f[a] < f[b];
}

template <typename T>
struct MinHeap {
    std::vector<T> values;

    void bubbleUp(size_t i){
        while (i != 0){
            size_t parent = (i - 1) >> 1;
            if (is_less(values[i], values[parent])){
                std::swap(values[i], values[parent]);
                ON_MOVE(values[i], i);
                ON_MOVE(values[parent], parent);
                i = parent;
            }else{
                break;
            }
        }
    }

    void sinkDown(size_t i){
        size_t n = values.size();

        while (1){
            size_t left_child = i*2 + 1;
            size_t right_child = i*2 + 2;

            if (left_child >= n) break;

            size_t minimum = (right_child >= n || is_less(values[left_child], values[right_child])) ?
                left_child : right_child;

            if (is_less(values[minimum], values[i])){
                std::swap(values[i], values[minimum]);
                ON_MOVE(values[i], i);
                ON_MOVE(values[minimum], minimum);
                i = minimum;
            }else{
                break;
            }
        }
    }

    void push(const T &value){
        values.push_back(value);
        ON_MOVE(values.back(), values.size() - 1);
        bubbleUp(values.size() - 1);
    }

    T removeAt(size_t i){
        assert(i < values.size());

        T result = values[i];

        if (values.size() > 1){
            values[i] = values.back();
            values.pop_back();
            ON_MOVE(values[i], i);
            sinkDown(i);
        }else{
            values.pop_back();
        }

        return result;
    }

    T pop(){
        return removeAt(0);
    }

    bool empty() const {
        return values.empty();
    }
};

void test_heap(int n){
    std::vector<int> a, b;
    MinHeap<int> h;
    for (int i = 0; i < n; i++){
        int x = rand() % 5;
        a.push_back(x);
        h.push(x);
    }

    while (!h.empty()){
        int x = h.pop();
        b.push_back(x);
    }

    std::sort(a.begin(), a.end());

    for (int i = 0; i < n; i++){
        assert(a[i] == b[i]);
    }
}

struct Node {
    uint32_t value;
};

bool operator < (Node a, Node b){
    return f[a.value] < f[b.value];
}

void on_frame(){
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    uint32_t state = 0x12345678;

    auto rd = [&](){
        state ^= state << 15;
        state ^= state >> 17;
        state ^= state << 5;
        return state;
    };

    for (size_t i = 0; i < nx*ny; i++){
        nodeOpened[i] = 0;
        nodeClosed[i] = 0;
        f[i] = 0;
        g[i] = 0;
        prev[i] = 0;
        heapIndex[i] = 0;
    }

    for (int i = 0; i < 20*1000; i++){
        int x = rd() % nx;
        int y = rd() % ny;
        blocked[x + y*nx] = 1;
    }

    uint32_t startX = 0;
    uint32_t startY = 3;
    uint32_t goalX = 201;
    uint32_t goalY = 150;

    uint32_t start = startX + startY*nx;
    uint32_t goal = goalX + goalY*nx;
    nodeOpened[start] = 1;

    /*
    MinHeap<uint32_t> openSet;
    openSet.push(start);
    */
    std::set<Node> openSet;
    openSet.insert(Node{start});

	double s = sqrt(2);
	int dx[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
	int dy[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
	double dist[8] = {s, 1, s, 1, 1, s, 1, s};

    double t = sec();

	while (!openSet.empty()){
	    auto it = openSet.begin();
        uint32_t node = it->value;
        openSet.erase(it);
        //uint32_t node = openSet.pop();
        int nodeX = node % nx;
        int nodeY = node / nx;
        nodeClosed[node] = 1;

        if (node == goal){
            break;
        }

        for (int k = 0; k < 8; k++){
            int x = nodeX + dx[k];
            int y = nodeY + dy[k];
            uint32_t neighbor = x + y*nx;

            if (x < 0 || x >= (int)nx || y < 0|| y >= (int)ny) continue;
            if (blocked[neighbor]) continue;

            if (x != nodeX && y != nodeY){
                if (blocked[nodeX + y*nx] || blocked[x + nodeY*nx]){
                    continue;
                }
            }

            if (nodeClosed[neighbor]) continue;

            double gNew = g[node] + dist[k];

            if (nodeOpened[neighbor] && gNew >= g[neighbor]) continue;

            prev[neighbor] = node;
            g[neighbor] = gNew;
            int gx = x - goalX;
            int gy = y - goalY;

            if (nodeOpened[neighbor]){
                openSet.erase(Node{neighbor});
                //openSet.removeAt(heapIndex[neighbor]);
            }

            f[neighbor] = gNew + sqrt(gx*gx + gy*gy);

            nodeOpened[neighbor] = 1;
            openSet.insert(Node{neighbor});
            //openSet.push(neighbor);
        }
	}

	double dt = sec() - t;
	printf("%f seconds\n", dt);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, window_width, 0, window_height, -1, +1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	uint32_t node = goal;
	glBegin(GL_LINE_STRIP);
    glColor3f(0, 1, 0);
	float gridSize = 4;
	glVertex2f((goalX + 0.5f)*gridSize, (goalY + 0.5f)*gridSize);
	while (nodeOpened[node]){
        uint32_t x = node % nx;
        uint32_t y = node / nx;
        glVertex2f((x + 0.5f)*gridSize, (y + 0.5f)*gridSize);

        if (node == start) break;

        node = prev[node];
	}
	glEnd();

	auto rect = [](float x0, float y0, float w, float h){
	    float x1 = x0 + w;
	    float y1 = y0 + h;
        glVertex2f(x0, y0);
        glVertex2f(x1, y0);
        glVertex2f(x1, y1);
        glVertex2f(x0, y1);
	};
	glBegin(GL_QUADS);
	for (uint32_t y = 0; y < ny; y++) for (uint32_t x = 0; x < nx; x++){
        uint32_t node = x + y*nx;
        if (blocked[node]){
            glColor3f(0, 0, 0);
            rect(x*gridSize, y*gridSize, gridSize, gridSize);
        }
	}
	glEnd();

    glutSwapBuffers();
}

void work(int frame){
    glutPostRedisplay();
    glutTimerFunc(20, work, frame + 1);
}

int main(int argc, char **argv){
/*
    for (int i = 0; i < 100; i++){
        test_heap(rand() % 10);
    }
*/
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(window_width, window_height);
    glutCreateWindow("");

    glutDisplayFunc(on_frame);
    work(0);
    glutMainLoop();
    return 0;
}

