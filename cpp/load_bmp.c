#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// convert cat.jpg -depth 8 -alpha on bmp:-

static int demangle_pixels(
    uint8_t *dst,
    const uint8_t *src,
    int32_t width,
    int32_t height,
    uint16_t bits_per_pixel,
    uint32_t compression_method,
    size_t n_bytes,
    int *offsets
){
    size_t src_offset = 0;
    
    switch (bits_per_pixel){
        case 8:
            if (compression_method != 0){
                printf("Compression method %u not supported for 8 bit bmp files", compression_method);
                return -1;
            }
        
            for (int32_t y = 0; y < height; y++){
                if (src_offset + width > n_bytes){
                    printf("File too short to be a BMP file\n");
                    return -1;
                }
                
                for (int32_t x = 0; x < width; x++){
                    uint8_t gray = src[src_offset++];
                    *dst++ = gray;
                    *dst++ = gray;
                    *dst++ = gray;
                    *dst++ = 0xff;
                }
                
                // allign to multiples of 4 bytes
                while (src_offset % 4 != 0) src_offset++;
            }
        break;
        
        case 24:
            if (compression_method != 0){
                printf("Compression method %u not supported for 24 bit bmp files", compression_method);
                return -1;
            }
            
            for (int32_t y = 0; y < height; y++){
                if (src_offset + 3*width > n_bytes){
                    printf("File too short to be a BMP file\n");
                    return -1;
                }
                
                for (int32_t x = 0; x < width; x++){
                    *dst++ = src[src_offset + 2];
                    *dst++ = src[src_offset + 1];
                    *dst++ = src[src_offset + 0];
                    *dst++ = 0xff;
                    src_offset += 3;
                }
                
                // allign to multiples of 4 bytes
                while (src_offset % 4 != 0) src_offset++;
            }
        break;
        
        case 32:
            if (compression_method == 3){
                
                for (int32_t y = 0; y < height; y++){
                    if (src_offset + 4*width > n_bytes){
                        printf("File too short to be a BMP file\n");
                        return -1;
                    }
                    
                    for (int32_t x = 0; x < width; x++){
                        for (int i = 0; i < 4; i++){
                            int offset = offsets[i];
                            
                            *dst++ = offset >= 0 ? src[src_offset + offset] : 0xff;
                        }
                        src_offset += 4;
                    }
                }
            }else{
                printf("Compression method %u not supported for 32 bit bmp files\n", compression_method);
                return -1;
            }
        break;
        
        default:
            printf("Bit depth %u not supported\n", bits_per_pixel);
            return -1;
    }
    
    return 0;
}

int load_bmp_from_memory(
    uint8_t **out_rgba,
    int *out_width,
    int *out_height,
    
    const uint8_t *data,
    size_t n_bytes
){
    if (n_bytes < 14 + 40){
        printf("Too short to be a bmp file\n");
        return -1;
    }
    
    if (data[0] != (uint8_t)'B' || data[1] != (uint8_t)'M'){
        printf("Not a BMP file - does not start with BM\n");
        return -1;
    }
    
    uint32_t bitmap_data_offset = *(uint32_t*)(data + 10);
    int32_t  width              = *( int32_t*)(data + 18);
    int32_t  height             = *( int32_t*)(data + 22);
    uint16_t bits_per_pixel     = *(uint16_t*)(data + 28);
    uint32_t compression_method = *(uint32_t*)(data + 30);
    
    if (width < 0) width = -width;
    if (height < 0) height = -height;
    
    int offsets[4] = {-1, -1, -1, -1};
    
    if (compression_method == 3){
        if (n_bytes < 14 + 40 + 4*4){
            printf("File too short to be a BMP file\n");
            return -1;
        }
        
        for (int i = 0; i < 4; i++){
            uint32_t mask = *(uint32_t*)(data + 14 + 40 + i*4);
            
            for (int j = 0; j < 4; j++){
                if ((mask >> j*8) == 0xff){
                    offsets[i] = j;
                }
            }
            
            printf("mask %i: 0x%08x - %i\n", i, mask, offsets[i]);
        }
    }
    
    const uint8_t *src = data + bitmap_data_offset;
    
    size_t n_dst_bytes = width*height*4;
    uint8_t *dst = (uint8_t*)malloc(n_dst_bytes);
    
    if (!dst){
        printf("Failed to allocate %zu bytes\n", n_dst_bytes);
        return -1;
    }
    
    int error = demangle_pixels(dst, src, width, height, bits_per_pixel, compression_method, n_bytes, offsets);
    
    if (error){
        free(dst);
        return error;
    }
    
    *out_rgba = dst;
    *out_width = width;
    *out_height = height;
    
    return 0;
}

uint8_t* load_file(const char *path, size_t *out_n_bytes){
    FILE *fp = fopen(path, "rb");
    
    if (fp == NULL){
        printf("ERROR - Failed to open %s\n", path);
        return NULL;
    }
    
    fseek(fp, 0, SEEK_END);
    
    size_t n_bytes = ftell(fp);
    
    rewind(fp);
    
    uint8_t *data = (uint8_t*)malloc(n_bytes + 1);
    
    if (data == NULL){
        printf("ERROR - Failed to allocate %zu bytes\n", n_bytes);
        return NULL;
    }
    
    size_t n_bytes_read = fread(data, 1, n_bytes, fp);
    data[n_bytes] = '\0';
    
    fclose(fp);
    
    if (n_bytes_read != n_bytes){
        printf("ERROR - Could read just %zu of %zu bytes\n", n_bytes_read, n_bytes);
        free(data);
        return NULL;
    }
    
    if (out_n_bytes) *out_n_bytes = n_bytes;
    
    return data;
}

int load_bmp(
    uint8_t **out_rgba,
    int *out_width,
    int *out_height,
    
    const char *path
){
    size_t n_bytes;
    uint8_t *data = load_file(path, &n_bytes);
    
    if (!data) return -1;
    
    return load_bmp_from_memory(out_rgba, out_width, out_height, data, n_bytes);
}

uint8_t *image_pixels;
int image_width;
int image_height;

#include <GL/glut.h>

void on_frame(){
    glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawPixels(image_width, image_height, GL_RGBA, GL_UNSIGNED_BYTE, image_pixels);

    glutSwapBuffers();
}

void work(int frame){
    glutPostRedisplay();
    glutTimerFunc(20, work, frame + 1);
}

int main(int argc, char **argv){
    const char *path;
    // bgra
    path = "../cat32.bmp";
    // abgr
    path = "../cat32csxrgb.bmp";
    // abgr, a=0xff
    path = "../cat32nocsxrgb.bmp";
    // abgr
    path = "../cat32csargb.bmp";
    // abgr
    path = "../cat32nocsargb.bmp";
    
    path = "cat8_345.bmp";
    path = "cat24_1123.bmp";
    path = "cat32csargb.bmp";
    
    load_bmp(&image_pixels, &image_width, &image_height, path);
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1920, 1080);
    glutCreateWindow("");

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glutDisplayFunc(on_frame);
    work(0);
    glutMainLoop();

	return 0;
}
