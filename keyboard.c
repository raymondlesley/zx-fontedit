// -- ---------------------------------------------------------------------- --
// zx_lib - C library for z88dk zx sdcc_iy
//
// keyboard - key mapping
//
// -- ---------------------------------------------------------------------- --

#include "keyboard.h"

#include "sysvars.h"

void set_keyboard_repeat_delay(ubyte delay)
{
    *sys_repdel = delay;
}

void set_keyboard_repeat(ubyte rate)
{
    *sys_repper = rate;
}
