#ifndef JOYPAD_H
#define JOYPAD_H

#include "definitions.h"
#include "registers.h"
#include "interrupts.h"

class Joypad 
{
public:
    Joypad();
    u8 get_state(bool select_dpad);
    void press_key(int key);
    void release_key(int key);
    
    const static int RIGHT  = 0;
    const static int LEFT   = 1;
    const static int UP     = 2;
    const static int DOWN   = 3;
    const static int A      = 4;
    const static int B      = 5;
    const static int SELECT = 6;
    const static int START  = 7;
    const static int NONE   = 8;

private:
    u8 state;
};

#endif