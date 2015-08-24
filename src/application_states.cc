#include "context.h"
#include "application_states.h"
#include "input.h"
#include "commands/process.h"
#include "commands/filemgt.h"
#include "commands/browsing.h"
#include "commands/misc.h"
#include "commands/state_cmd.h"
#include "commands/filtering.h"

#define CLOSE_STATE_PROMPT "filename: "
#define ADD_FILTER_STATE_PROMPT "add filter: "

CloseState::CloseState(Context &context, Controller &controller,
                       IState *parent_state) :
  State(context, controller, parent_state,
    {
      { new Ctrl(KEY_ESC),      [this](const IEvent& b) { m_invoker.create_and_execute<ExitCommand>(); } },
      { new Ctrl(MY_KEY_ENTER), [this](const IEvent& b) { m_invoker.create_and_execute<OpenFileCommand>(m_text); } },
      { new Ctrl(KEY_BACKSPACE),[this](const IEvent& b) { m_invoker.create_and_execute<BackspaceCommand>(this); } },
      { new Ctrl(KEY_DC),       [this](const IEvent& b) { m_invoker.create_and_execute<DeleteCommand>(this); } },
      { new Arrow(KEY_LEFT),    [this](const IEvent& b) { m_invoker.create_and_execute<LeftCommand>(this); } },
      { new Arrow(KEY_RIGHT),   [this](const IEvent& b) { m_invoker.create_and_execute<RightCommand>(this); } },
      { new Arrow(C_KEY_LEFT),  [this](const IEvent& b) { m_invoker.create_and_execute<LeftWordCommand>(this); } },
      { new Arrow(C_KEY_RIGHT), [this](const IEvent& b) { m_invoker.create_and_execute<RightWordCommand>(this); } },
      { new Nav(KEY_HOME),      [this](const IEvent& b) { m_invoker.create_and_execute<BegLineCommand>(this); } },
      { new Nav(KEY_END),       [this](const IEvent& b) { m_invoker.create_and_execute<EndLineCommand>(this); } },
      { new Event(FILE_OPENED), [this](const IEvent& b) { m_invoker.create_and_execute<ChangeStateCommand, state_e, const IEvent &>(state_e::OPEN_STATE, b); } },
      { new Printable(KLEENE),  [this](const IEvent& b) { m_invoker.create_and_execute<EnterChar, const IEvent &>(b, this); } }
    }, state_e::CLOSE_STATE)
{ }

void CloseState::enter(const IEvent &e) {
  LOGDBG("entering CloseState");
  // initialize the text string
  (*this).clear();
  // Update the model
  update();
}

void CloseState::resume(const IEvent &e) {
  LOGDBG("resume CloseState");
  // Update the model
  update();
}

void CloseState::exit(const IEvent &) {
  LOGDBG("exiting CloseState")
  (*this).clear();
  m_controller.set_prompt("");
  pos curpos = m_cur_pos;
  curpos.x += 0;
  m_controller.set_prompt_cursor_position(curpos);
}

void CloseState::update() {
  m_controller.set_prompt(CLOSE_STATE_PROMPT + m_text);
  pos curpos = m_cur_pos;
  curpos.x += strlen(CLOSE_STATE_PROMPT);
  m_controller.set_prompt_cursor_position(curpos);
}


OpenState::OpenState(Context &context, Controller &controller,
                     IState *parent_state) :
  State(context, controller, parent_state,
    {
      { new Ctrl(KEY_ESC),      [this](const IEvent& b) { m_invoker.create_and_execute<ExitCommand>(); } },
      { new Printable('q'),     [this](const IEvent& b) { m_invoker.create_and_execute<ExitCommand>(); } },
      { new Event(FILE_CLOSED), [this](const IEvent& b) { m_invoker.create_and_execute<FileClosedCommand>(); } }
    }, state_e::OPEN_STATE)
{}

void OpenState::enter(const IEvent &b) {
  LOGDBG("entering OpenState");
  m_invoker.create_and_execute<EnterStateCommand, state_e, const IEvent &>(state_e::BROWSE_STATE, b);
}

BrowseState::BrowseState(Context &context, Controller &controller,
                         IState *parent_state) :
  State(context, controller, parent_state,
    {
      { new Ctrl(KEY_ACK),    [this](const IEvent& b) { m_invoker.create_and_execute<EnterStateCommand, state_e, const IEvent &>(state_e::FILTER_STATE, b); } },
      { new Arrow(KEY_DOWN),  [this](const IEvent& b) { m_invoker.create_and_execute<DownCommand>(); } },
      { new Arrow(KEY_UP),    [this](const IEvent& b) { m_invoker.create_and_execute<UpCommand>(); } },
      { new Arrow(KEY_NPAGE), [this](const IEvent& b) { m_invoker.create_and_execute<PageDownCommand>(); } },
      { new Arrow(KEY_PPAGE), [this](const IEvent& b) { m_invoker.create_and_execute<PageUpCommand>(); } },
      { new Arrow(KEY_END),   [this](const IEvent& b) { m_invoker.create_and_execute<EndCommand>(); } },
      { new Arrow(C_KEY_END), [this](const IEvent& b) { m_invoker.create_and_execute<EndCommand>(); } },
      { new Arrow(KEY_HOME),  [this](const IEvent& b) { m_invoker.create_and_execute<HomeCommand>(); } },
      { new Arrow(C_KEY_HOME),[this](const IEvent& b) { m_invoker.create_and_execute<HomeCommand>(); } }
    }, state_e::BROWSE_STATE)
{}

