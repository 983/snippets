// Lists are approximately 10 times slower than vectors.
// SIMD does not help here since the vector method is already memory-bound.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <list>
#include <vector>
#include <immintrin.h>

float randf(){
    return rand()/(float)RAND_MAX;
}

double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9*t.tv_nsec;
}

struct Position {
    float x, y;
};

struct Velocity {
    float x, y;
};
    
size_t n = 4096;

template <typename CONTAINER1, typename CONTAINER2>
void test(CONTAINER1 positions, CONTAINER2 velocities){
    
    for (size_t i = 0; i < n; i++){
        Position p;
        p.x = randf();
        p.y = randf();
        
        positions.push_back(p);
        
        Velocity v;
        v.x = randf();
        v.y = randf();
        
        velocities.push_back(v);
    }
    
    float delta_time = 1.0f/60.0f;
    
    for (int k = 0; k < 10; k++){
        double t0 = sec();
        
        auto p = positions.begin();
        auto v = velocities.begin();
        
        for (; p != positions.end(); ++p, v++){
            p->x += delta_time*v->x;
            p->y += delta_time*v->y;
        }
        
        double t1 = sec();
        
        size_t n_bytes = n*(sizeof(Position)*2 + sizeof(Velocity));
        
        printf("%f seconds, %f gbyte/sec\n", t1 - t0, n_bytes*1e-9/(t1 - t0));
    }
    
    // prevent removal due to optimization
    float checksum = 0.0f;
    for (const Position &p : positions){
        checksum += p.x + p.y;
    }
    printf("checksum: %f\n", checksum);
}

void* aligned_malloc(size_t n_bytes, size_t alignment){
    char *ptr = (char*)malloc(n_bytes + alignment);
    
    while (size_t(ptr) % alignment != 0) ptr++;
    
    return ptr;
}

void test_simd(){
    Position *positions = (Position*)aligned_malloc(n*sizeof(*positions), 64);
    Velocity *velocities = (Velocity*)aligned_malloc(n*sizeof(*velocities), 64);
    
    for (size_t i = 0; i < n; i++){
        Position p;
        p.x = randf();
        p.y = randf();
        
        positions[i] = p;
        
        Velocity v;
        v.x = randf();
        v.y = randf();
        
        velocities[i] = v;
    }
    
    float delta_time = 1.0f/60.0f;
    
    for (int k = 0; k < 10; k++){
        double t0 = sec();
        
        // add two at once
        const size_t m = n/2;
        for (size_t i = 0; i < m; i++){
            __m128 *p = (__m128*)positions;
            __m128 *v = (__m128*)velocities;
            
            p[i] += delta_time*v[i];
        }
        
        double t1 = sec();
        
        size_t n_bytes = n*(sizeof(Position)*2 + sizeof(Velocity));
        
        printf("%f seconds, %f gbyte/sec\n", t1 - t0, n_bytes*1e-9/(t1 - t0));
    }
    
    // prevent removal due to optimization
    float checksum = 0.0f;
    for (size_t i = 0; i < n; i++){
        Position p = positions[i];
        
        checksum += p.x + p.y;
    }
    printf("checksum: %f\n", checksum);
}

int main(){
    srand(0);
    test(std::list<Position>(), std::list<Velocity>());
    srand(0);
    test(std::vector<Position>(), std::vector<Velocity>());
    srand(0);
    test_simd();
    
    return 0;
}
