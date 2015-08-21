/*! \brief Define a object able to get character commands
 *
 */

#include <cctype>
#include "types.h"
#include "logmacros.h"

/*
 * An interface defining an object able to be edited as text by Commands
 */
class IText {
public:
  virtual void set(std::string s) = 0;
  virtual void insert(char c) = 0;
  virtual void insert(char c, pos p) = 0;
  virtual void insert(const std::string &s) = 0;
  virtual void insert(const std::string &s, pos p) = 0;
  virtual void get_char(char &c) = 0;
  virtual void get_previous_char(char &c) = 0;
  virtual void shift_cur_char_left() = 0;
  virtual void shift_cur_char_right() = 0;
  virtual void shift_cur_word_left() = 0;
  virtual void shift_cur_word_right() = 0;
  virtual bool remove() = 0;
  virtual bool remove(pos p) = 0;
  virtual bool delete_() = 0;
  virtual void clear() = 0;
  virtual void update() = 0;

  virtual void get_curpos(pos &p) = 0;
  virtual void set_curpos(pos p) = 0;
  virtual void begline() = 0;
  virtual void endline() = 0;
};

/*
 * This represent an "TextEdit". Only the x coordinates of the cursor counts 
 * and there is only one string of characters.
 */
class OneLinerText : public IText {
public:
  OneLinerText() : m_text("") {
    m_cur_pos.x = 0;
    m_cur_pos.y = 0;
  }
  virtual void set(std::string s) {
    m_text = s;
    m_cur_pos.x = s.size();
    (*this).update();
  }
  virtual void insert(char c) {
    m_text.insert(m_cur_pos.x++, std::string(1, c));
    (*this).update();
  }
  virtual void insert(char c, pos p) {
    // Not sure what to do here??
  }
  virtual void insert(const std::string &s) {
    m_text.insert(m_cur_pos.x, s);
    m_cur_pos.x += s.size();
    (*this).update();
  }
  virtual void insert(const std::string &s, pos p) {
    // Not sure what to do here??
  }
  virtual void get_char(char &c) {
    c = m_text[m_cur_pos.x];
  }
  virtual void get_previous_char(char &c) {
    if (m_cur_pos.x > 1)
      c = m_text[m_cur_pos.x - 1];
  }
  virtual void shift_cur_char_left() {
    if (m_cur_pos.x > 0) {
      m_cur_pos.x--;
      (*this).update();
    }
  }
  virtual void shift_cur_char_right() {
    if (m_cur_pos.x < m_text.size()) {
      m_cur_pos.x++;
      (*this).update();
    }
  }
  virtual void shift_cur_word_left() {
    uint cur_x = m_cur_pos.x;
    while (cur_x > 0 && isalnum(m_text[cur_x - 1])) --cur_x;
    while (cur_x > 0 && !isalnum(m_text[cur_x - 1])) --cur_x;
    m_cur_pos.x = cur_x;
    (*this).update();
  }
  virtual void shift_cur_word_right() {
    uint cur_x = m_cur_pos.x;
    while (cur_x < m_text.size() && isalnum(m_text[cur_x])) cur_x++;
    while (cur_x < m_text.size() && !isalnum(m_text[cur_x])) cur_x++;
    m_cur_pos.x = cur_x;
    (*this).update();
  }
  virtual bool remove() {
    if (m_cur_pos.x > 0) {
      m_text.erase(--m_cur_pos.x, 1);
      (*this).update();
      return true;
    }
    // return false if nothing done
    return false;
  }
  virtual bool remove(pos p) {
    // Not sure what to do here??
    return false;
  }
  virtual bool delete_() {
    if (m_cur_pos.x < m_text.size()) {
      m_text.erase(m_cur_pos.x, 1);
      (*this).update();
      return true;
    }
    return false;
  }
  virtual void clear() {
    m_text.clear();
    m_cur_pos.x = 0;
    (*this).update();
  }

  virtual void get_curpos(pos &p) {
    p = m_cur_pos;
    (*this).update();
  }
  virtual void set_curpos(pos p) {
    if (p.x < m_text.size() + 1) {
      m_cur_pos = p;
      (*this).update();
    }
  }
  virtual void begline() {
    m_cur_pos.x = 0;
    (*this).update();
  }
  virtual void endline() {
    m_cur_pos.x = m_text.size();
    (*this).update();
  }
protected:
  pos m_cur_pos;
  std::string m_text;
};
