/*! \brief PromptModel for the ggrep application
 *
 *  The prompt model contain the prompt information.
 */

#ifndef  __PROMPT_MODEL_H__
#define __PROMPT_MODEL_H__

#include <string>
#include "types.h"
#include "model.h"

#define DEFAULT_PROMPT ":"

class PromptModel : public Model
{
public:
  PromptModel();
  DECLARE_ENTRY( PromptModel, prompt, std::string);
  DECLARE_ENTRY( PromptModel, cursor_position, pos);
};

#endif //__PROMPT_MODEL_H__