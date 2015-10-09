#include <iostream>
#include "filter_set.h"

std::ostream& operator<<(std::ostream& os, const filter_set_t& filter_set)
{
  for (auto re: filter_set.filters) {
    os << re.first;
    os << (filter_set.land ? " & " : " | ");
  }
  return os;
}
