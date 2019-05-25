#include "window.h"
#include "config.h"
#include <iostream>
#include <fstream>
#include <streambuf>

const unsigned int GameWindow::color_palettes[9][4] = {
    {0xf7e7c6U, 0xd68e49U, 0xa63725U, 0x331e50U},   // super GameBoy
    {0xffffffU, 0xb6b6b6U, 0x676767U, 0x000000U},   // grayscale
    {0xc4cfa1U, 0x8b956dU, 0x4d533cU, 0x1f1f1fU},   // green
    {0xe2f3e4U, 0x94e344U, 0x46878fU, 0x332c50U},   // green-blue
    {0xffffb5U, 0x7bc67bU, 0x6b8c42U, 0x5a3921U},   // Link's Awakening SGB
    {0xe0f8d0U, 0x88c070U, 0x346856U, 0x081820U},   // bgb
    {0xcececeU, 0x6f9edfU, 0x42678eU, 0x102533U},   // Megaman SGB (blue)
    {0xaedf1eU, 0xb62558U, 0x047e60U, 0x2c1700U},   // Metroid II SGB
    {0xffefffU, 0xf7b58cU, 0x84739cU, 0x181010U}    // Pokemon SGB
};

void check_glError(std::string msg)
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cout << msg << " " << std::hex << err << std::endl;
	}
}

const int SCREEN_W = 160;
const int SCREEN_H = 144;

const float SCREEN_QUAD[] = {  
//  position       texture coords      
    -1,  1,  0,    0,        SCREEN_H,
    -1, -1,  0,    0,        0,
     1, -1,  0,    SCREEN_W, 0,
    -1,  1,  0,    0,        SCREEN_H,
     1, -1,  0,    SCREEN_W, 0,
     1,  1,  0,    SCREEN_W, SCREEN_H
};

GameWindow::GameWindow(Joypad *pad, int scale, std::string title) :
    joypad(pad), 
    window_scale(scale), 
    key_pressed{0}, 
    draw(0), 
    quit(false), 
    current_palette(0)
{
    init_window(title);
    init_glcontext();
    compile_shader();
    init_screen_texture();
}

GameWindow::~GameWindow()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_Quit();
}

bool GameWindow::frame_drawn() { return draw; }

bool GameWindow::closed() 
{
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT || quit) {
        return true;
    }
    else {
        return false;
    }
    draw = false;
}

bool GameWindow::paused() 
{
    if (pause) {
        pause = false;
        return true;
    }
    else {
        return false;
    }
}

void GameWindow::draw_frame(u8 pixel_buffer[])
{
    glUniform1uiv(get_uniform("palette"), 4, color_palettes[current_palette]);
    glActiveTexture(GL_TEXTURE0);
    glTexSubImage2D(
        GL_TEXTURE_2D, 
        0,
        0,
        0, 
        160,
        144,
        GL_RED_INTEGER,
        GL_UNSIGNED_BYTE, 
        pixel_buffer
    );
    glDrawArrays(GL_TRIANGLES, 0, 6); 
    SDL_GL_SwapWindow(sdl_window);
    draw = true;
}

void GameWindow::init_window(std::string title) 
{
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0) {
        std::cout << "Initialization failed. SDL Error: " << SDL_GetError() << std::endl;
    }
    else {
        
        sdl_window = SDL_CreateWindow(
            title.c_str(),
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, 
            SCREEN_W * window_scale,
            SCREEN_H * window_scale, 
            SDL_WINDOW_OPENGL
        );
        if (sdl_window == nullptr) {
            std::cout << "Creating window failed. SDL ERROR: " << SDL_GetError() << std::endl;
        }
    }  
}

void GameWindow::init_glcontext()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    gl_context = SDL_GL_CreateContext(sdl_window);
    SDL_GL_SetSwapInterval(0); // 0 = immediate updates
    if (gl_context == nullptr) {
        std::cout << "Creating OpenGL context failed. SDL Error: " << SDL_GetError() << std::endl;
    }
    glewExperimental = GL_TRUE;
    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        std::cout << "Error initializing GLEW: " << glewGetErrorString(glew_err) << std::endl;
    }
}

int GameWindow::get_uniform(std::string shader_name)
{
    int uniformloc = glGetUniformLocation(shader_id, shader_name.c_str());
    if (uniformloc == -1) {
        std::cout << "Error: Uniform \"" << shader_name << "\" not found" << std::endl;
    }
    return uniformloc;
}

