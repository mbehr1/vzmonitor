#ifndef __rules_hpp_
#define __rules_hpp_

#include <string>
#include "condition.hpp"
#include "action.hpp"

struct json_object;
Condition *generate_condition(struct json_object *cond);

class Rule
{
public:
    Rule(std::string &name, struct json_object *condition, struct json_object *action);
    virtual ~Rule(){if (_action) delete _action; if (_cond) delete _cond;};

    void check();

protected:
    bool _last_state;
    std::string _name;
    Condition *_cond;
    Action *_action;

	friend class Action;
    friend std::ostream &operator<<(std::ostream &os, Rule const &r);
};

extern std::ostream &operator<<(std::ostream &os, Rule const &r);

#endif
