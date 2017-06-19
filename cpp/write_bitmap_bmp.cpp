#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

template <typename T>
struct Vec3 {
    T x, y, z;
};

template <typename T> Vec3<T> operator + (Vec3<T> a, Vec3<T> b){ return Vec3<T>{a.x + b.x, a.y + b.y, a.z + b.z}; }
template <typename T> Vec3<T> operator - (Vec3<T> a, Vec3<T> b){ return Vec3<T>{a.x - b.x, a.y - b.y, a.z - b.z}; }
template <typename T> Vec3<T> operator * (Vec3<T> a, Vec3<T> b){ return Vec3<T>{a.x * b.x, a.y * b.y, a.z * b.z}; }
template <typename T> Vec3<T> operator / (Vec3<T> a, Vec3<T> b){ return Vec3<T>{a.x / b.x, a.y / b.y, a.z / b.z}; }

template <typename T>
Vec3<T> operator * (T a, Vec3<T> b){
    return Vec3<T>{a*b.x, a*b.y, a*b.z};
}

typedef Vec3<float> Vec3f;

void put(uint8_t *data, int n, uint32_t value){
    for (int i = 0; i < n; i++) data[i] = (value >> i*8) & 0xff;
}

void write_bmp(const char *path, int width, int height, const uint8_t *data){
    uint8_t header[14 + 40];
    // "BM" in hex ascii
    put(header +  0, 1, 0x42);
    put(header +  1, 1, 0x4D);
    // total image size
    put(header +  2, 4, 14 + 40 + width*height*4);
    // unused
    put(header +  6, 4, 0);
    // total header size
    put(header + 10, 4, 14 + 40);
    // second header size
    put(header + 14, 4, 40);
    // image dimensions
    put(header + 18, 4, width);
    put(header + 22, 4, height);
    // number of color planes
    put(header + 26, 2, 1);
    // bits per pixel
    put(header + 28, 2, 4*8);
    // compression method (0 means no compression is used)
    put(header + 30, 4, 0);
    // size of image data
    put(header + 34, 4, width*height*4);
    // x and y pixels/meter, 72 DPI
    put(header + 38, 4, 2835);
    put(header + 42, 4, 2835);
    // unused
    put(header + 46, 4, 0);
    put(header + 50, 4, 0);
    
    FILE *fp = fopen(path, "wb");
    fwrite(header, 1, sizeof(header), fp);
    fwrite(data, 1, width*height*4, fp);
    fclose(fp);
}

int main(){
    int width = 640;
    int height = 480;
    
    // image pixels bottom left to top right
    uint8_t *data = (uint8_t*)malloc(width*height*4);
    
    for (int i = 0; i < width*height*4; i += 4){
        uint8_t gray = rand() & 1 ? 255 : 0;
        data[i + 0] = 0;
        data[i + 1] = gray;
        data[i + 2] = 0;
        data[i + 3] = 0xff;
    }
    
    write_bmp("test.bmp", width, height, data);
    
    free(data);
    
    printf("done\n");
    
    return 0;
}
