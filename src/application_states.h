/*! \brief Define the various states of the application
 *
 * The application can have many states depending of the commands requested by
 * the client.
 */

#ifndef __APPLICATION_STATES_H__
#define __APPLICATION_STATES_H__

#include "input.h"
#include "command.h"
#include "state.h"
#include "context.h"
#include "logmacros.h"

/*
 * No file opened in this state
 */
class CloseState : public State<CloseState> {
  CloseState(Context &context, Invoker &invoker, 
             const std::unique_ptr<IState> &parent_state,
             const std::list<IState &> &sub_states);
  void enter(const IEvent &) {
    LOGDBG("entering CloseState")
  }
  void exit(const IEvent &) {
    LOGDBG("exiting CloseState")
  }
};

/*
 * A file is opened, normal operation are possible
 */
class OpenState : public State<OpenState> {
  OpenState(Context &context, Invoker &invoker, 
            const std::unique_ptr<IState> &parent_state,
            const std::list<IState &> &sub_states);
  void enter(const IEvent &);
  void exit(const IEvent &) {
    LOGDBG("exiting OpenState")
  }
};

/*
 * Browsing the file, ready to do some real work.
 */
class BrowseState : public State<BrowseState> {
public:
  BrowseState(Context &context, Invoker &invoker, 
              const std::unique_ptr<IState> &parent_state,
              const std::list<IState &> &sub_states);
  void enter(const IEvent &) {
    LOGDBG("entering BrowseState")
  }
  void exit(const IEvent &) {
    LOGDBG("exiting BrowseState")
  }
};

#endif //__APPLICATION_STATES_H__