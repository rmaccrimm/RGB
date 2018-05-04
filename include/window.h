#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "definitions.h"

const int SCREEN_W = 160;
const int SCREEN_H = 144;
const int SCALE = 4;

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