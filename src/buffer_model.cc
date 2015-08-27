#include <mutex>
#include <cstring>
#include "logmacros.h"
#include "buffer.h"
#include "buffer_model.h"
#include "processor.h"

BufferModel::BufferModel(std::unique_ptr<IBuffer> &&buffer) :
    m_first_line_displayed(0), m_filter(*this), m_buffer(std::move(buffer))
{
  LOGDBG("BufferModel creator " << this);
  // Should be started elsewhere. In the controller ?
  //m_filter.start();
}

BufferModel::~BufferModel() {
  LOGDBG("BufferModel desctructor " << this);
}
