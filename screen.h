// -- ---------------------------------------------------------------------- --
// zx_lib - C library for z88dk zx sdcc_iy
//
// screen - screen output routines
//
// -- ---------------------------------------------------------------------- --

#include "types.h"

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

///
/// get_attr_address
///
/// get attribute memory location of specified point in character positions
///
/// @param x_chars x (column) coordinate in character positions
/// @param y_chars y (row)  coordinate in character positions
/// @return memory address of attribute byte
///
const char *get_attr_address(ubyte x_chars, ubyte y_chars);  // declaration

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

///
/// print_character_attr_at
///
/// output a single character at the specified screen location
/// setting screen cell attributes
///
/// @param row (0..23)
/// @param col column (0..31)
/// @param char character to print
/// @param ubyte attributes
///
void print_character_attr_at(ubyte row, ubyte col, char character, ubyte attributes);

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

///
/// print_string_attr_at
///
/// output string at specified screen location
/// setting screen cell attributes
///
/// @param row (0..23)
/// @param col column (0..31)
/// @param text pointer to array of (null-terminated) characters to print
/// @param ubyte attributes
///
void print_string_attr_at(ubyte row, ubyte col, char *text, ubyte attributes);

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

///
/// printf_attr_at
///
/// output formatted string at specified screen location
/// setting screen cell attributes
///
/// @param row (0..23)
/// @param col column (0..31)
/// @param fmt pointer to array of (null-terminated) format string
///
void printf_attr_at(ubyte row, ubyte col, ubyte attributes, char *fmt, ...);

///
/// plot
///
/// set a point on screen at x, y coordinates
///
/// @param x
/// @param y
///
void plot_xy(unsigned char x, unsigned char y);

