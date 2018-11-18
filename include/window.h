#ifndef WINDOW_H
#define WINDOW_H

#include "definitions.h"
#include "joypad.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <string>


class GameWindow 
{
public:
    GameWindow(Joypad *pad, int scale = 4, bool limit_framerate = true, std::string title = "");

    ~GameWindow();

    bool closed();

    bool paused();

    bool frame_drawn();

    void process_input();

    void update_background(u8 background[], int x, int y);

    void update_sprites(u8 sprites[]);

    void draw_frame();

	void set_bg_palette(u8 palette[]);

    bool draw;

private:
    Joypad *joypad;
    SDL_Window *sdl_window;
    SDL_GLContext gl_context;
    SDL_Event event;
    GLuint shader_id;
    GLuint screen_tex;
    GLuint sprite_tex;
	GLuint color_palette;
    const int window_scale;
    int key_pressed[8];
    
    bool pause;
    // shader uniforms
    bool background;
    int scrollx;
    int scrolly;
    int tex_type;


    enum Textures { BACKGROUND, SPRITES, WINDOW };

    void compile_shader();
    void init_window(std::string title);
    void init_glcontext(bool limit_framerate);
    void init_screen_texture();
    int get_uniform(std::string uniform_name);
};

#endif