#ifndef WINDOW_H
#define WINDOW_H

#include "definitions.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>


class GameWindow 
{
public:
    GameWindow(int scale = 4);
    ~GameWindow();
    bool closed();
    void draw_frame(float framebuffer[]);

private:
    SDL_Window *sdl_window;
    SDL_GLContext *gl_context;
    SDL_Event event;
    GLuint shader_id;
    const int window_scale;

    void compile_shader();
    void init_window();
    void init_glcontext();
    void init_screen_texture();
};

#endif