#include "types.h"
#include "logmacros.h"
#include "controller.h"
#include "event.h"
#include "command.h"
#include "input.h"

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
  _event_consumer(_event_queue)
  {}

void Controller::bind_view(IView &view) {
  _views.push_back(&view);
  // Registering the size of the view in the model
  // TODO Multi-view: Manage here a view_model containing the data of all the
  // binded views
  uint nlines = 0, ncols = 0;
  view.get_view_size(nlines, ncols);
  set_view_size(nlines, ncols);
}

// multi threaded input is a little buggy and not sure this is actually useful.
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
      const Event e = _event_consumer.take();
      _context.inject(e);
    } catch (UnhandledEvent e) {
      LOGERR("While injecting event into context, UnhandledEvent: " << e.what());
    } catch (std::runtime_error e) {
      LOGERR("While injecting event into context, runtime_error: " << e.what());
    }
  }
  // We don't join the input_thread, because it will be waiting for an input.
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



void Controller::inject(int key) {
  LOGDBG("Controller::inject called with key: " << key <<
         " (" << std::oct << key << std::dec << ")");
  try {
    const Event e = _input_factory.build_input(key);
    LOGDBG("e.get_eventid(): " << e.get_eventid());
#ifdef MULTI_THREADED_USER_INPUT
    _user_event_producer.produce(std::move(e));
#else
    _context.inject(e);
#endif
  } catch (UnhandledEvent e) {
    LOGERR("While creating event, UnhandledEvent: " << e.what());
  } catch (std::runtime_error e) {
    LOGERR("While creating event, runtime_error: " << e.what());
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
    // Register the views binded to the controller
    for (auto view: _views) {
      // Attach the browser's observer (the view's callback) to the buffer
      view->new_buffer(*buffer_model);
    }
    // Add the buffer model to the browser model
    _browser_model.emplace_buffer(
      std::move(std::unique_ptr<BufferModel>(buffer_model)));
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
    buffer->set_first_line_displayed().update() -= shift;
  else
    buffer->set_first_line_displayed().update() = 0;
}

void Controller::scroll_buffer_down(uint shift) {
  const std::unique_ptr<BufferModel> &buffer = (*_browser_model.get_current_buffer());
  // TODO: Get rid of constant. Prompt and header might take more than 1 line each!
  if ((_browser_model.get_view_line_number() - 2) > buffer->get_number_of_line() ||
      buffer->get_first_line_displayed() + shift >
        (buffer->get_number_of_line() - (_browser_model.get_view_line_number() - 2)))
  {
    buffer->set_first_line_displayed().update() =
      buffer->get_number_of_line() - _browser_model.get_view_line_number() + 2;
  } else {
    buffer->set_first_line_displayed().update() += shift;
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
  buffer->set_first_line_displayed().update() =
    buffer->get_number_of_line() - _browser_model.get_view_line_number() + 2;
}

/*
 * Filtering APIs
 */
void Controller::add_filter(const std::string &filter) {
  LOGDBG("add a new filter: " << filter);
  // Add the new regex to the set of filters of the current buffer
  const std::unique_ptr<BufferModel> &buffer = (*_browser_model.get_current_buffer());
  buffer->set_filter_set().update().filters.emplace_back(std::move(std::regex(filter)));
  // Signal the filtering processor
  buffer->m_filter.signal();
}

void Controller::reinit_current_buffer() {
  LOGDBG("reinit current buffer");
  const std::unique_ptr<BufferModel> &buffer = (*_browser_model.get_current_buffer());
  // Clear the filter set
  buffer->set_filter_set().update().filters.clear();
}
