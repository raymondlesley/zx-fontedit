// -- ---------------------------------------------------------------------- --
// zx_lib - C library for z88dk zx sdcc_iy
//
// keyboard - key mapping
//
// -- ---------------------------------------------------------------------- --

#include <arch/zx.h>
#include <input.h>
#include <sound.h>

#include "keyboard.h"

#include "screen.h"
#include "sysvars.h"

// -- ---------------------------------------------------------------------- --

#define COPYRIGHT 0x7F

// -- ---------------------------------------------------------------------- --

void set_keyboard_repeat_delay(ubyte delay)
{
    *sys_repdel = delay;
}

void set_keyboard_repeat(ubyte rate)
{
    *sys_repper = rate;
}

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
void input_string(ubyte row, ubyte col, char *buffer, int max_length)
{
    int length = 0;
    ubyte beep_length = *sys_rasp;
    static const ubyte beep_freq = 400;

    ubyte attr = get_row_col_attr(row, col);

    ubyte keypress = 0;
	do {  // text entry loop
        print_character_attr_at(row, col, attr|FLASH, ' ');
        while (in_inkey() != 0);  // wait for key up
		do {
			keypress = in_inkey();
		} while (keypress == 0);

        if (keypress == INKEY_ENTER) {
            // enter: null-terminate and return
            buffer[length] = 0;
        }
        else if (keypress == INKEY_DELETE && length > 0) {
            print_character_attr_at(row, col--, attr, ' ');
            length--;
        }
        else if (length == max_length) {
            // reached maximum - just beep
            bit_beep(beep_length, beep_freq);
        }
        else if (keypress >= ' ' && keypress <= COPYRIGHT) {
            // add character to end of string
            print_character_attr_at(row, col++, attr, keypress);
            buffer[length++] = keypress;
        }
        else {
            // unsupported character - just beep
            bit_beep(beep_length, beep_freq);
        }
    } while (keypress != INKEY_ENTER);

    print_character_attr_at(row, col, attr, ' ');
}
