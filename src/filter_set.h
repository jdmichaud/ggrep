#ifndef __FILTER_SET_H__
#define __FILTER_SET_H__

#include <list>
#include <regex>
#include <string>
#include <iostream>

/*
 * This structure is the logical representation of the filters set on each
 * buffer. The last filters added is at the tail of the queue.
 */
struct filter_set_t {
  std::list< std::pair<std::string, std::regex> > filters;
  bool  land; // logical and if true, logical or otherwise
  bool  dynamic; // last filter is dynamic i.e. being interactivally edited
};

std::ostream& operator<<(std::ostream& os, const filter_set_t& filter_set);

#endif //__FILTER_SET_H__
