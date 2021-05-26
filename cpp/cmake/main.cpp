// sudo apt install libsdl2-dev mesa-utils
// cmake . && make && ./MyProject
#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/gl.h>

int main(){
    int window_width = 800;
    int window_height = 600;
    int mouse_x = 0;
    int mouse_y = 0;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow(
        "Title goes here",
        0, 0,
        window_width,
        window_height,
        SDL_WINDOW_OPENGL |
        SDL_WINDOW_RESIZABLE);

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    SDL_GL_MakeCurrent(window, gl_context);

    int running = 1;
    while (running){
        // Process events in a loop
        SDL_Event event;
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_KEYDOWN){
                if (event.key.keysym.sym == SDLK_ESCAPE){
                    printf("Escape was pressed. Quit.\n");
                    running = 0;
                    break;
                }
            }

            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED){
                window_width = event.window.data1;
                window_height = event.window.data2;
                glViewport(0, 0, window_width, window_height);
                printf("Window resized to %i %i\n", window_width, window_height);
            }

            if (event.type == SDL_QUIT){
                printf("Window closed\n");
                running = 0;
                break;
            }

            if (event.type == SDL_MOUSEBUTTONDOWN){
                printf("Button %i pressed\n", event.button.button);
                mouse_x = event.button.x;
                mouse_y = event.button.y;
            }

            if (event.type == SDL_MOUSEMOTION){
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
            }

            if (event.type == SDL_MOUSEWHEEL){
                printf("Mouse wheel spinned by %i\n", event.wheel.y);
            }
        }

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // set up projection matrix for [0, width) x [height, 0) drawing
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glOrtho(0.0f, window_width, window_height, 0.0f, -1.0f, 1.0f);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        // draw a circle at mouse position
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 100; i++){
            float angle = 2.0f * 3.14159f * i / 100;
            float radius = 50.0f;
            float x = mouse_x + radius * cos(angle);
            float y = mouse_y + radius * sin(angle);
            glVertex2f(x, y);
        }
        glEnd();

        SDL_GL_SwapWindow(window);
        SDL_Delay(1000 / 60);
    }

    return 0;
}
