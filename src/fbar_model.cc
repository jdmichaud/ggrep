#include "logmacros.h"
#include "fbar_model.h"

FBarModel::FBarModel() :
  _functions({
      { "F1", "Help", false, false },
      { "F2", "Mark", false, false },
      { "F3", "Search", false, false }
  }) {
}

void FBarModel::add_function(function_t function) {
  this->_functions.push_back(function);
  LOGDBG("notify observers") 
  this->notify_observers();
}