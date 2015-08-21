/*! \brief Context is also known as State Machine
 *
 * The state machine knows the various state and the interrelation between those
 * states.
 */

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <map>
#include <stack>
#include <exception>

#include "command.h"
#include "state.h"

/*
 * This user-defined exception is thrown in the case no state is able to handle
 * the event injected into the state machine.
 */
class UnhandledEvent : public std::exception {
public:
  UnhandledEvent(const IState &state, const IEvent &event) : m_state(state), m_event(event) {}
  const char *what() const noexcept
  {
    static char msg[255];
    snprintf(msg, 255, 
            "Unhandled event (%i) injected into state machine from state (%s)",
            m_event.get_eventid(), m_state.get_type().name());
    return msg;
  }
private:
  const IState  &m_state;
  const IEvent  &m_event;
};

class Context {
public:
  Context(Controller &controller);
  /*! Pass the input from the user to the state for handling */
  void inject(const IEvent &event);
  /*!
   * Change the state of the context. The current state is exited for proper
   * disabling and the new state is entered
   * \param new_state is the new state to enter into
   * \param input is the input that triggered this change of state
   */
  void change_state(IState &new_state, const IEvent &input);
  /*!
   * Enter state while suspending the current state. The current is suspended 
   * and put on the stack. It can be resumed on exit_state
   */
  void enter_state(IState &new_state, const IEvent &input);
  /*!
   * Exit the current state and resume the next state in the stack. If no state
   * was in the stack, the result is undefined behavior.
   */
  void exit_state(const IEvent &input);
  /*!
   * Backtracak to the previous state. Can be used to go back to a previous 
   * state when the ESC key is used.
   * If the state has not previous state, raise std::runtime_exception
   */
  void backtrack(const IEvent &);
  /*!
   * Get the current state.
   */
  IState &get_current() { return *m_state; }
  /*!
   * Provides an accessor to the state of the states machine from the emum 
   * state_e
   */
  IState &get_state(state_e state);
private:
  /*
   * Current state. It's infortunate we have to use a pointer here and we will 
   * not be testing for NULL all the time. TODO: find a better way.
   */
  IState *m_state;
  /* All the possible of the state machines */
  std::map<state_e, IState *> m_states;
  /* States stack to keep an history of visited states and be able to backatrack
     from a state */
  std::stack<IState *> m_state_stack;
};

#endif //__CONTEXT_H__