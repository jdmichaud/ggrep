// http://hughm.cs.ukzn.ac.za/~murrellh/os/notes/ncurses.html#using
#include <sstream>
#include <algorithm>
#include <curses.h>
#include "logmacros.h"
#include "terminal_view.h"
#include "state.h"
#include "curses/utils.h"
#include "range.h"

TerminalView::TerminalView(BrowserModel &browser_model, FBarModel &fbar_model,
                           PromptModel &prompt_model, StateModel &state_model) :
  _nlines(0), _ncols(0),
  _browser_model(browser_model),
  _fbar_model(fbar_model),
  _prompt_model(prompt_model),
  _state_model(state_model),
  _prompt_string_index(0)
{
}

TerminalView::~TerminalView() {
  // Restore terminal settings
  endwin();
}

uint TerminalView::init() {
  // Initialize the ncurses library
  initscr();
  // Send character by character, do not wait for newline
  cbreak();
  // In halfdelay mode, getch will return after n tenth of a second
  // This is important for the input thread to regularly check if the controller
  // is interrupted
  halfdelay(1);
  // Disable handling of the KEY_ENTER by the terminal
  //raw(); comment raw() because it gives control over Ctrl+C which we don't
  // want! In case our main thread hang, we want to be able to kill the
  // application
  nonl();
  // Tell ncurses not to echo typed character to the screen. We'll take care of
  // that.
  noecho();
  // Catch special characters Backspace, Delete and the four arrow keys by
  // getch()
  keypad(stdscr, TRUE);
  // No delay on escape key
  set_escdelay(0);
  // Get the screen size
  getmaxyx(stdscr, this->_nlines, this->_ncols);
  LOGINF("Screen initialized with " << this->_nlines << " lines and "
         << this->_ncols << " cols");
  // Draw the screen at least once
  this->redraw_all();
  return 0;
}

void TerminalView::notify_browser_changed() {
  LOGDBG_("notify_browser_changed called")
  this->redraw_all();
  // Refresh the screen
  refresh();
}

void TerminalView::notify_buffer_changed() {
  LOGDBG_("notify_buffer_changed called")
  this->redraw_all();
  // Refresh the screen
  refresh();
}

void TerminalView::notify_fbar_changed() {
  LOGDBG_("notify_fbar_changed called")
  this->redraw_fbar(_fbar_model);
  // Refresh the screen
  refresh();
}

void TerminalView::notify_prompt_changed() {
  LOGDBG_("notify_prompt_changed called")
  this->redraw_prompt(_prompt_model);
  // Refresh the screen
  refresh();
}

void TerminalView::notify_state_changed() {
  LOGDBG_("notify_state_changed called")
  this->redraw_all();
  // Refresh the screen
  refresh();
}

void TerminalView::redraw_all() {
  this->redraw_buffer(**_browser_model.get_current_buffer());
  this->redraw_fbar(_fbar_model);
  this->redraw_prompt(_prompt_model);
  this->redraw_cursor(_state_model);
}

void TerminalView::redraw_buffer(BufferModel &buffer_model) {
  LOGDBG_("TerminalView::redraw_buffer called");
  if (_state_model.get_state() == state_e::OPEN_STATE
      || _state_model.get_state() == state_e::BROWSE_STATE
      || _state_model.get_state() == state_e::FILTER_STATE
      || _state_model.get_state() == state_e::ADD_FILTER_STATE)
  {
    uint first_line = buffer_model.get_first_line_displayed();
    uint number_of_line = buffer_model.get_number_of_line();
    LOGDBG_("range: buffer_model.get_first_line_displayed(): " << first_line)
    LOGDBG_("range: buffer_model.get_number_of_line(): " << number_of_line)
    print_buffer(stdscr, buffer_model.get_text(),
                 range(first_line, number_of_line),
                 0, this->_nlines, this->_ncols, 0, false);
    if (buffer_model.get_display_attributes()) {
      print_attrs(stdscr, buffer_model.get_attrs(), buffer_model.get_text(),
                   range(first_line,
                         std::max(0,
                          (int) std::min(first_line + this->_nlines,
                                         number_of_line) - 0)),
                   0, this->_nlines, this->_ncols, 0, false);
    }
    redraw_prompt(_prompt_model);
  }
}

void TerminalView::redraw_fbar(FBarModel &fbar_model) {
  LOGDBG_("redraw_fbar called")
  // Draw the function bar
  wmove(stdscr, 0, 0);
  auto &last = *(--fbar_model.get_functions().end());
  for(auto function : fbar_model.get_functions()) {
    wprintw(stdscr, "%s %s", function.key.c_str(), function.label.c_str());
    if (&function != &last)
      wprintw(stdscr, "  ");
  }
  // For debugging purposes, display the current state
  wmove(stdscr, 0, this->_ncols - 20);
  // Clear the line
  wclrtoeol(stdscr);
  std::stringstream statestream;
  statestream << _state_model.get_state();
  wprintw(stdscr, "%s", statestream.str().c_str());
}

