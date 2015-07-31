/*! \brief BrowserModel for the ggrep application
 *
 *  The BrowserModel contains information about what is supposed to be
 *  displayed. This BrowserModel is an observable and is observed by the view.
 *  Upon change, the BrowserModel shall notify the observers.
 */

#ifndef __BROWSER_MODEL_H__
#define __BROWSER_MODEL_H__

#include <vector>
#include <memory>
#include "types.h"
#include "model.h"
#include "buffer_model.h"

class BrowserModel : public Model
{
public:
  BrowserModel();

  DECLARE_ENTRY( BrowserModel, buffers, buffer_list);
  DECLARE_ENTRY( BrowserModel, current_buffer, buffer_list::iterator );
  DECLARE_ENTRY( BrowserModel, view_line_number, uint );

public:
  /*
   * Get the current displayed Buffer/BufferModel pair.
   * This converts the iterator to the BufferModel object.
   */
  buffer_list::const_iterator get_current_buffer();
  /*
   * Return the nth buffer in the buffer list
   */
  Update<buffer_list::iterator> set_nth_buffer(uint idx);
  /*
   * Set the current buffer to the pointer buffer by the _const_ iterator
   */
  void set_current_buffer(buffer_list::const_iterator);
  /*
   * Add a new buffer
   */
  void emplace_buffer(std::unique_ptr<BufferModel> &&buffer_model);

private:
  BrowserModel(const BrowserModel &) = delete;
};

#endif //__BROWSER_MODEL_H__