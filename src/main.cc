#include <getopt.h>
#include <fstream>

#include "logmacros.h"
#include "terminal_view.h"
#include "controller.h"
#include "debug.h"
#include "exceptions.h"

#define VERSION 0.3

int version() {
  std::cout << "ggrep " << VERSION;
#if DEBUG
  std::cout << " -- debug version";
#endif
  std::cout << std::endl;
  std::cout << "Copyright (C) 2015 Jean-Daniel Michaud." << std::endl;
  std::cout << "License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>." << std::endl;
  std::cout << "This is free software: you are free to change and redistribute it." << std::endl;
  std::cout << "There is NO WARRANTY, to the extent permitted by law." << std::endl;
  std::cout << std::endl;
  std::cout << "Written by Jean-Daniel Michaud" << std::endl;
  return 0;
}

int help() {
  std::cout << "Usage: ggrep [OPTION]... [FILE]..." << std::endl;
  return 0;
}

int do_version, do_help;
struct option longopts[] = {
   { "version", no_argument,       & do_version,    1   },
   { "help",    no_argument,       & do_help,       1   },
   { 0, 0, 0, 0 }
};

void manage_options(int argc, char **argv, char *&filename) {
  // If not filename is provided, the value is NULL
  filename = nullptr;
  char c;
  while ((c = getopt_long(argc, argv, "vh", longopts, NULL)) != -1) {
    switch (c) {
    case 0: /* getopt_long() set a variable, just keep going */
      break;
    case 'h':
      help();
      exit(0);
    case 'v':
      version();
      exit(0);
    case 1:
      break;
    case ':':   /* missing option argument */
      std::cerr << argv[0] << "option `-" << optopt
                << "' requires an argument" << std::endl;
      break;
    case '?':
    default:    /* invalid option */
      std::cerr << argv[0] << "option `-" << optopt
                << "' is invalid: ignored" << std::endl;
      break;
    }
  }
  // Treat non option argument (filename)
  for (int i = optind; i < argc; i++) // TODO: manage all filenames
    filename = argv[i];
}

Controller *g_controller = NULL;

void terminate(int sig, siginfo_t *info, void *oldact) {
  // Mask SIGINT signal
  sigset_t set;
  sigaddset(&set, SIGINT);
  if (pthread_sigmask(SIG_BLOCK, &set, NULL) != 0)
    LOGERR("pthread_sigmask failed");
    // We continue anyway
  LOGINF("User abortion requested");
  if (g_controller) {
    g_controller->interrupt();
    // Inject whatever just to unblock the waiting thread
    g_controller->inject(0);
  } else {
    LOGERR("Controller is not instanciated, expect undefined behavior");
  }
}

void install_sigsegv_handler() {
  // Setup SIGINT handlers
  struct sigaction sa_int;
  sa_int.sa_sigaction = terminate;
  sigemptyset (&sa_int.sa_mask);
  sa_int.sa_flags = SA_RESTART | SA_SIGINFO;
  sigaction(SIGINT, &sa_int, NULL);
}

void install_sigint_handler() {
  // Setup SIGSEGV handlers
  struct sigaction sa;
  sa.sa_sigaction = bt_sighandler;
  sigemptyset (&sa.sa_mask);
  sa.sa_flags = SA_RESTART | SA_SIGINFO;
  sigaction(SIGSEGV, &sa, NULL);
}

int main(int argc, char **argv) {
  LOGDBG("main starts here --------------------------------------------------");
  // For the stack trace in case of a crash
  install_sigsegv_handler();
  // Parse the options
  char *filename = nullptr;
  manage_options(argc, argv, filename);
  // Create the various model used by the application
  BrowserModel  browser_model;
  FBarModel     fbar_model;
  PromptModel   prompt_model;
  StateModel    state_model;
  BufferFactory buffer_factory;
  // Create a ncurses terminal view. Could also be a Windows terminal view or a
  // Qt window for example.
  TerminalView view(browser_model, fbar_model, prompt_model, state_model);
  view.init();
  // Create the controller
  Controller controller(browser_model, fbar_model, prompt_model, state_model,
                        buffer_factory);
  // Install interrupt routine
  g_controller = &controller;
  install_sigint_handler();
  // Bind the view to the controller so the controller knows where to listen for
  // inputs
  controller.bind_view(view);
  // If a filename was provided, create a buffer in the controller
  if (filename != nullptr) controller.create_buffer(filename);
  // Create the thread that listens to the user inputs and interprets those
  // inputs
  controller.start();
  return 0;
}
