#include "logmacros.h"
#include "fbar_model.h"

FBarModel::FBarModel() :
  m_functions({
      { "F1", "Help", false, false },
      { "F2", "Mark", false, false },
      { "F3", "Search", false, false }
  }) {
}

void FBarModel::add_function(function_t function) {
  auto u = this->set_functions();
  u.update().push_back(function);
  LOGDBG("TEST");
}
