/*! \brief Define the state/context pattern
 *
 * Loosely based on the state design pattern.
 */

#ifndef __STATES_H__
#define __STATES_H__

#include <list>
#include <utility>
#include <memory>
#include <typeinfo>
#include <functional>

#include "event.h"
#include "command.h"
#include "logmacros.h"

/*
 * Application state
 */
enum class state_e {
  DEFAULT_STATE,
  CLOSE_STATE,
  OPEN_STATE,
  BROWSE_STATE,
  ERROR_STATE,
  FILTER_STATE,
  ADD_FILTER_STATE
};

/*
 * The operator<< overload for state_e is used for easier logging/debugging
 */
std::ostream& operator<<(std::ostream& os, const state_e& state);

class Context;

class IState {
public:
  virtual ~IState() {};
  /*!
   * Handle a event or forward to parent.
   * Returns true if the event was handled, false otherwise.
   */
  virtual bool handle(const IEvent &) = 0;
  /*!
   * Function executed when the state is entered during a change of state
   */
  virtual void enter(const IEvent &) = 0;
  /*!
   * Function executed when the state is exited during a change of state
   */
  virtual void exit(const IEvent &) = 0;
  /*!
   * Function executed when the state is temporarily exited. This can happend
   * when entering a substate. Once the substate exits, the suspended state is
   * resumed
   */
  virtual void suspend(const IEvent &) = 0;
  /*!
   * Function executed when the state is resumed after having been suspended.
   */
  virtual void resume(const IEvent &) = 0;
  /*!
   * Return the type_info of the IState object
   */
  virtual const std::type_info& get_type() const = 0;
  /*!
   * Return the ID of ths state
   */
  virtual state_e get_id() const = 0;
  /*!
   * Set the state which we was active before entering this state
   */
  virtual void set_previous_state(IState *state) = 0;
  /*!
   * Get the state which we was active before entering this state
   */
  virtual IState *get_previous_state() const = 0;
};

/*
 * Streaming operator for IState to string operation
 */
std::ostream& operator<<(std::ostream& os, const IState& s);

/*
 * The mapping between input and actions is a combination of searching through
 * a vector for an IEvent that would positively response to an is_a(key) call
 * and an escalation to the parent state if no IEvent is_a key.
 * So you will first go through the action map of the current state and for each
 * input call is_a with the key. The first positive answer will trigger the
 * corresponding action. If not IEvent is_a key, then the same algorithm is
 * applied to the actiona map of the parent state, and up to the main state.
 * No IEvent found is a failure case and will raise an exception
 */
// An action is a function taking an input as parameter
typedef std::function<void(const IEvent &)> action_t;
/* The transition matrix is a list. The order is which the IEvent are stacked
   is meaningful */
/* TODO: Use a unique_ptr here !!! */
typedef std::list<std::pair<IEvent *, action_t> > action_map_t;

template <typename T>
class State : public IState {
public:
  State(Context &context, Controller &controller,
        IState *parent_state, action_map_t action_map, state_e id) :
    m_controller(controller),
    m_context(context),
    m_invoker(controller, this),
    m_previous_state(NULL),
    m_parent_state(parent_state),
    m_action_map(action_map),
    m_id(id)
  {}
  virtual const std::type_info& get_type() const { return typeid(T); }
  /*
   * This algorithm determines how state inhertance works vis-a-vis the handling
   * of events. First, the action map of the state is walked and the first event
   * found that is_a event specified triggs its corresponding actions. If no
   * event satisfy the requirement, handling is passed to the parent state.
   */
  virtual bool handle(const IEvent &event) {
    for (auto action: m_action_map) {
      LOGDBG("action.first: " << typeid(*action.first).name() << " " <<
             "get_eventid: " << action.first->get_eventid());
      if (action.first->is_equal(event)) {
        LOGDBG("Found the event in the map");
        action.second(event);
        return true;
      }
    }
    // If we are here, then no appropriate event was found in the action map.
    // Forwarding to parent if have a parent, otherwise just return false
    if (m_parent_state != nullptr) {
      LOGDBG("Could not find the event, trying with parent state");
      return m_parent_state->handle(event);
    }
    return false;
  }
  inline state_e get_id() const { return m_id; };
  void set_previous_state(IState *state) { m_previous_state = state; }
  IState *get_previous_state() const { return m_previous_state;  }
protected:
  Controller              &m_controller;
  Context                 &m_context;
  Invoker                 m_invoker;
  IState                  *m_previous_state;
  IState                  *m_parent_state;
  action_map_t            m_action_map;
  state_e                 m_id;
private:
  // No copying for you!
  State(const State &) = delete;
};

#endif //__STATES_H__