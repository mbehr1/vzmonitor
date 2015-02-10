#ifndef __config_options_hpp_
#define __config_options_hpp_

#include <string>
#include <map>
#include <list>
#include <memory>

struct json_object;

enum LogVerbosity {
	LOG_NONE=0,
	LOG_ERROR=5,
	LOG_WARNING=10,
	LOG_INFO=15,
	LOG_DEBUG=20,
	LOG_VERBOSE=25
};

class GlobalOptions {
public:
	GlobalOptions() : _port(8082), _verbosity(LOG_ERROR) {};
	GlobalOptions( struct json_object *jo); // init from json object

	int _port; // http port to listen to data for
	LogVerbosity _verbosity;
	int _maxChannelDataAge; // ChannelData older than that will be removed (secs)
};

extern class GlobalOptions *gGlobalOptions;

typedef std::map<std::string, std::string> MAP_StrStr;
class Rule;
typedef std::list<std::shared_ptr<Rule>> List_ShPtrRule;

bool parseConfigFile(const char *fileName, GlobalOptions *&go, MAP_StrStr &channels, List_ShPtrRule &rules); // creates object if go NULL

void print(const LogVerbosity &level, const char *format, ... );

#endif
