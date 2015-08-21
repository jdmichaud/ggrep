#include <cstring>
#include "logmacros.h"
#include "buffer.h"
#include "buffer_model.h"

BufferModel::BufferModel(std::unique_ptr<IBuffer> &&buffer) :
    m_first_line_displayed(0), m_buffer(std::move(buffer))
{
  LOGDBG("BufferModel creator " << this);
  m_line_number = m_buffer->get_number_of_line();
  // Load the text from the buffer
  (*this).reinit_text();
}

BufferModel::~BufferModel() {
  LOGDBG("BufferModel desctructor " << this);
}

void BufferModel::reinit_text() {
  m_text = new char*[m_line_number + 1];
  std::memcpy(m_text, m_buffer->get_buffer(0),
              (m_line_number + 1) * sizeof (char*));
}
