// -- ---------------------------------------------------------------------- --
// zx_lib - C library for z88dk zx sdcc_iy
//
// sysvars - definition of system variables
//
// -- ---------------------------------------------------------------------- --

#include "sysvars.h"

extern ubyte *sys_repdel = (ubyte *)0x5C09;  // 'REPDEL' system variable - keyboard repeat delay
extern ubyte *sys_repper = (ubyte *)0x5C0A;  // 'REPPER' system variable - keyboard repeat rate
extern uword *sys_chars  = (uword *)0x5C36;  // 'chars' sytem variable - pointer to font
