#include <ncurses.h>
#include <map>
#include <cmath>
#include <regex>
#include <tuple>
#include <utility>
#include "logmacros.h"
#include "processors/search.h"
#include "buffer_model.h"

SearchEngine::SearchEngine(BufferModel &buffer_model) :
  ProcessorThread(std::bind(&SearchEngine::find, this)),
  AttributeHolder(buffer_model.get_file_buffer()->get_number_of_line()),
  m_buffer_model(buffer_model),
  m_low_boundary(0), m_high_boundary(0)
{}

bool SearchEngine::match(const char *line, const std::string &term,
                         uint &position, uint &length) {
  const char *found = strstr(line, term.c_str());
  if (found != nullptr) {
    position = found - line;
    length = std::strlen(term.c_str());
    LOGDBG("found term: " << term << " in line: " << line);
    return true;
  }
  LOGDBG("did not find term: " << term << " in line: " << line);
  return false;
}

void SearchEngine::find() {
  // Will hold the result of the match which will be converted to attributes
  uint position; uint length;
  // Initialize variables
  m_high_boundary = m_low_boundary = m_buffer_model.get_search_focus_line();
  // Get the search direction
  bool forward = m_buffer_model.get_search_forward();
  // Reset the signal set on the first start
  (*this).reset_signal(); // reset it to false
  // As long as we are not interrupted
  while (!m_interrupted) {
    // If no search tern has been set or we are done with our analysis then we wait.
    if ((m_buffer_model.get_search_term().size() == 0 ||
        (m_buffer_model.get_search_forward() && m_high_boundary >= m_buffer_model.get_file_buffer()->get_number_of_line()) ||
        (!m_buffer_model.get_search_forward() && m_low_boundary <= 0)))
    {
      LOGDBG("searching paused");
      (*this).wait(); // Wait until someone signals us
      LOGDBG("searching started, signal: " << m_signaled << ", interrupt" << m_interrupted);
      // Have we been interrupted ? Early exit.
      if (m_interrupted) return;
    }
    if (m_signaled) {
      // Reinit the boundaries 
      m_high_boundary = m_low_boundary = m_buffer_model.get_search_focus_line(); 
      forward = m_buffer_model.get_search_forward();
      (*this).rearm();
    }
    // Pick the next line to analyze depending on the direction of the search
    uint next_line = forward ? m_high_boundary : m_low_boundary;
    LOGDBG("searching line " << next_line);
    // Does the current line match the search term
    if (match(m_buffer_model.get_file_buffer()->get_text()[next_line],
              m_buffer_model.get_search_term(), position, length)) {
      LOGDBG("line " << next_line << " matches");
      // Add the attribute
      LOCK_MODEL(m_buffer_model)
      m_buffer_model.set_search_attributes().update()->
        add_attr(next_line, position, position + length, A_REVERSE);
      m_buffer_model.set_found_items().update().emplace_back(
        found_item_t(next_line, position, position + length));
    }
    // Expand the known territory
    if (forward) m_high_boundary = next_line + 1;
    else m_low_boundary = next_line - 1;
  }
}

void SearchEngine::rearm() {
  (*this).reset_signal(); // reset it to false
  // Clear the attributes first
  (*this).clear_attrs();
  {
    LOCK_MODEL(m_buffer_model)
    // Clear the found item list
    m_buffer_model.set_found_items().update().clear();
  }
}

void SearchEngine::clear_result() {
  LOCK_MODEL(m_buffer_model)
  m_buffer_model.set_found_items().update().clear();
  (*this).clear_attrs();
}
