#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <vector>
#include <assert.h>

double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9*t.tv_nsec;
}

uint32_t xorshift32(){
    static uint32_t state = 0x12345678;
    
    uint32_t x = state;
    
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    
    state = x;
    
    return x;
}

void sort2(uint32_t &a, uint32_t &b){
    if (a > b){
        std::swap(a, b);
    }
}

#define N_VALUES 16

struct Pair {
    uint32_t a, b;
};

bool are_same(uint32_t *a, uint32_t *b){
    for (int i = 0; i < N_VALUES; i++){
        if (a[i] != b[i]) return false;
    }
    
    return true;
}

struct SortingNetwork {
    std::vector<Pair> indices;
    
    SortingNetwork(size_t n): indices(n){
        randomize();
    }
    
    void randomize(){
        for (size_t k = 0; k < size(); k++){
            uint32_t a = xorshift32() % N_VALUES;
            uint32_t b = xorshift32() % N_VALUES;
            
            sort2(a, b);
            
            indices[k] = Pair{a, b};
        }
    }
    
    bool is_sorting_network(int n_tests = 100*1000) const {
        uint32_t values[N_VALUES];
        uint32_t sorted_values[N_VALUES];
        
        for (int k = 0; k < n_tests; k++){
            for (int i = 0; i < N_VALUES; i++){
                uint32_t x = xorshift32();
                
                values[i] = x;
                sorted_values[i] = x;
            }
            
            std::sort(sorted_values, sorted_values + N_VALUES);
            
            sort(values);
            
            if (!are_same(values, sorted_values)) return false;
        }
        
        return true;
    }
    
    double error(int n_tests) const {
        double err = 0.0;
        
        uint32_t values[N_VALUES];
        uint32_t sorted_values[N_VALUES];
        
        for (int k = 0; k < n_tests; k++){
            for (int i = 0; i < N_VALUES; i++){
                uint32_t x = xorshift32();
                
                values[i] = x;
                sorted_values[i] = x;
            }
            
            std::sort(sorted_values, sorted_values + N_VALUES);
            
            sort(values);
            
            for (int i = 0; i < N_VALUES; i++){
                err += values[i] != sorted_values[i];
            }
        }
        
        return err/n_tests;
    }
    
    void sort(uint32_t *values) const {
        for (size_t k = 0; k < size(); k++){
            Pair p = indices[k];
            
            sort2(values[p.a], values[p.b]);
        }
    }
    
    void scramble(){
        if (xorshift32() & 1){            
            uint32_t k = xorshift32() % size();
            
            uint32_t a = xorshift32() % N_VALUES;
            uint32_t b = xorshift32() % N_VALUES;
            
            sort2(a, b);
            
            indices[k] = Pair{a, b};
        }else{
            uint32_t i = xorshift32() % size();
            uint32_t j = xorshift32() % size();
            
            std::swap(indices[i], indices[j]);
        }
    }
    
    size_t size() const {
        return indices.size();
    }
};

void find_sorting_network_by_shrinking(){
    int n_tests = 10000;
    
    while (1){
        SortingNetwork larger(300);
        
        while (!larger.is_sorting_network()){
            larger = SortingNetwork(larger.size());
        }
        
        while (1){
            
            SortingNetwork temp(larger);
            
            uint32_t n_scrambles = xorshift32() % 10 + 1;
            
            for (uint32_t k = 0; k < n_scrambles; k++){
                temp.scramble();
            }
            
            if (temp.error(n_tests) <= larger.error(n_tests)){// && temp.is_sorting_network()){
                larger = temp;
                printf("improved error: %f\n", larger.error(n_tests));
            }
            
            for (size_t k = 0; k < larger.size(); k++){
                temp = larger;
        
                temp.indices.erase(temp.indices.begin() + k);
                
                if (temp.is_sorting_network()){
                    larger = temp;
                    printf("Found sorting network of size %lu\n", larger.size());
                    for (const auto &p : larger.indices){
                        printf("{%u, %u}, ", p.a, p.b);
                    }
                    printf("\n");
                    break;
                }
            }
        }
    }
}

