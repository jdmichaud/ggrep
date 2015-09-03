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
#include "logmacros.h"

#define LOWER_BOUND_EVENT   KEY_MAX + 1
#define FILE_OPENED         LOWER_BOUND_EVENT + 1
#define FILE_CLOSED         LOWER_BOUND_EVENT + 2
#define APP_STARTED         LOWER_BOUND_EVENT + 3
#define ERROR               LOWER_BOUND_EVENT + 4
#define WARNING             LOWER_BOUND_EVENT + 5
#define QUESTION            LOWER_BOUND_EVENT + 6
#define BACKTRACK           LOWER_BOUND_EVENT + 7
#define UNDO                LOWER_BOUND_EVENT + 8
#define FILTER_ADDED        LOWER_BOUND_EVENT + 9
#define REDRAW_ALL          LOWER_BOUND_EVENT + 10
#define REDRAW_FBAR         LOWER_BOUND_EVENT + 11
#define REDRAW_PROMPT       LOWER_BOUND_EVENT + 12
#define REDRAW_BUFFER       LOWER_BOUND_EVENT + 13
#define REDRAW_BROWSER      LOWER_BOUND_EVENT + 14
#define REDRAW_STATE        LOWER_BOUND_EVENT + 15

#ifndef __EVENT_H__
#define __EVENT_H__

class IEvent {
public:
  virtual ~IEvent() {};
  /*! Get the stored keycode */
  virtual inline uint get_eventid() const noexcept = 0;
  /*! Check if the return belongs to the specific Input class */
  virtual inline bool is_a(const IEvent& event) const noexcept = 0;
  virtual inline bool is_a(uint event_id) const noexcept = 0;
  virtual inline bool is_equal(const IEvent& event) const noexcept = 0;
  virtual inline bool is_equal(uint event_id) const noexcept = 0;
  virtual const void *get_data() const noexcept = 0;
};

/*! Compare an IEvent with a eventid */
inline bool operator==(const IEvent &lhs, uint eventid) {
  return (lhs.get_eventid() == eventid);
}
inline bool operator!=(const IEvent &lhs, uint eventid) {
  return (lhs.get_eventid() != eventid);
}

/*! Compare an IEvent with another */
inline bool operator==(const IEvent &lhs, const IEvent &rhs) {
  return (lhs == rhs.get_eventid());
}
inline bool operator!=(const IEvent &lhs, const IEvent &rhs) {
  return (lhs != rhs.get_eventid());
}

class Event : public IEvent {
public:
  Event(uint eventid) : m_eventid(eventid), m_data(nullptr) {
    LOGDBG("Event::Event(uint): m_eventid == " << m_eventid);
  }
  Event(const Event &e) : m_eventid(e.m_eventid), m_data(e.m_data) {
    LOGDBG("Event::Event(const Event &): m_eventid == " << m_eventid);
  }
  virtual ~Event() {}
  /*! Get the stored keycode */
  virtual inline uint get_eventid() const noexcept { return m_eventid; }
  /*! Check if the return belongs to the specific Input class */
  virtual inline bool is_a(const IEvent& event) const noexcept {
    return is_a(event.get_eventid());
  }
  virtual inline bool is_a(uint eventid) const noexcept {
    return m_eventid == eventid;
  }
  virtual inline bool is_equal(const IEvent& event) const noexcept {
    return is_equal(event.get_eventid());
  }
  virtual inline bool is_equal(uint eventid) const noexcept {
    return m_eventid == eventid;
  }
  const void *get_data() const noexcept { return m_data; }
protected:
  uint m_eventid;
  void *m_data;
};

/*
 * This event is used to convey messages to the prompt (or later on popups?)
 */
class PromptMessage : public Event {
public:
  PromptMessage(uint eventid, const std::string &msg) :
    Event(eventid)
  {
    m_data = new std::string(msg);
  }
  ~PromptMessage() {
    if (m_data) delete (std::string *) m_data;
  }
};

inline bool isredraw(uint event_id) {
  return (event_id == REDRAW_ALL
          || event_id == REDRAW_FBAR
          || event_id == REDRAW_PROMPT
          || event_id == REDRAW_BUFFER
          || event_id == REDRAW_BROWSER
          || event_id == REDRAW_STATE);
}

class Redraw : public Event {
public:
  Redraw(uint eventid) : Event(eventid) {}
  ~Redraw() {}
};

#endif //__EVENT_H__