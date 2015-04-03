/*! \brief A view to display the filtered text on a text based terminal
 *
 *  The terminal view is observing the model and updates the content of the
 *  screen. Additionally to the filtered text, the text terminal displays a
 *  function bar mapping the function keys (Fx) with commands and a prompt line
 *  where filtered text or command shall be entered.
 *
 *  Terminal view is using ncurses to manipulate text over a text terminal.
 */

#ifndef __VIEW_H__
#define __VIEW_H__

#include <vector>
#include "observer.hpp"
#include "model.h"

class TerminalView : public Observer
{
public:
    TerminalView();

    void notify(Model &model);
};

#endif //__VIEW_H__