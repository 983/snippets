#include "util.c"
#include "my_window.c"
#include "mat4.c"
#include <dirent.h>

#define MAX_PATH_LENGTH 2048

void reload_image(Image *image, char *path, int step){
    free(image->data);

    if (step != 0){
        char *slash = strrchr((char*)path, '/');
        char name[MAX_PATH_LENGTH];
        char directory[MAX_PATH_LENGTH];

        if (slash){
            snprintf(name, sizeof(name), "%s", slash + 1);
            snprintf(directory, sizeof(directory), "%.*s", (int)(slash - path), path);
        }else{
            snprintf(name, sizeof(name), "%s", path);
            snprintf(directory, sizeof(directory), ".");
        }

        DIR *d = opendir(directory);

        char *names[10000];
        int n_names = 0;

        while (1){
            struct dirent *entry = readdir(d);

            if (!entry) break;

            if (might_be_image_file(entry->d_name)){
                names[n_names++] = strdup(entry->d_name);
            }

            // TODO grow names
            if (n_names >= 10000) break;
        }

        closedir(d);

        natsort((const char**)names, n_names);

        int index = -1;

        for (int i = 0; i < n_names; i++){
            if (0 == strcmp(names[i], name)){
                index = i;
            }
        }

        if (index == -1){
            if (n_names == 0){
                fprintf(stderr, "ERROR: Failed to find image in directory:\n%s\n", directory);
                exit(-1);
            }

            index = 0;
        }

        index = (index + n_names + step) % n_names;

        int n = snprintf(path, MAX_PATH_LENGTH, "%s/%s", directory, names[index]);

        if (n >= MAX_PATH_LENGTH - 1){
            fprintf(stderr, "Path too long\n");
            exit(-1);
        }

        for (int i = 0; i < n_names; i++){
            free(names[i]);
        }
    }

    load_image(image, path);
}

void update_title(MyWindow *window, Image *image, const char *path){
    char title[65536];
    snprintf(title, sizeof(title), "%i x %i - %s", image->width, image->height, path);
    XStoreName(window->display, window->window_id, title);
}

int main(int argc, char **argv){
    char path[MAX_PATH_LENGTH] = "test.png";

    if (argc >= 2){
        snprintf(path, MAX_PATH_LENGTH, "%s", argv[1]);
    }

    Image image[1];
    memset(image, 0, sizeof(image));
    reload_image(image, path, 0);

    MyWindow window[1];
    init_window(window,
        mini(image->width, 1920),
        mini(image->height, 1080));

    update_title(window, image, path);

    Vec2 mouse = v2(0, 0);
    Vec2 mouse_old = v2(0, 0);
    char is_button_down[256];
    memset(is_button_down, 0, sizeof(is_button_down));

    float dx = 0;
    float dy = 0;
    float scale = fmax(
        image->width / (float)window->width,
        image->height / (float)window->height);

    const float zoom_factor = 3.0f / 4.0f;

    int running = 1;
    while (running){
        // finite loop to prevent hanging in case of infinite event loop
        // forgot why infinite event loops happened but they definitely did happen in some cases
        for (int k = 0; k < 100; k++){
            if (!XPending(window->display)) break;

            XEvent event;
            XNextEvent(window->display, &event);
            // can look stuff up with "xev" program
            switch (event.type){
                case KeyPress:
                    printf("keycode: %i\n", event.xkey.keycode);
                    switch (event.xkey.keycode){
                        // quit (q)
                        case 24:
                            running = 0;
                            break;

                        // space
                        case 65:
                            dx = 0;
                            dy = 0;
                            scale = fmax(
                                image->width / (float)window->width,
                                image->height / (float)window->height);
                            break;

                        // F5
                        case 71:
                            reload_image(image, path, 0);
                            update_title(window, image, path);
                            break;

                        // left
                        case 113:
                        // up
                        case 111:
                            reload_image(image, path, -1);
                            update_title(window, image, path);
                            break;

                        // right
                        case 114:
                        // down
                        case 116:
                            reload_image(image, path, +1);
                            update_title(window, image, path);
                            break;
                    }
                    break;

                case ButtonPress:
                    is_button_down[event.xbutton.button] = 1;

                    mouse_old = mouse;
                    mouse = v2(event.xmotion.x, event.xmotion.y);

                    dx += mouse.x * scale;
                    dy += mouse.y * scale;

                    if (event.xbutton.button == 4){
                        // zoom in
                        scale *= zoom_factor;
                    }
                    if (event.xbutton.button == 5){
                        // zoom out
                        scale /= zoom_factor;
                    }

                    dx -= mouse.x * scale;
                    dy -= mouse.y * scale;
                    break;

                case ButtonRelease:
                    is_button_down[event.xbutton.button] = 0;
                    break;

                case KeyRelease:
                    break;

                case ConfigureNotify:
                    window->width = event.xconfigure.width;
                    window->height = event.xconfigure.height;
                    break;

                case MotionNotify:
                    mouse_old = mouse;
                    mouse = v2(event.xmotion.x, event.xmotion.y);

                    if (is_button_down[1] || is_button_down[2] || is_button_down[3]){
                        dx -= scale * (mouse.x - mouse_old.x);
                        dy -= scale * (mouse.y - mouse_old.y);
                    }
                    break;

                case Expose:
                break;
           }
        }

        int width = window->width;
        int height = window->height;

        MyFramebuffer framebuffer[1];
        init_framebuffer(framebuffer, window, width, height);

        uint32_t *src = (uint32_t*)image->data;
        uint32_t *dst = (uint32_t*)framebuffer->ximage->data;

        int shift_scale = scale * 65536;
        int dx_int = dx;
        int dy_int = dy;

        for (int y_dst = 0; y_dst < height; y_dst++){
            for (int x_dst = 0; x_dst < width; x_dst++){
                int x_src = (shift_scale * x_dst >> 16) + dx_int;
                int y_src = (shift_scale * y_dst >> 16) + dy_int;

                uint32_t color = 0;

                if (x_src >= 0 && x_src < image->width && y_src >= 0 && y_src < image->height){
                    color = src[x_src + (image->height - 1 - y_src) * image->width];
                }

                dst[x_dst + y_dst * width] = color;
            }
        }

/*
        // copy top left rectangle from entire screen
        int x = 0;
        int y = 0;
        Window root_window = RootWindow(window->display, window->screen);
        XShmGetImage(window->display, root_window, framebuffer->ximage, x, y, AllPlanes);

        uint32_t *rgba = (uint32_t*)framebuffer->ximage->data;

        for (int y = 0; y < height; y++){
            for (int x = 0; x < width; x++){
                int dx = x - width / 2;
                int dy = y - height / 2;
                if (dx * dx + dy * dy < 100 * 100){
                    rgba[x + y * width] = xorshift32();
                }
            }
        }
*/
        draw_framebuffer(framebuffer, window);

        release_framebuffer(framebuffer, window);

        usleep(30 * 1000);
    }

    free(image->data);

    XCloseDisplay(window->display);

    return 0;
}
