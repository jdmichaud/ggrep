/*! \brief User inputs
 *
 * The Input class represent the possible user inputs.
 */

#ifndef __INPUT_H__
#define __INPUT_H__

#include <limits.h>
/*
 * Weird to have a dependency to curses here. Or will we have other type of
 * views match the curses defs for all inputs i.e. curses macros become our
 * canonical representation for user input?
*/
#include <curses.h>

#include "event.h"

/*!
 * The KLEENE macro is used to match all keys of a certain types.
 * Printable(32) is a space
 * Printable(KLEENE) is all printable
 */
#define KLEENE UINT_MAX


/*! \brief Input represents a user input
 * IInput is the interface defining a user input.
 * Subclasses represent different subclasses of user input (printable, arrows,
 * etc).
 * Each subclasses can be instanciated to represent a particular key (SPACE is
 * Printabl(32)) or a general class (all arrow keys are Arrow(KLEENE))
 */
class IInput {
public:
  /*! Check if the Input is of a generic kind (see KLEENE definition) */
  virtual inline bool isgeneric() const noexcept = 0;
};

class Input : public IInput, public Event {
public:
  Input(uint eventid) : Event(eventid) {}
  Input() : Event(KLEENE) {}

  inline bool isgeneric() const noexcept { return KLEENE == m_eventid; }
  virtual inline bool is_equal(const IEvent& event) const noexcept {
    return is_equal(event.get_eventid());
  }
  virtual inline bool is_equal(uint keycode) const noexcept {
    if ((*this).isgeneric())
      return ((*this).is_a(keycode));
    return ((*this).get_eventid() == keycode);
  }
};

/*! Compare an IInput with a eventid */

inline bool operator==(const Input &lhs, uint keycode) {
  return lhs.is_equal(keycode);
}

inline bool operator!=(const Input &lhs, uint keycode) {
  return !lhs.is_equal(keycode);
}

inline bool isprint(uint keycode) {
  return (keycode >= 32 && keycode <= int('~'));
}

/*! From ascii 32 (SPACE) to ascii 126 (~) */
class Printable : public Input {
public:
  using Input::Input;
  virtual inline bool is_a(uint keycode) const noexcept {
    return isprint(keycode);
  }
};

/*!
 * Arrow keys and their modifier:
 * KEY_DOWN  down-arrow key
 * KEY_UP  up-arrow key
 * KEY_LEFT  left-arrow key
 * KEY_RIGHT  right-arrow key
 *
 */
#define C_KEY_DOWN      01014           /* ctrl+down-arrow key */
#define C_KEY_UP        01065           /* ctrl+up-arrow key */
#define C_KEY_LEFT      01041           /* ctrl+left-arrow key */
#define C_KEY_RIGHT     01060           /* ctrl+right-arrow key */

inline bool isarrow(uint keycode) {
  return (keycode == KEY_DOWN
          || keycode == KEY_UP
          || keycode == KEY_LEFT
          || keycode == KEY_RIGHT
          || keycode == C_KEY_DOWN
          || keycode == C_KEY_UP
          || keycode == C_KEY_LEFT
          || keycode == C_KEY_RIGHT);
}

class Arrow : public Input {
public:
  using Input::Input;
  virtual inline bool is_a(uint keycode) const noexcept {
    return isarrow(keycode);
  }
};

/*!
 * Navigation keys and their modifier:
 * KEY_HOME  home key
 * KEY_END  end key
 * KEY_NPAGE  next-page key
 * KEY_PPAGE  previous-page key
 */
#define C_KEY_HOME      01026            /* ctrl+home key */
#define C_KEY_END       01021            /* ctrl+end key */
#define C_KEY_NPAGE     01045            /* ctrl+next-page key */
#define C_KEY_PPAGE     01052            /* ctrl+previous-page key */

#define KEY_TAB         011

inline bool isnav(uint keycode) {
  return (keycode == KEY_HOME
          || keycode == KEY_END
          || keycode == KEY_NPAGE
          || keycode == KEY_PPAGE
          || keycode == C_KEY_HOME
          || keycode == C_KEY_END
          || keycode == C_KEY_NPAGE
          || keycode == C_KEY_PPAGE
          || keycode == KEY_TAB
          || keycode == KEY_BTAB
          );
}

class Nav : public Input {
public:
  using Input::Input;
  virtual inline bool is_a(uint keycode) const noexcept {
    return isnav(keycode);
  }
};

/*!
 * Functions keys (room for 64):
 * From KEY_F0 410 to KEY_F63 473
 */
