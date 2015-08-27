#include <curses.h>
#include <algorithm>
#include "buffer_model.h"
#include "utils.h"

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

void print_buffer(WINDOW *w, BufferModel &buffer_model,
                  uint first_line, uint first_column, uint lines, uint columns,
                  uint xoffset, bool wordwrap) {
  LOGFN();
  // We will print nlines - prompt - fbar
  uint nb_line = lines - 2; // TODO: don't assume two dead lines!
  // next_line is the read pointer
  uint next_line = first_line;
  // Get the number of lines in the buffer
  uint buffer_line_number = buffer_model.get_number_of_line();
  while (nb_line > 0 && next_line < buffer_line_number) {
    // Get the line to print
    const char *line = buffer_model.get_text()[next_line];
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
        wmove(w, next_line - first_line + 1, xoffset); // TODO: don't assume one line header
        // Print the line
        printed_char += print_line(w, line, columns - xoffset, xoffset);
        /// Move the line header to the rest of the string
        line += columns - first_line;
        // Decrement the number of lines remaining on the screen and in the buffer
        nb_line--;
        // We will stop printing if we don't wordwrap, or we printed the full line
        // or there is no more room on the screen.
      } while (wordwrap && printed_char < char_to_print
               && nb_line > 0);
    }
    // Move to next line in read buffer
    next_line++;
  }
  // Clear the rest of the lines
  int x, y;
  getyx(stdscr, y, x);
  while (nb_line--) {
    // TODO: don't assume one line header
    wmove(stdscr, lines - 1 - nb_line, x + xoffset);
    wclrtoeol(stdscr);
  }
}

