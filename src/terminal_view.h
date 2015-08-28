/*! \brief A view to display the filtered text on a text based terminal
 *
 *  The terminal view is observing the model and updates the buffer of the
 *  screen. Additionally to the filtered text, the text terminal displays a
 *  function bar mapping the function keys (Fx) with commands and a prompt line
 *  where filtered text or command shall be entered.
 *
 *  Terminal view is using ncurses to manipulate text over a text terminal.
 */

#ifndef __TERMINAL_VIEW_H__
#define __TERMINAL_VIEW_H__

#include <assert.h>
#include <vector>
#include "view.h"
#include "browser_model.h"
#include "fbar_model.h"
#include "prompt_model.h"
#include "state_model.h"
#include "controller.h"

class TerminalView : public IView
{
public:
  TerminalView(BrowserModel &browser_model, FBarModel &fbar_model,
               PromptModel &prompt_model, StateModel &state_model);
  ~TerminalView();

  /* IView implementation */
  void prompt(Controller &);
  void get_view_size(uint &nlines, uint ncols);


  uint init();
  void notify_browser_changed(IObservable &observable);
  void notify_buffer_changed(IObservable &observable);
  void notify_fbar_changed(IObservable &observable);
  void notify_prompt_changed(IObservable &observable);
  void notify_state_changed(IObservable &observable);

private:
  void redraw_buffer(BufferModel &);
  void redraw_fbar(FBarModel &);
  void redraw_prompt(PromptModel &);
  void redraw_cursor(StateModel &);
  void redraw_all();

private:
  /** Terminal related information
   */
  uint _nlines;
  uint _ncols;

  /** Models
   */
  BrowserModel  &_browser_model;
  FBarModel     &_fbar_model;
  PromptModel   &_prompt_model;
  StateModel    &_state_model;

  /** This is ugly and temporary as long as we don't have widgets
   */
  uint _prompt_string_index;
};

#endif //__TERMINAL_VIEW_H__