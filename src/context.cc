#include "context.h"
#include "event.h"
#include "state.h"
#include "application_states.h"

#include "logmacros.h"

Context::Context(IInvoker &invoker) : m_invoker(invoker) {
  m_states[CLOSE_STATE] = CloseState(*this, m_invoker, nullptr);
  m_states[OPEN_STATE] = OpenState(*this, m_invoker, nullptr);
  m_states[BROWSE_STATE] = BrowseState(*this, m_invoker, m_states[OPEN_STATE]);
  // Starting point of the state machine
  m_state = m_states[CLOSE_STATE];
}

void Context::inject(const IEvent &event) {
  if (!m_state.handle(event))
    throw UnhandledEvent(m_state, event);
}

void Context::change_state(IState &new_state, const IEvent &input) {
  m_state.exit(input);
  m_state = new_state;
  m_state.enter(intput)
}

IState &Context::get_state(state_e state) {
  return m_states[state];
}
