#include <cmath>
#include <regex>
#include "logmacros.h"
#include "processor.h"
#include "buffer_model.h"

FilterEngine::FilterEngine(BufferModel &buffer_model) :
  ProcessorThread(std::bind(&FilterEngine::filter, this)),
  m_buffer_model(buffer_model)
{
  // Initialize the filter list
  m_buffer_model.retrieve_filter_set(m_filter_set);
}

/*
 * Match a line of text versus a set of provided filters
 */
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

/*
 * Main function of the filtering thread. Will wait for an input and go through
 * buffer to match against the provided filters.
 * Once the buffer has been filtered, it will wait for another input until it is
 * interrupted.
 */
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
      LOGDBG("filtering started, signal: " << m_signaled);
      // This instruction might seem strange but we need to get out of the loop
      // by resetting the line pointer...
      if (m_signaled) { current_buffer_line = 0; }
    }
    // m_signaled will be set to true if someone changed the filters or on the
    // first loop (set in ProcessorThread::start)
    if (m_signaled) { (*this).rearm(current_buffer_line); }
    // Have we been interrupted ?
    if (m_interrupted) return;
    // Does the current line match the filter set
    if (match(file_text[current_buffer_line], m_filter_set)) {
      // Yes, add it to the model
      LOGDBG("line " << current_buffer_line << " matches");
      m_buffer_model.add_filtered_line(file_text[current_buffer_line]);
    }
    // Look at the next line
    ++current_buffer_line;
    // Log the progress for debuggin purposes
    uint progress = (float) current_buffer_line / (float) number_of_line_in_file * 100;
    if (progress % 5 == 0)
      m_buffer_model.set_filter_processing_progress().update() = progress;
  }
}

void FilterEngine::rearm(uint &current_buffer_line) {
  (*this).reset_signal(); // reset it to false
  // Clear the model buffer
  LOGDBG("clear filtered lined");
  m_buffer_model.clear_filtered_line();
  // Position the current character string to be added
  current_buffer_line = 0;
  // Retrieve the filter list from the buffer.
  m_buffer_model.retrieve_filter_set(m_filter_set);
  LOGDBG("retrieved filter: " << m_filter_set);
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

/*
 * Add a matching line to the filtered buffer
 */
void FilteredBuffer::add_line(char *line) {
  if (number_of_filtered_line >= m_original_number_of_line) {
    LOGERR("add line " << number_of_filtered_line << " to a buffer of size " <<
           m_original_number_of_line);
    // Should never happen
    throw std::runtime_error("buffer overflow in filtered buffer");
  }
  m_filtered_lines[number_of_filtered_line++] = line;
}