void TerminalView::redraw_prompt(PromptModel &prompt_model) {
  LOGDBG_("redraw_prompt called with application state: " <<
         _state_model.get_state());
  // move the cursor at the beginning of the line
  wmove(stdscr, this->_nlines - 1, 0);
  // Clear the line
  wclrtoeol(stdscr);
  /*
   * Print a state prompt
   */
  const std::unique_ptr<BufferModel> &buffer =
    (*_browser_model.get_current_buffer());
  if (_state_model.get_state() == state_e::OPENING_STATE
      || _state_model.get_state() == state_e::ADD_FILTER_STATE
      || _state_model.get_state() == state_e::ERROR_STATE) {
    // compute where to start the prompt string
    _prompt_string_index =
      std::max((int) std::min(prompt_model.get_cursor_position().x,
                             _prompt_string_index),
               (int) (prompt_model.get_cursor_position().x - this->_ncols + 1));
    LOGDBG_("_prompt_string_index: " << _prompt_string_index);
    // Print the prompt
    wprintw(stdscr, "%s",
            &prompt_model.get_prompt().c_str()[_prompt_string_index]);
  } else if (_state_model.get_state() == state_e::BROWSE_STATE
             || _state_model.get_state() == state_e::FILTER_STATE) {
    // display the number of lines and the current top line (25 because 2^32 is
    // the maximum number of lines we can load so we can at display at most
    // 4294967296/4294967296)
    wmove(stdscr, this->_nlines - 1, this->_ncols - 25);
    wprintw(stdscr, "%9i/%i",
            buffer->get_first_line_displayed(), buffer->get_number_of_line());
    // If at the top of the file, display a Top tag
    if (buffer->get_first_line_displayed() == 0) {
      wmove(stdscr, this->_nlines - 1, this->_ncols - 3);
      wprintw(stdscr, "Top");
    // If at the end of the file, display a End tag
    // TODO: don't assume two dead lines!
    } else if (buffer->get_first_line_displayed() >=
               buffer->get_number_of_line() - this->_nlines + 2) {
      wmove(stdscr, this->_nlines - 1, this->_ncols - 3);
      wprintw(stdscr, "End");
    }
  }
  if (_state_model.get_state() == state_e::FILTER_STATE) {
    // Show filtering progress if not 100%
    if (buffer->get_filter_processing_progress() != 100) {
      wmove(stdscr, this->_nlines - 1, this->_ncols - 35);
      wprintw(stdscr,
              (std::to_string(buffer->get_filter_processing_progress()) + " %%").c_str());
    }
    // While in filter state, show the type of filter (OR / AND)
    wmove(stdscr, this->_nlines - 1, this->_ncols - 40);
    if (buffer->get_filter_set().land) {
      wprintw(stdscr, "AND");
    } else {
      wprintw(stdscr, "OR");
    }
  }

  // The cursor is actually moved only if we are in a state when the prompt is
  // focused
  redraw_cursor(_state_model);
}

void TerminalView::redraw_cursor(StateModel &state_model) {
  LOGDBG_("application state: " << state_model.get_state());
  /*
   * Display the curso when a state prompt is expecting an input
   */
  if (state_model.get_state() == state_e::OPENING_STATE
      || state_model.get_state() == state_e::ADD_FILTER_STATE) {
    curs_set(1);
    LOGDBG_("_prompt_model.get_cursor_position().x: " << _prompt_model.get_cursor_position().x);
    LOGDBG_("_prompt_string_index: " << _prompt_string_index);
    wmove(stdscr, this->_nlines - 1,
          _prompt_model.get_cursor_position().x - _prompt_string_index);
  }
  // In ERROR_STATE, no cursor
  else if (state_model.get_state() == state_e::ERROR_STATE) {
    curs_set(0);
  }
  else if (state_model.get_state() == state_e::OPEN_STATE) {
    curs_set(0);
  }
  else if (state_model.get_state() == state_e::BROWSE_STATE) {
    curs_set(0);
  }
  else if (state_model.get_state() == state_e::FILTER_STATE) {
    curs_set(0);
  }
}

/*
 * Special functions for controller interaction
 */
void TerminalView::prompt(Controller &controller) {
  int key = 0;
  while ((key = getch()) == ERR)
    if (controller.is_interrupted()) return ;
  controller.inject_key(key);
}

void TerminalView::get_view_size(uint &nlines, uint ncols) {
  nlines = this->_nlines;
  ncols = this->_ncols;
}
