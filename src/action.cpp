#include <stdio.h>
#include <json-c/json.h>
#include "action.hpp"

Action::Action(struct json_object *jo)
{
    if (!jo) throw "Action::Action jo missing";
    if (json_object_get_type(jo)!=json_type_string)
        throw "Action::Action currently only string supported!";
    _cmd = json_object_get_string(jo);
}

void Action::fire(bool active)
{
    if (active)
        printf("Action fired: '%s'!\n", _cmd.c_str());
    else
        printf("Action disarmed: '%s'!\n", _cmd.c_str());
}

