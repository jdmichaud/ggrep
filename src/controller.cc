#include "logmacros.h"
#include "controller.h"

Controller::Controller(ContentModel &content_model, FBarModel &fbar_model, 
                       PromptModel &prompt_model) :
  _content_model(&content_model),
  _fbar_model(&fbar_model),
  _prompt_model(&prompt_model),
  _interrupted(false) {
}

void Controller::bind_view(IView &view) {
  _views.push_back(&view);
}

void Controller::inject(int key) {
  LOGDBG("Controller::inject called with key: " << key);
  if (key == 27) {
    _interrupted = true;
  } else {
    _fbar_model->add_function({ "A", "TOTO", false, false });
  }
}

void Controller::start() {
  // For now, let's assume one view with a blocking prompt
  if (_views.size() != 0)
    while (!_interrupted)
      _views.front()->prompt(*this);
}
