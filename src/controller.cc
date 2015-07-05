#include "logmacros.h"
#include "controller.h"
#include "event.h"

Controller::Controller(BrowserModel &browser_model, 
                       FBarModel &fbar_model,
                       PromptModel &prompt_model) :
  _interrupted(false),
  _browser_model(browser_model),
  _fbar_model(fbar_model),
  _prompt_model(prompt_model),
  _invoker(this),
  _context(_invoker),
  _current_buffer_idx(0) {

}

void Controller::bind_view(IView &view) {
  _views.push_back(&view);
}

void Controller::inject(int key) {
  LOGDBG("Controller::inject called with key: " << key << " (" << std::oct << key << std::dec << ")");
  if (key == 27) {
    _interrupted = true;
  }
}

void Controller::start() {
  // For now, let's assume one view with a blocking prompt
  if (_views.size() != 0)
    while (!_interrupted)
      _views.front()->prompt(*this);
}

template <typename StreamType>
bool Controller::create_buffer(const std::string &filepath) {
  try {
    _browser_model.set_current_buffer().update().push_back(new BufferModel(new Buffer<StreamType>(filepath)));
    _context.inject(Event(FILE_OPENED));
  }
  catch (std::runtime_error e) {
    LOGERR("runtime exception creating buffer: " << e.what());
    // TODO: find a way to display an error. Popup or message in prompt ?
  }
}

void Controller::activate_buffer(size_type idx) {
  _browser_model.set_current_buffer((*this).get_nth_buffer(idx));
}

void Controller::activate_buffer(buffer_list::iterator it) {
  _browser_model.set_current_buffer(it);
}
