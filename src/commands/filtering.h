/*! \brief Filtering commands
 *
 */
#include <stdlib.h>
#include "controller.h"
#include "application_states.h"
#include "logmacros.h"

#ifndef __FILTERING_COMMAND_H__
#define __FILTERING_COMMAND_H__

class AddFilterCommand : public Command {
public:
  AddFilterCommand(Controller &controller, Invoker &invoker, IState *state,
                   const std::string &filter) :
    Command(controller, invoker, state), m_filter(filter) {}

  virtual void execute() {
    m_controller.add_filter(m_filter);
    m_invoker.create_and_execute<BacktrackCommand>();
  }
  virtual void unexecute() { /* non undoable */ }
  virtual bool unexecutable() { return false; };
private:
  std::string m_filter;
};

#endif // __FILTERING_COMMAND_H__