#include <iostream>

#include "state.h"

std::ostream& operator<<(std::ostream& os, const state_e& state)
{
  switch (state) {
    case state_e::CLOSE_STATE:  os << "CLOSED_STATE"; break;
    case state_e::OPEN_STATE:   os << "OPEN_STATE"; break;
    case state_e::BROWSE_STATE: os << "BROWSE_STATE"; break;
    case state_e::ERROR_STATE:  os << "ERROR_STATE"; break;
    default: os << "*Unknown state*";
  }
  return os;
}