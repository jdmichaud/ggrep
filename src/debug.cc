#include <signal.h>
#include <execinfo.h>
/* get REG_EIP from ucontext.h */
#ifndef __USE_GNU
# define __USE_GNU
#endif
#ifndef _XOPEN_SOURCE
# define _XOPEN_SOURCE
#endif
#include <ucontext.h>

#include "debug.h"
#include "logmacros.h"

/*
 * Ugly function to print a backtrace in case of crash.
 * Should probably be somewhere else
 */
void bt_sighandler(int sig, siginfo_t *info,
                   void *secret) {

  void *trace[16];
  char **messages = (char **)nullptr;
  int i, trace_size = 0;
  ucontext_t *uc = (ucontext_t *)secret;

  /* Do something useful with siginfo_t */
  if (sig == SIGSEGV) {
#ifdef __APPLE__
    LOGERR("Got signal " << sig << ", faulty address is " << info->si_addr);
#else
    LOGERR("Got signal " << sig << ", faulty address is " << info->si_addr <<
           ", from " << uc->uc_mcontext.gregs[PC_REG]);
#endif
  }
  else {
    LOGERR("Got signal " << sig);
  }

  trace_size = backtrace(trace, 16);
  /* overwrite sigaction with caller's address */
#ifdef __APPLE__
  trace[1] = 0;
#else
  trace[1] = (void *) uc->uc_mcontext.gregs[PC_REG];
#endif

  messages = backtrace_symbols(trace, trace_size);
  /* skip first stack frame (points here) */
  LOGERR("[bt] Execution path:\n");
  for (i=1; i<trace_size; ++i)
  {
    LOGERR("[bt] #" << i << " " << messages[i]);

    /* find first occurence of '(' or ' ' in message[i] and assume
     * everything before that is the file name. (Don't go beyond 0 though
     * (string terminator)*/
    //size_t p = 0; this generates a warning ...
    unsigned int p = 0;
    while(messages[i][p] != '(' && messages[i][p] != ' '
            && messages[i][p] != 0)
        ++p;

    char syscom[256];
    // last parameter is the filename of the symbol
    sprintf(syscom,"addr2line %p -e %.*s", trace[i] , p, messages[i] );
    // Execute the command and set its result in a file
    FILE *addr2line_output_file = popen(syscom, "r");
    char addr2line_output[MAX_BT_SIZE];
    // Read the file and log it. Max size is MAX_BT_SIZE
    fread(addr2line_output, sizeof(char), MAX_BT_SIZE, addr2line_output_file);
    LOGERR(addr2line_output);
    // Close the output file
    pclose(addr2line_output_file);
  }
  exit(0);
}
