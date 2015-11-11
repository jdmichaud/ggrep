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
  ProcessorThread(std::bind(&SearchEngine::filter, this)),
  AttributeHolder(buffer_model.get_file_buffer()->get_number_of_line()),
  m_buffer_model(buffer_model),
  m_low_boundary(0), m_high_boundary(0),
  m_focused_item(m_found_items.begin())
{}

bool SearchEngine::match(const char *line, const std::string term,
                         std::cmatch &matches) {
  if ((match = std::regex_search(line, matches, term)))
    return true;
  return false;
}

void SearchEngine::find() {
  // Will hold the result of the match which will be converted to attributes
  std::cmatch matches;
  // Get the search direction
  bool forward = m_buffer_model.get_search_forward()
  // As long as we are not interrupted
  while (!m_interrupted) {
    // If no search tern has been set or we are done with our analysis then we wait.
    while ((m_buffer_model.get_search_term().size() == 0 ||
           (forward && m_high_boundary >= m_buffer_model.get_file_buffer()->get_number_of_line()) ||
           (!forward && m_low_boundary <= 0))
           && !m_interrupted) // if we are interrupted, we stop waiting
    {
      LOGDBG("searchiing paused");
      (*this).wait(); // Wait until someone signals us
      LOGDBG("searching started, signal: " << m_signaled);
      // Have we been interrupted ?
      if (m_interrupted) return;
      // This instruction might seem strange but we need to get out of the loop
      // by resetting the line pointer...
      if (m_signaled) { 
        m_high_boundary = m_low_boundary = m_buffer_model.get_file_buffer(); 
      }
    }
    // Pick the next line to analyze depending on the direction of the search
    uint next_line = forward ? m_high_boundary + 1 : m_low_boundary - 1;
    // Does the current line match the search term
    if (match(m_buffer_model.get_file_buffer()->get_text()[next_line],
              m_buffer_model.get_search_term(), matches)) {
      // Expand the known territory
      forward ? m_high_boundary = next_line : m_low_boundary = next_line;


    }
  }
}

void SearchEngine::rearm(uint &current_buffer_line, uint &current_filtered_line) {

}

void SearchEngine::clear_result() {

}

void SearchEngine::get_to_next_found_item() {

}

void SearchEngine::get_to_previous_found_item() {

}

