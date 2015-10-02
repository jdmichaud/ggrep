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
  virtual void unexecute() { /* not undoable */ }
  virtual bool unexecutable() { return false; };
private:
  state_e m_to_state;
  const IEvent &m_event;
};

class EnterStateCommand : public Command {
public:
  EnterStateCommand(Controller &controller, Invoker &invoker, IState *state,
                    state_e to_state, const IEvent &b) : 
    Command(controller, invoker, state), m_to_state(to_state), m_event(b) {}
  virtual void execute() {
    m_controller.enter_state(m_to_state, m_event);
  }
  virtual void unexecute() { /* not undoable */ }
  virtual bool unexecutable() { return false; };
private:
  state_e m_to_state;
  const IEvent &m_event;
};

class ExitStateCommand : public Command {
public:
  ExitStateCommand(Controller &controller, Invoker &invoker, IState *state,
                   const IEvent &b) : 
    Command(controller, invoker, state), m_event(b) {}
  virtual void execute() {
    m_controller.exit_state(m_event);
  }
  virtual void unexecute() { /* not undoable */ }
  virtual bool unexecutable() { return false; };
private:
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
  virtual void unexecute() { /* not undoable */ }
  virtual bool unexecutable() { return false; };
private:
};

/*
 * Inject an event into the controller
 */
class InjectCommand : public Command {
public:
  InjectCommand(Controller &controller, Invoker &invoker, IState *state,
                const Event &event) :
    Command(controller, invoker, state), m_event(event) {}
  virtual void execute() {
    m_controller.inject(m_event);
  }
  virtual void unexecute() { /* non unexecutable */ }
  virtual bool unexecutable() { return false; };
private:
  const Event &m_event;
};

#endif // __STATE_CMD_H__