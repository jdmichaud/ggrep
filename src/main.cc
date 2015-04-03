#include <fstream>
#include "logmacros.h"
#include "terminal_view.h"

int main(int argc, char **argv) {
  ContentModel content_model;
  FBarModel fbar_model;
  PromptModel prompt_model;
  TerminalView view(content_model, fbar_model, prompt_model);
  view.init();
  view.run();
  return 0;
}