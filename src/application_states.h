/*! \brief Define the various states of the application
 *
 * The application can have many states depending of the commands requested by
 * the client.
 */

#ifndef __APPLICATION_STATES_H__
#define __APPLICATION_STATES_H__

#include "text.h"
#include "input.h"
#include "command.h"
#include "state.h"
#include "context.h"
#include "logmacros.h"

/*
 * No file opened in this state
 */
class CloseState : public State<CloseState>, public OneLinerText {
public:
  CloseState(Context &context, Controller &controller, IState *parent_state);
  void enter(const IEvent &);
  void exit(const IEvent &) {
    LOGDBG("exiting CloseState")
  }
  virtual void update();
};

/*
 * A file is opened, normal operation are possible
 */
class OpenState : public State<OpenState> {
public:
  OpenState(Context &context, Controller &controller, IState *parent_state);
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
  BrowseState(Context &context, Controller &controller, IState *parent_state);
  void enter(const IEvent &) {
    LOGDBG("entering BrowseState")
  }
  void exit(const IEvent &) {
    LOGDBG("exiting BrowseState")
  }
};

/*
 * Browsing the file, ready to do some real work.
 */
class ErrorState : public State<ErrorState> {
public:
  ErrorState(Context &context, Controller &controller, IState *parent_state);
  void enter(const IEvent &);
  void exit(const IEvent &);
private:
  std::string prompt;
  std::string input;
};

#endif //__APPLICATION_STATES_H__