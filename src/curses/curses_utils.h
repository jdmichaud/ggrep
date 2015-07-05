/*! \brief Utilitary function for curses
 *
 */

#ifndef __INPUT_H__
#define __INPUT_H__

#include <curses.h>

/*! \brief print a string to the window
 *
 * Print a string on the window where the cursor is positioned.
 * data represents a null terminated character string.
 * max_char is the maximum number of char to print.
 * The function returns the number of characters printed.
 */
uint print_line(WINDOW *w, char *data, size_t max_char);

/*! \brief print a buffer to the WINDOW
 *
 * Will repeatldy call print_line on all the lines up to _nline - 1
 * The lines will start at column
 * if wordwrap is true, string will be displayed entirely on several lines.
 */
void print_buffer(WINDOW *w, char *data, uint column, bool wordwrap);


#endif // __INPUT_H__