void GameWindow::init_screen_texture()
{
    // Create new texture for screen quad
    glGenTextures(1, &screen_tex);
    glGenTextures(1, &sprite_tex);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen_tex);
    // No texture smoothing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // Wrap textures to implement proper scrolling behaviour
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_R8UI, 
        160,
        144,
        0, 
        GL_RED_INTEGER,
        GL_UNSIGNED_BYTE, 
        0
    );

	check_glError("Screen Texture:");
    
    GLuint screen_vao;
    GLuint screen_vbo;
    glGenVertexArrays(1, &screen_vao);
    glGenBuffers(1, &screen_vbo);
    glBindVertexArray(screen_vao);
    glBindBuffer(GL_ARRAY_BUFFER, screen_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(SCREEN_QUAD), SCREEN_QUAD, GL_STATIC_DRAW);
    // vertex positions
    glEnableVertexAttribArray(0);    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // vertex texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glUseProgram(shader_id);
    glUniform1i(get_uniform("screen_texture"), 0);
    glUniform4uiv(get_uniform("palette"), 1, color_palettes[current_palette]);

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "End init: 0x" << std::hex << err << std::endl;
    }
}

void GameWindow::compile_shader()
{
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    int success;
    char err_log[512];

    std::string root(PROJECT_ROOT);

    std::ifstream vert_file(root + "/src/vertex.glsl");
    if (!vert_file.good()) {
        // TODO - error code
        std::cout << "Error reading vertex shader: " << std::endl;
    }
    std::string vert_src_str((std::istreambuf_iterator<char>(vert_file)), 
                              std::istreambuf_iterator<char>());
    const char *vert_src = vert_src_str.c_str();                             

    std::ifstream frag_file(root + "/src/fragment.glsl");
    if (!frag_file.good()) {
        std::cout << "Error reading fragment shader: " << std::endl;
    }
    std::string frag_src_str((std::istreambuf_iterator<char>(frag_file)), 
                             std::istreambuf_iterator<char>());
    const char *frag_src = frag_src_str.c_str();
    
    glShaderSource(vertex, 1, &vert_src, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, err_log);
        std::cout << "Vertex shader compilation failed: " << err_log << std::endl;
    }

    glShaderSource(fragment, 1, &frag_src, nullptr);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, nullptr, err_log);
        std::cout << "Fragment shader compilation failed: " << err_log << std::endl;
    }

    shader_id = glCreateProgram();
    glAttachShader(shader_id, vertex);
    glAttachShader(shader_id, fragment);
    glLinkProgram(shader_id);
    glGetProgramiv(shader_id, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_id, 512, NULL, err_log);
        std::cout << "Shader program compilation failed: " << err_log << std::endl;
    }
}

void GameWindow::process_input()
{
    int key;
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
            quit = true;
            return;
        }
        switch (auto key_code = event.key.keysym.sym)
        {
        case SDLK_LEFT:
            key = Joypad::LEFT;
            break;
        case SDLK_RIGHT:
            key = Joypad::RIGHT;
            break;
        case SDLK_UP:
            key = Joypad::UP;
            break;
        case SDLK_DOWN:
            key = Joypad::DOWN;
            break;
        case SDLK_a:
            key = Joypad::A;
            break;
        case SDLK_b:
            key = Joypad::B;
            break;
        case SDLK_p:
            key = Joypad::NONE;
            pause = true;
            break;
        case SDLK_RETURN:
            key = Joypad::START;
            break;
        case SDLK_BACKSPACE:
            key = Joypad::SELECT;
            break;
        case SDLK_1:
        case SDLK_2:
        case SDLK_3:
        case SDLK_4:
        case SDLK_5:
        case SDLK_6:
        case SDLK_7:
        case SDLK_8:
        case SDLK_9:
            key = Joypad::NONE;
            current_palette = key_code - SDLK_1;
            break;
        default:
            return;
        };
        if (event.type == SDL_KEYDOWN) {
            if (!key_pressed[key]) {
                joypad->press_key(key);
                key_pressed[key] = !key_pressed[key];
            }
        }
        else {
            if (key_pressed[key]) {
                joypad->release_key(key);
                key_pressed[key] = !key_pressed[key];
            }
        }
    }
    draw = false;
}
