/*

gcc screenshot.c -o screenshot -lgdi32 && screenshot

*/
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

double sec(){
    LARGE_INTEGER f, t;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t);
    return t.QuadPart / (double)f.QuadPart;
}

void screenshot(){
    HDC hdc = GetDC(0);

    BITMAPINFO bmih;
    memset(&bmih, 0, sizeof(bmih));
    bmih.bmiHeader.biSize = sizeof(bmih.bmiHeader); 
    
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    
    HDC hCaptureDC  = CreateCompatibleDC(hdc);

    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, width, height);
    HGDIOBJ hOld = SelectObject(hCaptureDC, hBitmap);

    double t = sec();
    
    BitBlt(hCaptureDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY|CAPTUREBLT); 

    double dt = sec() - t;
    printf("%f seconds\n", dt);
    
    if(0 == GetDIBits(hdc, hBitmap, 0, 0, NULL, &bmih, DIB_RGB_COLORS)){
        fprintf(stderr, "failed to read BITMAPINFO\n");
    }

    uint8_t *pixels = (uint8_t*)malloc(bmih.bmiHeader.biSizeImage);

    bmih.bmiHeader.biCompression = BI_RGB;

    if (0 == GetDIBits(hdc, hBitmap, 0, bmih.bmiHeader.biHeight, (LPVOID)pixels, &bmih, DIB_RGB_COLORS)){
        fprintf(stderr, "failed to read pixels\n");
    }
    
    int size = bmih.bmiHeader.biSizeImage;
    
    BITMAPFILEHEADER bmfh;
    memset(&bmfh, 0, sizeof(bmfh));
    // magic number to identify file as bmp file
    bmfh.bfType = 0x4d42;
    bmfh.bfSize = sizeof(bmfh) + sizeof(bmih.bmiHeader) + size;
    bmfh.bfOffBits = sizeof(bmfh) + sizeof(bmih.bmiHeader);
    
    FILE *fp = fopen("screenshot.bmp", "wb");
    fwrite(&bmfh, 1, sizeof(bmfh), fp);
    fwrite(&bmih.bmiHeader, 1, sizeof(bmih.bmiHeader), fp);
    fwrite(pixels, 1, size, fp);
    fclose(fp);
    
    free(pixels);
    
    DeleteObject(hBitmap);
    
    SelectObject(hCaptureDC, hOld);
    DeleteDC(hCaptureDC);

    ReleaseDC(NULL, hdc);
}

int main(){
    screenshot();
}
