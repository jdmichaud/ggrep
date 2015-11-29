/*! \brief Model for the ggrep application
 *
 * The Model implements the methods to access a data map and call an observable
 * when the map is modified
 */

#ifndef __MODEL_H__
#define __MODEL_H__

#include <map>
#include <mutex>
#include <string>
#include <functional>

#include "logmacros.h"
#include "observable.h"

typedef std::function<void()> notify_callback_t;

/*!
 * This macro declares a new entry in the model from a NAME and a TYPE.
 * It will create appropriate accessors.
 */
#define DECLARE_ENTRY(MODEL_CLASS, NAME, ...) \
public: \
  const __VA_ARGS__ & get_##NAME() const { \
    return m_##NAME;  \
  } \
  void set_##NAME ( __VA_ARGS__ && value) { \
    m_##NAME = std::move(value); \
    (*this).notify_observers(); \
  } \
  Update<__VA_ARGS__> set_##NAME () { \
    notify_callback_t f = std::bind(&MODEL_CLASS::notify_observers, this); \
    return Update<__VA_ARGS__>( m_##NAME , f ); \
  } \
private: \
  __VA_ARGS__ m_##NAME; \

/* 
 * Used when accessing complex members that are not thread safe (like STL
 * container). The lock is for the whole model so use only when necessary.
 */
#define LOCK_MODEL(OBJECT) std::lock_guard<std::mutex> lock(OBJECT.m_mutex);

/*! Update is a Proxy object that calls notify_observers on destruction
 * Can also be used to simulate transation like so:
 * {
 *  Update update = model.set_some_vector();
 *  update().push_back(4);
 *  update().push_back(5);
 *  update().push_back(6);
 * } // Observer notified once on 'update' variable desctruction
 */
template <typename T>
class Update {
public:
  Update(T& value, notify_callback_t &callback) :
    m_value(value), m_callback(callback) {
    LOGDBG_("constructor of Update " << this << " -- callback: " << &m_callback)
  }
  Update(const Update &&u) : m_value(u.m_value), m_callback(u.m_callback) {
    LOGDBG_("copy of Update " << &u << " to Update " << this << " -- callback: " << &m_callback)
  }
  ~Update() {
    LOGDBG_("destructor of Update " << this << " -- callback: " << &m_callback)
    m_callback();
  }

  T& update() {
    LOGDBG_("update of Update " << this << " -- callback: " << &m_callback)
    return m_value;
  }

private:
  T &m_value;
  notify_callback_t m_callback;
private:
  // Don't copy this object, move it
  Update(const Update &) = delete;
};

class Model : public Observable {
public:
  // Used for object wide locking. Use with care...
  std::mutex m_mutex;
};

#endif //__MODEL_H__