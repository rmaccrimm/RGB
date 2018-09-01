#include "joypad.h"
#include "functions.h"
#include <iostream>

Joypad::Joypad() : state(0xff) {}

u8 Joypad::get_state(bool select_dpad)
{
    if (select_dpad) {
        return (2 << 4) | (state & 0xf);
    }
    else {
        return (1 << 4) | ((state >> 4) & 0xf);
    }
}

void Joypad::press_key(int key) 
{     
    std::cout << "Pressed key " << key << std::endl;
    state = utils::reset(state, key); 
}

void Joypad::release_key(int key) 
{ 
    std::cout << "Released key " << key << std::endl;
    state = utils::set(state, key); 
}
