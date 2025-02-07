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
#include "keyboard.h"

// -- ---------------------------------------------------------------------- --
// constants

#define EDIT_PANEL_TOP 1
#define EDIT_PANEL_LEFT 11
#define EDIT_PANEL_WIDTH 10
#define EDIT_PANEL_HEIGHT 10
#define PREVIEW_PANEL_LEFT 26
#define PREVIEW_PANEL_TOP 7
#define PREVIEW_PANEL_WIDTH 3
#define PREVIEW_PANEL_HEIGHT 3
#define PREVIEW_PANEL_X 216  // (26+1)*8 = x coord of top left
#define PREVIEW_PANEL_Y 64   // (7+1)*8  = y coord of top left

#define NOTE_PANEL_TOP 20
#define NOTE_PANEL_LEFT 0

#define USER_FONT_AREA (ubyte *)0xF000
#define FONT_SIZE 768
#define FONT_OFFSET  256  // 32 * 8 = ASCII 00..31

// -- ---------------------------------------------------------------------- --

struct rowcol_t { ubyte row; ubyte col; };
typedef struct rowcol_t rowcol;

// on-screen location for each character - starting with ' '

rowcol character_location[] = {
  {16, 14}, {16, 15}, {16, 16}, {16, 17}, {16, 18}, {16, 19},  //  !"#$%
  {16, 20}, {16, 21}, {16, 22}, {16, 23}, {16, 24}, {16, 25},  // &'()*+
  {16, 26}, {16, 27}, {16, 28}, {16, 29},                      // ,-./
  {16, 2},  {16, 3},  {16, 4},  {16, 5},  {16, 6},  {16, 7},   // 012345
  {16, 8},  {16, 9},  {16, 10}, {16, 11},                      // 6789
  {18, 2},  {18, 3},  {18, 4},  {18, 5},  {18, 6},  {18, 7},   // :;<=>?
  {18, 8},  {12, 2},  {12, 3},  {12, 4},  {12, 5},  {12, 6},   // @ABCDE
  {12, 7},  {12, 8},  {12, 9},  {12, 10}, {12, 11}, {12, 12},  // FGHIJK
  {12, 13}, {12, 14}, {12, 15}, {12, 16}, {12, 17}, {12, 18},  // LMNOPQ
  {12, 19}, {12, 20}, {12, 21}, {12, 22}, {12, 23}, {12, 24},  // RSTUVW
  {12, 25}, {12, 26}, {12, 27},                                // XYZ
  {18, 10}, {18, 11}, {18, 12}, {18, 13}, {18, 14}, {18, 15},  // [\]^_£
  {14, 2},  {14, 3},  {14, 4},  {14, 5},  {14, 6},  {14, 7},   // abcdef
  {14, 8},  {14, 9},  {14, 10}, {14, 11}, {14, 12}, {14, 13},  // ghijkl
  {14, 14}, {14, 15}, {14, 16}, {14, 17}, {14, 18}, {14, 19},  // mnopqr
  {14, 20}, {14, 21}, {14, 22}, {14, 23}, {14, 24}, {14, 25},  // stuvwx
  {14, 26}, {14, 27},                                          // yz
  {18, 17}, {18, 18}, {18, 19}, {18, 20}, {18, 21}             // {|}~©
};

// -- ---------------------------------------------------------------------- --
// font routines

void copy_font(ubyte *from, ubyte *to);
void copy_font(ubyte *from, ubyte *to)
{
	// NB: actual font data starts at ' ' character - i.e. 32*8 bytes on
	to += FONT_OFFSET;
	from += FONT_OFFSET;

	for (uword i = 0; i < FONT_SIZE; i++) {
		*to++ = *from++;
	}
}

ubyte *user_font = SYS_CHARS_DEFAULT;

void set_font(uword *font_base);
void set_font(uword *font_base)
{
	*sys_chars = font_base;
}

void setup_font(void);
void setup_font(void)
{
	user_font = (ubyte *)*sys_chars;

	// check if current font (CHARS) = default, or already using user font
	if (user_font == SYS_CHARS_DEFAULT) {
		// copy system font into user font
		copy_font(SYS_CHARS_DEFAULT, USER_FONT_AREA);

		// set user font
		user_font = USER_FONT_AREA;
		set_font(user_font);
	}
	// else already using user font
}


// -- ---------------------------------------------------------------------- --
// screen routines

