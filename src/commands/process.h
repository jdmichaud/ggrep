/*! \brief Commands to manage ggrep process
 *
 */
#include <stdlib.h>

class ExitCommand : public Command {
public:
  ExitCommand(Controller &controller, int exitcode = 0) : 
    Command(controller), m_exitcode(exitcode) {}

  virtual void execute() {
    exit(m_exitcode);
  }
  virtual void unexecute() {
    // non unexecutable
  }
  virtual bool unexecutable() { return false; };

private:
  uint m_exitcode;
};
