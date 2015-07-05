#include <fstream>
#include "logmacros.h"
#include "terminal_view.h"
#include "controller.h"

int main(int argc, char **argv) {
  BrowserModel browser_model;
  FBarModel fbar_model;
  PromptModel prompt_model;

  TerminalView view(browser_model, fbar_model, prompt_model);
  view.init();

  Controller controller(browser_model, fbar_model, prompt_model);
  controller.bind_view(view);
  controller.start();
  return 0;
}