#include <json-c/json.h>
#include <stdio.h>
#include <stdarg.h>
#include <sstream>
#include <sys/time.h>
#include <unistd.h>
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
            print(LOG_INFO, " channel: %s = %s", uuid.c_str(), alias.c_str());
            channels[uuid] = alias;
        }
    } else {
        print (LOG_WARNING, "no channels configured");
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
            print(LOG_INFO, " added '%s'", ss.str().c_str());
        }
    } else {
        print(LOG_WARNING, "no rules configured");
        return false;
    }

    return true;
}

bool parseConfigFile(const char *fileName, GlobalOptions *&go, MAP_StrStr &channels, List_ShPtrRule &rules)
{
    // parse file
    FILE *file = fopen(fileName, "r");
    if (file){
        print(LOG_VERBOSE, "opened config file");
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
                struct json_object *value;
                if (json_object_object_get_ex(jo, "global", &value)) {
                    if (go) delete go;
                    go = new GlobalOptions(value);
                }
                if (json_object_object_get_ex(jo, "channels", &value)) {
                    if (!parseChannels(value, channels)){
                        print(LOG_ERROR, "error parsing channels!");
                        return false;
                    }
                }
                if (json_object_object_get_ex(jo, "rules", &value)) {
                    if (!parseRules(value, rules)) {
                        print(LOG_ERROR, "error parsing rules!");
                        return false;
                    }
                }
            } else {
                print(LOG_ERROR, "parseConfigFile, no jo!");
            }
            json_object_put(jo);
        } while((jerr = json_tokener_get_error(tok)) == json_tokener_continue);
        if (tok->char_offset < static_cast<int>(filedata.length())){
            print(LOG_ERROR, "error processing config file. Not parsed: %s", filedata.substr(tok->char_offset).c_str());
            json_tokener_free(tok);
            return false;
        }
        json_tokener_free(tok);
        if (jerr != json_tokener_success) {
            print(LOG_ERROR, "error parsing config file: %s", json_tokener_error_desc(jerr));
            return false;
        }

        // create GlobalOptions with given json object

    } else {
        print(LOG_ERROR, "failed to open %s", fileName);
    }
    // if no go exists yet (e.g. empty config file) create from default:
    if (!go) go=new GlobalOptions();

    return true;
}

GlobalOptions::GlobalOptions(json_object *jo) :
	_port(8082), _verbosity(LOG_WARNING), _maxChannelDataAge(60*60), _initialDelay(15)
{
    if (!jo) return;
    if (json_object_get_type(jo)!=json_type_object) throw "wrong json type for global";
    struct json_object *value;
    if (json_object_object_get_ex(jo, "port", &value)) {
        _port = json_object_get_int(value);
        print(LOG_VERBOSE, "GlobalOptions: port %d", _port);
    } // else stay with default

    if (json_object_object_get_ex(jo, "verbosity", &value)) {
        _verbosity = (LogVerbosity) json_object_get_int(value);
        print(LOG_VERBOSE, "GlobalOptions: verbosity %d", _verbosity);
    } // else stay with default

    if (json_object_object_get_ex(jo, "maxChannelDataAge", &value)) {
        _maxChannelDataAge = json_object_get_int(value);
        print(LOG_VERBOSE, "GlobalOptions: maxChannelDataAge %d", _maxChannelDataAge);
    } // else stay with default (1h)

	if (json_object_object_get_ex(jo, "initialDelay", &value)) {
		_initialDelay = json_object_get_int(value);
		print(LOG_VERBOSE, "GlobalOptions: initialDelay %d", _initialDelay);
	} // else stay with default (15s)

}

void print(const LogVerbosity &level, const char *format,... ) {
	if (level > (gGlobalOptions ? gGlobalOptions->_verbosity : LOG_WARNING)) {
		return; /* skip message if its under the verbosity level */
	}

	struct timeval now;
	struct tm * timeinfo;
	char prefix[24];
	size_t pos = 0;

	gettimeofday(&now, NULL);
	timeinfo = localtime(&now.tv_sec);

	/* format timestamp */
	pos += strftime(prefix+pos, 18, "[%b %d %H:%M:%S]", timeinfo);

	va_list args;
	va_start(args, format);
	/* print to stdout/stderr */
	if (getppid() != 1) { /* running as fork in background? */
		FILE *stream = (level > 0) ? stdout : stderr;

		fprintf(stream, "%-24s", prefix);
		vfprintf(stream, format, args);
		fprintf(stream, "\n");
	}
	va_end(args);

	va_start(args, format);
	// append to logfile
/*	if (options.logfd()) {
		fprintf(options.logfd(), "%-24s", prefix);
		vfprintf(options.logfd(), format, args);
		fprintf(options.logfd(), "\n");
		fflush(options.logfd());
	} */
	va_end(args);
}

