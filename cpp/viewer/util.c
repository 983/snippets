#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

double sec(){
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + 1e-9*t.tv_nsec;
}

uint32_t xorshift32_state = 0x12345678;

uint32_t xorshift32(){
    uint32_t x = xorshift32_state;

    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    xorshift32_state = x;

    return x;
}

int mini(int a, int b){
    return a < b ? a : b;
}

int clampi(int x, int a, int b){
    return x < a ? a : x > b ? b : x;
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

#include <FreeImage.h>
#include <stdlib.h>

typedef struct {
    uint8_t *data;
    int width;
    int height;
} Image;

int load_image(Image *image, const char *path){
    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(path, 0);

    FIBITMAP *bitmap = FreeImage_Load(format, path, 0);

    if (!bitmap){
        fprintf(stderr, "ERROR: Failed to load image:\n%s\n", path);
        return -1;
    }

    // convert to RGBA if necessary
    if (FreeImage_GetBPP(bitmap) != 32){
        FIBITMAP *temp = bitmap;
        bitmap = FreeImage_ConvertTo32Bits(bitmap);
        FreeImage_Unload(temp);
    }

    int width = FreeImage_GetWidth(bitmap);
    int height = FreeImage_GetHeight(bitmap);

    uint8_t *data_bgra = FreeImage_GetBits(bitmap);

    uint8_t *data_rgba = malloc(width * height * 4);

    for (int i = 0; i < width * height * 4; i += 4){
        uint32_t b = data_bgra[i + 2];
        uint32_t g = data_bgra[i + 1];
        uint32_t r = data_bgra[i + 0];
        uint32_t a = data_bgra[i + 3];

        uint8_t bg_r = 0x80;
        uint8_t bg_g = 0x80;
        uint8_t bg_b = 0x80;

        r = (a * r + (255 - a) * bg_r) >> 8;
        g = (a * g + (255 - a) * bg_g) >> 8;
        b = (a * b + (255 - a) * bg_b) >> 8;

        data_rgba[i + 0] = r;
        data_rgba[i + 1] = g;
        data_rgba[i + 2] = b;
        data_rgba[i + 3] = a;
    }

    FreeImage_Unload(bitmap);

    image->width = width;
    image->height = height;
    image->data = data_rgba;

    return 0;
}
