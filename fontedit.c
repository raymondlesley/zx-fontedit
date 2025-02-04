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

void draw_main_screen(void);
void draw_main_screen(void)
{
	zx_cls(INK_BLACK | PAPER_WHITE);
	draw_screen_border();
	draw_edit_panel(EDIT_PANEL_LEFT, EDIT_PANEL_TOP);
	draw_preview_panel(PREVIEW_PANEL_LEFT, PREVIEW_PANEL_TOP);
	draw_edit_to_preview_lines();
	draw_divider(11);
	draw_divider(20);
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

	zx_border(INK_BLACK);

	draw_main_screen();


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
		ubyte character = in_inkey();
		if (character) {
			printf_attr_at(22, 3, INK_BLACK|PAPER_WHITE|FLASH, "'%c', %02x ", character, character);
			if (character == ' ') break;
		}
		else {
			print_string_attr_at(22, 3, "        ", INK_BLACK|PAPER_WHITE);
		}
	}

	zx_cls(INK_BLACK | PAPER_WHITE);

	return(0);
}

// -- ---------------------------------------------------------------------- --
