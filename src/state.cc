#include <iostream>

#include "state.h"

std::ostream& operator<<(std::ostream& os, const state_e& state)
{
  switch (state) {
    case state_e::CLOSE_STATE:  os << "CLOSED_STATE"; break;
    case state_e::OPEN_STATE:   os << "OPEN_STATE"; break;
    case state_e::BROWSE_STATE: os << "BROWSE_STATE"; break;
    case state_e::ERROR_STATE:  os << "ERROR_STATE"; break;
    case state_e::FILTER_STATE: os << "FILTER_STATE"; break;
    case state_e::ADD_FILTER_STATE: os << "ADD_FILTER_STATE"; break;
    default: os << "*Unknown state*";
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const IState& s) {
  os << s.get_id();
  return os;
};