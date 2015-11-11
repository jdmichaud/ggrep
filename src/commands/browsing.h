/*! \brief All the browsing command
 *
 */

#ifndef __BROWSING_H__
#define __BROWSING_H__

#include "buffer_model.h"
#include "controller.h"
#include "command.h"

/*!
 * Move cursor one character down
 */
class DownCommand : public Command {
public:
  DownCommand(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    m_controller.scroll_buffer_down(1);
  }
  virtual void unexecute() {
    m_controller.scroll_buffer_up(1);
  }
private:
};

/*!
 * Move cursor one character up
 */
class UpCommand : public Command {
public:
  UpCommand(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    m_controller.scroll_buffer_up(1);
  }
  virtual void unexecute() {
    m_controller.scroll_buffer_down(1);
  }
private:
};

/*!
 * Move cursor one page down
 */
class PageDownCommand : public Command {
public:
  PageDownCommand(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    m_controller.scroll_buffer_page_down();
  }
  virtual void unexecute() {
    m_controller.scroll_buffer_page_up();
  }
private:
};

/*!
 * Move cursor one character up
 */
class HomeCommand : public Command {
public:
  HomeCommand(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state), m_previous_current_line(0) {}

  virtual void execute() {
    m_controller.scroll_buffer_up(UINT_MAX);
    // TODO: to implement
    m_previous_current_line = 0;
  }
  virtual void unexecute() {
    //m_controller.set_buffer_current_line(m_previous_current_line);
  }
private:
  uint m_previous_current_line;
};

/*!
 * Move cursor one page down
 */
class EndCommand : public Command {
public:
  EndCommand(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state), m_previous_current_line(0) {}

  virtual void execute() {
    m_controller.scroll_buffer_end();
    // TODO: to implement
    m_previous_current_line = 0;
  }
  virtual void unexecute() {
    //m_controller.set_buffer_current_line(m_previous_current_line);
  }
private:
  uint m_previous_current_line;
};



/*!
 * Move cursor one page up
 */
class PageUpCommand : public Command {
public:
  PageUpCommand(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    m_controller.scroll_buffer_page_up();
  }
  virtual void unexecute() {
    m_controller.scroll_buffer_page_down();
  }
private:
};


/*!
 * Move cursor one character left
 */
class LeftCommand : public Command {
public:
  LeftCommand(Controller &controller, Invoker &invoker, IState *state,
              IText *text) :
    Command(controller, invoker, state), m_text(text) {}

  virtual void execute() {
    m_text->shift_cur_char_left();
  }
  virtual void unexecute() {
    m_text->shift_cur_char_right();
  }
private:
  IText *m_text;
};

/*!
 * Move cursor one character righr
 */
class RightCommand : public Command {
public:
  RightCommand(Controller &controller, Invoker &invoker, IState *state,
               IText *text) :
    Command(controller, invoker, state), m_text(text) {}

  virtual void execute() {
    m_text->shift_cur_char_right();
  }
  virtual void unexecute() {
    m_text->shift_cur_char_left();
  }
private:
  IText *m_text;
};

/*!
 * Move cursor one word left
 */
class LeftWordCommand : public Command {
public:
  LeftWordCommand(Controller &controller, Invoker &invoker, IState *state,
                  IText *text) :
    Command(controller, invoker, state), m_text(text) {}

  virtual void execute() {
    m_text->shift_cur_word_left();
  }
  virtual void unexecute() {
    m_text->shift_cur_word_right();
  }
private:
  IText *m_text;
};

/*!
 * Move cursor one word right
 */
class RightWordCommand : public Command {
public:
  RightWordCommand(Controller &controller, Invoker &invoker, IState *state,
                   IText *text) :
    Command(controller, invoker, state), m_text(text) {}

  virtual void execute() {
    m_text->shift_cur_word_right();
  }
  virtual void unexecute() {
    m_text->shift_cur_word_left();
  }
private:
  IText *m_text;
};

/*!
 * Move cursor at the beginning of the line
 */
class BegLineCommand : public Command {
public:
  BegLineCommand(Controller &controller, Invoker &invoker, IState *state,
                 IText *text) :
    Command(controller, invoker, state), m_text(text) {}

  virtual void execute() {
    m_text->get_curpos(m_previous_curpos);
    m_text->begline();
  }
  virtual void unexecute() {
    m_text->set_curpos(m_previous_curpos);
  }
private:
  IText *m_text;
  pos m_previous_curpos;
};

/*!
 * Move cursor at the end of the line
 */
class EndLineCommand : public Command {
public:
  EndLineCommand(Controller &controller, Invoker &invoker, IState *state,
                 IText *text) :
    Command(controller, invoker, state), m_text(text) {}

  virtual void execute() {
    m_text->get_curpos(m_previous_curpos);
    m_text->endline();
  }
  virtual void unexecute() {
    m_text->set_curpos(m_previous_curpos);
  }
private:
  IText *m_text;
  pos m_previous_curpos;
};

/*
 * Toggle the display of attributes
 */
class SetToggleAttributesCommand : public Command {
public:
  SetToggleAttributesCommand(Controller &controller, Invoker &invoker,
                             IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    m_controller.set_attributes();
  }
  virtual void unexecute() {
    m_controller.unset_attributes();
  }
private:
};

class UnsetToggleAttributesCommand : public Command {
public:
  UnsetToggleAttributesCommand(Controller &controller, Invoker &invoker,
                               IState *state) :
    Command(controller, invoker, state) {}
  virtual void execute() {
    m_controller.unset_attributes();
  }
  virtual void unexecute() {
    m_controller.set_attributes();
  }
private:
};

class ToggleAttributesCommand : public Command {
public:
  ToggleAttributesCommand(Controller &controller, Invoker &invoker,
                          IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {
    m_controller.toggle_attributes();
  }
  virtual void unexecute() {
    m_controller.toggle_attributes();
  }
private:
};

#endif //__BROWSING_H__