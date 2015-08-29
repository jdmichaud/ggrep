/*! \brief Processors declarations
 *
 * Processors are class that run a thread, analyze the model and modify it for
 * a result to be displayed on screen.
 */

#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <list>
#include <mutex>
#include <string>
#include <thread>
#include <functional>
#include <condition_variable>

#include "logmacros.h"

/**
 * A processor is a class representing a thread executing a task in the
 * background. This task is able to modify the model and inject events into the
 * controller. It shall be able to be started, stopped and signalled in case the
 * input parameters to its processing are updated.
 */
class Processor {
public:
  /** Starts the thread if the thread is not already started. Non blocking. */
  virtual void start() = 0;
  /**
   * Stops the thread. The thread may not stop immediatly.
   */
  virtual void stop() = 0;
  /**
   * Signal the thread that its input parameters from the model has been
   * updated
   */
  virtual void signal() = 0;
};

class ProcessorThread : public Processor {
public:
  ProcessorThread(std::function<void()> fn) : m_interrupted(false), 
    m_function(fn) {}
  void start() {
    LOGDBG("starting thread");
    m_interrupted = false;
    if (!m_thread.joinable())
      m_thread = std::thread(m_function);
    else
      LOGERR("thread already started!");
    m_signaled = true;
  }
  ~ProcessorThread() {
    LOGFN()
    if (m_thread.joinable()) {
      stop();
      m_thread.join();
    }
  }
  void stop() { m_interrupted = true; signal(); }
  void signal() { LOGDBG("signled"); m_signal.notify_all(); m_signaled = true; }
  inline void reset_signal() { m_signaled = false; }
  /*
   * There can be two states in which we are signaled:
   * 1. We were done and were waiting for something to do: this is managed by
   *    the condition variable
   * 2. We were busy processing but the input parameter changed and we need to
   *    start over: this is were the boolean m_signaled is used.
   */
  void wait() {
    std::unique_lock<std::mutex> lock(m_wait_mutex);
    m_signal.wait(lock);
  }
protected:
  bool                      m_interrupted;
  bool                      m_signaled;
private:
  std::thread               m_thread;
  std::function<void()>     m_function;
  std::mutex                m_wait_mutex;
  std::condition_variable   m_signal;
};

// Forward declaration
class BufferModel;
struct filter_set_t;

class FilterEngine : public ProcessorThread {
public:
  FilterEngine(BufferModel &buffer_model);
  /*
   * Start once the signal is raised (or on first call to start).
   * Check the content of the filter set. If the filter set is empty, wait for a
   * signal.
   */
  void filter();
  /*
   * As long as there is no filter or no text to filter, we wait. Unless we are
   * interrupted.
   */
  void wait_for_something_to_filter(char * const *current_file_line);
private:
  /*
   * Match a character string from the buffer with a particular filter set.
   */
  bool match(const char *line, const filter_set_t &filter_set);

private:
  //Controller  &m_controller;
  BufferModel &m_buffer_model;
};

#endif // __PROCESSOR_H__