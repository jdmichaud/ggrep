/*! \brief Utilitary function for curses
 *
 */

#ifndef __CURSES_H__
#define __CURSES_H__

#include <memory>
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
 * buffer_model contains the list of string to display
 * r is a range which contains the index of the line we want to print
 * first_column in the index of the column to be displayed first
 * lines in the number of lines in available for display
 * columns in the number of columns in available for display
 * xoffset is where the print function shall start printing on the screen
 * if wordwrap is true, string will be displayed entirely on several lines.
 */
template <typename range_type>
void print_buffer(WINDOW *w, char * const *buffer, const range_type& r,
                  uint first_column, uint lines, uint columns,
                  uint xoffset, bool wordwrap);

/*! \brief overprint the text with its attributes to the window
 *
 * attr_list contains the list of attributes to print
 * r is a range which contains the index of the line we want to print
 * first_column in the index of the column to be displayed first
 * lines in the number of lines in available for display
 * columns in the number of columns in available for display
 * xoffset is where the print function shall start printing on the screen
 * if wordwrap is true, string will be displayed entirely on several lines.
 */
template <typename range_type>
void print_attrs(WINDOW *w, attr_list_t attr_list, char * const *buffer,
                 const range_type& r, uint first_column, uint lines,
                 uint columns, uint xoffset, bool wordwrap);


#endif // __CURSES_H__