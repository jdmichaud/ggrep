/*! \brief Filtering declarations
 *
 * Classes used by the interactive filtering functionality
 */

#include <list>
#include <string>
#include "logmacros.h"
#include "filter_set.h"
#include "buffer.h"
#include "processors/attrs.h"
#include "processors/processor.h"

#ifndef __FILTER_H__
#define __FILTER_H__

// Forward declaration
class BufferModel;

/*
 * This class implements a buffer containing the result of the FilterEngine. It
 * implements the standard IBuffer interface but does not hold a copy of the
 * lines filtered, only pointers to the original IBuffer.
 */
class FilteredBuffer : public Buffer {
public:
  FilteredBuffer(std::shared_ptr<IBuffer> &buffer);
  ~FilteredBuffer();
  /*
   * Standard IBuffer APIs
   */
  bool is_binary() { return false; }
  char **get_text() const;
  uint get_number_of_line() const;
  uint get_original_number_of_line() const;
  void set_number_of_line(uint) { /* Not applicable */ }
  /*
   * Specific FilteredBuffer APIs
   */
  /** Clear the filtered line buffer */
  void clear();
  /** Add a line to the filtered line buffer */
  void add_line(char *line);
private:
  uint number_of_filtered_line;
  char **m_filtered_lines;
  uint m_original_number_of_line;
};

// TODO: AttributeHolder should be a composition not an inheritance!
class FilterEngine : public ProcessorThread, public AttributeHolder {
public:
  FilterEngine(BufferModel &buffer_model);
  /*
   * Start once the signal is raised (or on first call to start).
   * Check the content of the filter set. If the filter set is empty, wait for a
   * signal.
   */
  void filter();
  /*
   * Re-arm the filter state so that the buffer of filtered line is clear, and a
   * new filtering can begin
   */
  void rearm(uint &current_buffer_line, uint &current_filtered_line);
private:
  /*
   * Match a character string from the buffer with a particular filter set.
   */
  bool match(const char *line, const filter_set_t &filter_set,
             uint &position, uint &length);

private:
  BufferModel &m_buffer_model;
  // We maintain a local filter list in order to avoid taking a mutex for each
  // line analyzed.
  filter_set_t m_filter_set;
};

#endif // __FILTER_H__