#include <json-c/json.h>
#include <stdio.h>
#include <sstream>
#include "config_options.hpp"
#include "rules.hpp"

bool parseChannels(struct json_object *jo, MAP_StrStr &channels)
{
    if (!jo) return false;
    if (json_object_get_type(jo)!=json_type_array) throw "wrong json type for channels. array expected!";
    int nrChannels;
    if ((nrChannels=json_object_array_length(jo))>=1) {
        // for each object:
        for (int i = 0; i < nrChannels; i++) {
            struct json_object *jb = json_object_array_get_idx(jo, i);
            // each object should consist of "uuid" and "alias"
            std::string uuid;
            struct json_object *value;
            if (json_object_object_get_ex(jb, "uuid", &value)) {
               uuid = json_object_get_string(value);
            } else {
                throw "channel without uuid!";
            }
            std::string alias;
            if (json_object_object_get_ex(jb, "alias", &value)) {
               alias = json_object_get_string(value);
            } else {
                alias = uuid; // using uuid as alias
            }
            printf(" channel: %s = %s\n", uuid.c_str(), alias.c_str());
            channels[uuid] = alias;
        }
    } else {
        printf ("no channels configured\n");
        return false;
    }

    return true;
}

bool parseRules(struct json_object *jo, List_ShPtrRule &rules)
{
    if (!jo) return false;
    if (json_object_get_type(jo)!=json_type_array) throw "wrong json type for rules. array expected!";
    int nrRules;
    if ((nrRules=json_object_array_length(jo))>=1) {
        // for each object:
        for (int i = 0; i < nrRules; i++) {
            struct json_object *jb = json_object_array_get_idx(jo, i);
            // each object should consist of: (name opt), condition, action
            std::string name ("rule ");
            name.append(std::to_string(rules.size()+1));

            struct json_object *value;
            if (json_object_object_get_ex(jb, "name", &value)){
                name = json_object_get_string(value);
            }

            struct json_object *cond, *action;
            if (!json_object_object_get_ex(jb, "condition", &cond))
                throw "condition needed";
            if (!json_object_object_get_ex(jb, "action", &action))
                throw "action needed";

            Rule *r=new Rule(name, cond, action);
            rules.push_back(std::shared_ptr<Rule> (r));
            std::ostringstream ss;
            ss << *r;
            printf(" rule '%s' added:\n", ss.str().c_str());
        }
    } else {
        printf ("no rules configured\n");
        return false;
    }

    return true;
}

bool parseConfigFile(const char *fileName, GlobalOptions *&go, MAP_StrStr &channels, List_ShPtrRule &rules)
{
    // parse file
    FILE *file = fopen(fileName, "r");
    if (file){
        printf("opened config file\n");
        std::string filedata;
        char buf[1024];
        while(fgets(buf, sizeof(buf), file)){
            filedata.append(buf);
        }
        fclose(file);
        // now try to parse this as a json object:
        struct json_tokener *tok = json_tokener_new();
        json_tokener_error jerr;
        do{
            json_object *jo = json_tokener_parse_ex(tok, filedata.c_str(), filedata.length());
            if (jo) {
                printf("processing jo\n");
                struct json_object *value;
                if (json_object_object_get_ex(jo, "global", &value)) {
                    if (go) delete go;
                    go = new GlobalOptions(value);
                }
                if (json_object_object_get_ex(jo, "channels", &value)) {
                    if (!parseChannels(value, channels)){
                        printf("error parsing channels!\n");
                        return false;
                    }
                }
                if (json_object_object_get_ex(jo, "rules", &value)) {
                    if (!parseRules(value, rules)) {
                        printf("error parsing rules!\n");
                        return false;
                    }
                }
            } else {
                printf("no jo!\n");
            }
            json_object_put(jo);
        } while((jerr = json_tokener_get_error(tok)) == json_tokener_continue);
        if (tok->char_offset < static_cast<int>(filedata.length())){
            printf("error processing config file. Not parsed: %s\n", filedata.substr(tok->char_offset).c_str());
            json_tokener_free(tok);
            return false;
        }
        json_tokener_free(tok);
        if (jerr != json_tokener_success) {
            printf("error parsing config file: %s\n", json_tokener_error_desc(jerr));
            return false;
        }

        // create GlobalOptions with given json object

    } else {
        printf("failed to open %s\n", fileName);
    }
    // if no go exists yet (e.g. empty config file) create from default:
    if (!go) go=new GlobalOptions();

    return true;
}

GlobalOptions::GlobalOptions(json_object *jo) :
    _port(8082), _verbosity(LOG_ERROR)
{
    if (!jo) return;
    if (json_object_get_type(jo)!=json_type_object) throw "wrong json type for global";
    struct json_object *value;
    if (json_object_object_get_ex(jo, "port", &value)) {
        _port = json_object_get_int(value);
        printf("GlobalOptions: port %d\n", _port);
    } // else stay with default

    if (json_object_object_get_ex(jo, "verbosity", &value)) {
        _verbosity = (LogVerbosity) json_object_get_int(value);
        printf("GlobalOptions: verbosity %d\n", _verbosity);
    } // else stay with default

}
