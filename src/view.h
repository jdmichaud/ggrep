/*! \brief A generic view, base of concrete views
 * The view observes the model and is being observed by the controller.
 */

#ifndef __VIEW_H__
#define __VIEW_H__

#include <vector>

class Controller;

class IView
{
public:
  /** Wait for a user interaction.
   */
  virtual void prompt(Controller &) = 0;
};

#endif // __VIEW_H__
