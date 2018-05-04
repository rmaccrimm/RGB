#include "window.h"
#include <iostream>

const float SCREEN_QUAD[] = {  
//  position       texture coords      
    -1,  1,  0,    0, 1,
    -1, -1,  0,    0, 0,
     1, -1,  0,    1, 0,
    -1,  1,  0,    0, 1,
     1, -1,  0,    1, 0,
     1,  1,  0,    1, 1
};

const char *VERT_SRC = 
    "#version 330 core\n"
    "layout (location = 0) in vec3 inPos;\n"
    "layout (location = 1) in vec2 inTexCoords;\n"
    "out vec2 texCoords;\n"
    "void main() {\n"
        "gl_Position = vec4(inPos, 1.0);\n"
        "texCoords = inTexCoords; }";

const char *FRAG_SRC = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec2 texCoords;\n"
    "uniform sampler2D screen_texture;\n"
    "void main() {\n"
        "FragColor = texture(screen_texture, texCoords); }";

GameWindow::GameWindow()
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

bool GameWindow::closed() 
{
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT) {
        return true;
    }
    else {
        return false;
    }
}

void GameWindow::draw_frame(float framebuffer[])
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_W, SCREEN_H, 0, GL_RGB, GL_FLOAT, framebuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6); 
    SDL_GL_SwapWindow(sdl_window);
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
            SCREEN_W * SCALE,
            SCREEN_H * SCALE, 
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

    glShaderSource(vertex, 1, &VERT_SRC, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, err_log);
        std::cout << "Vertex shader compilation failed: " << err_log << std::endl;
    }

    glShaderSource(fragment, 1, &FRAG_SRC, nullptr);
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

void GameWindow::init_screen_texture()
{
    // Create new texture object
    GLuint screen_tex;
    glGenTextures(1, &screen_tex);
    glBindTexture(GL_TEXTURE_2D, screen_tex);
    // No texture smoothing
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Set active texture unit
    glActiveTexture(GL_TEXTURE0);
    
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
}