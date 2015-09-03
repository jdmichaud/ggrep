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

/*
 * Set the current buffer in the buffer model to the filtered buffer
 */
class EnableFiltering : public Command {
public:
  EnableFiltering(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    m_controller.enable_filtering_on_current_buffer();
  }
  virtual void unexecute() {
    m_controller.disable_filtering_on_current_buffer();
  }
  virtual bool unexecutable() { return true; };
};

/*
 * Set the current buffer in the buffer model to the unfiltered buffer (loaded
 * file)
 */
class DisableFiltering : public Command {
public:
  DisableFiltering(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    m_controller.disable_filtering_on_current_buffer();
  }
  virtual void unexecute() {
    m_controller.enable_filtering_on_current_buffer();
  }
  virtual bool unexecutable() { return true; };
};


#endif // __FILTERING_COMMAND_H__