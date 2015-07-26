/*! \brief StateModel for the ggrep application
 *
 * The state model contains the state of the application. It is a replication
 * of he controller's context but with an underlying Model pattern in order for
 * the view to adapt its display depending of the state.
 */

#ifndef __STATE_MODEL_H__
#define __STATE_MODEL_H__

#include <string>
#include "types.h"
#include "model.h"
#include "state.h"

class StateModel : public Model
{
public:
  StateModel();
  DECLARE_ENTRY( StateModel, state, state_e );
};

#endif //__STATE_MODEL_H__