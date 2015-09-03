#include <regex>
#include "logmacros.h"
#include "processor.h"
#include "buffer_model.h"

FilterEngine::FilterEngine(BufferModel &buffer_model) :
  ProcessorThread(std::bind(&FilterEngine::filter, this)),
  m_buffer_model(buffer_model)
{}

bool FilterEngine::match(const char *line, const filter_set_t &filter_set) {
  bool match = false;
  std::cmatch matches;
  if (filter_set.land) {
    // If we want to AND the results
    for (const auto &re: filter_set.filters) {
      // If just one regex does not match, exit
      if (!(match = std::regex_search(line, matches, re.second)))
        return false;
    }
  } else {
    // If we want to OR the results
    for (const auto &re: filter_set.filters) {
      // If just one regex does match, exit
      if ((match = std::regex_search(line, matches, re.second)))
        return true;
    }
  }
  return match;
}

void FilterEngine::filter() {
  std::list<std::regex> regexes;
  // Erase the content of the model first
  m_buffer_model.clear_filtered_line();
  // Retrieve the content of the file buffer
  char * const *file_text = m_buffer_model.get_file_buffer()->get_text();
  // Position the current character string to be added
  uint current_buffer_line = 0;
  // Get number of line in file
  uint number_of_line_in_file =
    m_buffer_model.get_file_buffer()->get_number_of_line();
  // Reset the signal set on the first start
  (*this).reset_signal(); // reset it to false
  // As long as we are not interrupted
  while (!m_interrupted) {
    // If no filter has been set or we are done with our analysis then we wait.
    while ((m_buffer_model.get_filter_set().filters.empty() ||
            current_buffer_line >= number_of_line_in_file)
           && !m_interrupted) // if we are interrupted, we stop waiting
    {
      LOGDBG("filtering paused");
      (*this).wait(); // Wait until someone signals us
      LOGDBG("filtering started");
      // m_signaled will be set to true if someone changed the filters or on the
      // first loop (set in ProcessorThread::start)
      if (m_signaled) {
        (*this).reset_signal(); // reset it to false
        // Clear the model buffer
        m_buffer_model.clear_filtered_line();
        // Position the current character string to be added
        current_buffer_line = 0;
      }
    }
    // Have we been interrupted ?
    if (m_interrupted) return;
    // Does the current line match the filter set
    if (match(file_text[current_buffer_line], m_buffer_model.get_filter_set())) {
      // Yes, add it to the model
      m_buffer_model.add_filtered_line(file_text[current_buffer_line]);
    }
    // Look at the next line
    ++current_buffer_line;
  }
}

FilteredBuffer::FilteredBuffer(std::shared_ptr<IBuffer> &buffer) {
  // Initialize the pointers to the filtered lines
  m_original_number_of_line = buffer->get_number_of_line();
  m_filtered_lines = new char*[m_original_number_of_line];
  memset(m_filtered_lines, 0, m_original_number_of_line * sizeof (char));
  number_of_filtered_line = 0;
}

FilteredBuffer::~FilteredBuffer() {
  delete[] m_filtered_lines;
}

char **FilteredBuffer::get_text() const {
  LOGDBG("get_text");
  return m_filtered_lines;
}

uint FilteredBuffer::get_original_number_of_line() const {
  return m_original_number_of_line;
}

uint FilteredBuffer::get_number_of_line() const {
  return number_of_filtered_line;
}

void FilteredBuffer::clear() {
  number_of_filtered_line = 0;
  memset(m_filtered_lines, 0, m_original_number_of_line * sizeof (char));
}

void FilteredBuffer::add_line(char *line) {
  if (number_of_filtered_line >= m_original_number_of_line) {
    LOGERR("add line " << number_of_filtered_line << " to a buffer of size " <<
           m_original_number_of_line);
    // Should never happen
    throw std::runtime_error("buffer overflow in filtered buffer");
  }
  m_filtered_lines[number_of_filtered_line++] = line;
}
