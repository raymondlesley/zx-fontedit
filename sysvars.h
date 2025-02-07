// -- ---------------------------------------------------------------------- --
// zx_lib - C library for z88dk zx sdcc_iy
//
// sysvars - definition of system variables
//
// -- ---------------------------------------------------------------------- --

#include "types.h"

extern ubyte *sys_repdel; // 'REPDEL' system variable - keyboard repeat delay
extern ubyte *sys_repper; // 'REPPER' system variable - keyboard repeat rate
extern uword *sys_chars;  // 'chars' sytem variable - pointer to font
#define SYS_CHARS_DEFAULT  (ubyte *)0x3C00
