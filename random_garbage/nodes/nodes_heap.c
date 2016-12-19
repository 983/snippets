#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#define NEW(T, n) (T*)malloc(sizeof(T)*(n))
#define SWAP(T, a, i, j) { T temp = a[i]; a[i] = a[j]; a[j] = temp; }
#define WHERE printf("Line: %i\n", __LINE__);

double sec(){
    return clock()/(double)CLOCKS_PER_SEC;
    /*
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9*t.tv_nsec;
    */
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

int is_less(uint32_t a, uint32_t b){
    return f[a] < f[b];
}

void ON_MOVE(int node, int i){
    heapIndex[node] = i;
}

struct MinHeap {
    uint32_t *values;
    size_t n;
};

void MinHeap_init(struct MinHeap *h){
    h->n = 0;
    h->values = NEW(uint32_t, nx*ny);
}

void MinHeap_free(struct MinHeap *h){
    free(h->values);
    h->values = 0;
    h->n = 0;
}

void bubbleUp(struct MinHeap *h, size_t i){
    uint32_t *values = h->values;

    while (i != 0){
        size_t parent = (i - 1) >> 1;
        if (is_less(values[i], values[parent])){
            SWAP(uint32_t, values, i, parent);
            ON_MOVE(values[i], i);
            ON_MOVE(values[parent], parent);
            i = parent;
        }else{
            break;
        }
    }
}

void sinkDown(struct MinHeap *h, size_t i){
    uint32_t *values = h->values;
    size_t n = h->n;

    while (1){
        size_t left_child = i*2 + 1;
        size_t right_child = i*2 + 2;

        if (left_child >= n) break;

        size_t minimum = (right_child >= n || is_less(values[left_child], values[right_child])) ?
            left_child : right_child;

        if (is_less(values[minimum], values[i])){
            SWAP(uint32_t, values, i, minimum);
            ON_MOVE(values[i], i);
            ON_MOVE(values[minimum], minimum);
            i = minimum;
        }else{
            break;
        }
    }
}

void push(struct MinHeap *h, uint32_t value){
    uint32_t *values = h->values;
    values[h->n++] = value;
    ON_MOVE(values[h->n - 1], h->n - 1);
    bubbleUp(h, h->n - 1);
}

uint32_t removeAt(struct MinHeap *h, size_t i){
    uint32_t *values = h->values;
    assert(i < h->n);

    uint32_t result = values[i];

    if (h->n > 1){
        values[i] = values[h->n - 1];
        h->n--;
        ON_MOVE(values[i], i);
        sinkDown(h, i);
    }else{
        h->n--;
    }

    return result;
}

uint32_t pop(struct MinHeap *h){
    return removeAt(h, 0);
}

uint32_t state;

uint32_t rd(){
    state ^= state << 15;
    state ^= state >> 17;
    state ^= state << 5;
    return state;
}

void loop(){
    state = 0x12345678;

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

    struct MinHeap openSet[1];
    MinHeap_init(openSet);
    push(openSet, start);

	double s = sqrt(2);
	int dx[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
	int dy[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
	double dist[8] = {s, 1, s, 1, 1, s, 1, s};

    double t = sec();
	while (openSet->n > 0){
        uint32_t node = pop(openSet);
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
                removeAt(openSet, heapIndex[neighbor]);
            }

            f[neighbor] = gNew + sqrt(gx*gx + gy*gy);

            nodeOpened[neighbor] = 1;
            push(openSet, neighbor);
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

	free(openSet->values);

	printf("%f seconds, %i opened, %i closed, %i path length\n", dt, n_opened, n_closed, path_length);
}

void init(){
    blocked = NEW(uint8_t, nx*ny);
    nodeOpened = NEW(uint8_t, nx*ny);
    nodeClosed = NEW(uint8_t, nx*ny);
    f = NEW(double, nx*ny);
    g = NEW(double, nx*ny);
    prev = NEW(uint32_t, nx*ny);
    heapIndex = NEW(size_t, nx*ny);
}

#ifdef EMSCRIPTEN

#include <emscripten.h>

int main(){
    init();
    emscripten_set_main_loop(loop, 60, 1);
    return 0;
}

#else

int main(){
    init();

    for (int i = 0; i < 10; i++) loop();

    return 0;
}

#endif
