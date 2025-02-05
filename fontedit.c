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

void draw_screen_border(void);
void draw_screen_border(void)
{
	print_string_attr_at(0, 0, "/------------------------------\\", INK_WHITE|PAPER_BLACK);
	for (int row = 1; row<23; row++) {
		// print_string_at(row, 0, "|                              |");
		print_character_attr_at(row, 0, '|', INK_WHITE|PAPER_BLACK);
		print_character_attr_at(row, 31, '|', INK_WHITE|PAPER_BLACK);
	}
	print_string_attr_at(23, 0, "\\------------------------------/", INK_WHITE|PAPER_BLACK);
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
	print_string_attr_at(row, 1, "                              ", INK_WHITE|PAPER_BLACK);
}

void draw_characters(void);
void draw_characters(void)
{
	rowcol location;

	draw_character_row(12);
	draw_character_row(14);
	draw_character_row(16);
	draw_character_row(18);

	for (char character=' '; character <= 0x7F; character++) {
		location = character_location[character - ' '];
		if (location.row != 0) {
			print_character_at(location.row, location.col, character);
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

// -- ---------------------------------------------------------------------- --

int main(void);
int main(void) {
	int x=128, y=81;
	uword s = 0;
	int num_colours = 8;
	static const ubyte colours[8] = {INK_BLACK, INK_BLUE, INK_RED, INK_MAGENTA, INK_GREEN, INK_CYAN, INK_YELLOW, INK_WHITE};

	zx_border(INK_BLACK);

	draw_main_screen();

	ubyte character = 0;
	ubyte last_character = 0;
	do {  // edit loop
		character = in_inkey();
		rowcol location;
		if (character >= ' ' && character <= 0x7F) {
			location = character_location[character-32];
			print_character_attr_at(location.row+1, location.col, character, INK_BLACK|PAPER_WHITE|FLASH);
			last_character = character;
		}
		else if (character == 0x85) {
			// special case: (c)
			character = 0x7F;
			location = character_location[character-32];
			print_character_attr_at(location.row+1, location.col, character, INK_BLACK|PAPER_WHITE|FLASH);
			last_character = character;
		}
		else if (last_character != 0) {
			location = character_location[last_character-32];
			print_character_attr_at(location.row+1, location.col, last_character, INK_BLACK|PAPER_WHITE);
			printf_at(22, 15, "last: 0x%02x ", last_character);
		}
	} while (character != 0x83);  // loop until break by SYMB+Q

	return(0);
}

// -- ---------------------------------------------------------------------- --
