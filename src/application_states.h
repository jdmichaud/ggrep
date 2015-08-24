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
  void exit(const IEvent &);
  void suspend(const IEvent &) {
    LOGDBG("suspend CloseState");
  }
  void resume(const IEvent &);
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
    LOGDBG("exiting OpenState");
  }
  void suspend(const IEvent &) {
    LOGDBG("suspend OpenState");
  }
  void resume(const IEvent &) {
    LOGDBG("resume OpenState");
  }
};

/*
 * Browsing the file, ready to do some real work.
 */
class BrowseState : public State<BrowseState> {
public:
  BrowseState(Context &context, Controller &controller, IState *parent_state);
  void enter(const IEvent &) {
    LOGDBG("entering BrowseState");
  }
  void exit(const IEvent &) {
    LOGDBG("exiting BrowseState");
  }
  void suspend(const IEvent &) {
    LOGDBG("suspend BrowseState");
  }
  void resume(const IEvent &) {
    LOGDBG("resume BrowseState");
  }
};

/*
 * Buffer is being filtered.
 */
class FilterState : public State<FilterState> {
public:
  FilterState(Context &context, Controller &controller, IState *parent_state);
  void enter(const IEvent &);
  void exit(const IEvent &);
  void suspend(const IEvent &) {
    LOGDBG("suspend FilterState");
  }
  void resume(const IEvent &) {
    LOGDBG("resume FilterState");
  }
};

/*
 * Add a filter into the filter set for a particular buffer.
 */
class AddFilterState : public State<AddFilterState>, public OneLinerText {
public:
  AddFilterState(Context &context, Controller &controller, IState *parent_state);
  void enter(const IEvent &);
  void exit(const IEvent &);
  void suspend(const IEvent &) {
    LOGDBG("suspend FilterState");
  }
  void resume(const IEvent &);
  virtual void update();
};

/*
 * Browsing the file, ready to do some real work.
 */
class ErrorState : public State<ErrorState> {
public:
  ErrorState(Context &context, Controller &controller, IState *parent_state);
  void enter(const IEvent &);
  void exit(const IEvent &);
  void suspend(const IEvent &) {
    LOGDBG("suspend ErrorState");
  }
  void resume(const IEvent &) {
    LOGDBG("resume ErrorState");
  }
private:
  std::string prompt;
  std::string input;
};

#endif //__APPLICATION_STATES_H__