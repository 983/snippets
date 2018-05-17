#include <windows.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

const char *capture_window_title = "Google Chrome";
int max_width = 1920;
int max_height = 1080;
int draw_mouse_cursor = 1;
// overrides capture_window_title
int capture_full_desktop = 1;

double sec(){
    LARGE_INTEGER f, t;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&t);
    return t.QuadPart / (double)f.QuadPart;
}

typedef struct {
    uint8_t *data;
    int width;
    int height;
} Image;

void put(uint8_t *data, int n, uint32_t value){
    for (int i = 0; i < n; i++) data[i] = (value >> i*8) & 0xff;
}

void write_bmp(const char *path, const uint8_t *data, int width, int height){
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

void save_image_bmp(Image *image, const char *path){
    write_bmp(path, image->data, image->width, image->height);
}

typedef struct _ICONINFOEX {
    DWORD   cbSize;
    BOOL    fIcon;
    DWORD   xHotspot;
    DWORD   yHotspot;
    HBITMAP hbmMask;
    HBITMAP hbmColor;
    WORD    wResID;
    TCHAR   szModName[MAX_PATH];
    TCHAR   szResName[MAX_PATH];
} ICONINFOEX;

typedef BOOL (*get_icon_info_ex_func)(HICON, ICONINFOEX*);

get_icon_info_ex_func get_icon_info_ex;

int window_to_image(HWND hwnd, Image *image){
    HDC hdc = GetDC(hwnd);

    BITMAP header;
    memset(&header, 0, sizeof(header));
    
    HGDIOBJ obj = GetCurrentObject(hdc, OBJ_BITMAP);
    GetObject(obj, sizeof(BITMAP), &header);
    
    int width = header.bmWidth;
    int height = header.bmHeight;
    
    if (width > max_width) width = max_width;
    if (height > max_height) height = max_height;

    HDC hCaptureDC  = CreateCompatibleDC(hdc);
    
    HBITMAP hbitmap = CreateCompatibleBitmap(hdc, width, height);
    
    HGDIOBJ hOld = SelectObject(hCaptureDC, hbitmap);

    BitBlt(hCaptureDC, 0, 0, width, height, hdc, 0, 0, SRCCOPY | CAPTUREBLT);
    
    if (draw_mouse_cursor){     
        CURSORINFO cursor = {sizeof(cursor)};
        GetCursorInfo(&cursor);
        
        RECT rect;
        GetWindowRect(hwnd, &rect);
        
        ICONINFOEX info = {sizeof(info)};
        get_icon_info_ex(cursor.hCursor, &info);
        
        int x = cursor.ptScreenPos.x - rect.left - info.xHotspot;
        int y = cursor.ptScreenPos.y - rect.top  - info.yHotspot;
        
        BITMAP bmpCursor = {0};
        GetObject(info.hbmColor, sizeof(bmpCursor), &bmpCursor);
        DrawIconEx(
            hCaptureDC,
            x, y,
            cursor.hCursor,
            bmpCursor.bmWidth,
            bmpCursor.bmHeight,
            0, NULL, DI_NORMAL);
    }
    
    BITMAPINFO bmih;
    memset(&bmih, 0, sizeof(bmih));
    bmih.bmiHeader.biSize = sizeof(bmih.bmiHeader); 
    if(0 == GetDIBits(hdc, hbitmap, 0, 0, NULL, &bmih, DIB_RGB_COLORS)){
        printf("failed to read BITMAPINFO\n");
        return -1;
    }
    
    uint8_t *pixels = (uint8_t*)malloc(bmih.bmiHeader.biSizeImage);
    
    bmih.bmiHeader.biCompression = BI_RGB;
    
    if (0 == GetDIBits(hdc, hbitmap, 0, bmih.bmiHeader.biHeight, (LPVOID)pixels, &bmih, DIB_RGB_COLORS)){
        printf("failed to read pixels\n");
        return -1;
    }
    
    DeleteObject(hbitmap);
    
    SelectObject(hCaptureDC, hOld);
    DeleteDC(hCaptureDC);

    ReleaseDC(NULL, hdc);
    
    image->data = pixels;
    image->width = width;
    image->height = height;
    
    return 0;
}

HWND found_window = NULL;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    char buffer[1024];
    
    GetWindowText(hwnd, buffer, sizeof(buffer));
    
    if (strstr(buffer, capture_window_title)){
        found_window = hwnd;
        return FALSE;
    }
    
    return TRUE;
}

