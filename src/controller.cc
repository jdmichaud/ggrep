#include <thread>
#include <functional>

#include "types.h"
#include "logmacros.h"
#include "controller.h"
#include "event.h"
#include "command.h"
#include "input.h"

using std::placeholders::_1;

#define MULTI_THREADED_USER_INPUT 1

Controller::Controller(BrowserModel &browser_model,
                       FBarModel &fbar_model,
                       PromptModel &prompt_model,
                       StateModel &state_model,
                       IBufferFactory &buffer_factory) :
  _interrupted(false),
  _browser_model(browser_model),
  _fbar_model(fbar_model),
  _prompt_model(prompt_model),
  _state_model(state_model),
  _buffer_factory(buffer_factory),
  _context(Context(*this)),
  _user_event_producer(_event_queue),
  _processor_input_producer(_event_queue),
  _event_consumer(_event_queue),
  _main_thread_id(std::this_thread::get_id())
  {
    _browser_model.register_observer(std::bind( &Controller::route_callback, this, REDRAW_BROWSER, _1 ));
    _fbar_model.register_observer(std::bind( &Controller::route_callback, this, REDRAW_FBAR, _1 ));
    _prompt_model.register_observer(std::bind( &Controller::route_callback, this, REDRAW_PROMPT, _1 ));
    _state_model.register_observer(std::bind( &Controller::route_callback, this, REDRAW_STATE, _1 ));
  }

void Controller::bind_view(IView &view) {
  _views.push_back(&view);
  // Registering the size of the view in the model
  // TODO Multi-view: Manage here a view_model containing the data of all the
  // binded views
  uint nlines = 0, ncols = 0;
  view.get_view_size(nlines, ncols);
  set_view_size(nlines, ncols);
}

void Controller::route_callback(uint event_id, IObservable &observable) {
  (*this)._route_callback(event_id);
}

void Controller::_route_callback(uint event_id) {
  // Are we on the main thread ?
  if (_main_thread_id == std::this_thread::get_id()) {
    // yes, then call the views directly
    switch (event_id) {
      case REDRAW_BUFFER:
        for (auto view: _views) view->notify_buffer_changed();
        break;
      case REDRAW_BROWSER:
        for (auto view: _views) view->notify_browser_changed();
        break;
      case REDRAW_FBAR:
        for (auto view: _views) view->notify_fbar_changed();
        break;
      case REDRAW_PROMPT:
        for (auto view: _views) view->notify_prompt_changed();
        break;
      case REDRAW_STATE:
        for (auto view: _views) view->notify_state_changed();
        break;
      default:
        // Browser callback will probably trigger a redraw all.
        for (auto view: _views) view->notify_browser_changed();
      }
  } else {
    // otherwise, inject a REDRAW event
    (*this).inject(Redraw(event_id));
  }
}

void Controller::compress_redraw_event() {
  // Peep the following event
  Event const *next_e = nullptr;
  next_e = _event_consumer.peep();
  // As long as we have REDRAW event
  int compressed_event = 0;
  while (next_e != nullptr && isredraw(next_e->get_eventid())) {
    // pop them
    _event_consumer.take();
    next_e = _event_consumer.peep();
    ++compressed_event;
  }
}

#ifdef MULTI_THREADED_USER_INPUT
void Controller::start() {
  // Spawn the view thread
  // TODO Multi-view: Only one thread started here, should start the thread in
  // bind_view. But to make it simple for now it is started here. Otherwose we
  // need move the thread variable in the class to be able to join it here...
  std::thread input_thread(std::bind(&Controller::view_start, this));
  // While we are not interrupted ...
  while (!_interrupted)
  {
    // ... consumer event and do something about it
    try {
      // Take the event to handle
      const Event e = _event_consumer.take();
      // If the event to handle is a REDRAW event of some kind, try to compress
      if (isredraw(e.get_eventid())) compress_redraw_event();
      _context.inject(e);
    } catch (UnhandledEvent e) {
      LOGERR("While injecting event into context, UnhandledEvent: " << e.what());
    } catch (std::runtime_error e) {
      LOGERR("While injecting event into context, runtime_error: " << e.what());
    }
  }
  // Join the input thread
  input_thread.join();
}

