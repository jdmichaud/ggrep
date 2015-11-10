/*! \brief Search declarations
 *
 * Classes used by the interactive searching functionality
 */

#ifndef __SEARCH_H__
#define __SEARCH_H__

// Forward declaration
class BufferModel;

class SearchEngine : public ProcessorThread {
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
  void rearm(uint &current_buffer_line, uint &current_filtered_line);
  /*
   * Clear the result of the find engine
   */
  void clear_result();
  /*
   * Get the next item in our list of found items
   */
  void get_to_next_found_item();
  /*
   * Get the previous item in our list of found items
   */
  void get_to_previous_found_item();
private:
  /*
   * Match a character string from the buffer with a particular filter set.
   */
  bool match(const char *line, const filter_set_t &filter_set,
             std::cmatch &matches);

private:
  //Controller  &m_controller;
  BufferModel &m_buffer_model;
  // We maintain a local filter list in order to avoid taking a mutex for each
  // line analyzed.
  filter_set_t m_filter_set;
};

#endif // __SEARCH_H__