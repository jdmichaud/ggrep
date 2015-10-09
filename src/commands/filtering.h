/*! \brief Filtering commands
 *
 */
#include <stdlib.h>
#include "constants.h"
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

/*
 * Reset the filtering.
 */
class ResetFiltering : public Command {
public:
  ResetFiltering(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    m_controller.clear_filtering_on_current_buffer();
  }
  virtual void unexecute() { /* non undoable */ }
  virtual bool unexecutable() { return false; };
};

/*
 * Switch between AND/OR filter
 */
class SwitchFilterType : public Command {
public:
  SwitchFilterType(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    m_controller.switch_filter_type();
  }
  virtual void unexecute() {
    m_controller.switch_filter_type();
  }
  virtual bool unexecutable() { return true; };
};

/*******************************************************************************
 * The current filter commands are used to manipulate the "head" of the filter
 * list. This head is dynamically updated with the content of the prompt command
 ******************************************************************************/

/*
 * Create current filter in the filter set
 */
class CreateCurrentFilterEntry : public Command {
public:
  CreateCurrentFilterEntry(Controller &controller, Invoker &invoker,
                           IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
  }
  virtual void unexecute() { /* non unexecutable */ }
  virtual bool unexecutable() { return false; };
};

/*
 * Update the current filter entry with the prompt value
 */
class UpdateCurrentFilterEntry : public Command {
public:
  UpdateCurrentFilterEntry(Controller &controller, Invoker &invoker,
                           IState *state, const std::string &filter,
                           const IEvent &b) :
    Command(controller, invoker, state), m_filter(filter),
    m_input(b.get_eventid()) {}
  virtual void execute() {
    // If the filter size is on the threshold and we added character, the filter
    // needs to be added
    if (m_filter.size() == FILTER_STRING_MIN_SIZE
        && m_input != KEY_BKSP && m_input != KEY_DC) {
      m_controller.add_filter(m_filter);
      m_controller.set_filter_dynamic();
    }
    // If the filter size is greater then the threshold, then we just update it
    else if (m_filter.size() > FILTER_STRING_MIN_SIZE)
      m_controller.update_last_filter(m_filter);
    // If the filter size is *right* below the threashold and we just removed a
    // character, we need to remove the filter
    else if (m_filter.size() < FILTER_STRING_MIN_SIZE - 1
             && m_input == KEY_BKSP && m_input == KEY_DC) {
      m_controller.remove_last_filter();
      m_controller.unset_filter_dynamic();
    }
  }
  virtual void unexecute() { /* non unexecutable */ }
  virtual bool unexecutable() { return false; };
private:
  std::string m_filter;
  Input m_input;
};

/*
 * Remove the current filter from the filter list (the entry has been cancelled)
 */
class CancelCurrentFilterEntry : public Command {
public:
  CancelCurrentFilterEntry(Controller &controller, Invoker &invoker,
                           IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    // remove the current filter
    m_controller.remove_last_filter();
    m_controller.unset_filter_dynamic();
  }
  virtual void unexecute() { /* non unexecutable */ }
  virtual bool unexecutable() { return false; };
};

/*
 * Remove the current filter from the filter list (the entry has been cancelled)
 */
class EnterCurrentFilterEntry : public Command {
public:
  EnterCurrentFilterEntry(Controller &controller, Invoker &invoker,
                          IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    // set the flag that validates the last filter and indicates it is not
    // interactively edited
    m_controller.unset_filter_dynamic();
  }
  virtual void unexecute() {
    // Remove the head of the filter list
    m_controller.remove_last_filter();
  }
  virtual bool unexecutable() { return true; };
};

#endif // __FILTERING_COMMAND_H__