void Controller::view_start() {
#else
void Controller::start() {
#endif
  // TODO Multi-view: For now, let's assume one view with a blocking prompt
  if (_views.size() != 0)
    while (!_interrupted)
      _views.front()->prompt(*this);
}

void Controller::inject_key(int key) {
 LOGDBG("Controller::inject_key called with key: " << key <<
         " (" << std::oct << key << std::dec << ")");
  try {
    (*this).inject(_event_factory.build_event(key));
  } catch (std::runtime_error e) {
    LOGERR("While creating event, runtime_error: " << e.what());
  }
}

void Controller::inject(const Event e) {
  try {
    LOGDBG("e.get_eventid(): " << e.get_eventid());
#ifdef MULTI_THREADED_USER_INPUT
    _user_event_producer.produce(std::move(e));
#else
    _context.inject(e);
#endif
  } catch (UnhandledEvent e) {
    LOGERR("While creating event, UnhandledEvent: " << e.what());
  }
}

void Controller::change_state(state_e state, const IEvent &event) {
  // Change the state in the state model
  _state_model.set_state().update() = state;
  // Change the state in the context
  _context.change_state(_context.get_state(state), event);
}

void Controller::enter_state(state_e state, const IEvent &event) {
  // Change the state in the state model
  _state_model.set_state().update() = state;
  // Change the state in the context
  _context.enter_state(_context.get_state(state), event);
}

void Controller::exit_state(const IEvent &event) {
  // Change the state in the context
  _context.exit_state(event);
  // and in the state model
  LOGDBG("New current state: " << _context.get_current().get_id());
  _state_model.set_state().update() = _context.get_current().get_id();
}

void Controller::backtrack(const IEvent &e) {
  _context.exit_state(e);
  _state_model.set_state().update() = _context.get_current().get_id();
}

bool Controller::create_buffer(const std::string &filepath) {
  try {
    // Create the buffer model
    BufferModel *buffer_model =
      new BufferModel(_buffer_factory.create_buffer(filepath));
    // Attach the controller observer to the newly created buffer
    buffer_model->register_observer(std::bind( &Controller::route_callback,
                                               this, REDRAW_BUFFER, _1 ));
    // Add the buffer model to the browser model
    _browser_model.emplace_buffer(
      std::move(std::unique_ptr<BufferModel>(buffer_model)));
    // Generate the FILE_OPENED event into the state machine
    _context.inject(Event(FILE_OPENED));
  }
  catch (std::runtime_error e) {
    LOGERR("runtime exception creating buffer: " << e.what());
    // TODO: find a way to display an error. Popup or message in prompt ?
    return false;
  } catch (OpenFileException e) {
    LOGERR("OpenFileException creating buffer: " << e.what());
    raise_error(e.what());
    return false;
  }
  return true;
}

buffer_list::const_iterator Controller::get_active_buffer() {
  return _browser_model.get_current_buffer();
}

buffer_list::const_iterator Controller::get_nth_buffer(uint idx) const {
  return std::next(_browser_model.get_buffers().begin(), idx);
}

void Controller::activate_buffer(uint idx) {
  _browser_model.set_current_buffer().update() =
    _browser_model.set_nth_buffer(idx).update();
}

void Controller::activate_buffer(buffer_list::const_iterator it) {
  _browser_model.set_current_buffer(it);
}

/*
 * Prompt manipulation
 */
void Controller::set_prompt(const std::string &prompt) {
  _prompt_model.set_prompt().update() = prompt;
}

void Controller::set_prompt_cursor_position(const pos &p) {
  _prompt_model.set_cursor_position().update() = p;
}

void Controller::clear_prompt() {
  _prompt_model.set_prompt().update().clear();
}

const std::string &Controller::get_prompt() const {
  return _prompt_model.get_prompt();
}

/*
 * Error, Warnings and Questions
 */
void Controller::raise_error(const std::string &error_msg) {
  PromptMessage msg(ERROR, error_msg);
  (*this).enter_state(state_e::ERROR_STATE, msg);
}

/*
 * Browsing APIs
 */
void Controller::set_view_size(uint nlines, uint ncolumns) {
  _browser_model.set_view_line_number().update() = nlines;
}

void Controller::scroll_buffer_up(uint shift) {
  const std::unique_ptr<BufferModel> &buffer = (*_browser_model.get_current_buffer());
  if (buffer->get_first_line_displayed() >= shift)
    buffer->set_first_line_displayed(buffer->get_first_line_displayed() - shift);
  else
    buffer->set_first_line_displayed(0);
}

void Controller::scroll_buffer_down(uint shift) {
  const std::unique_ptr<BufferModel> &buffer = (*_browser_model.get_current_buffer());
  // TODO: Get rid of constant. Prompt and header might take more than 1 line each!
  if ((_browser_model.get_view_line_number() - 2) > buffer->get_number_of_line()) {
    // Do nothing
  }
  else if (buffer->get_first_line_displayed() + shift >
           (buffer->get_number_of_line() - (_browser_model.get_view_line_number() - 2))) {
    buffer->set_first_line_displayed(
      buffer->get_number_of_line() - _browser_model.get_view_line_number() + 2);
  } else {
    buffer->set_first_line_displayed(buffer->get_first_line_displayed() + shift);
  }
}

void Controller::scroll_buffer_page_up() {
  scroll_buffer_up(_browser_model.get_view_line_number());
}

void Controller::scroll_buffer_page_down() {
  scroll_buffer_down(_browser_model.get_view_line_number());
}

void Controller::scroll_buffer_end() {
  const std::unique_ptr<BufferModel> &buffer = (*_browser_model.get_current_buffer());
  buffer->set_first_line_displayed(
    buffer->get_number_of_line() - _browser_model.get_view_line_number() + 2);
}

/*
 * Filtering APIs
 */
void Controller::add_filter(const std::string &filter) {
  LOGDBG("add a new filter: " << filter);
  // Add the new regex to the set of filters of the current buffer
  const std::unique_ptr<BufferModel> &buffer = (*_browser_model.get_current_buffer());
  buffer->set_filter_set().update().filters.emplace_back(
    std::move(std::make_pair(filter, std::regex(filter))));
  LOGDBG("buffer->get_filter_set().filters.size(): " << buffer->get_filter_set().filters.size());
  // Signal the filtering processor
  buffer->m_filter.signal();
  // Switch to filtered buffer
  buffer->enable_filtering();
}

void Controller::enable_filtering_on_current_buffer() {
  const std::unique_ptr<BufferModel> &buffer = (*_browser_model.get_current_buffer());
  buffer->enable_filtering();
}

void Controller::disable_filtering_on_current_buffer() {
  const std::unique_ptr<BufferModel> &buffer = (*_browser_model.get_current_buffer());
  buffer->disable_filtering();
}

void Controller::clear_filtering_on_current_buffer() {
  LOGDBG("reinit current buffer");
  const std::unique_ptr<BufferModel> &buffer = (*_browser_model.get_current_buffer());
  // Clear the filter set
  buffer->set_filter_set().update().filters.clear();
}

void Controller::switch_filter_type() {
  const std::unique_ptr<BufferModel> &buffer = (*_browser_model.get_current_buffer());
  buffer->set_filter_set().update().land = !buffer->get_filter_set().land;
  // Signal the filtering processor
  buffer->m_filter.signal();
}