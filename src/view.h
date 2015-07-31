/*! \brief A generic view, base of concrete views
 * The view observes the model and is being observed by the controller.
 */

#ifndef __VIEW_H__
#define __VIEW_H__

#include <vector>
#include "model.h"

class Controller;

class IView
{
public:
  /** Wait for a user interaction.
   */
  virtual void prompt(Controller &) = 0;
  /** Signal the creation of a new buffer
   */
  virtual void new_buffer(Model &) = 0;
  /** Provide the view size for text browsing commands
  */
  virtual void get_view_size(uint &nlines, uint ncols) = 0;
};

#endif // __VIEW_H__
