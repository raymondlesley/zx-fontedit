// -- ---------------------------------------------------------------------- --
// zx_lib - C library for z88dk zx sdcc_iy
//
// keyboard - key mapping
//
// -- ---------------------------------------------------------------------- --

#include "types.h"

#define INKEY_a  0x00

#define INKEY_LEFT          0x08
#define INKEY_DOWN          0x0A
#define INKEY_UP            0x0B
#define INKEY_RIGHT         0x09

#define INKEY_DELETE        0x0C
#define INKEY_ENTER         0x0D

#define INKEY_SYMB_E        0x85
#define INKEY_SYMB_S        0x7C
#define INKEY_SYMB_Q        0x83
#define INKEY_SYMB_W        0x84

void set_keyboard_repeat_delay(ubyte delay);
void set_keyboard_repeat(ubyte rate);

///
/// input_string
///
/// get string input from keyboard
///
/// @param row start position of text on screen
/// @param col start position of text on screen
/// @param buffer (char *)buffer to hold input and returned (null-terminated) string - should be max_length+1 in size
/// @param max_length maximum number of characters to input
///
void input_string(ubyte row, ubyte col, char *buffer, int max_length);
