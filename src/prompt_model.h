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

private:
    std::string _prompt;
};

#endif //__PROMPT_MODEL_H__