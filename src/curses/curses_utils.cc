#include "curese_utils.h"

uint print_line(WINDOW *w, char *data, size_t max_char) {
  max_char = min(strlen(data), max_char);
  uint printed_char = vw_printw(stdscr, "%.*s", max_char, data);
  return printed_char;
}

void print_buffer(WINDOW *w, char *data, uint column, bool wordwrap) {
  // Move cursor at the beginning of the view
  wmove(w, 1, 0);
  // We will print nlines - prompt - fbar
  uint nb_line = this->_nlines - 2;
  // next_line is the read pointer
  uint next_line = 0;
  while (nb_line > 0) {
    print_line(buffer_model.get_text()[next_line], 
               wordwrap ? this->_ncols : nb_line * this->_ncols);
    uint line_size = strlen(get_text()[next_line]);
    // next_line pointer is after the following \0, which is strlen(line) 
    // characters after the current pointer
    next_line += line_size;
    nb_line -= (next_line / this->_ncols) + 1;
  }
  refresh();
}

