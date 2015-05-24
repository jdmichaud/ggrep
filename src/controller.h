/*! \brief Controller for the ggrep application
 *
 *  The controller gathers the input from the view and update the models
 *  accordingly.
 */

#ifndef  __CONTROLLER_H__
#define __CONTROLLER_H__

#include <list>
#include "view.h"
#include "buffer_model.h"
#include "fbar_model.h"
#include "prompt_model.h"

class Controller
{
public:
  Controller(BufferModel &buffer_model, 
             FBarModel &fbar_model, 
             PromptModel &prompt_model);

  /**
   * A view injects a character through this api.
   */
  void inject(int key);

  /** Bind a view to the controller. The controller is supposed to interact only
    * with models and not the view. But this priviledged access to the view is
    * restricted to special interaction like requesting an input (prompt).
    * 
    */
  void bind_view(IView &);

  inline bool is_interrupted() { return _interrupted; }

  void start();

private:
  std::list<IView *> _views;
  bool  _interrupted;

  // For now let's assume only one set of model
  BufferModel   &_buffer_model;
  FBarModel     &_fbar_model;
  PromptModel   &_prompt_model;

};

#endif //__CONTROLLER_H__