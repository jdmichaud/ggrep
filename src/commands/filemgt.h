/*! \brief All the file management command
 *
 */

#ifndef __FILEMGT_H__
#define __FILEMGT_H__

#include "buffer_model.h"
#include "controller.h"
#include "command.h"

/*!
 * This command is used to open a file whose filename is passed as parameters
 */
class OpenFileCommand : public Command {
public:
  OpenFileCommand(Controller &controller, Invoker &invoker,
                  IState *state, const std::string &filepath) :
    Command(controller, invoker, state), m_filepath(filepath) {}

  virtual void execute() {
    m_controller.create_buffer(m_filepath);
  }
  virtual void unexecute() {
    // non unexecutable
  }
  virtual bool unexecutable() { return false; };

private:
  const std::string& m_filepath;
};

/*!
 * Command used when a file is closed.
 * Not sure what it's supposed to do yet...
 */
class FileClosedCommand : public Command {
public:
  FileClosedCommand(Controller &controller, Invoker &invoker, IState *state) :
    Command(controller, invoker, state) {}

  virtual void execute() {}
  virtual void unexecute() {
    // non unexecutable
  }
  virtual bool unexecutable() { return false; };

private:
};

/*!
 * Switch buffer. Can take an numerical id or a iterator.
 */
class SwitchBuffer : public Command {
public:
  SwitchBuffer(Controller &controller, Invoker &invoker, IState *state, 
               int buffer_idx) :
    Command(controller, invoker, state), 
    m_new_buffer(controller.get_nth_buffer(buffer_idx)) {}
  SwitchBuffer(Controller &controller, Invoker &invoker, IState *state, 
               buffer_list::iterator new_buffer) :
    Command(controller, invoker, state), m_new_buffer(new_buffer) {}

  virtual void execute() {
    m_former_buffer = m_controller.get_active_buffer();
    m_controller.activate_buffer(m_new_buffer);
  }

  virtual void unexecute() {
    m_controller.activate_buffer(m_former_buffer);
  }

private:
  buffer_list::const_iterator m_new_buffer;
  buffer_list::const_iterator m_former_buffer;
};

#endif //__FILEMGT_H__