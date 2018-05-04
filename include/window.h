#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "definitions.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 576;
const int RES_WIDTH = 160;
const int RES_HEIGHT = 144;

class GameWindow 
{
public:
    GameWindow();
    ~GameWindow();
    bool closed();
    void draw_frame(float framebuffer[]);

private:
    SDL_Window *sdl_window;
    SDL_GLContext *gl_context;
    SDL_Event event;
    GLuint shader_id;

    void compile_shader();
    void init_window();
    void init_glcontext();
    void init_screen_texture();
};

#endif