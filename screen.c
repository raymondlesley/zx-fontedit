// -- ---------------------------------------------------------------------- --
// zx_lib - C library for z88dk zx sdcc_iy
//
// screen - screen output routines
//
// -- ---------------------------------------------------------------------- --

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <arch/zx.h>

#include "screen.h"

///
/// get_char_address
///
/// get screen memory location of specified point in character positions
///
/// @param x_chars x (column) coordinate in character positions
/// @param y_chars y (row)  coordinate in character positions
/// @return memory address of character top-left pixel
///
const char *get_char_address(ubyte x_chars, ubyte y_chars)   // definition
{
	return zx_cxy2saddr(x_chars, y_chars);
}

static const uword *sys_chars = (uword *)0x5C36;  // 'chars' sytem variable

///
/// print_character_at
///
/// output a single character at the specified screen location
///
/// @param row (0..23)
/// @param col column (0..31)
/// @param char character to print
///
void print_character_at(ubyte row, ubyte col, char character)
{
	const char *chars = (char *)*(sys_chars); // default = 0x3C00
	const uword character_offset = character << 3;  // 8* as each char takes 8 bytes

	char *screen_location = (char *)get_char_address(col, row);
	char *character_location = (char *)(chars + character_offset);

	for (int counter = 0; counter < 8; counter++) {
		// copy byte to screen memory
		*screen_location = *character_location;
		// and move to next pixel row
		screen_location += 0x0100;
		character_location++;
	}
}

///
/// print_string_at
///
/// output string at specified screen location
///
/// @param row (0..23)
/// @param col column (0..31)
/// @param text pointer to array of (null-terminated) characters to print
///
void print_string_at(ubyte row, ubyte col, char *text)
{
	const char *chars = (char *)*(sys_chars); // default = 0x3C00

	char character = *(text++);
	uword character_offset = character << 3;  // 8* as each char takes 8 bytes

	char *screen_location = (char *)get_char_address(col, row);
	char *character_location = (char *)(chars + character_offset);

	do {
		char *pixel_row = screen_location;  // start at the top left
		for (int counter = 0; counter < 8; counter++) {
			// copy byte to screen memory
			*pixel_row = *character_location;
			// and move to next pixel row
			pixel_row += 0x0100;
			character_location++;  // next row of pixels
		}
		character = *(text++);  // next character
		character_offset = character << 3;
		character_location = (char *)(chars + character_offset);
		screen_location++;  // step to next character position
	} while (character);  // assume null-terminated
}

///
/// printf_at
///
/// output formatted string at specified screen location
///
/// @param row (0..23)
/// @param col column (0..31)
/// @param fmt pointer to array of (null-terminated) format string
///
void printf_at(ubyte row, ubyte col, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	char buffer[256];

	(void)vsprintf(buffer, fmt, args);
	print_string_at(row, col, buffer);

	va_end(args);
}

///
/// plot
///
/// set a point on screen at x, y coordinates
///
/// @param x
/// @param y
///
void plot_xy(unsigned char x, unsigned char y)
{
    *zx_pxy2saddr(x,y) |= zx_px2bitmask(x);
}

// -- ---------------------------------------------------------------------- --
