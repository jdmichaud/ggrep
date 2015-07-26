/*! \brief Meta command for state manipuation
 *
 */

#ifndef __STATE_CMD_H__
#define __STATE_CMD_H__

#include "event.h"
#include "state.h"
#include "command.h"

/*!
 * Change the state of the controller
 */
class ChangeStateCommand : public Command {
public:
  ChangeStateCommand(Controller &controller, Invoker &invoker, IState *state,
                     state_e to_state, const IEvent &b) : 
    Command(controller, invoker, state), m_to_state(to_state), m_event(b) {}
  virtual void execute() {
    m_controller.change_state(m_to_state, m_event);
  }
  virtual void unexecute() {
    m_controller.backtrack(Event(UNDO));
  }
private:
  state_e m_to_state;
  const IEvent &m_event;
};

/*!
 * Backtrack to the previous state
 */
class BacktrackCommand : public Command {
public:
  BacktrackCommand(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state) {}
  virtual void execute() {
    m_controller.backtrack(Event(BACKTRACK));
  }
  virtual void unexecute() { /* non executable */; }
private:
};


#endif // __STATE_CMD_H__