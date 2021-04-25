#include <stdlib.h>
#include <sys/shm.h>
#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include <X11/Xutil.h>
#include <unistd.h>

typedef struct {
    unsigned long window_id;
    Display *display;
    int screen;
    GC gc;
    int width;
    int height;
} MyWindow;

void init_window(MyWindow *window, int width, int height){
    unsigned long window_id = 0;

    Display *display = XOpenDisplay(NULL);
    int screen = DefaultScreen(display);

    GC gc = DefaultGC(display, screen);

    Window root_window = RootWindow(display, screen);

    window_id = XCreateSimpleWindow(
        display, root_window,
        0, 0, width, height,
        0, BlackPixel(display, screen), WhitePixel(display, screen));

    XMapWindow(display, window_id);

    XSelectInput(display, window_id,
        ExposureMask |
        KeyPressMask |
        KeyReleaseMask |
        ButtonPressMask |
        ButtonReleaseMask |
        ButtonMotionMask |
        PointerMotionMask |
        StructureNotifyMask);

    window->gc = gc;
    window->display = display;
    window->window_id = window_id;
    window->screen = screen;
    window->width = width;
    window->height = height;
}

typedef struct {
    XImage *ximage;
    XShmSegmentInfo info;
    int width;
    int height;
} MyFramebuffer;

void init_framebuffer(MyFramebuffer *framebuffer, MyWindow *window, int width, int height){
    XShmSegmentInfo *info = &framebuffer->info;
    Display *display = window->display;

    Visual *visual = DefaultVisual(display, window->screen);
    int depth = DefaultDepth(display, window->screen);
    XImage *ximage = XShmCreateImage(display, visual, depth, ZPixmap, NULL, info, width, height);
    info->shmid = shmget(IPC_PRIVATE, ximage->bytes_per_line * ximage->height, IPC_CREAT | 0777);
    ximage->data = (char*)shmat(info->shmid, 0, 0);
    info->shmaddr = ximage->data;
    info->readOnly = False;
    XShmAttach(display, info);
    XSync(display, False);
    shmctl(info->shmid, IPC_RMID, 0);

    framebuffer->ximage = ximage;
    framebuffer->width = width;
    framebuffer->height = height;
}

void draw_framebuffer(MyFramebuffer *framebuffer, MyWindow *window){
    int width = framebuffer->width;
    int height = framebuffer->height;

    XShmPutImage(
        window->display,
        window->window_id,
        window->gc,
        framebuffer->ximage,
        // x/y coordinates into ximage (if you only want to draw a subimage)
        0, 0,
        // x/y coordinates where to draw
        0, 0,
        width, height, False);
}

void release_framebuffer(MyFramebuffer *framebuffer, MyWindow *window){
    XShmSegmentInfo *info = &framebuffer->info;
    XShmDetach(window->display, info);
    shmdt(info->shmaddr);
    XDestroyImage(framebuffer->ximage);
}
