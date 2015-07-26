#include "buffer.h"
#include "buffer_model.h"

BufferModel::BufferModel(std::unique_ptr<IBuffer> &&buffer) : m_buffer(std::move(buffer)) {}