/*! \brief ContentModel for the ggrep application
 *
 *  The ContentModel contains information about what is supposed to be
 *  displayed. This ContentModel is an observable and is observed by the view.
 *  Upon change, the ContentModel shall notify the observers.
 */

#ifndef  __CONTENT_MODEL_H__
#define __CONTENT_MODEL_H__

#include <vector>
#include "types.h"
#include "observable.hpp"

class ContentModel : public Observable
{
public:
    ContentModel();

    void set_first_scr_line(uint first_scr_line);
    uint get_first_scr_line();
    void set_last_scr_line(uint first_scr_line);
    uint get_last_scr_line();

private:
    uint _first_scr_line;
    uint _last_scr_line;
    std::vector<char *> _displayed_lines;
};

#endif //__CONTENT_MODEL_H__