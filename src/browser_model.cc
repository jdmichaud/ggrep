#include <memory>
#include "model.h"
#include "browser_model.h"

BrowserModel::BrowserModel() :
  m_current_buffer(m_buffers.end()), m_view_line_number(0)
{
}

buffer_list::const_iterator BrowserModel::get_current_buffer()
{
  return m_current_buffer;
}

Update<buffer_list::iterator> BrowserModel::set_nth_buffer(uint idx)
{
  notify_callback_t f = std::bind(&BrowserModel::notify_observers, this);
  buffer_list::iterator nth = std::next(m_buffers.begin(), idx);
  return Update<buffer_list::iterator>(nth, f);
}

void BrowserModel::set_current_buffer(buffer_list::const_iterator it)
{
  (*this).set_current_buffer().update() = std::find(m_buffers.begin(),
                                                    m_buffers.end(), *it);
}

void BrowserModel::emplace_buffer(std::unique_ptr<BufferModel> &&buffer_model) {
  LOGDBG("&buffer_model: " << &(*buffer_model));
  // Kepp the update alive so the current buffer as time to be set
  Update<buffer_list> u = (*this).set_buffers();
  // push back the new buffer
  u.update().emplace_back(std::move(buffer_model));
  // if no buffer was set before, then set the current buffer to the newly
  // created one
  if (m_buffers.size() == 1) {
    (*this).set_current_buffer().update() = m_buffers.begin();
  }
}
