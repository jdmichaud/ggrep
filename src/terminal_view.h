/*! \brief A view to display the filtered text on a text based terminal
 *
 *  The terminal view is observing the model and updates the content of the
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
#include "content_model.h"
#include "fbar_model.h"
#include "prompt_model.h"
#include "controller.h"

class TerminalView : public IView
{
public:
  TerminalView(ContentModel &content_model, FBarModel &fbar_model, 
               PromptModel &prompt_model);
  ~TerminalView();

  uint init();
  void notify(IObservable &observable) { assert(false); }
  void notify(ContentModel &content_model);
  void notify(FBarModel &fbar_model);
  void notify(PromptModel &prompt_model);

  void prompt(Controller &);

private:
  void redraw_content(ContentModel &);
  void redraw_fbar(FBarModel &);
  void redraw_prompt(PromptModel &);

private:
  /** Terminal related information
   */
  uint _nlines;
  uint _ncols;

  /** Models
   */
  ContentModel  *_content_model;
  FBarModel     *_fbar_model;
  PromptModel   *_prompt_model;
};

#endif //__TERMINAL_VIEW_H__