/*! \brief BufferModel for the ggrep application
 *
 */

#ifndef  __BUFFER_MODEL_H__
#define __BUFFER_MODEL_H__

#include <list>
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

typedef std::list<tattr_t> attr_list;

/*
 * Points to text hold by a Buffer and gives information as to where the model
 * is pointing to and what attributes applies to the text.
 */
template <typename StreamType = std::ifstream>
class BufferModel : public Model
{
public:
  BufferModel(std::unique_ptr<Buffer<StreamType> > &buffer);

  DECLARE_ENTRY( BufferModel, line_number, uint )
  DECLARE_ENTRY( BufferModel, text, char ** )
  DECLARE_ENTRY( BufferModel, attrs, attr_list ** );

private:
  std::unique_ptr<Buffer<StreamType> > m_buffer;
};

typedef std::list<std::unique_ptr<BufferModel<> > > buffer_list;

#endif //__BUFFER_MODEL_H__