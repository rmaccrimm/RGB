#include "joypad.h"
#include "functions.h"

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

void Joypad::press_key(int key) { state = utils::reset(state, key); }

void Joypad::release_key(int key) { state = utils::set(state, key); }
