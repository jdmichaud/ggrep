
#include <curses.h>

#include <list>
#include <algorithm>

#include "buffer_model.h"
#include "utils.h"
#include "range.h"

uint print_line(WINDOW *w, const char *data, size_t max_char, uint xoffset) {
  // Get the maximum character we can print
  max_char = std::min(strlen(data), max_char);
  // is \r the last character of the line (\n was stripped out by getline)
  uint cr = data[max_char - 1] == '\r' ? 1 : 0;
  // print the string
  uint printed_char = wprintw(stdscr, "%.*s", max_char - cr, data);
  // Clear the rest of the line
  wclrtoeol(stdscr);
  // Return the number of printed char
  return printed_char;
}

template <typename range_type>
void print_buffer(WINDOW *w, char * const *buffer, const range_type& r,
                  uint first_column, uint lines, uint columns,
                  uint xoffset, bool wordwrap) {
  LOGFN();
  // We will print nlines - prompt - fbar
  uint nb_line = lines - 2; // TODO: don't assume two dead lines!
  // position on the screen
  uint screen_line = 0;
  for (int i : r) {
    // Get the line to print
    const char *line = buffer[i];
    // If the line is NULL, just move to the next line
    if (line != nullptr) {
      // Advance by the columns shift
      line += first_column;
      // How much to print ?
      uint char_to_print = strlen(line);
      // As long as the line is not completely printed
      uint printed_char = 0;
      do {
        // Move cursor at the beginning of the line
        wmove(w, screen_line + 1, xoffset); // TODO: don't assume one line header
        // Print the line
        printed_char += print_line(w, line, columns - xoffset, xoffset);
        // Move the line header to the rest of the string
        line += printed_char;
        // Decrement the number of lines remaining on the screen
        nb_line--;
        // We will stop printing if we don't wordwrap, or we printed the full line
        // or there is no more room on the screen.
      } while (nb_line > 0 && wordwrap && printed_char < char_to_print);
    }
    // Move to next line in read buffer
    screen_line++;
    // If we reach the end of the screen, break
    if (!nb_line) break;
  }
  // Clear the rest of the lines
  int x, y;
  getyx(stdscr, y, x);
  while (nb_line) {
    // TODO: don't assume one line header
    wmove(stdscr, lines - 1 - nb_line, xoffset);
    wclrtoeol(stdscr);
    nb_line--;
  }
}

template void print_buffer< std::list<uint> >
                  (WINDOW *w, char * const *buffer, const std::list<uint>& r,
                  uint first_column, uint lines, uint columns,
                  uint xoffset, bool wordwrap);
template void print_buffer< range >
                  (WINDOW *w, char * const *buffer, const range& r,
                  uint first_column, uint lines, uint columns,
                  uint xoffset, bool wordwrap);