void free_image(Image *image){
    free(image->data);
    memset(image, 0, sizeof(*image));
}

std::vector<uint32_t> stream;

#define FRAME_RESIZE       1
#define FRAME_SAME_AS_LAST 2
#define FRAME_UNCOMPRESSED 3
#define FRAME_NEW          4

void compress(Image *prev_image, Image *curr_image){
    uint32_t *a = (uint32_t*)prev_image->data;
    uint32_t *b = (uint32_t*)curr_image->data;
    
    int width = curr_image->width;
    int height = curr_image->height;
    
    if (prev_image->width != width || prev_image->height != height){
        stream.push_back(FRAME_RESIZE);
        stream.push_back(width);
        stream.push_back(height);
        
        stream.push_back(FRAME_UNCOMPRESSED);
        int n_uncompressed = width*height;
        stream.push_back(width*height);
        stream.insert(stream.end(), b, b + n_uncompressed);
    }else{
        stream.push_back(FRAME_NEW);
        
        int k = 0;
        int n = width*height;
        
        while (k < n){
            int same_start = k;
            while (k < n && a[k] == b[k]) k++;
            int n_same = k - same_start;
            stream.push_back(FRAME_SAME_AS_LAST);
            stream.push_back(n_same);
            
            int different_start = k;
            while (k < n && a[k] != b[k]) k++;
            int n_different = k - different_start;
            
            stream.push_back(FRAME_UNCOMPRESSED);
            stream.push_back(n_different);
            stream.insert(stream.end(), &b[different_start], &b[different_start] + n_different);
        }
    }
}

void save_frames(const char *directory){
    if (stream.empty()) return;
    
    mkdir(directory);
    
    std::vector<uint32_t> last_frame;
    std::vector<uint32_t> curr_frame;
    size_t frame_pos = 0;
    size_t stream_pos = 0;
    int n_frames = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    
    while (stream_pos < stream.size()){
        uint32_t command = stream[stream_pos++];
        
        switch (command){
            case FRAME_SAME_AS_LAST:{
                uint32_t n = stream[stream_pos++];
                assert(curr_frame.size() == last_frame.size());
                assert(frame_pos + n <= curr_frame.size());
                for (; n; n--){
                    curr_frame[frame_pos] = last_frame[frame_pos];
                    frame_pos++;
                }
            }break;
            
            case FRAME_NEW:
                assert(curr_frame.size() == frame_pos);
                
                if (curr_frame.size() > 0){
                    char path[4096];
                    snprintf(path, sizeof(path), "%s/%d.bmp", directory, (int)n_frames);
                    n_frames++;
                    
                    write_bmp(path, (uint8_t*)curr_frame.data(), width, height);
                }
                
                last_frame = curr_frame;
                frame_pos = 0;
            break;
            
            case FRAME_RESIZE:{
                width  = stream[stream_pos++];
                height = stream[stream_pos++];
                
                curr_frame.resize(width*height);
            }break;
            
            case FRAME_UNCOMPRESSED:{
                uint32_t n = stream[stream_pos++];
                assert(frame_pos + n <= curr_frame.size());
                for (; n; n--){
                    curr_frame[frame_pos++] = stream[stream_pos++];
                }
            }break;
        }
    }
}

int main(){
    HINSTANCE user32 = LoadLibrary("user32.dll");
    
    get_icon_info_ex = (get_icon_info_ex_func)GetProcAddress(user32, "GetIconInfoExW");
    assert(get_icon_info_ex != NULL);
    
    EnumWindows(EnumWindowsProc, 0);
    
    if (!found_window){
        MessageBox(NULL, "Could not find window", "Error", MB_OK);
        return -1;
    }
    
    Image prev_image[1];
    memset(prev_image, 0, sizeof(*prev_image));
    
    if (capture_full_desktop){
        found_window = GetDesktopWindow();
    }
    
    while (1){
        if (GetAsyncKeyState(VK_ESCAPE)){
            printf("quit\n");
            break;
        }
        
        double t = sec();
        
        Image image[1];
        int err = window_to_image(found_window, image);
        if (err){
            MessageBox(NULL, "The recorded window was closed", "Error", MB_OK);
            break;
        }
        //save_image_bmp(image, "image.bmp");
        
        compress(prev_image, image);
        
        free_image(prev_image);
        
        *prev_image = *image;
        
        double dt = sec() - t;
        printf("%f seconds, %f mbyte\n", dt, stream.size()*4*1e-6);
        
        Sleep(1000/30);
    }
    
    save_frames("frames");
    
    return 0;
}
