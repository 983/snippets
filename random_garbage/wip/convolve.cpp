#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <time.h>

static double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9*t.tv_nsec;
}

static float randf(){
    return rand()/(1.0f + RAND_MAX);
}
    
int width = 224;
int height = 224;
int r = 1;
int d = 2*r + 1;

std::vector<float> kernel(d*d);
std::vector<float> image(width*height);
std::vector<float> result(width*height, 0.0f);

void foo(){
    for (int y = r; y < height - r; y++){
        for (int x = r; x < width - r; x++){
            float sum = 0.0f;
            int k = 0;
            for (int y2 = y - r; y2 <= y + r; y2++){
                for (int x2 = x - r; x2 <= x + r; x2++){
                    sum += image[x2 + y2*width]*kernel[k++];
                }
            }
            result[x + y*width] = sum;
        }
    }
}

int main(){
    
    for (int k = 0; k < 10; k++){
        
        for (int i = 0; i < d*d; i++){
            kernel[i] = randf();
        }
        
        for (int i = 0; i < width*height; i++){
            image[i] = randf();
        }
        
        double t = sec();   
        
        foo();
        
        double dt = sec() - t;
        
        float checksum = 0.0f;
        
        for (int i = 0; i < width*height; i++){
            checksum += result[i];
        }
        
        printf("%f mflops, checksum %f\n", 1e-6*width*height*d*d/dt, checksum);
    }
    
    return 0;
}
