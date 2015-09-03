/*
 *
 *  Created by Jean-Daniel Michaud - 05-Mar-2014
 *
 */

#ifndef __CONSUMER_HPP__
#define __CONSUMER_HPP__

#include "work_queue.h"

/*! \class IProducer
 *  \brief Consume T from the WorkQueue
 */
template<typename T>
class IConsumer
{
public:
  /*!
   * take a T from the WorkQueue in a thread-sage way.
   * If no items are available, blocks until an item is added by an IProducer
   * using the underlying mechanisms implemented by the IWorkQueue.
   */
  virtual T take() = 0;

  /*!
   * Assign the element at the top of the queue wihtout poping it.
   * If no items are available, the pointer is assigned nullptr.
   */
  virtual T const * peep() = 0;

protected:
  IConsumer() {}
};

/*! \class Consumer
 *  \brief Implemetation of IConsumer
 */
template<typename T>
class Consumer : public IConsumer<T>
{
public:
  Consumer(WorkQueue<T> &work_queue) : m_work_queue(work_queue) {}
  virtual ~Consumer() {}

  virtual T take()
  {
    return m_work_queue.pop(true); // blocking
  }

  virtual T const * peep()
  {
    if (m_work_queue.empty()) {
      LOGDBG("*** queue empty");
      return nullptr;
    }
    else {
      const T& _t = m_work_queue.top(); // non blocking
      LOGDBG("*** _t: " << &_t);
      return &_t;
    }
  }

protected:
  /*!
   * m_work_queue is provided at construction and is shared between
   * all the IProducer and IConsumer that need to interact.
   */
  WorkQueue<T> &m_work_queue;

private:
//  Consumer() { }
};

#endif // __CONSUMER_HPP__