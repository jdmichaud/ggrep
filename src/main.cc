#include <fstream>

#include "logmacros.h"
#include "terminal_view.h"
#include "controller.h"
#include "debug.h"

int main(int argc, char **argv) {
  LOGDBG("main starts here --------------------------------------------------");
  // For stack trace in case of crash
  struct sigaction sa;
  sa.sa_sigaction = bt_sighandler;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_SIGINFO;
  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);

  BrowserModel  browser_model;
  FBarModel     fbar_model;
  PromptModel   prompt_model;
  StateModel    state_model;
  BufferFactory buffer_factory;

  TerminalView view(browser_model, fbar_model, prompt_model, state_model);
  view.init();

  Controller controller(browser_model, fbar_model, prompt_model, state_model, 
                        buffer_factory);
  controller.bind_view(view);
  controller.start();
  return 0;
}