FilterState::FilterState(Context &context, Controller &controller,
                         IState *parent_state) :
  State(context, controller, parent_state,
    {
      { new Ctrl(KEY_ESC),      [this](const IEvent& b) { m_invoker.create_and_execute<BacktrackCommand>(); } }
    }, state_e::FILTER_STATE)
{ }

void FilterState::enter(const IEvent &e) {
  LOGDBG("entering FilterState");
  m_invoker.create_and_execute<EnterStateCommand, state_e, const IEvent &>(state_e::ADD_FILTER_STATE, e);
}

void FilterState::exit(const IEvent &e) {
  LOGDBG("exiting FilterState");
  m_controller.reinit_current_buffer();
}

AddFilterState::AddFilterState(Context &context, Controller &controller,
                         IState *parent_state) :
  State(context, controller, parent_state,
    {
      { new Ctrl(KEY_ESC),      [this](const IEvent& b) { m_invoker.create_and_execute<BacktrackCommand>(); } },
      { new Ctrl(MY_KEY_ENTER), [this](const IEvent& b) { m_invoker.create_and_execute<AddFilterCommand>(m_text); } },
      { new Ctrl(KEY_BACKSPACE),[this](const IEvent& b) { m_invoker.create_and_execute<BackspaceCommand>(this); } },
      { new Ctrl(KEY_DC),       [this](const IEvent& b) { m_invoker.create_and_execute<DeleteCommand>(this); } },
      { new Arrow(KEY_LEFT),    [this](const IEvent& b) { m_invoker.create_and_execute<LeftCommand>(this); } },
      { new Arrow(KEY_RIGHT),   [this](const IEvent& b) { m_invoker.create_and_execute<RightCommand>(this); } },
      { new Arrow(C_KEY_LEFT),  [this](const IEvent& b) { m_invoker.create_and_execute<LeftWordCommand>(this); } },
      { new Arrow(C_KEY_RIGHT), [this](const IEvent& b) { m_invoker.create_and_execute<RightWordCommand>(this); } },
      { new Nav(KEY_HOME),      [this](const IEvent& b) { m_invoker.create_and_execute<BegLineCommand>(this); } },
      { new Nav(KEY_END),       [this](const IEvent& b) { m_invoker.create_and_execute<EndLineCommand>(this); } },
      { new Printable(KLEENE),  [this](const IEvent& b) { m_invoker.create_and_execute<EnterChar, const IEvent &>(b, this); } }
    }, state_e::FILTER_STATE)
{ }

void AddFilterState::enter(const IEvent &e) {
  LOGDBG("entering AddFilterState");
  // initialize the text string
  (*this).clear();
  // Update the model
  update();
}

void AddFilterState::resume(const IEvent &e) {
  LOGDBG("resume AddFilterState");
  // Update the model
  update();
}

void AddFilterState::exit(const IEvent &) {
  LOGDBG("exiting AddFilterState");
  (*this).clear();
  m_controller.set_prompt("");
  pos curpos = m_cur_pos;
  curpos.x += 0;
  m_controller.set_prompt_cursor_position(curpos);
}

void AddFilterState::update() {
  LOGDBG("AddFilterState::update begin");
  m_controller.set_prompt(ADD_FILTER_STATE_PROMPT + m_text);
  pos curpos = m_cur_pos;
  curpos.x += strlen(ADD_FILTER_STATE_PROMPT);
  m_controller.set_prompt_cursor_position(curpos);
}

ErrorState::ErrorState(Context &context, Controller &controller,
                       IState *parent_state) :
  State(context, controller, parent_state,
    {
      { new Ctrl(KEY_ESC),      [this](const IEvent& b) { m_invoker.create_and_execute<BacktrackCommand>(); } },
      { new Ctrl(MY_KEY_ENTER), [this](const IEvent& b) { m_invoker.create_and_execute<BacktrackCommand>(); } }
    }, state_e::ERROR_STATE)
{}

void ErrorState::enter(const IEvent &e) {
  LOGDBG("entering ErrorState");
  m_controller.set_prompt(*(const std::string *) e.get_data());
}

void ErrorState::exit(const IEvent &) {
  LOGDBG("exiting ErrorState")
}
