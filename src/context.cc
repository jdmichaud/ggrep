#include <stdexcept>
#include "context.h"
#include "event.h"
#include "state.h"
#include "application_states.h"

#include "logmacros.h"

Context::Context(Controller &controller) {
  m_states.emplace(state_e::DEFAULT_STATE,
                   new DefaultState(*this, controller, nullptr));
  m_states.emplace(state_e::CLOSE_STATE,
                   new CloseState(*this, controller,
                                  m_states[state_e::DEFAULT_STATE]));
  m_states.emplace(state_e::OPENING_STATE,
                   new OpeningState(*this, controller,
                                    m_states[state_e::DEFAULT_STATE]));
  m_states.emplace(state_e::OPEN_STATE,
                   new OpenState(*this, controller,
                                 m_states[state_e::DEFAULT_STATE]));
  m_states.emplace(state_e::ERROR_STATE,
                   new ErrorState(*this, controller,
                                  m_states[state_e::DEFAULT_STATE]));
  m_states.emplace(state_e::BROWSE_STATE,
                   new BrowseState(*this, controller,
                                   m_states[state_e::OPEN_STATE]));
  m_states.emplace(state_e::FILTER_STATE,
                   new FilterState(*this, controller,
                                   m_states[state_e::BROWSE_STATE]));
  m_states.emplace(state_e::ADD_FILTER_STATE,
                   new AddFilterState(*this, controller,
                                      m_states[state_e::OPEN_STATE]));
  m_states.emplace(state_e::SEARCH_STATE,
                   new SearchState(*this, controller,
                                   m_states[state_e::BROWSE_STATE]));
  // Starting point of the state machine
  m_state = m_states[state_e::CLOSE_STATE];
  m_state->enter(Event(APP_STARTED));
}

void Context::inject(const IEvent &event) {
  if (!m_state->handle(event))
    throw UnhandledEvent(*m_state, event);
}

void Context::change_state(IState &new_state, const IEvent &input) {
  LOGDBG(" " << m_state << " -> " << new_state);
  m_state->exit(input);
  new_state.set_previous_state(NULL);
  m_state = &new_state;
  m_state->enter(input);
}

void Context::enter_state(IState &new_state, const IEvent &input) {
  LOGDBG(" -> " << m_state);
  m_state->suspend(input);
  m_state_stack.push(m_state);
  new_state.set_previous_state(m_state);
  m_state = &new_state;
  m_state->enter(input);
}

void Context::exit_state(const IEvent &input) {
  LOGDBG(" <- " << m_state);
  m_state->exit(input);
  m_state = m_state_stack.top();
  m_state_stack.pop();
  m_state->resume(input);
}

IState &Context::get_state(state_e state) {
  return *m_states[state];
}
