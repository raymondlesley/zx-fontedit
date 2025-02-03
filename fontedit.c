// -- ---------------------------------------------------------------------- --
// ZX FontEdit - ZX Spectrum application for font and UDG editing
//
// written to be cross-compiled using z88dk sdcc_iy
//
// -- ---------------------------------------------------------------------- --

#include <stdio.h>
#include <stdlib.h>

#include <arch/zx.h>
#include <input.h>

#include "types.h"
#include "sysvars.h"
#include "screen.h"

/*

///
/// get_char_address
///
/// get screen memory location of specified point in character positions
///
/// @param x_chars x (column) coordinate in character positions
/// @param y_chars y (row)  coordinate in character positions
/// @return memory address of character top-left pixel
///
const char *get_char_address(ubyte x_chars, ubyte y_chars);  // declaration
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
void print_character_at(ubyte row, ubyte col, char character);
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
void print_string_at(ubyte row, ubyte col, char *text);
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
void printf_at(ubyte row, ubyte col, char *fmt, ...);
void printf_at(ubyte row, ubyte col, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	char buffer[256];

	(void)vsprintf(buffer, fmt, args);
	print_string_at(row, col, buffer);

	va_end(args);
}

void plot(unsigned char x, unsigned char y) {
    *zx_pxy2saddr(x,y) |= zx_px2bitmask(x);
}

*/

// -- ---------------------------------------------------------------------- --

void draw_screen_border(void);
void draw_screen_border(void)
{
	print_string_at(0, 0, "/------------------------------\\");
	for (int row = 1; row<23; row++) {
		// print_string_at(row, 0, "|                              |");
		print_character_at(row, 0, '|');
		print_character_at(row, 31, '|');
	}
	print_string_at(23, 0, "\\------------------------------/");
}

void draw_edit_panel(ubyte left, ubyte top);
void draw_edit_panel(ubyte left, ubyte top)
{
	print_string_at(top, left, "+--------+");
	uword bottom = top + 9;
	uword right = left + 9;
	for (ubyte row = top + 1; row < bottom; row++) {
		print_character_at(row, left, '|');
		print_character_at(row, right, '|');
	}
	print_string_at(bottom, left, "+--------+");
}

// -- ---------------------------------------------------------------------- --

int main(void);
int main(void) {
	int x=128, y=81;
	uword s = 0;
	// char buffer[10];
	int num_colours = 8;
	static const ubyte colours[8] = {INK_BLACK, INK_BLUE, INK_RED, INK_MAGENTA, INK_GREEN, INK_CYAN, INK_YELLOW, INK_WHITE};

	// myfunc(0, buffer);

	zx_cls(INK_BLACK | PAPER_WHITE);
	zx_border(INK_BLACK);

	draw_screen_border();
	draw_edit_panel(1, 3);


	s = 0;
	while (s < 65535) {
		s++;
		if (x>0 && x<256 && y>0 && y<192) {
			plot_xy(x, y);
		}
		switch (rand() / 8192) {  // 0, 1, 2 or 3
			case 0: x++; break;
			case 1: x--; break;
			case 2: y++; break;
			case 3: y--; break;
		}
		if (1) { // }((s % 1000) == 0) {
			printf_at(21, 2, "%u ", s);
		}
		if (in_test_key()) break;
	}

	return(0);
}

// -- ---------------------------------------------------------------------- --
