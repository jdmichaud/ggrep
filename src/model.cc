#include "model.h"

Model::Model() :
    _first_scr_line(0),
    _last_scr_line(0)
{
}

void Model::set_first_scr_line(uint first_scr_line) {
  _first_scr_line = first_scr_line;
  notify_observers();
}

uint Model::get_first_scr_line() {
  return _first_scr_line;
}

void Model::set_last_scr_line(uint last_scr_line) {
  _last_scr_line = last_scr_line;
  notify_observers();
}

uint Model::get_last_scr_line() {
  return _last_scr_line;
}
