/*! \brief Application events
 *
 * The Event class represent an event in the application.
 * A user input is an event. The application can generate event.
 */

#include <stdexcept>
/*
 * See comment in Input.h. curses will be used for other type of input (dos 
 * console). We need this here so that the enums for event starts at KEY_MAX + 1
 */
#include <curses.h>
#include "types.h"

#define LOWER_BOUND_EVENT   KEY_MAX + 1
#define FILE_OPENED         LOWER_BOUND_EVENT + 1
#define FILE_CLOSED         LOWER_BOUND_EVENT + 2

#ifndef __EVENT_H__
#define __EVENT_H__

class IEvent {
public:
  /*! Get the stored keycode */
  virtual inline uint get_eventid() const noexcept = 0;
  /*! Check if the return belongs to the specific Input class */
  virtual inline bool is_a(const IEvent& event) const noexcept = 0;
  virtual inline bool is_a(uint event_id) const noexcept = 0;
};

/*! Compare an IEvent with a eventid */
inline bool operator==(const IEvent &lhs, uint eventid) {
  return (lhs.get_eventid() == eventid);
}

/*! Compare an IEvent with another */
inline bool operator==(const IEvent &lhs, const IEvent &rhs) {
  return (lhs == rhs.get_eventid());
}

class Event : public IEvent {
public:
  Event(uint eventid) : m_eventid(eventid) {}
  /*! Get the stored keycode */
  virtual inline uint get_eventid() const noexcept { return m_eventid; }
  /*! Check if the return belongs to the specific Input class */
  virtual inline bool is_a(const IEvent& event) const noexcept {
    return is_a(event.get_eventid());
  }
  virtual inline bool is_a(uint eventid) const noexcept {
    return m_eventid == eventid;
  }
protected:
  uint m_eventid;
};

#endif //__EVENT_H__