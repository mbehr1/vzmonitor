#include <stdio.h>
#include <stdlib.h>
#include <json-c/json.h>
#include <sstream>
#include "action.hpp"
#include "config_options.hpp"
#include "rules.hpp"

Action::Action(struct json_object *jo)
{
    if (!jo) throw "Action::Action jo missing";
    if (json_object_get_type(jo)!=json_type_string)
        throw "Action::Action currently only string supported!";
    _cmd = json_object_get_string(jo);
}

void Action::fire(bool active, const Rule *r)
{
    std::ostringstream ss;
    if (r) ss << *r;

    if (active) {
        print(LOG_ERROR, "%s -> Action fired: '%s'!", ss.str().c_str(),_cmd.c_str());
        std::string cmd = _cmd;
        cmd.append(" fire");
        system(cmd.c_str());
    } else {
        print(LOG_ERROR, "%s -> Action disarmed: '%s'!", ss.str().c_str(), _cmd.c_str());
        std::string cmd = _cmd;
        cmd.append(" disarmed");
        system(cmd.c_str());
    }
}

std::ostream &operator<<(std::ostream &os, Action const &r)
{
    os << std::string("Action '") << r._cmd << std::string("'");
    return os;
}
