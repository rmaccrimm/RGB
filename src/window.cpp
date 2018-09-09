#include "window.h"
#include <iostream>
#include <fstream>
#include <streambuf>

void check_glError(std::string msg)
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		std::cout << msg << " " << std::hex << err << std::endl;
	}
}

const float SCREEN_QUAD[] = {  
//  position       texture coords      
    -1,  1,  0,    0, 1,
    -1, -1,  0,    0, 0,
     1, -1,  0,    1, 0,
    -1,  1,  0,    0, 1,
     1, -1,  0,    1, 0,
     1,  1,  0,    1, 1
};

GameWindow::GameWindow(Joypad *pad, int scale) : 
    joypad(pad), window_scale(scale), key_pressed{0}, draw(0)
{
    init_window();
    init_glcontext();
    compile_shader();
    init_screen_texture();
}

GameWindow::~GameWindow()
{
    SDL_GL_DeleteContext(gl_context);
    SDL_Quit();
}

void GameWindow::draw_frame(u8 framebuffer[])
{  
    glActiveTexture(GL_TEXTURE0);
    glTexSubImage2D(
        GL_TEXTURE_2D, 
        0,
        0,
        0, 
        constants::screen_w,
        constants::screen_h, 
        GL_RED_INTEGER,
        GL_UNSIGNED_BYTE, 
        &framebuffer[0]
    );
    glDrawArrays(GL_TRIANGLES, 0, 6); 
    SDL_GL_SwapWindow(sdl_window);
    draw = true;
}

void GameWindow::set_palette(u8 palette[])
{
    glActiveTexture(GL_TEXTURE1);
    for (int i = 0; i < 4; i++) {
        bgp[i] = palette[i];
        std::cout << (int)palette[i] << ' ';
    }
    std::cout << std::endl;
    glBindTexture(GL_TEXTURE_RECTANGLE, color_palette);
    glTexSubImage2D(
        GL_TEXTURE_RECTANGLE,
        0,
        0,
        0,
        4,
        1,
        GL_RED_INTEGER,
        GL_UNSIGNED_BYTE,
        &bgp[0]
    );
}

void GameWindow::init_screen_texture()
{
    // Create new texture for screen quad
    glGenTextures(1, &screen_tex);
    glGenTextures(1, &color_palette);
    

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screen_tex);
    // No texture smoothing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_R8UI, 
        constants::screen_w,
        constants::screen_h, 
        0, 
        GL_RED_INTEGER,
        GL_UNSIGNED_BYTE, 
        0
    );

	check_glError("Screen Texture:");

    glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_RECTANGLE, color_palette);
    check_glError("Bind Texture:");
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    check_glError("Color Palette Params:");
    bgp[0] = 0xff;
    bgp[1] = 0xb6;
    bgp[2] = 0x1d;
    bgp[3] = 0x00;
	glTexImage2D(
		GL_TEXTURE_RECTANGLE,
		0,
		GL_R8UI,
		4,
        1,
		0,
		GL_RED_INTEGER,
		GL_UNSIGNED_BYTE,
		&bgp[0]
	);
	// glBindTexture(GL_TEXTURE_1D, 0);
	check_glError("Color Palette Texture:");
    
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
    int uniformloc = glGetUniformLocation(shader_id, "screen_texture");
    if (uniformloc == -1) {
        std::cout << "Error: Uniform \"screen_texture\" not found" << std::endl;
    }
    glUniform1i(uniformloc, 0);
	
	uniformloc = glGetUniformLocation(shader_id, "color_palette");
	if (uniformloc == -1) {
		std::cout << "Error: Uniform \"color_palette\" not found" << std::endl;
	}
	glUniform1i(uniformloc, 1);

    GLenum err;
    while((err = glGetError()) != GL_NO_ERROR) {
        std::cout << "End init: 0x" << std::hex << err << std::endl;
    }
}

bool GameWindow::closed() 
{
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT) {
        return true;
    }
    else {
        return false;
    }
    draw = false;
}

bool GameWindow::frame_drawn() { return draw; }

void GameWindow::process_input()
{
    int key;
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
        // TODO - define a map with reconfigurable keys, just do keymap[event.key...]
        switch (event.key.keysym.sym)
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
        case SDLK_RETURN:
            key = Joypad::START;
            break;
        case SDLK_BACKSPACE:
            key = Joypad::SELECT;
            break;
        default:
            break;
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


void GameWindow::init_window() 
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Initialization failed. SDL Error: " << SDL_GetError() << std::endl;
    }
    else {
        sdl_window = SDL_CreateWindow(
            "Gameboy Emulator", 
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, 
            constants::screen_w * window_scale,
            constants::screen_h * window_scale, 
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
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GLContext gl_context = SDL_GL_CreateContext(sdl_window);
    if (gl_context == nullptr) {
        std::cout << "Creating OpenGL context failed. SDL Error: " << SDL_GetError() << std::endl;
    }
    glewExperimental = GL_TRUE;
    GLenum glew_err = glewInit();
    if (glew_err != GLEW_OK) {
        std::cout << "Error initializing GLEW: " << glewGetErrorString(glew_err) << std::endl;
    }
}

void GameWindow::compile_shader()
{
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    int success;
    char err_log[512];

    std::ifstream vert_file("../../../src/vertex.glsl");
    if (!vert_file.good()) {
        // TODO - error code
        std::cout << "Error reading vertex shader: " << std::endl;
    }
    std::string vert_src_str((std::istreambuf_iterator<char>(vert_file)), 
                              std::istreambuf_iterator<char>());
    const char *vert_src = vert_src_str.c_str();                             

    std::ifstream frag_file("../../../src/fragment.glsl");
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

