/*! \brief FBarModel for the ggrep application
 *
 *  The FBarModel holds the status of the function bar.
 */

#ifndef  __FBAR_MODEL_H__
#define __FBAR_MODEL_H__

#include <string>
#include <vector>
#include "types.h"
#include "model.h"

typedef struct {
  std::string key;
  std::string label;
  bool reverse_video;
  bool disabled;
} function_t;

class FBarModel : public Model
{
public:
  FBarModel();
  void add_function(function_t function);

  DECLARE_ENTRY( FBarModel, functions, std::vector<function_t> )
};

#endif //__FBAR_MODEL_H__