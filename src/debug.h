#include <signal.h>

#define MAX_BT_SIZE 1024
#ifdef __x86_64__
# define PC_REG      REG_RIP
#else
# define PC_REG      REG_EIP
#endif

void bt_sighandler(int sig, siginfo_t *info,
                   void *secret);
