/*
 *
 *  Created by Jean-Daniel Michaud - 02-Apr-2015
 *
 */

#ifndef __OBSERVABLE_TEST_HPP__
#define __OBSERVABLE_TEST_HPP__

#include <list>
#include <functional>

class IObservable;

typedef std::function<void(IObservable &)> observer_callback_t;

/*! \class IObservable
 *  \brief Subject being observed
 */
class IObservable
{
public:
  /*!
   * Register an observer to the subject.
   * \throws bad_alloc if out of memory.
   */
  virtual void register_observer(observer_callback_t const&) = 0;

protected:
  /*!
   * Notify the observers that the subject state changed.
   * Do nothing if no observer is registered.
   */
  virtual void notify_observers() = 0;
};


/*! \class Producer
 *  \brief Implementation of IProducer
 */
class Observable : public IObservable
{
public:
  Observable() {}
  virtual ~Observable() {}
  void register_observer(observer_callback_t const& cb) {
    m_observers.push_back(cb);
  }
protected:
  virtual void notify_observers() {
    // Go through the registered observer and notify them on changes
    for (std::list<observer_callback_t>::iterator it = m_observers.begin();
         it != m_observers.end(); ++it)
      // Pass this to the callback to distinguish between several IObservable
      (*it)(*this);
  }
protected:
  std::list<observer_callback_t> m_observers;
};

#endif // __OBSERVABLE_TEST_HPP__