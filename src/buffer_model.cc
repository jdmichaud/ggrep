#include "buffer.h"
#include "buffer_model.h"

BufferModel::BufferModel(std::unique_ptr<Buffer> &buffer) : m_buffer(buffer) {}