/*! \brief Commands to manage ggrep process
 *
 */

#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <stdlib.h>
#include "controller.h"

class ExitCommand : public Command {
public:
  ExitCommand(Controller &controller, Invoker &invoker, IState *state,
              int exitcode = 0) :
    Command(controller, invoker, state), m_exitcode(exitcode) {}

  virtual void execute() {
    LOGDBG("interrupting the controller");
    m_controller.interrupt();
  }
  virtual void unexecute() {
    // non unexecutable
  }
  virtual bool unexecutable() { return false; };

private:
  uint m_exitcode;
};

#endif // __PROCESS_H__