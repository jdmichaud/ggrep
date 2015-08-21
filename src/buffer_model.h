/*! \brief BufferModel for the ggrep application
 *
 */

#ifndef  __BUFFER_MODEL_H__
#define __BUFFER_MODEL_H__

#include <list>
#include <regex>
#include <memory>
#include <fstream>

#include "types.h"
#include "model.h"
#include "buffer.h"

/*
 * This structure describes the text attributes to apply to a certain contigous
 * text zone
 */
typedef struct {
  int attrs_mask;
  uint start_pos;
  uint end_pos;
} tattr_t;

typedef std::list<std::list<tattr_t>> attr_list;

/*
 * This structure is the logical representation of the filters set on each
 * buffer
 */
typedef struct {
  std::list<std::regex> filters;
  bool  land; // logical and if true, logical or otherwise
} filter_set_t;

/*
 * Points to text hold by a Buffer and gives information as to where the model
 * is pointing to and what attributes applies to the text.
 */
class BufferModel : public Model
{
public:
  BufferModel(std::unique_ptr<IBuffer> &&buffer);
  ~BufferModel();

  DECLARE_ENTRY( BufferModel, line_number, uint );
  DECLARE_ENTRY( BufferModel, first_line_displayed, uint );
  DECLARE_ENTRY( BufferModel, attrs, attr_list );
  DECLARE_ENTRY( BufferModel, filter_set, filter_set_t );
public:
  /*!
   * DECLARE_ENTRY macro do not work well on pointers to pointers...
   */
  const char * const * get_text() const { return m_text; }
  Update<char **> set_text() {
    notify_callback_t f = std::bind(&BufferModel::notify_observers, this);
    return Update<char **>(m_text, f);
  }

public:
  char * const *get_buffer_content() { return m_buffer->get_buffer(0); }
  void reinit_text();
private:
  std::unique_ptr<IBuffer> m_buffer;
  // Should we have the thread object in the model ? Looks ugly but I don't see
  // another easy and straighforward way... TODO: Improve this
  std::thread m_filtering_thread;
  char **m_text;
};

typedef std::list<std::unique_ptr<BufferModel> > buffer_list;

#endif //__BUFFER_MODEL_H__