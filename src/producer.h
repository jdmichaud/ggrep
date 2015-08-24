/*
 *
 *  Created by Jean-Daniel Michaud - 05-Mar-2014
 *
 */

#ifndef __PRODUCER_HPP__
#define __PRODUCER_HPP__

#include "work_queue.h"

/*! \class IProducer
 *  \brief Produces T and place them in the WorkQueue
 */
template<typename T>
class IProducer
{
public:

  /*!
   * produce adds a T on the WorkQueue in a thread-sage way and
   * signal the addition to IConsumer using the underlying mechanisms
   * implemented by the IWorkQueue.
   */
  virtual void produce(const T&&) = 0;

protected:
  IProducer () {}
};

/*! \class Producer
 *  \brief Implementation of IProducer
 */
template <typename T>
class Producer : public IProducer<T>
{
public:
  Producer(WorkQueue<T> &work_queue) : m_work_queue(work_queue) {}

  virtual void produce(const T&& t)
  {
    m_work_queue.add(std::move(t));
  }

private:
  /*!
   * m_work_queue is provided at construction and is shared between
   * all the IProducer and IConsumer that need to interact.
   */
  WorkQueue<T> &m_work_queue;
};

#endif // __PRODUCER_HPP__