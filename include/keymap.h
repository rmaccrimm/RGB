#ifndef KEYMAP_H
#define KEYMAP_H

#include <SDL2/SDL.h>
#include <map>
#include "joypad.h"

static const std::map<SDL_Keycode, int> key_map = {
    {SDLK_LEFT, Joypad::LEFT},
    {SDLK_RIGHT, Joypad::RIGHT},
    {SDLK_UP, Joypad::UP},
    {SDLK_DOWN,Joypad::DOWN},
    {SDLK_a, Joypad::A},
    {SDLK_b, Joypad::B},
    {SDLK_p, Joypad::NONE},
    {SDLK_RETURN, Joypad::START},
    {SDLK_BACKSPACE, Joypad::SELECT},
    {SDLK_1, Joypad::NONE},
    {SDLK_2, Joypad::NONE},
    {SDLK_3, Joypad::NONE},
    {SDLK_4, Joypad::NONE},
    {SDLK_5, Joypad::NONE},
    {SDLK_6, Joypad::NONE},
    {SDLK_7, Joypad::NONE},
    {SDLK_8, Joypad::NONE},
    {SDLK_9, Joypad::NONE},
    {SDLK_BACKQUOTE, Joypad::NONE}
};

#endif