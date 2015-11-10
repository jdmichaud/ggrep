/*! \brief Searching commands
 *
 */
#include <stdlib.h>
#include "constants.h"
#include "controller.h"
#include "logmacros.h"

#ifndef __SEARCHING_COMMAND_H__
#define __SEARCHING_COMMAND_H__

/*
 * Change the search term in the model
 */
class UpdateSearchTerm : public Command {
public:
  UpdateSearchTerm(Controller &controller, Invoker &invoker, IState *state,
                   const std::string &term) :
    Command(controller, invoker, state), m_term(term) {}

  virtual void execute() {
    m_controller.set_search_term(m_term);
  }
  virtual void unexecute() { /* non undoable */ }
  virtual bool unexecutable() { return false; };
private:
  std::string m_term;
};

class ForwardSearchCommand : public Command {
public:
  ForwardSearchCommand(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    m_controller.search_forward();
  }
  virtual void unexecute() { /* non undoable */ }
  virtual bool unexecutable() { return false; };
};

class ReverseSearchCommand : public Command {
public:
  ReverseSearchCommand(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    m_controller.search_backward();
  }
  virtual void unexecute() { /* non undoable */ }
  virtual bool unexecutable() { return false; };
};

#endif // __SEARCHING_COMMAND_H__