/*
 *
 *  Created by Jean-Daniel Michaud - 05-Mar-2014
 *
 */

#ifndef __WORK_QUEUE_HPP__
#define __WORK_QUEUE_HPP__

#include <stdexcept>
#include <list>
#include <utility>
#include <condition_variable>

#include "logmacros.h"

/*! \class IWorkQueue
 * \brief Represents a queue of Ts.
 * A producer will add Ts.
 * A consumer will pop Ts.
 */
template <typename T>
class IWorkQueue
{
public:
  /*!
   * Add a thing in a thread safe way.
   * Signal a addition through a condition variable.
   * \throws bad_alloc if out of memory.
   */
  virtual void add(const T t) = 0;

  /*!
   * Pop a thing in a thread safe way.
   * \param blocking If true and queue empty, the call will block until a
   * Producer add a T. blocking is false if not specified.
   */
  virtual T pop(bool blocking = false) = 0;

  /*!
   * Get the size of the queue
   */
  virtual size_t size() = 0;

  /*!
   * Is the queue empty
   */
  virtual bool empty() = 0;
};

/*! \class WorkQueue
 * \brief Implementation of IWorkQueue
 */
template <typename T>
class WorkQueue : public IWorkQueue<T>
{
public:
  /*!
   * WorkQueue Constructor
   */
  WorkQueue()
  {
  }
  virtual ~WorkQueue()
  {
  }

  virtual void add(const T t)
  {
    LOGDBG("add: " << &t);
    std::lock_guard<std::mutex> guard(m_mutex);
    m_queue.emplace_back(t);
    m_condv.notify_all();
  }

  virtual T pop(bool blocking = false)
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_queue.empty())
    {
      if (blocking)
        m_condv.wait(lock);
      else
        // Proper crash to avoid a seg fault
        throw std::runtime_error("pop on empty queue");
    }
    T value = m_queue.front();
    m_queue.pop_front();
    LOGDBG("pop: " << &value);
    return value;
  }

  virtual size_t size()
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_queue.size();
  }

  virtual bool empty()
  {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_queue.empty();
  }

protected:
  std::list<T>            m_queue;

  std::mutex              m_mutex;
  std::condition_variable m_condv;
};

#endif // __WORK_QUEUE_HPP__
