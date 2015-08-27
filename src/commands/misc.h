/*! \brief Miscellenous commands
 *
 */
#include <stdlib.h>
#include "controller.h"
#include "application_states.h"
#include "logmacros.h"

#ifndef __MISC_H__
#define __MISC_H__

class EnterChar : public Command {
public:
  EnterChar(Controller &controller, Invoker &invoker, IState *state,
            const IEvent &b, IText *text) :
    Command(controller, invoker, state),
    m_text(text),
    m_input(b.get_eventid()) {}

  virtual void execute() {
    LOGFN();
    m_text->insert((char) m_input.get_eventid());
  }
  virtual void unexecute() {
    m_text->remove();
  }
  virtual bool unexecutable() { return true; };
private:
  IText *m_text;
  Input m_input;
};

class BackspaceCommand : public Command {
public:
  BackspaceCommand(Controller &controller, Invoker &invoker, IState *state,
                   IText *text) :
    Command(controller, invoker, state), m_text(text) {}
  virtual void execute() {
    m_nothing_done = true;
    // Get the character we remove for the undo action
    m_text->get_previous_char(m_removed_char);
    // Delete left character and move the cursor back and authorize undo
    // Remove will return false if nothing was done
    m_nothing_done = !m_text->remove();
  }
  virtual void unexecute() {
    // If a character was effectively deleted, add it back
    if (!m_nothing_done)
      m_text->insert(m_removed_char);
  }
  virtual bool unexecutable() { return true; };
private:
  IText *m_text;
  char m_removed_char;
  bool m_nothing_done;
};

class DeleteCommand : public Command {
public:
  DeleteCommand(Controller &controller, Invoker &invoker, IState *state,
                IText *text) :
    Command(controller, invoker, state), m_text(text) {}
  virtual void execute() {
    m_nothing_done = true;
    // Get the character we remove for the undo action
    m_text->get_char(m_removed_char);
    m_nothing_done = !m_text->delete_();
  }
  virtual void unexecute() {
    if (!m_nothing_done) {
      // Delete the character on the right
      m_text->insert(m_removed_char);
      // And place the cursor back in its place
      pos curpos;
      m_text->get_curpos(curpos);
      curpos.x--;
      m_text->set_curpos(curpos);
    }
  }
  virtual bool unexecutable() { return true; };
private:
  IText *m_text;
  char m_removed_char;
  bool m_nothing_done;
};

#endif // __MISC_H__