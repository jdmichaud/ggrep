#include "browser_model.h"

BrowserModel::BrowserModel() :
  m_current_buffer(m_buffers.end())
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
  set_current_buffer().update() = std::find(m_buffers.begin(), 
                                            m_buffers.end(), *it);
}