int main(){
    int n_tests = 100;
    
    SortingNetwork best(200);
    
    best.indices = {
        {14, 15}, {8, 10}, {6, 8}, {3, 9}, {3, 6}, {7, 12}, {0, 15}, {0, 9}, {4, 6}, {6, 11}, {3, 13}, {4, 12}, {1, 10}, {1, 9}, {1, 7}, {3, 5}, {2, 10}, {6, 7}, {0, 5}, {8, 12}, {2, 3}, {12, 15}, {2, 4}, {13, 15}, {3, 5}, {8, 13}, {9, 10}, {1, 14}, {1, 6}, {3, 14}, {9, 10}, {0, 13}, {11, 12}, {5, 7}, {13, 14}, {15, 15}, {2, 7}, {12, 14}, {0, 3}, {0, 3}, {4, 5}, {7, 12}, {6, 13}, {6, 8}, {0, 1}, {15, 15}, {0, 2}, {8, 11}, {1, 4}, {5, 9}, {12, 15}, {10, 14}, {5, 8}, {14, 15}, {8, 13}, {2, 5}, {13, 14}, {3, 6}, {4, 11}, {4, 5}, {9, 12}, {10, 12}, {6, 11}, {1, 2}, {9, 13}, {10, 11}, {6, 8}, {4, 4}, {12, 12}, {1, 3}, {6, 9}, {7, 13}, {11, 13}, {4, 6}, {7, 10}, {12, 14}, {9, 10}, {5, 8}, {5, 6}, {7, 9}, {7, 8}, {2, 3}, {9, 11}, {8, 9}, {12, 13}, {4, 4}, {2, 3}, {3, 4}, {1, 11}, {0, 4}, {4, 5}, {6, 7}, {9, 10}, {5, 6}, {10, 11},
    };
    
    best.indices = {
        {14, 15}, {8, 10}, {1, 14}, {3, 9}, {5, 9}, {10, 14}, {12, 15}, {0, 9}, {4, 6}, {4, 5}, {4, 8}, {6, 10}, {1, 7}, {0, 5}, {2, 10}, {6, 11}, {2, 10}, {6, 7}, {1, 9}, {8, 12}, {2, 5}, {13, 15}, {2, 4}, {2, 11}, {3, 5}, {7, 15}, {9, 10}, {2, 8}, {1, 6}, {3, 14}, {2, 13}, {0, 13}, {11, 12}, {5, 7}, {13, 14}, {6, 13}, {13, 15}, {12, 14}, {0, 3}, {9, 12}, {12, 14}, {7, 12}, {14, 15}, {6, 8}, {0, 1}, {4, 8}, {0, 2}, {8, 11}, {1, 4}, {0, 9}, {10, 12}, {10, 14}, {5, 8}, {10, 11}, {8, 13}, {2, 5}, {13, 14}, {3, 6}, {3, 5}, {4, 5}, {8, 10}, {4, 6}, {7, 13}, {1, 2}, {9, 13}, {6, 13}, {11, 13}, {0, 4}, {4, 4}, {1, 3}, {6, 9}, {7, 13}, {6, 8}, {12, 15}, {12, 14}, {7, 10}, {9, 10}, {5, 8}, {5, 6}, {7, 9}, {7, 8}, {4, 6}, {9, 11}, {8, 9}, {12, 13}, {10, 14}, {2, 3}, {3, 4}, {1, 11}, {10, 11}, {4, 5}, {6, 7}, {9, 10},
    };
    
    best.indices = {
        {1, 12}, {8, 10}, {3, 14}, {3, 9}, {4, 13}, {10, 14}, {12, 15}, {0, 9}, {2, 10}, {4, 5}, {6, 7}, {6, 10}, {1, 7}, {4, 11}, {7, 15}, {6, 11}, {6, 9}, {6, 7}, {1, 9}, {13, 15}, {2, 5}, {8, 12}, {2, 10}, {5, 10}, {3, 7}, {2, 4}, {9, 10}, {2, 8}, {1, 6}, {0, 5}, {1, 9}, {0, 13}, {11, 12}, {5, 7}, {13, 14}, {6, 13}, {5, 15}, {12, 14}, {0, 3}, {9, 12}, {0, 2}, {7, 12}, {14, 15}, {6, 8}, {0, 1}, {4, 8}, {12, 14}, {8, 11}, {1, 4}, {4, 14}, {10, 12}, {10, 14}, {5, 8}, {10, 11}, {8, 13}, {2, 5}, {5, 14}, {3, 6}, {3, 5}, {4, 6}, {8, 10}, {9, 13}, {7, 13}, {4, 5}, {0, 7}, {6, 13}, {6, 9}, {3, 12}, {1, 2}, {1, 3}, {11, 13}, {7, 9}, {6, 8}, {12, 15}, {12, 14}, {7, 10}, {9, 10}, {5, 8}, {5, 6}, {2, 3}, {7, 8}, {3, 4}, {8, 9}, {9, 11}, {12, 13}, {6, 7}, {2, 3}, {8, 12}, {3, 9}, {10, 11}, {4, 5},
    };

/*    
    Found sorting network of size 91
{1, 12}, {8, 10}, {3, 14}, {3, 9}, {4, 13}, {10, 14}, {12, 15}, {0, 9}, {2, 10}, {4, 5}, {6, 7}, {6, 10}, {1, 7}, {4, 11}, {7, 15}, {6, 11}, {6, 9}, {6, 7}, {1, 9}, {13, 15}, {2, 5}, {8, 12}, {2, 10}, {5, 10}, {3, 7}, {2, 4}, {9, 10}, {2, 8}, {1, 6}, {0, 5}, {1, 9}, {0, 13}, {11, 12}, {5, 7}, {13, 14}, {6, 13}, {5, 15}, {12, 14}, {0, 3}, {9, 12}, {0, 2}, {7, 12}, {14, 15}, {6, 8}, {0, 1}, {4, 8}, {12, 14}, {8, 11}, {1, 4}, {4, 14}, {10, 12}, {10, 14}, {5, 8}, {10, 11}, {8, 13}, {2, 5}, {5, 14}, {3, 6}, {3, 5}, {4, 6}, {8, 10}, {9, 13}, {7, 13}, {4, 5}, {0, 7}, {6, 13}, {6, 9}, {3, 12}, {1, 2}, {1, 3}, {11, 13}, {7, 9}, {6, 8}, {12, 15}, {12, 14}, {7, 10}, {9, 10}, {5, 8}, {5, 6}, {2, 3}, {7, 8}, {3, 4}, {8, 9}, {9, 11}, {12, 13}, {6, 7}, {2, 3}, {8, 12}, {3, 9}, {10, 11}, {4, 5}, 
Found sorting network of size 90
{7, 11}, {2, 9}, {5, 12}, {6, 9}, {4, 15}, {1, 2}, {0, 3}, {7, 9}, {11, 13}, {6, 11}, {10, 15}, {3, 14}, {3, 4}, {14, 15}, {1, 7}, {3, 6}, {0, 5}, {5, 10}, {4, 9}, {12, 14}, {13, 14}, {8, 10}, {2, 13}, {8, 12}, {1, 8}, {4, 11}, {3, 4}, {6, 8}, {9, 10}, {1, 3}, {4, 5}, {11, 12}, {9, 15}, {14, 15}, {2, 2}, {2, 4}, {0, 7}, {12, 14}, {4, 12}, {3, 6}, {0, 4}, {3, 9}, {1, 6}, {4, 12}, {4, 9}, {11, 12}, {8, 11}, {5, 7}, {7, 9}, {4, 15}, {10, 12}, {4, 6}, {7, 13}, {14, 15}, {7, 11}, {13, 14}, {4, 5}, {5, 8}, {10, 13}, {7, 13}, {7, 8}, {9, 11}, {11, 13}, {9, 13}, {0, 2}, {5, 7}, {12, 14}, {14, 15}, {7, 12}, {2, 7}, {8, 10}, {4, 5}, {0, 7}, {6, 8}, {9, 10}, {0, 1}, {8, 15}, {6, 7}, {12, 14}, {10, 12}, {2, 3}, {1, 2}, {12, 13}, {10, 11}, {2, 4}, {8, 9}, {4, 14}, {3, 5}, {3, 4}, {5, 6}, 
Found sorting network of size 89
{7, 11}, {2, 9}, {11, 13}, {6, 9}, {8, 14}, {1, 2}, {0, 3}, {7, 9}, {5, 12}, {6, 11}, {10, 15}, {3, 14}, {1, 7}, {14, 15}, {3, 4}, {3, 6}, {0, 5}, {8, 15}, {5, 8}, {12, 14}, {13, 14}, {8, 10}, {2, 13}, {4, 12}, {4, 8}, {5, 6}, {3, 4}, {7, 12}, {9, 10}, {1, 3}, {6, 8}, {3, 11}, {14, 15}, {4, 15}, {13, 14}, {11, 12}, {0, 7}, {2, 4}, {8, 11}, {6, 10}, {0, 4}, {12, 14}, {3, 6}, {2, 5}, {4, 9}, {7, 11}, {5, 7}, {0, 13}, {7, 9}, {11, 11}, {10, 12}, {4, 6}, {14, 15}, {4, 5}, {7, 8}, {7, 13}, {5, 13}, {10, 11}, {10, 13}, {5, 8}, {5, 7}, {9, 11}, {11, 13}, {9, 11}, {0, 2}, {0, 11}, {12, 14}, {5, 10}, {14, 15}, {11, 12}, {8, 10}, {2, 7}, {6, 9}, {6, 8}, {9, 10}, {6, 7}, {4, 5}, {0, 1}, {12, 14}, {4, 15}, {2, 3}, {1, 2}, {2, 4}, {10, 11}, {12, 13}, {8, 9}, {5, 6}, {3, 5}, {3, 4}, 
Found sorting network of size 88
{0, 10}, {3, 14}, {1, 1}, {3, 10}, {1, 12}, {2, 15}, {13, 14}, {2, 12}, {7, 11}, {2, 4}, {8, 15}, {0, 5}, {5, 9}, {14, 15}, {12, 15}, {0, 3}, {3, 14}, {5, 8}, {7, 14}, {10, 11}, {3, 13}, {2, 13}, {1, 3}, {0, 5}, {5, 6}, {12, 14}, {3, 14}, {4, 8}, {5, 12}, {9, 11}, {1, 2}, {8, 13}, {9, 10}, {2, 9}, {6, 15}, {4, 10}, {6, 8}, {4, 12}, {8, 10}, {11, 12}, {12, 14}, {5, 7}, {13, 15}, {3, 6}, {1, 5}, {6, 9}, {3, 6}, {5, 7}, {8, 11}, {2, 5}, {3, 7}, {6, 8}, {9, 13}, {7, 9}, {10, 12}, {3, 4}, {4, 6}, {14, 15}, {4, 7}, {10, 13}, {4, 5}, {0, 5}, {10, 11}, {3, 4}, {2, 4}, {9, 11}, {9, 10}, {7, 8}, {8, 10}, {5, 7}, {12, 14}, {6, 7}, {12, 13}, {7, 8}, {0, 4}, {0, 1}, {2, 3}, {1, 2}, {12, 13}, {11, 12}, {13, 14}, {10, 11}, {2, 14}, {5, 6}, {4, 11}, {2, 3}, {7, 9}, {8, 9}, 
Found sorting network of size 87
{1, 9}, {0, 9}, {0, 6}, {1, 15}, {6, 15}, {9, 11}, {3, 10}, {1, 3}, {9, 14}, {6, 7}, {11, 14}, {2, 9}, {4, 8}, {5, 14}, {13, 15}, {4, 13}, {2, 4}, {5, 12}, {0, 15}, {12, 15}, {8, 11}, {6, 8}, {3, 4}, {0, 5}, {8, 9}, {9, 11}, {14, 15}, {10, 13}, {9, 10}, {7, 11}, {11, 13}, {9, 12}, {12, 15}, {11, 12}, {5, 7}, {13, 15}, {4, 8}, {8, 10}, {13, 14}, {0, 2}, {7, 10}, {2, 4}, {12, 14}, {1, 14}, {3, 11}, {0, 4}, {8, 12}, {6, 9}, {4, 9}, {7, 14}, {3, 12}, {1, 2}, {7, 13}, {10, 12}, {3, 5}, {2, 5}, {7, 8}, {8, 9}, {5, 10}, {10, 13}, {12, 14}, {5, 8}, {1, 7}, {10, 11}, {3, 4}, {1, 3}, {8, 10}, {9, 11}, {5, 7}, {9, 10}, {2, 4}, {0, 6}, {0, 1}, {4, 8}, {7, 8}, {4, 5}, {2, 6}, {5, 6}, {4, 13}, {4, 5}, {2, 3}, {1, 2}, {6, 7}, {11, 13}, {8, 9}, {3, 4}, {12, 13}, 
Found sorting network of size 86
{2, 4}, {14, 15}, {3, 10}, {6, 9}, {4, 5}, {7, 8}, {6, 11}, {10, 13}, {3, 14}, {12, 14}, {7, 12}, {1, 13}, {5, 10}, {1, 5}, {9, 14}, {2, 5}, {0, 8}, {0, 15}, {13, 15}, {5, 12}, {9, 11}, {11, 14}, {0, 4}, {8, 13}, {4, 11}, {6, 7}, {12, 13}, {8, 9}, {1, 3}, {10, 13}, {11, 13}, {11, 12}, {12, 15}, {9, 10}, {3, 4}, {13, 15}, {0, 2}, {0, 6}, {3, 7}, {5, 7}, {1, 6}, {2, 4}, {7, 10}, {2, 8}, {12, 14}, {5, 13}, {13, 14}, {2, 5}, {7, 8}, {14, 15}, {2, 8}, {10, 13}, {0, 1}, {13, 14}, {8, 9}, {4, 9}, {4, 5}, {1, 3}, {10, 12}, {3, 6}, {4, 6}, {3, 10}, {2, 3}, {10, 11}, {5, 10}, {5, 7}, {8, 10}, {9, 11}, {4, 9}, {9, 10}, {4, 8}, {11, 12}, {7, 8}, {6, 7}, {7, 8}, {4, 5}, {3, 4}, {7, 9}, {5, 6}, {4, 5}, {1, 2}, {2, 3}, {8, 9}, {10, 11}, {6, 7}, {3, 4}, 
Found sorting network of size 85
{2, 4}, {14, 15}, {3, 10}, {6, 9}, {4, 5}, {7, 8}, {6, 11}, {10, 13}, {3, 14}, {12, 14}, {7, 12}, {1, 13}, {5, 10}, {1, 5}, {9, 14}, {2, 5}, {0, 8}, {0, 15}, {13, 15}, {5, 12}, {9, 11}, {11, 14}, {0, 4}, {8, 13}, {4, 11}, {6, 7}, {12, 13}, {8, 9}, {1, 3}, {10, 13}, {11, 13}, {11, 12}, {12, 15}, {9, 10}, {3, 4}, {13, 15}, {0, 2}, {0, 6}, {3, 7}, {5, 7}, {1, 6}, {2, 4}, {7, 10}, {2, 8}, {12, 14}, {5, 13}, {13, 14}, {2, 5}, {7, 8}, {14, 15}, {2, 8}, {10, 13}, {0, 1}, {13, 14}, {8, 9}, {4, 9}, {4, 5}, {1, 3}, {10, 12}, {3, 6}, {4, 6}, {3, 10}, {2, 3}, {10, 11}, {5, 10}, {5, 7}, {8, 10}, {9, 11}, {4, 9}, {9, 10}, {4, 8}, {11, 12}, {7, 8}, {6, 7}, {7, 8}, {4, 5}, {3, 4}, {7, 9}, {5, 6}, {4, 5}, {1, 2}, {2, 3}, {8, 9}, {10, 11}, {6, 7}, 
Found sorting network of size 84
{8, 15}, {0, 14}, {5, 13}, {1, 10}, {1, 8}, {13, 14}, {11, 15}, {3, 6}, {4, 9}, {5, 6}, {7, 9}, {4, 7}, {0, 2}, {2, 10}, {2, 4}, {6, 12}, {10, 12}, {3, 7}, {0, 7}, {0, 5}, {9, 15}, {1, 5}, {3, 11}, {2, 13}, {2, 8}, {7, 11}, {4, 7}, {9, 14}, {11, 14}, {4, 9}, {6, 11}, {0, 9}, {1, 10}, {5, 10}, {3, 13}, {0, 3}, {9, 11}, {1, 6}, {5, 6}, {7, 11}, {0, 2}, {4, 5}, {7, 8}, {8, 13}, {11, 15}, {2, 4}, {8, 11}, {0, 1}, {12, 14}, {10, 11}, {6, 7}, {12, 13}, {1, 2}, {8, 9}, {14, 15}, {5, 10}, {11, 14}, {2, 3}, {6, 9}, {1, 2}, {5, 9}, {11, 12}, {4, 6}, {13, 14}, {7, 8}, {5, 7}, {8, 10}, {7, 8}, {3, 6}, {3, 4}, {6, 7}, {9, 11}, {4, 5}, {4, 14}, {10, 11}, {3, 4}, {12, 13}, {4, 5}, {8, 9}, {2, 3}, {7, 8}, {9, 10}, {5, 6}, {11, 12}, 
Found sorting network of size 83
{0, 12}, {9, 13}, {11, 13}, {0, 4}, {6, 12}, {11, 15}, {7, 10}, {1, 10}, {3, 6}, {8, 15}, {9, 14}, {6, 14}, {6, 8}, {9, 15}, {10, 15}, {4, 15}, {2, 10}, {4, 14}, {5, 6}, {7, 9}, {0, 2}, {1, 5}, {4, 5}, {0, 1}, {10, 12}, {11, 15}, {0, 11}, {4, 11}, {3, 7}, {5, 10}, {6, 7}, {6, 8}, {4, 9}, {7, 11}, {13, 14}, {5, 8}, {0, 3}, {9, 13}, {8, 13}, {2, 4}, {11, 14}, {1, 6}, {12, 14}, {7, 12}, {4, 6}, {10, 11}, {5, 9}, {4, 7}, {14, 15}, {9, 10}, {7, 8}, {1, 2}, {8, 9}, {2, 3}, {0, 15}, {11, 14}, {12, 13}, {6, 7}, {1, 11}, {1, 2}, {7, 13}, {2, 6}, {7, 8}, {4, 5}, {3, 7}, {4, 15}, {9, 11}, {5, 6}, {8, 10}, {11, 12}, {3, 4}, {9, 11}, {2, 3}, {4, 5}, {10, 11}, {13, 14}, {6, 7}, {12, 13}, {5, 6}, {8, 9}, {7, 8}, {9, 10}, {11, 12}, 
Found sorting network of size 82
{0, 3}, {13, 15}, {4, 8}, {5, 12}, {2, 9}, {4, 11}, {11, 13}, {1, 8}, {3, 10}, {10, 14}, {1, 11}, {3, 11}, {0, 10}, {0, 2}, {6, 7}, {8, 15}, {4, 6}, {2, 10}, {0, 1}, {7, 13}, {8, 9}, {5, 11}, {8, 12}, {13, 14}, {11, 13}, {13, 13}, {3, 4}, {1, 8}, {6, 10}, {6, 7}, {2, 5}, {10, 15}, {5, 7}, {9, 12}, {12, 14}, {9, 13}, {1, 6}, {5, 6}, {2, 4}, {7, 12}, {7, 11}, {9, 10}, {12, 13}, {4, 8}, {10, 11}, {3, 9}, {7, 9}, {4, 5}, {1, 2}, {5, 9}, {3, 11}, {11, 12}, {8, 9}, {11, 15}, {5, 10}, {6, 11}, {5, 15}, {14, 15}, {2, 7}, {6, 7}, {0, 3}, {13, 14}, {2, 3}, {5, 6}, {0, 1}, {7, 8}, {9, 12}, {10, 11}, {8, 9}, {8, 10}, {0, 13}, {12, 13}, {3, 4}, {4, 5}, {6, 7}, {7, 9}, {7, 8}, {9, 11}, {5, 6}, {2, 3}, {9, 10}, {1, 2}, 
Found sorting network of size 81
{2, 6}, {5, 14}, {3, 7}, {2, 10}, {0, 2}, {0, 3}, {3, 5}, {9, 11}, {2, 11}, {5, 7}, {1, 13}, {4, 12}, {9, 15}, {1, 8}, {0, 9}, {11, 12}, {10, 15}, {7, 13}, {1, 9}, {1, 3}, {5, 6}, {10, 14}, {6, 15}, {6, 9}, {1, 4}, {4, 5}, {8, 14}, {8, 11}, {0, 3}, {9, 13}, {2, 10}, {3, 11}, {12, 14}, {6, 8}, {2, 7}, {12, 13}, {5, 10}, {9, 10}, {7, 8}, {3, 5}, {8, 11}, {0, 1}, {10, 15}, {1, 6}, {3, 7}, {1, 9}, {14, 15}, {2, 4}, {2, 3}, {4, 6}, {1, 2}, {4, 7}, {10, 11}, {8, 9}, {5, 12}, {9, 10}, {12, 14}, {2, 4}, {5, 8}, {5, 7}, {6, 7}, {11, 13}, {7, 8}, {10, 12}, {9, 10}, {8, 9}, {3, 5}, {7, 8}, {11, 14}, {5, 6}, {13, 15}, {13, 14}, {3, 4}, {11, 12}, {6, 7}, {9, 10}, {1, 10}, {1, 14}, {10, 11}, {4, 5}, {2, 3},
    */
    
    best.indices = {
        {2, 6}, {5, 14}, {3, 7}, {2, 10}, {0, 2}, {0, 3}, {3, 5}, {9, 11}, {2, 11}, {5, 7}, {1, 13}, {4, 12}, {9, 15}, {1, 8}, {0, 9}, {11, 12}, {10, 15}, {7, 13}, {1, 9}, {1, 3}, {5, 6}, {10, 14}, {6, 15}, {6, 9}, {1, 4}, {4, 5}, {8, 14}, {8, 11}, {0, 3}, {9, 13}, {2, 10}, {3, 11}, {12, 14}, {6, 8}, {2, 7}, {12, 13}, {5, 10}, {9, 10}, {7, 8}, {3, 5}, {8, 11}, {0, 1}, {10, 15}, {1, 6}, {3, 7}, {1, 9}, {14, 15}, {2, 4}, {1, 14}, {4, 6}, {1, 2}, {4, 7}, {10, 11}, {8, 9}, {5, 12}, {9, 10}, {12, 14}, {2, 4}, {5, 8}, {5, 7}, {6, 7}, {11, 13}, {7, 8}, {10, 12}, {9, 10}, {8, 9}, {3, 5}, {7, 8}, {11, 14}, {5, 6}, {13, 15}, {13, 14}, {3, 4}, {11, 12}, {6, 7}, {9, 10}, {1, 10}, {2, 3}, {10, 11}, {4, 5}
    };
    
    assert(best.is_sorting_network());
    
    while (1){
        
        while (1){
            n_tests += 10;
            if (n_tests >= 10000) n_tests = 100;
            
            SortingNetwork temp(best);
            
            uint32_t n_scrambles = xorshift32() % 5 + 1;
            
            for (uint32_t k = 0; k < n_scrambles; k++){
                temp.scramble();
            }
            
            if (temp.error(n_tests) <= best.error(n_tests)){
                best = temp;
                //printf("improved error: %f %i\n", best.error(n_tests), n_tests);
            }
            
            if (best.is_sorting_network()){
                printf("Found sorting network of size %lu\n", best.size());
                
                for (const auto &p : best.indices){
                    printf("{%u, %u}, ", p.a, p.b);
                }
                printf("\n");
                
        
                best.indices.pop_back();
                break;
            }
        }
    }
    
    return 0;
}
