#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <Windows.h>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL.h>

#include "debug.h"
#include "definitions.h"
#include "register16bit.h"
#include "register8bit.h"
#include "processor.h"
#include "video.h"

#undef main

int main(int argc, char *argv[])
{  
    SDL_Window* window = init_window();

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

    GLuint shader;
    compile_shader(shader);

    // Set texture uniform in shader
    glUseProgram(shader);
    int uniformloc = glGetUniformLocation(shader, "screen_texture");
    if (uniformloc == -1) {
        std::cout << "Error: Uniform \"screen_texture\" not found" << std::endl;
    }
    glUniform1i(uniformloc, 0);

    init_screen_texture();

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

		float pixels[48];
		for (int i = 0; i < 48; i++) {
			pixels[i] = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 4, 4, 0, GL_RGB, GL_FLOAT, pixels);

        glDrawArrays(GL_TRIANGLES, 0, 6); 
        SDL_GL_SwapWindow(window);
		Sleep(100);
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
