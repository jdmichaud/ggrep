/*! \brief BufferModel for the ggrep application
 *
 */

#ifndef  __BUFFER_MODEL_H__
#define __BUFFER_MODEL_H__

#include <list>
#include <regex>
#include <mutex>
#include <memory>
#include <fstream>

#include "types.h"
#include "model.h"
#include "buffer.h"
#include "processor.h"
#include "filter_set.h"

/*
 * Points to text hold by a Buffer and gives information as to where the model
 * is pointing to and what attributes applies to the text.
 */
class BufferModel : public Model
{
public:
  BufferModel(std::shared_ptr<IBuffer> &&buffer);
  ~BufferModel();

  DECLARE_ENTRY( BufferModel, filter_set, filter_set_t );
  DECLARE_ENTRY( BufferModel, filter_processing_progress, uint );
  DECLARE_ENTRY( BufferModel, display_attributes, bool );
public:
  // Set the current visible buffer
  void set_current_buffer(std::shared_ptr<IBuffer>);
public:
  // Get/Set the text of the current buffer
  char ** get_text();
  // Get/Set the number of lines in the current buffer
  uint get_number_of_line() const ;
  void set_number_of_line(uint i);
  // Get/Set the current pointer (first line displayed) of the current buffer
  uint get_first_line_displayed() const ;
  void set_first_line_displayed(uint i);
  // Get/Set attributes of the current buffer
  attr_list_t get_attrs();
  void clear_attrs();
public:
  // Should we have the thread object in the model ? Looks ugly but I don't see
  // another easy and straighforward way... TODO: Improve this
  FilterEngine m_filter;
  /*
   * Functions used by the FilterProcessor
   */
  void clear_filtered_line();
  void add_filtered_line(char *line);
  void enable_filtering();
  void disable_filtering();
  std::shared_ptr<FileBuffer> get_file_buffer();
  void retrieve_filter_set(filter_set_t &filter_set);
  /** Add the filtered line and an associated attribute */
  void add_match(char *line, uint filtered_line_index, 
                 uint start_pos, uint end_pos);
  /*
   * Functions used to thread-safely manipulate the filter list
   */
  void add_filter(const std::string &filter);
  void update_last_filter(const std::string &filter);
  void remove_last_filter();
private:
  std::shared_ptr<FileBuffer> m_file_buffer;
  std::shared_ptr<FilteredBuffer> m_filtered_buffer;
  std::shared_ptr<IBuffer> m_current_buffer;
  std::mutex m_filter_set_mutex;
};

typedef std::list<std::unique_ptr<BufferModel> > buffer_list;

#endif //__BUFFER_MODEL_H__
