/*! \brief User inputs
 *
 * The Input class represent the possible user inputs.
 */

#ifndef __EVENT_FACTORY_H__
#define __EVENT_FACTORY_H__

#include "event.h"
#include "input.h"

class EventFactory {
public:
  Event build_event(uint event_id) {
    if (isprint(event_id))             return Printable(event_id);
    else if (isarrow(event_id))        return Arrow(event_id);
    else if (isnav(event_id))          return Nav(event_id);
    else if (isfunctionkey(event_id))  return Function(event_id);
    else if (isctrl(event_id))         return Ctrl(event_id);
    else if (isredraw(event_id))       return Redraw(event_id);

    LOGERR("EventFactory: unknown event_id " << event_id);
    throw std::runtime_error("unknown event id " + std::to_string(event_id));
  }
};

#endif //__EVENT_FACTORY_H__