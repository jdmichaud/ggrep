#include "browser_model.h"

BrowserModel::BrowserModel() :
  m_current_buffer(m_buffers.end())
{
}

Update<std::unique_ptr<BufferModel<> > > BrowserModel::get_current_buffer()
{
  return Update<BufferModel<> >(*m_current_buffer,
                                std::bind(&BrowserModel<>::notify_observers, this));
}