/*! \brief Model for the ggrep application
 *
 *  The model contains information about what is supposed to be displayed.
 *  This model is an observable and is observed by the view.
 *  Upon change, the model shall notify the observers.
 */

#ifndef  __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "types.h"
#include "observable.hpp"

class Model : public Observable
{
public:
    Model();

    void set_first_scr_line(uint first_scr_line);
    uint get_first_scr_line();
    void set_last_scr_line(uint first_scr_line);
    uint get_last_scr_line();

private:
    uint _first_scr_line;
    uint _last_scr_line;
    std::vector<char *> _displayed_lines;
};

#endif //__MODEL_H__