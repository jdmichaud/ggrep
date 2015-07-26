/*! \brief Constructs buffer given filepaths
 *
 * The factory is used to construct buffer when a filepath is provided,
 * The factory can be mocked to produce mock buffer for testing.
 */

#ifndef __BUFFER_FACTORY_H__
#define __BUFFER_FACTORY_H__

#include <memory>

#include "logmacros.h"
#include "buffer.h"

/*!
 * Factory for IBuffer objects.
 */
class IBufferFactory {
public:
  /*
   * Return a buffer object loaded with the file pointed to by filepath.
   */
  virtual std::unique_ptr<IBuffer> create_buffer(const std::string &filepath) = 0;
};

/*!
 * Implementation of the IBufferFactory interface.
 */
class BufferFactory : public IBufferFactory {
  std::unique_ptr<IBuffer> create_buffer(const std::string &filepath) {
    LOGDBG("Create buffer with filepath: " << filepath);
//    return std::make_unique<IBuffer>(filepath);
    return std::unique_ptr<IBuffer>(new Buffer(filepath));
  }
};

#endif // __BUFFER_H__