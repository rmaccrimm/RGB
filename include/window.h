#ifndef WINDOW_H
#define WINDOW_H

#include "definitions.h"
#include "joypad.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <string>
#include <map>


class GameWindow 
{
public:
    GameWindow(Joypad *pad, int scale = 4, std::string title = "");

    ~GameWindow();

    bool closed();

    bool paused();

    bool frame_drawn();

    void process_input();

    void draw_frame(u8 pixel_buffer[]);

    bool draw;

private:
    Joypad *joypad;
    SDL_Window *sdl_window;
    SDL_GLContext gl_context;
    SDL_Event event;
    GLuint shader_id;
    GLuint screen_tex;
    GLuint sprite_tex;
    const int window_scale;
    std::map<SDL_Keycode, bool> key_pressed;
    
    bool pause;
    bool quit;

    // shader uniforms
    bool background;
    int scrollx;
    int scrolly;
    int tex_type;
    
    int current_palette;
    static const unsigned int color_palettes[9][4];
    bool invert_colors;

    enum Textures { BACKGROUND, SPRITES, WINDOW };

    void compile_shader();
    void init_window(std::string title);
    void init_glcontext();
    void init_screen_texture();
    int get_uniform(std::string uniform_name);
};

#endif