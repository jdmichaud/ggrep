/*! \brief BrowserModel for the ggrep application
 *
 *  The BrowserModel contains information about what is supposed to be
 *  displayed. This BrowserModel is an observable and is observed by the view.
 *  Upon change, the BrowserModel shall notify the observers.
 */

#ifndef __BROWSER_MODEL_H__
#define __BROWSER_MODEL_H__

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

  /*
   * Get the current displayed Buffer/BufferModel pair.
   * This converts the iterator to the BufferModel object.
   */
  Update<std::unique_ptr<BufferModel<> > > get_current_buffer();
};

#endif //__BROWSER_MODEL_H__