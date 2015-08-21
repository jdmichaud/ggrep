/*! \brief Implements the Command pattern
 */

#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include <list>
#include <mutex>
#include <vector>
#include <memory>
#include <typeinfo>

#include "logmacros.h"

class IState;

class ICommand {
public:
  virtual ~ICommand() {};
  /*! execute the command */
  virtual void execute() = 0;
  /*! unexecute the command */
  virtual void unexecute() = 0;
  /*! is the command undoable */
  virtual bool unexecutable() = 0;
};

class Controller;
class Invoker;

class Command : public ICommand {
public:
  Command(Controller &controller, Invoker& invoker, IState *state) :
    m_controller(controller), m_invoker(invoker), m_state(state) {}
  virtual ~Command() {};
  /*! By default, they all are */
  virtual bool unexecutable() { return true; };
protected:
  Controller &m_controller;
  Invoker &m_invoker;
  IState *m_state;
};

class ICommandFactory {
public:
  virtual ~ICommandFactory() = 0;
  /*! Create a command and return it */
};

class IInvoker {
public:
  virtual ~IInvoker() {};
  /*!
   * Add the command in the execute pipe and trigger it's execution
   */
  virtual void execute(std::shared_ptr<ICommand> &&) = 0;
  /*!
   * Undo the last undoable set of command
   */
  virtual void undo() = 0;
};

class Invoker : public IInvoker {
public:
  Invoker(Controller &controller, IState *state) :
    m_controller(controller),
    m_state(state)
  {}

  template<typename T, typename... Args>
  std::shared_ptr<T> &&create_command(Args... as) {
    T *t = new T(m_controller, *this, as...);
    return t;
  }
  template<typename T, typename... Args>
  void create_and_execute(Args... as) {
    std::shared_ptr<T> t(new T(m_controller, *this, m_state, as...));
    (*this).execute(t);
  }
  void execute(std::shared_ptr<ICommand> &&command) {
    // Execute the command
    LOGDBG("about to execute " << typeid(*command).name());
    command->execute();
    // We are about to manipulate the stack, let's take the mutex
    std::lock_guard<std::mutex> guard(m_stack_lock);
    // Clear the undone stack because redo a command on a unknown state lead to
    // an undefined behavior
    m_undone_command.clear();
    // Push the command on the stack of executed command
    m_executed_command.emplace_back(command);
  }
  void undo() {
    // Undo the last command
    LOGDBG("about to undo " << typeid(*this).name())
    m_executed_command.back()->unexecute();
    // We are about to manipulate the stack, let's take the mutex
    std::lock_guard<std::mutex> guard(m_stack_lock);
    // and push it on the undone stack to be potentially redone
    m_undone_command.emplace_back(m_executed_command.back());
    // Remove it from the executed stack
    m_executed_command.pop_back();
  }

private:
  Controller            &m_controller;
  IState                *m_state;
  std::mutex            m_stack_lock;
  std::list<std::shared_ptr<ICommand> > m_executed_command;
  std::list<std::shared_ptr<ICommand> > m_undone_command;
};

#endif //__COMMANDS_H__