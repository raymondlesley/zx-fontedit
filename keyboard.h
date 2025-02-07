// -- ---------------------------------------------------------------------- --
// zx_lib - C library for z88dk zx sdcc_iy
//
// keyboard - key mapping
//
// -- ---------------------------------------------------------------------- --

#define INKEY_a  0x00

#define INKEY_LEFT          0x08
#define INKEY_DOWN          0x0A
#define INKEY_UP            0x0B
#define INKEY_RIGHT         0x09

#define INKEY_SYMB_E        0x85
#define INKEY_SYMB_S        0x7C
#define INKEY_SYMB_Q        0x83
#define INKEY_SYMB_W        0x84

void set_keyboard_repeat_delay(ubyte delay;)
void set_keyboard_repeat(ubyte rate);