void draw_screen_border(void);
void draw_screen_border(void)
{
	print_string_attr_at(0, 0, INK_WHITE|PAPER_BLACK, "/------------------------------\\");
	for (int row = 1; row<23; row++) {
		// print_string_at(row, 0, "|                              |");
		print_character_attr_at(row, 0, INK_WHITE|PAPER_BLACK, '|');
		print_character_attr_at(row, 31, INK_WHITE|PAPER_BLACK, '|');
	}
	print_string_attr_at(23, 0, INK_WHITE|PAPER_BLACK, "\\------------------------------/");
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

void draw_preview_panel(ubyte left, ubyte top);
void draw_preview_panel(ubyte left, ubyte top)
{
	print_string_at(top, left, "+-+");
	uword bottom = top + 2;
	uword right = left + 2;
	for (ubyte row = top + 1; row < bottom; row++) {
		print_character_at(row, left, '|');
		print_character_at(row, right, '|');
	}
	print_string_at(bottom, left, "+-+");
}

void draw_edit_to_preview_lines(void);
void draw_edit_to_preview_lines(void)
{
	// draw edit panel top right to preview panel top left
	ubyte line1_startx = (EDIT_PANEL_LEFT + EDIT_PANEL_WIDTH) * 8;
	ubyte line1_starty = (EDIT_PANEL_TOP + 1) * 8;
	ubyte line1_endx = PREVIEW_PANEL_LEFT * 8;
	ubyte line1_endy = PREVIEW_PANEL_TOP * 8;

	draw_line(line1_startx, line1_starty, line1_endx, line1_endy);

	// draw edit window bottom right to preview panel bottom left
	ubyte line2_startx = (EDIT_PANEL_LEFT + EDIT_PANEL_WIDTH) * 8;
	ubyte line2_starty = (EDIT_PANEL_TOP + EDIT_PANEL_HEIGHT) * 8 - 4;
	ubyte line2_endx = PREVIEW_PANEL_LEFT * 8;
	ubyte line2_endy = (PREVIEW_PANEL_TOP + PREVIEW_PANEL_HEIGHT) * 8 - 4;

	draw_line(line2_startx, line2_starty, line2_endx, line2_endy);
}

void draw_divider(ubyte row);
void draw_divider(ubyte row)
{
	print_string_at(row, 0, "+------------------------------+");
}

void draw_character_row(ubyte row);
void draw_character_row(ubyte row)
{
	print_string_attr_at(row, 1, INK_WHITE|PAPER_BLACK, "                              ");
}

void draw_characters(void);
void draw_characters(void)
{
	rowcol location;

	draw_character_row(12);
	draw_character_row(14);
	draw_character_row(16);
	draw_character_row(18);

	set_font(SYS_CHARS_DEFAULT);  // start with ROM font
	for (char character=' '; character <= 0x7F; character++) {
		location = character_location[character - ' '];
		if (location.row != 0) {
			print_character_at(location.row, location.col, character);
		}
	}

	set_font(user_font);  // switch to user font
	for (char character=' '; character <= 0x7F; character++) {
		location = character_location[character - ' '];
		if (location.row != 0) {
			print_character_at(location.row+1, location.col, character);
		}
	}
}

void draw_main_screen(void);
void draw_main_screen(void)
{
	zx_cls(INK_BLACK | PAPER_WHITE);
	draw_screen_border();
	draw_edit_panel(EDIT_PANEL_LEFT, EDIT_PANEL_TOP);
	draw_preview_panel(PREVIEW_PANEL_LEFT, PREVIEW_PANEL_TOP);
	draw_edit_to_preview_lines();
	draw_divider(11);
	draw_characters();
	draw_divider(20);
}

void edit_character(ubyte character);
void edit_character(ubyte character)
{
	// First: show character in preview panel
	print_character_at(PREVIEW_PANEL_TOP+1, PREVIEW_PANEL_LEFT+1, character);

	// Second: load bitmap into edit panel
	const ubyte *chars = user_font;  // (char *)*(sys_chars); // default = 0x3C00
	const uword character_offset = character << 3;  // 8* as each char takes 8 bytes
	ubyte *character_location = (ubyte *)(chars + character_offset);
	printf_at(4, 1, "0x%04x", chars);
	printf_at(5, 1, "0x%04x", character_location);

	ubyte bitmap[8];
	ubyte bitmask;

	for (ubyte row = 0; row < 8; row++) {
		// get pixel row
		// output pixels
		bitmap[row] = *character_location;

		bitmask = 0x80;  // start with leftmost bit
		for (ubyte column = 0; column < 8; column++) {
			if (bitmap[row] & bitmask) {
				// bit set
				set_attr_at(row + EDIT_PANEL_TOP + 1, column + EDIT_PANEL_LEFT + 1, INK_WHITE|PAPER_BLACK);
			}
			else {
				// bit reset
				set_attr_at(row + EDIT_PANEL_TOP + 1, column + EDIT_PANEL_LEFT + 1, INK_BLACK|PAPER_WHITE);
			}

			// next pixel
			bitmask >>= 1;
		}

		// next row
		character_location++;
	}

	// Third: show edit mode commands in lower panel
	print_string_at(NOTE_PANEL_TOP+1, NOTE_PANEL_LEFT+1, "5678=move;    space=flip   ");
	print_string_at(NOTE_PANEL_TOP+2, NOTE_PANEL_LEFT+1, "SYMB+W=write; SYMB+Q=cancel");
	ubyte x = 0;
	ubyte y = 0;
	bitmask = 0x80;
	print_character_at(EDIT_PANEL_TOP+y+1, EDIT_PANEL_LEFT+x+1, '+');

	// Fourth: start edit loop
	while (in_inkey() != 0);  // wait for key up
	ubyte keypress = 0;
	do {  // edit loop
		do {
			keypress = in_inkey();
		} while (keypress == 0);
		print_character_at(EDIT_PANEL_TOP+y+1, EDIT_PANEL_LEFT+x+1, ' ');
		if (keypress == '5' || keypress == INKEY_LEFT) {
			// left
			if (x > 0)  x--;
			bitmask <<= 1;
		}
		else if (keypress == '6' || keypress == INKEY_DOWN) {
			// down
			if (y < 7)  y++;
		}
		else if (keypress == '7' || keypress == INKEY_UP) {
			// up
			if (y > 0)  y--;
		}
		else if (keypress == '8' || keypress == INKEY_RIGHT) {
			// right
			if (x < 7)  x++;
			bitmask >>= 1;
		}
		else if (keypress == ' ') {
			// flip bit
			if (bitmap[y] & bitmask) {
				// bit set: reset it
				set_attr_at(y + EDIT_PANEL_TOP + 1, x + EDIT_PANEL_LEFT + 1, INK_BLACK|PAPER_WHITE);
				bitmap[y] &= ~bitmask;
				unplot_xy(PREVIEW_PANEL_X+x, PREVIEW_PANEL_Y+y);
			}
			else {
				// bit reset: set it
				set_attr_at(y + EDIT_PANEL_TOP + 1, x + EDIT_PANEL_LEFT + 1, INK_WHITE|PAPER_BLACK);
				bitmap[y] |= bitmask;
				plot_xy(PREVIEW_PANEL_X+x, PREVIEW_PANEL_Y+y);
			}
		}
		else if (keypress == INKEY_SYMB_W) {
			// save
			character_location = (ubyte *)(chars + character_offset);
			printf_at(6, 1, "0x%04x", chars);
			printf_at(7, 1, "0x%04x", character_location);

			for (ubyte row = 0; row < 8; row++) {
				// get pixel row
				// output pixels
				*character_location++ = bitmap[row];
			}
			break;
		}
		print_character_at(EDIT_PANEL_TOP+y+1, EDIT_PANEL_LEFT+x+1, '+');
		while (in_inkey() != 0);  // wait for key up
	} while (keypress != INKEY_SYMB_Q);  // loop until break by SYMB+Q

	// Last: tidy up
	for (ubyte row = 0; row < 8; row++) {
		for (ubyte column = 0; column < 8; column++) {
			set_attr_at(row + EDIT_PANEL_TOP + 1, column + EDIT_PANEL_LEFT + 1, INK_BLACK|PAPER_WHITE);
		}
	}
	print_character_at(EDIT_PANEL_TOP+y+1, EDIT_PANEL_LEFT+x+1, ' ');  // cursor
	print_character_at(PREVIEW_PANEL_TOP+1, PREVIEW_PANEL_LEFT+1, ' ');  // preview
	print_string_at(NOTE_PANEL_TOP+1, NOTE_PANEL_LEFT+1, "                            ");  // instructions
	print_string_at(NOTE_PANEL_TOP+2, NOTE_PANEL_LEFT+1, "                            ");  // instructions
}

// -- ---------------------------------------------------------------------- --

int main(void);
int main(void) {
	//int x=128, y=81;
	//uword s = 0;
	//int num_colours = 8;
	//static const ubyte colours[8] = {INK_BLACK, INK_BLUE, INK_RED, INK_MAGENTA, INK_GREEN, INK_CYAN, INK_YELLOW, INK_WHITE};

	zx_border(INK_BLACK);
	setup_font();
	set_font(SYS_CHARS_DEFAULT);
	draw_main_screen();
	set_font(user_font);

	ubyte character = 0;
	ubyte last_character = 0;
	do {  // edit loop
		// print instructions
		print_string_at(NOTE_PANEL_TOP+1, NOTE_PANEL_LEFT+1, "Hit char to edit SYMB+E=\x7F   ");
		print_string_at(NOTE_PANEL_TOP+2, NOTE_PANEL_LEFT+1, "                 SYMB+W=menu");

		do {
			character = in_inkey();
		} while (character == 0);
		printf_at(2, 1, "char:0x%02x ", character);
		rowcol location;
		if (character >= ' ' && character <= 0x7F) {
			location = character_location[character-32];
			print_character_attr_at(location.row+1, location.col, INK_BLACK|PAPER_WHITE|FLASH, character);
			last_character = character;
			edit_character(character);
		}
		else if (character == INKEY_SYMB_E) {
			// special case: SYMB+E = (c)
			character = 0x7F;
			location = character_location[character-32];
			print_character_attr_at(location.row+1, location.col, INK_BLACK|PAPER_WHITE|FLASH, character);
			last_character = character;
			edit_character(character);
		}
		if (last_character != 0) {
			location = character_location[last_character-32];
			print_character_attr_at(location.row+1, location.col, INK_BLACK|PAPER_WHITE, last_character);
			printf_at(3, 1, "last:0x%02x ", last_character);
		}
	} while (character != INKEY_SYMB_Q);  // loop until break by SYMB+Q

	return(0);
}

// -- ---------------------------------------------------------------------- --
