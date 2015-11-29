/*! \brief Search declarations
 *
 * Classes used by the interactive searching functionality
 */

#ifndef __SEARCH_H__
#define __SEARCH_H__

#include <list>

#include "processors/processor.h"
#include "processors/attrs.h"

// Forward declaration
class BufferModel;

typedef struct found_item_t {
  uint line;
  uint start_pos;
  uint end_pos;
  found_item_t(uint l, uint s, uint e) : line(l), start_pos(s), end_pos(e) {}
} found_item_t;

// TODO: AttributeHolder should be a composition not an inheritance!
class SearchEngine : public ProcessorThread, public AttributeHolder {
public:
  SearchEngine(BufferModel &buffer_model);
  /*
   * Start once the signal is raised (or on first call to start).
   * Check the content of the search term set. If the search term is different
   * from the empty string, start looking for the term in the buffer and produce
   * search items.
   */
  void find();
  /*
   * Re-arm the filter state so that the buffer of filtered line is clear, and a
   * new filtering can begin
   */
  void rearm();
  /*
   * Clear the result of the find engine
   */
  void clear_result();
private:
  /*
   * Match a character string from the buffer with a particular filter set.
   */
  bool match(const char *line, const std::string &term,
             uint &position, uint &length);

private:
  //Controller  &m_controller;
  BufferModel &m_buffer_model;
  // lower indexed line searched
  uint m_low_boundary;
  // higher indexed line searched
  uint m_high_boundary;
};

#endif // __SEARCH_H__