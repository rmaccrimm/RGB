#include <iostream>
#include <vector>
#include <string>

#include "GL/glew.h"
#include "SDL2/SDL_opengl.h"
#include "SDL2/SDL.h"

#include "debug.h"
#include "definitions.h"
#include "register16bit.h"
#include "register8bit.h"
#include "processor.h"

#undef main

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

float screen_quad[] = {  
//  position       texture coords      
    -1,  1,  0,    0, 1,
    -1, -1,  0,    0, 0,
     1, -1,  0,    1, 0,
    -1,  1,  0,    0, 1,
     1, -1,  0,    1, 0,
     1,  1,  0,    1, 1
};

void compile_shader(GLuint &shader_id)
{
    const std::string vertex_src = 
        "#version 330 core\n"
        "layout (location = 0) in vec3 inPos;\n"
        "layout (location = 1) in vec2 inTexCoords;\n"
        "out vec2 texCoords;\n"
        "void main() {\n"
            "gl_Position = vec4(inPos, 1.0);\n"
            "texCoords = inTexCoords; }";

    const std::string fragment_src = 
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "in vec2 texCoords;\n"
        "uniform sampler2D screen_texture;\n"
        "void main() {\n"
            "FragColor = texture(screen_texture, texCoords); }";

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    int success;
    char err_log[512];

    const char *v_src = vertex_src.c_str();
    glShaderSource(vertex, 1, &v_src, nullptr);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, nullptr, err_log);
        std::cout << "Vertex shader compilation failed: " << err_log << std::endl;
    }

    const char *f_src = fragment_src.c_str();
    glShaderSource(fragment, 1, &f_src, nullptr);
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

int main(int argc, char *argv[])
{  
    SDL_Window* window = nullptr;
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "Initialization failed. SDL Error: " << SDL_GetError() << std::endl;
    }
    else {
        window = SDL_CreateWindow(
            "Gameboy Emulator", 
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, 
            SCREEN_WIDTH,
            SCREEN_HEIGHT, 
            SDL_WINDOW_OPENGL
        );
        if (window == NULL) {
            std::cout << "Creating window failed. SDL ERROR: " << SDL_GetError() << std::endl;
        }
    }
    
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	if (glcontext == nullptr) {
		std::cout << "Creating OpenGL context failed. SDL Error: " << SDL_GetError() << std::endl;
	}

	glewExperimental = GL_TRUE;
	GLenum glew_err = glewInit();
	if (glew_err != GLEW_OK) {
		std::cout << "Error initializing GLEW: " << glewGetErrorString(glew_err) << std::endl;
	}

    float pixels[] = {
        0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f
    };

    GLuint shader;
    compile_shader(shader);
    // Set texture uniform in shader
    glUseProgram(shader);
    int uniformloc = glGetUniformLocation(shader, "screen_texture");
    if (uniformloc == -1) {
        std::cout << "Error: Uniform \"screen_texture\" not found" << std::endl;
    }
    glUniform1i(uniformloc, 0);

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
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad), screen_quad, GL_STATIC_DRAW);
    // vertex positions
    glEnableVertexAttribArray(0);    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    // vertex texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    //glBindVertexArray(0);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glDisable(GL_DEPTH_TEST);
	
	SDL_Event event;
    while (true) {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) {
            break;
        }
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		// set texture data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);

        glDrawArrays(GL_TRIANGLES, 0, 6); 
        SDL_GL_SwapWindow(window);
    }

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

    SDL_GL_DeleteContext(glcontext);
    SDL_Quit();
        
    
    /*Processor gb_cpu;
    size_t rom_size;
    std::vector<u8> rom_data = read_rom(rom_size, "DMG_ROM.bin");

    gb_cpu.map_to_memory(rom_data.data(), rom_size, 0);
    gb_cpu.run();
    
    for (unsigned int i = 0; i < rom_data.size(); i++) {
        if (i != 0 && (i % 16 == 0))
            std::cout << std::endl;
        std::cout << std::setw(2) << std::setfill('0') << std::hex << (int)rom_data[i] << ' ';
    }
    std::cout << std::endl;
    
    gb_cpu.print_register_values();*/
    
    //SDL_DestroyWindow(window);
    

    return 0;
}
