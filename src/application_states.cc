#include "constants.h"
#include "context.h"
#include "application_states.h"
#include "input.h"
#include "commands/process.h"
#include "commands/filemgt.h"
#include "commands/browsing.h"
#include "commands/misc.h"
#include "commands/state_cmd.h"
#include "commands/filtering.h"

DefaultState::DefaultState(Context &context, Controller &controller,
                           IState *parent_state) :
  State(context, controller, parent_state,
    {
      { new Event(REDRAW_BUFFER), [&controller](const IEvent& b) { controller._route_callback(REDRAW_BROWSER); } },
      { new Event(REDRAW_BROWSER),[&controller](const IEvent& b) { controller._route_callback(REDRAW_BROWSER); } },
      { new Event(REDRAW_FBAR),   [&controller](const IEvent& b) { controller._route_callback(REDRAW_FBAR); } },
      { new Event(REDRAW_PROMPT), [&controller](const IEvent& b) { controller._route_callback(REDRAW_PROMPT); } },
      { new Event(REDRAW_STATE),  [&controller](const IEvent& b) { controller._route_callback(REDRAW_STATE); } },
      { new Event(REDRAW_ALL),    [&controller](const IEvent& b) { controller._route_callback(REDRAW_ALL); } },
    }, state_e::DEFAULT_STATE)
{ }

CloseState::CloseState(Context &context, Controller &controller,
                       IState *parent_state) :
  State(context, controller, parent_state, {}, state_e::CLOSE_STATE)
{ }

void CloseState::enter(const IEvent &e) {
  LOGDBG("entering CloseState");
  // We never stay in CloseState, but always go in opening state
  m_invoker.create_and_execute<EnterStateCommand, state_e, const IEvent &>(state_e::OPENING_STATE, e);
}

OpeningState::OpeningState(Context &context, Controller &controller,
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
      { new Event(FILE_OPENING),[this](const IEvent& b) { m_controller.set_prompt("");
                                                          pos curpos = m_cur_pos;
                                                          curpos.x += 0;
                                                          m_controller.set_prompt_cursor_position(curpos); } },
      { new Event(FILE_OPENED), [this](const IEvent& b) { m_invoker.create_and_execute<ChangeStateCommand,
                                                                                       state_e,
                                                                                       const IEvent &>(state_e::OPEN_STATE, b); } },
      { new Printable(KLEENE),  [this](const IEvent& b) { m_invoker.create_and_execute<EnterChar, const IEvent &>(b, this); } }
    }, state_e::OPENING_STATE)
{}

void OpeningState::enter(const IEvent &e) {
  LOGDBG("entering OpeningState");
  // initialize the text string
  (*this).clear();
  // Update the model
  update();
}

void OpeningState::resume(const IEvent &e) {
  LOGDBG("resume OpeningState");
  // Update the model
  update();
}

void OpeningState::exit(const IEvent &) {
  LOGDBG("exiting OpeningState")
  (*this).clear();
  m_controller.set_prompt("");
  pos curpos = m_cur_pos;
  curpos.x += 0;
  m_controller.set_prompt_cursor_position(curpos);
}

void OpeningState::update() {
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
      { new Event(FILE_CLOSED), [this](const IEvent& b) { m_invoker.create_and_execute<FileClosedCommand>(); } },
      { new Printable('/'),     [this](const IEvent& b) { m_invoker.create_and_execute<EnterStateCommand, state_e, const IEvent &>(state_e::SEARCH_STATE, b);
                                                          m_invoker.create_and_execute<InjectCommand>(b); } }
      { new Printable('?'),     [this](const IEvent& b) { m_invoker.create_and_execute<EnterStateCommand, state_e, const IEvent &>(state_e::SEARCH_STATE, b);
                                                          m_invoker.create_and_execute<InjectCommand>(b); } }
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
      // Ctrl+F = KEY_ACK
      { new Ctrl(KEY_ACK),    [this](const IEvent& b) { m_invoker.create_and_execute<EnterStateCommand, state_e, const IEvent &>(state_e::FILTER_STATE, b);
                                                        m_invoker.create_and_execute<InjectCommand>(KEY_ACK); } },
      { new Ctrl(KEY_SHD),    [this](const IEvent& b) { m_invoker.create_and_execute<ToggleAttributesCommand>(); } },
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
      // Ctrl+F = KEY_ACK
      { new Ctrl(KEY_ACK),    [this](const IEvent& b) { m_invoker.create_and_execute<EnterStateCommand, state_e, const IEvent &>(state_e::ADD_FILTER_STATE, b); } },
      { new Ctrl(KEY_ESC),    [this](const IEvent& b) { m_invoker.create_and_execute<ResetFiltering>();
                                                        m_invoker.create_and_execute<BacktrackCommand>(); } },
      // Ctrl + O = KEY_SIN
      { new Ctrl(KEY_SIN),    [this](const IEvent& b) { m_invoker.create_and_execute<SwitchFilterType>(); } },
    }, state_e::FILTER_STATE)
{ }

void FilterState::enter(const IEvent &e) {
  LOGDBG("entering FilterState");
  m_invoker.create_and_execute<EnableFiltering>();
}

void FilterState::exit(const IEvent &e) {
  LOGDBG("exiting FilterState");
  m_invoker.create_and_execute<DisableFiltering>();
  m_controller.clear_filtering_on_current_buffer();
}

