#include <fstream>
#include "logmacros.h"
#include "terminal_view.h"
#include "controller.h"

int main(int argc, char **argv) {
  ContentModel content_model;
  FBarModel fbar_model;
  PromptModel prompt_model;

  TerminalView view(content_model, fbar_model, prompt_model);
  view.init();

  Controller controller(content_model, fbar_model, prompt_model);
  controller.bind_view(view);
  controller.start();
  return 0;
}