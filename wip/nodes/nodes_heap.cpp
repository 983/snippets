#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <math.h>

template <typename T>
void my_swap(T &a, T &b){
    T temp(a);
    a = b;
    b = temp;
}

double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9*t.tv_nsec;
}

const uint32_t nx = 256;
const uint32_t ny = 256;

uint8_t *blocked;
uint8_t *nodeOpened;
uint8_t *nodeClosed;
double *f;
double *g;
uint32_t *prev;
size_t *heapIndex;

struct Node {
    uint32_t value;
};

bool operator < (Node a, Node b){
    return f[a.value] < f[b.value];
}

void ON_MOVE(Node node, int i){
    heapIndex[node.value] = i;
}

template <typename T>
bool is_less(T a, T b){
    return a < b;
}

template <typename T>
struct MinHeap {
    T *values;
    size_t _n;

    MinHeap(): _n(0){
        values = (T*)malloc(sizeof(T)*nx*ny);
    }

    ~MinHeap(){
        free(values);
    }

    void bubbleUp(size_t i){
        while (i != 0){
            size_t parent = (i - 1) >> 1;
            if (is_less(values[i], values[parent])){
                my_swap(values[i], values[parent]);
                ON_MOVE(values[i], i);
                ON_MOVE(values[parent], parent);
                i = parent;
            }else{
                break;
            }
        }
    }

    size_t size() const {
        return _n;
    }

    void sinkDown(size_t i){
        size_t n = size();

        while (1){
            size_t left_child = i*2 + 1;
            size_t right_child = i*2 + 2;

            if (left_child >= n) break;

            size_t minimum = (right_child >= n || is_less(values[left_child], values[right_child])) ?
                left_child : right_child;

            if (is_less(values[minimum], values[i])){
                my_swap(values[i], values[minimum]);
                ON_MOVE(values[i], i);
                ON_MOVE(values[minimum], minimum);
                i = minimum;
            }else{
                break;
            }
        }
    }

    void push(const T &value){
        values[_n++] = value;
        ON_MOVE(values[size() - 1], size() - 1);
        bubbleUp(size() - 1);
    }

    T removeAt(size_t i){
        assert(i < size());

        T result = values[i];

        if (size() > 1){
            values[i] = values[size() - 1];
            _n--;
            ON_MOVE(values[i], i);
            sinkDown(i);
        }else{
            _n--;
        }

        return result;
    }

    T pop(){
        return removeAt(0);
    }

    bool empty() const {
        return size() == 0;
    }
};

void loop(){
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

    MinHeap<Node> openSet;
    openSet.push(Node{start});

	double s = sqrt(2);
	int dx[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
	int dy[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
	double dist[8] = {s, 1, s, 1, 1, s, 1, s};

    double t = sec();

	while (!openSet.empty()){
        uint32_t node = openSet.pop().value;
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
                openSet.removeAt(heapIndex[neighbor]);
            }

            f[neighbor] = gNew + sqrt(gx*gx + gy*gy);

            nodeOpened[neighbor] = 1;
            openSet.push(Node{neighbor});
        }
	}

	double dt = sec() - t;

	int n_opened = 0;
	int n_closed = 0;
	int path_length = 0;

	for (uint32_t y = 0; y < ny; y++) for (uint32_t x = 0; x < nx; x++){
        uint32_t node = x + y*nx;
        if (nodeClosed[node]) n_closed++;
        if (nodeOpened[node]) n_opened++;
	}

	uint32_t node = goal;
	while (nodeOpened[node]){
        path_length++;

        if (node == start) break;

        node = prev[node];
	}

	printf("%f seconds, %i opened, %i closed, %i path length\n", dt, n_opened, n_closed, path_length);
}

#include <emscripten.h>

#define NEW(T, n) (T*)malloc(sizeof(T)*(n))

int main(){

    blocked = NEW(uint8_t, nx*ny);
    nodeOpened = NEW(uint8_t, nx*ny);
    nodeClosed = NEW(uint8_t, nx*ny);
    f = NEW(double, nx*ny);
    g = NEW(double, nx*ny);
    prev = NEW(uint32_t, nx*ny);
    heapIndex = NEW(size_t, nx*ny);

    emscripten_set_main_loop(loop, 60, 1);

    return 0;
}

