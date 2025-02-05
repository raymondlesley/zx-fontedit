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
#include "sysvars.h"

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

//static const uword *sys_chars = (uword *)0x5C36;  // 'chars' sytem variable

///
/// get_attr_address
///
/// get attribute memory location of specified point in character positions
///
/// @param x_chars x (column) coordinate in character positions
/// @param y_chars y (row)  coordinate in character positions
/// @return memory address of attribute byte
///
const char *get_attr_address(ubyte x_chars, ubyte y_chars)
{
    return zx_cxy2aaddr(x_chars, y_chars);
}

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
/// print_character_attr_at
///
/// output a single character at the specified screen location
/// setting screen cell attributes
///
/// @param row (0..23)
/// @param col column (0..31)
/// @param ubyte attributes
/// @param char character to print
///
void print_character_attr_at(ubyte row, ubyte col, ubyte attributes, char character)
{
    print_character_at(row, col, character);

	char *attr_location = (char *)get_attr_address(col, row);
    *attr_location = attributes;
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
/// print_string_attr_at
///
/// output string at specified screen location
/// setting screen cell attributes
///
/// @param row (0..23)
/// @param col column (0..31)
/// @param ubyte attributes
/// @param text pointer to array of (null-terminated) characters to print
///
void print_string_attr_at(ubyte row, ubyte col, ubyte attributes, char *text)
{
	const char *chars = (char *)*(sys_chars); // default = 0x3C00

	char character = *(text++);
	uword character_offset = character << 3;  // 8* as each char takes 8 bytes

	char *screen_location = (char *)get_char_address(col, row);
    ubyte *attr_location = (ubyte *)get_attr_address(col, row);
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
		*attr_location = attributes;
		character = *(text++);  // next character
		character_offset = character << 3;
		character_location = (char *)(chars + character_offset);
		screen_location++;  // step to next character position
		attr_location++;
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
/// printf_attr_at
///
/// output formatted string at specified screen location
/// setting screen cell attributes
///
/// @param row (0..23)
/// @param col column (0..31)
/// @param ubyte attributes
/// @param fmt pointer to array of (null-terminated) format string
///
void printf_attr_at(ubyte row, ubyte col, ubyte attributes, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	char buffer[256];

	(void)vsprintf(buffer, fmt, args);
	print_string_attr_at(row, col, attributes, buffer);

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

///
/// draw_line
///
/// set a point on screen at x, y coordinates
///
/// @param x1 - left coord of start point
/// @param y1 - top coord of start point
/// @param x2 - left coord of end point
/// @param y2 - top coord of end point
///
void draw_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2)
{
    int x = x1;
    int y = y1;
    int x_gap = x2 - x1;
    int y_gap = y2 - y1;
    int x_dir = x2 > x1 ? 1 : -1;
    int y_dir = y2 > y1 ? 1 : -1;

    if (abs(x_gap) > abs(y_gap)) {
        // work x-wise
        while (x <= x2) {
            plot_xy(x, y);
            // next y point
            x += x_dir;
            // gradient = y_gap/x_gap
            y = y1 + y_gap * (x - x1) / x_gap;
        }
    }
    else {
        // work y-wise
        while (y <= y2) {
            plot_xy(x, y);
            // next y point
            y += y_dir;
            // gradient = x_gap/y_gap
            x = x1 + x_gap * (y-y1) / y_gap;
        }
    }
}

// -- ---------------------------------------------------------------------- --