void FilterState::resume(const IEvent &e) {
  LOGDBG("resume FilterState");
  // If no filter set, just bail
  if (m_controller.get_number_of_filter_on_current_buffer() == 0) {
    m_invoker.create_and_execute<BacktrackCommand>();
  }
}

AddFilterState::AddFilterState(Context &context, Controller &controller,
                         IState *parent_state) :
  State(context, controller, parent_state,
    {
      { new Ctrl(KEY_ESC),      [this](const IEvent& b) { m_invoker.create_and_execute<CancelCurrentFilterEntry>();
                                                          m_invoker.create_and_execute<BacktrackCommand>(); } },
      { new Ctrl(MY_KEY_ENTER), [this](const IEvent& b) { m_invoker.create_and_execute<EnterCurrentFilterEntry>();
                                                          m_invoker.create_and_execute<BacktrackCommand>(); } },
      { new Ctrl(KEY_BACKSPACE),[this](const IEvent& b) { m_invoker.create_and_execute<BackspaceCommand>(this);
                                                          m_invoker.create_and_execute<UpdateCurrentFilterEntry,
                                                                                       const std::string &,
                                                                                       const IEvent &>(m_text, b); } },
      { new Ctrl(KEY_DC),       [this](const IEvent& b) { m_invoker.create_and_execute<DeleteCommand>(this);
                                                          m_invoker.create_and_execute<UpdateCurrentFilterEntry,
                                                                                       const std::string &,
                                                                                       const IEvent &>(m_text, b); } },
      { new Arrow(KEY_LEFT),    [this](const IEvent& b) { m_invoker.create_and_execute<LeftCommand>(this); } },
      { new Arrow(KEY_RIGHT),   [this](const IEvent& b) { m_invoker.create_and_execute<RightCommand>(this); } },
      { new Arrow(C_KEY_LEFT),  [this](const IEvent& b) { m_invoker.create_and_execute<LeftWordCommand>(this); } },
      { new Arrow(C_KEY_RIGHT), [this](const IEvent& b) { m_invoker.create_and_execute<RightWordCommand>(this); } },
      { new Nav(KEY_HOME),      [this](const IEvent& b) { m_invoker.create_and_execute<BegLineCommand>(this); } },
      { new Nav(KEY_END),       [this](const IEvent& b) { m_invoker.create_and_execute<EndLineCommand>(this); } },
      { new Printable(KLEENE),  [this](const IEvent& b) { m_invoker.create_and_execute<EnterChar, const IEvent &>(b, this);
                                                          m_invoker.create_and_execute<UpdateCurrentFilterEntry,
                                                                                       const std::string &,
                                                                                       const IEvent &>(m_text, b); } }
    }, state_e::FILTER_STATE)
{ }

void AddFilterState::enter(const IEvent &e) {
  LOGDBG("entering AddFilterState");
  // initialize the text string
  (*this).clear();
  // Update the model
  update();
  // Create the current filter entry in the filter set of the FilteringProcessor
  m_invoker.create_and_execute<CreateCurrentFilterEntry>();
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

SearchState::SearchState(Context &context, Controller &controller,
                       IState *parent_state) :
  State(context, controller, parent_state,
    {
      { new Ctrl(KEY_ESC),      [this](const IEvent& b) { (*this).clear();
                                                          m_invoker.create_and_execute<BacktrackCommand>(); } },
      { new Printable('/'),     [this](const IEvent& b) { ; } },
      { new Printable('?'),     [this](const IEvent& b) { ; } },
      { new Ctrl(MY_KEY_ENTER), [this](const IEvent& b) { m_invoker.create_and_execute<BacktrackCommand>(); } },
      { new Printable(KLEENE),  [this](const IEvent& b) { m_invoker.create_and_execute<EnterChar, const IEvent &>(b, this);
                                                          m_invoker.create_and_execute<UpdateSearchTerm,
                                                                                       const std::string &,
                                                                                       const IEvent &>(m_text, b);
                                                          (*this).perform_search(); } }
    }, state_e::SEARCH_STATE), m_forward_search(true)
{}

void SearchState::enter(const IEvent e&) {
  LOGDBG("entering SearchState");
  // Set which way the search will perform
  if (e == '/') (*this).m_forward_search = true;
  else (*this).m_forward_search = false; // else must be '?'
  // Backup the current buffer position
  (*this).m_initial_first_line = m_controller.get_first_line_displayed();
  // Update the model
  update();
}

// TODO: this code is replicated in all OneLiner text
void SearchState::exit(const IEvent e&) {
  LOGDBG("exiting SearchState");
  (*this).clear();
  m_controller.set_prompt("");
  pos curpos = m_cur_pos;
  curpos.x += 0;
  m_controller.set_prompt_cursor_position(curpos);
}

void SearchState::resume(const IEvent &e) {
  LOGDBG("resume SearchState");
  // Update the model
  update();
}

// TODO: this code is replicated in all OneLiner text
void SearchState::update() {
  const char *prompt = (*this).m_forward_search ?
    SEARCH_STATE_PROMPT : RSEARCH_STATE_PROMPT;
  m_controller.set_prompt(prompt + m_text);
  pos curpos = m_cur_pos;
  curpos.x += strlen(prompt);
  m_controller.set_prompt_cursor_position(curpos);
}

void SearchState::perform_search()
{
  if ((*this).m_forward_search)
    m_invoker.create_and_execute<ForwardSearchCommand>();
  else
    m_invoker.create_and_execute<ReverseSearchCommand>();
}