#define C_KEY_F0        0440
#define C_KEY_F1        0441
#define C_KEY_F2        0442
#define C_KEY_F3        0443
#define C_KEY_F4        0444
#define C_KEY_F5        0445
#define C_KEY_F6        0446
#define C_KEY_F7        0447
#define C_KEY_F8        0448
#define C_KEY_F9        0449
#define C_KEY_F10       0450
#define C_KEY_F11       0451
#define C_KEY_F12       0452
#define C_KEY_F(n)      (C_KEY_F0+(n))

inline bool isfunctionkey(uint keycode) {
  return ((keycode >= KEY_F0 && keycode <= KEY_F(64))
           || (keycode >= C_KEY_F0 && keycode <= C_KEY_F(64)));
}

class Function : public Input {
public:
  using Input::Input;
  virtual inline bool is_a(uint keycode) const noexcept {
    return isfunctionkey(keycode);
  }
};

/*!
 * Various special control keys:
 * ?
 */
// Linux terminal translates KEY_ENTER into newline
#define MY_KEY_ENTER    015

#define C_KEY_IC        01033
#define C_KEY_DC        01006

#define KEY_0       000 /* '\0' */
#define KEY_SHD     001 /* (start of heading) */
#define KEY_STXT    002 /* (start of text) */
#define KEY_ETXT    003 /* (end of text) */
#define KEY_EOF     004 /* (end of transmission */
#define KEY_ENQ     005 /* (enquiry) */
#define KEY_ACK     006 /* (acknowledge) */
#define KEY_BLL     007 /* '\a' (bell) */
#define KEY_BKSP    010 /* '\b' (backspace) */
#define KEY_HTAB    011 /* '\t' (horizontal tab)    */
#define KEY_NL      012 /* '\n' (new line) */
#define KEY_VTAB    013 /* '\v' (vertical tab) */
#define KEY_FFD     014 /* '\f' (form feed) */
#define KEY_CRT     015 /* '\r' (carriage ret) */
#define KEY_SOUT    016 /* (shift out) */
#define KEY_SIN     017 /* (shift in) */
#define KEY_DLESC   020 /* (data link escape) */
#define KEY_DC1     021 /* (device control 1) */
#define KEY_DC2     022 /* (device control 2) */
#define KEY_DC3     023 /* (device control 3) */
#define KEY_DC4     024 /* (device control 4) */
#define KEY_NACK    025 /* (negative ack.) */
#define KEY_SIDLE   026 /* (synchronous idle) */
#define KEY_EOT     027 /* (end of trans. blk) */
#define MY_KEY_CANCEL  030 /* (cancel) */
#define KEY_EOM     031 /* (end of medium) */
#define KEY_SUB     032 /* (substitute) */
#define KEY_ESC     033 /* (escape) */
#define KEY_FSP     034 /* (file separator) */
#define KEY_GSP     035 /* (group separator) */
#define KEY_RSP     036 /* (record separator) */
#define KEY_USP     037 /* (unit separator) */

inline bool isctrl(uint keycode) {
  return (keycode == MY_KEY_ENTER
          || keycode == KEY_BACKSPACE
          || keycode == KEY_DL
          || keycode == KEY_IL
          || keycode == KEY_DC
          || keycode == KEY_IC
          || keycode == C_KEY_IC
          || keycode == C_KEY_DC
          || keycode == KEY_SDC
          || keycode == KEY_0
          || keycode == KEY_SHD
          || keycode == KEY_STXT
          || keycode == KEY_ETXT
          || keycode == KEY_EOF
          || keycode == KEY_ENQ
          || keycode == KEY_ACK
          || keycode == KEY_BLL
          || keycode == KEY_BKSP
          || keycode == KEY_HTAB
          || keycode == KEY_NL
          || keycode == KEY_VTAB
          || keycode == KEY_FFD
          || keycode == KEY_CRT
          || keycode == KEY_SOUT
          || keycode == KEY_SIN
          || keycode == KEY_DLESC
          || keycode == KEY_DC1
          || keycode == KEY_DC2
          || keycode == KEY_DC3
          || keycode == KEY_DC4
          || keycode == KEY_NACK
          || keycode == KEY_SIDLE
          || keycode == KEY_EOT
          || keycode == MY_KEY_CANCEL
          || keycode == KEY_EOM
          || keycode == KEY_SUB
          || keycode == KEY_ESC
          || keycode == KEY_FSP
          || keycode == KEY_GSP
          || keycode == KEY_RSP
          || keycode == KEY_USP
          );
}

class Ctrl : public Input {
public:
  using Input::Input;
  virtual inline bool is_a(uint keycode) const noexcept {
    return isctrl(keycode);
  }
};

#endif //__INPUT_H__