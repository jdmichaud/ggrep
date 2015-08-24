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