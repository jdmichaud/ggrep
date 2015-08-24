#include <regex>
#include "processor.h"

FilterEngine::FilterEngine(BufferModel &buffer_model) :
  ProcessorThread(std::bind(&FilterEngine::filter, this)),
  m_buffer_model(buffer_model)
{}

bool FilterEngine::match(const char *line, const filter_set_t &filter_set) {
  bool match = false;
  if (filter_set.land) {
    // If we want to AND the results
    for (const auto &re: filter_set.filters) {
      // If just one regex does not match, exit
      if (!(match = std::regex_match(line, re)))
        return false;
    }
  } else {
    // If we want to OR the results
    for (const auto &re: filter_set.filters) {
      // If just one regex does match, exit
      if ((match = std::regex_match(line, re)))
        return true;
    }
  }
  return match;
}

void FilterEngine::filter() {
  std::list<std::regex> regexes;
  // Erase the content of the model first
  memset(m_buffer_model.set_text().update(), 0,
         m_buffer_model.get_line_number() + 1);
  // Retrieve the content of the file buffer
  char * const *file_text = m_buffer_model.get_buffer_content();
  // Position the current character string to be added
  uint current_buffer_line = 0;
  // Position the current character string to be analyzed
  char * const *current_file_line = file_text;
  // As long as we are not interrupted
  while (!m_interrupted) {
    // If no filter has been set or we are done with our analysis then we wait.
    if (m_buffer_model.get_filter_set().filters.empty() ||
        *current_file_line != nullptr)
      (*this).wait(); // Wait until someone signals us
    // m_signaled will be set to true if someone changed the filters or on the
    // first loop (set in ProcessorThread::start)
    if (m_signaled) {
      m_signaled = false; // reset it to false
      // Clear the model buffer
      memset(m_buffer_model.set_text().update(), 0,
             m_buffer_model.get_line_number() + 1);
      // Position the current character string to be added
      current_buffer_line = 0;
      // Position the current character string to be analyzed
      current_file_line = file_text;
    }
    // Does the current line match the filter set
    if (match(*current_file_line, m_buffer_model.get_filter_set())) {
      // Yes, add it to the model
      m_buffer_model.set_text().update()[current_buffer_line++] = *current_file_line;
    }
    // Look at the next line
    ++current_file_line;
  }
}
