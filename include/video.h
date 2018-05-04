#ifndef VIDEO_H
#define VIDEO_H

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <string>
#include "definitions.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int RES_WIDTH = 4;
const int RES_HEIGHT = 4;

struct Pixel 
{
    float r;
    float g;
    float b;
};

class GameWindow 
{
public:
    GameWindow();
    ~GameWindow();
    bool closed();
    void draw_frame(float framebuffer[]);

//private:
    SDL_Window *sdl_window;
    SDL_GLContext *gl_context;
    SDL_Event event;
    unsigned int shader_id;

    void compile_shader();
    void init_window();
    void init_glcontext();
    void init_screen_texture();
};

const float SCREEN_QUAD[] = {  
//  position       texture coords      
    -1,  1,  0,    0, 1,
    -1, -1,  0,    0, 0,
     1, -1,  0,    1, 0,
    -1,  1,  0,    0, 1,
     1, -1,  0,    1, 0,
     1,  1,  0,    1, 1
};

const std::string VERT_SRC = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 inPos;\n"
    "layout (location = 1) in vec2 inTexCoords;\n"
    "out vec2 texCoords;\n"
    "void main() {\n"
        "gl_Position = vec4(inPos, 1.0);\n"
        "texCoords = inTexCoords; }";

const std::string FRAG_SRC = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoords;\n"
    "uniform sampler2D screen_texture;\n"
    "void main() {\n"
        "FragColor = texture(screen_texture, texCoords); }";

#endif