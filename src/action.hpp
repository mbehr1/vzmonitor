#ifndef __action_hpp_
#define __action_hpp_

#include <string>

struct json_object;

class Action
{
public:
    Action(struct json_object *jo);
    virtual ~Action(){};
    virtual void fire(bool active);
protected:
    std::string _cmd;
};

#endif
