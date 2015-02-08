/**
 * main source file for vzmonitor
 *
 * @package vzmonitor
 * @copyright Copyright (c) 2015, Matthias Behr
 * @license http://www.gnu.org/licenses/gpl.text GNU Public License V2
 * @author Matthias Behr < mbehr (a) mcbehr.de >
 */

/* TODO list
 - cmdline options
 - daemonize
 - multihost support /hostuuid/add/data... (or oauth,...)
 - debug problem that first (large) http request misses the last bracket ]

*/


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <string>
#include <map>
#include <json-c/json.h>

#include <microhttpd.h>
#include "config.hpp"
#include "gitSha1.h"

#include "config_options.hpp"
#include "channel_data.hpp"
#include "rules.hpp"

GlobalOptions *gGlobalOptions=0;
MAP_StrStr gChannels;
List_ShPtrRule gRules;
MAP_ChannelDataList gChannelData;

typedef std::map<std::string, std::string*> MAP_SSP;
MAP_SSP gMapUUIDInput;

pthread_mutex_t gChannelDataMutex = PTHREAD_MUTEX_INITIALIZER;

void process_new_input()
{
	pthread_mutex_lock(&gChannelDataMutex);
	for ( MAP_SSP::iterator it=gMapUUIDInput.begin(); it!=gMapUUIDInput.end(); ++it){
		// printf("processing uuid=%s : %s\n", it->first.c_str(), it->second ? it->second->c_str() : "<null>");
		std::string *str = it->second;
		if (str){

			// if uuid is a known channel?
			MAP_StrStr::const_iterator git = gChannels.find(it->first);
			if (git!=gChannels.end()){
				// add reading! TODO lock access against other threads!
				LIST_ChannelData &list = gChannelData[git->second];
				// now treat str as a json str:
				struct json_object *jo = json_tokener_parse(str->c_str());
				if (jo) {
					int nr;
					if ((nr=json_object_array_length(jo))>=1) {
						// for each object:
						printf("process_new_input: adding %d values\n", nr);
						for (int i = 0; i < nr; i++) {
							struct json_object *jb = json_object_array_get_idx(jo, i);
							int nrr = json_object_array_length(jb);
							if (nrr==2){
								double t = json_object_get_double(json_object_array_get_idx(jb, 0));
								double v = json_object_get_double(json_object_array_get_idx(jb, 1));
								// printf(" adding %f / %f\n", t, v);
								list.push_back(ChannelData(t,v));
							} else {
								printf("reading with %d items ignored!", nrr);
							}
						}
					}
					json_object_put(jo);
				} else {
					printf(" couldn't json parse '%s'\n", str->c_str());
				}
			}
			str->clear();
		}
	}
	pthread_mutex_unlock(&gChannelDataMutex);
}

const int POSTBUFFERSIZE=256;

enum ConnectionType {
	POST,
	GET
};

struct connection_info_struct
{
	ConnectionType connectiontype;
	char *answerstring;
	struct MHD_PostProcessor *postprocessor;
};

int iterate_post (void *coninfo_cls, enum MHD_ValueKind kind, const char *key,
				  const char *filename, const char *content_type,
				  const char *transfer_encoding, const char *data,
				  uint64_t off, size_t size)
{
	struct connection_info_struct *con_info = (struct connection_info_struct *)coninfo_cls;
	printf("iterate_post: key=%s, filename=%s, content_type=%s, transfer_encoding=%s, size=%d",
		   key, filename, content_type, transfer_encoding, (int)size);
	(void) con_info;

	return MHD_YES;
}

int answer_to_connection(
	void *cls,
	struct MHD_Connection *connection,
	const char *uri,
	const char *method,
	const char *version,
	const char *upload_data,
	size_t *upload_data_size,
	void **con_cls
	)
{
	int status = MHD_NO;
	int response_code = MHD_HTTP_NOT_FOUND;
	struct MHD_Response *response;

//	const char *encoding = MHD_lookup_connection_value(connection, MHD_HEADER_KIND,
//													   MHD_HTTP_HEADER_CONTENT_TYPE);

//	printf("http request received: method=%s uri=%s encoding = %s upload_data = %p, upload_data_size=%u\n",
//		   method, uri, encoding ? encoding : "<null>", upload_data, upload_data_size ? *upload_data_size : -1);



	if (NULL == *con_cls) {
		struct connection_info_struct *con_info;
		con_info = (struct connection_info_struct *)malloc(sizeof(struct connection_info_struct));
		if (NULL == con_info) return MHD_NO;
		con_info->answerstring = NULL;

		if (0 == strcmp (method, "POST")){
			con_info->postprocessor = MHD_create_post_processor(connection,
																POSTBUFFERSIZE,
																iterate_post,
																(void*)con_info);
			if (!con_info->postprocessor){
// we ignore that for application/json	printf("couldn't create postprocessor!\n");
//				free (con_info);
//				return MHD_YES;
			}
			con_info->connectiontype = POST;
		}else
			con_info->connectiontype = GET;
		*con_cls = (void*) con_info;
		// printf("con_cls created!\n");
		return MHD_YES;
	}

	if (strcmp(method, "POST")==0) {
		std::string s_uri(uri);
		if (s_uri.compare(0, 10, "/add/data/")==0){

			std::string s_uuid_inclext (s_uri, 10);
			unsigned found = s_uuid_inclext.find_last_of(".");
			std::string s_uuid = s_uuid_inclext.substr(0, found);
			//printf(" adding UUID=%s\n", s_uuid.c_str());
			std::string *&uuid_input = gMapUUIDInput[s_uuid];
			if (!uuid_input) uuid_input = new std::string();

			struct connection_info_struct *con_info = (struct connection_info_struct *)*con_cls;
			if (*upload_data_size != 0){
				if (con_info->postprocessor)
					MHD_post_process( con_info->postprocessor, upload_data, *upload_data_size);
				else {
					// we process the data on our own!
					//printf("upload_data=%s\n", upload_data);
					uuid_input->append(upload_data, *upload_data_size);
				}
				*upload_data_size = 0;
				return MHD_YES;
			} // TODO return error

			// POST request done: process data:
			//printf("uuid_input=%s\n", uuid_input ? uuid_input->c_str() : "<null>");
			process_new_input();

			response_code = MHD_HTTP_OK;
			const char *response_str = "OK added!";
			response = MHD_create_response_from_data(strlen(response_str), (void *)response_str, false, false); // must_free, must_copy
			MHD_add_response_header(response, "Content-type", "text/text");
			status = MHD_queue_response(connection, response_code, response);
			MHD_destroy_response(response);
		} // else TODO return error!

	}else{ // not POST (most likely GET)
		response_code = MHD_HTTP_OK;
		const char *response_str = "OK / ignored everything ;-)";
		response = MHD_create_response_from_data(strlen(response_str), (void *)response_str, false, false); // must_free, must_copy
		MHD_add_response_header(response, "Content-type", "text/text");
		status = MHD_queue_response(connection, response_code, response);
		MHD_destroy_response(response);
	}
	return status;
}

void request_completed(void *cls, struct MHD_Connection *connection,
					   void **con_cls,
					   enum MHD_RequestTerminationCode toe)
{
	struct connection_info_struct *con_info = (struct connection_info_struct *)*con_cls;
	if (NULL == con_info) return;
	if (con_info->connectiontype == POST) {
		printf( " POST request completed\n");

		if (con_info->postprocessor)
			MHD_destroy_post_processor( con_info->postprocessor );
		if (con_info->answerstring) free (con_info->answerstring);
	}
	free( con_info );
	*con_cls = NULL;
}

volatile bool gStop = false;

void quit(int sig) {
	gStop = true;
}

int main(int argc, char *argv[]) {

	struct MHD_Daemon *httpd_handle = NULL;

	printf("%s version %s\n", PACKAGE, g_GIT_SHALONG);

	// install signal handler
	struct sigaction action;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	action.sa_handler = quit;
	sigaction(SIGINT, &action, NULL); // ctrl-c
	sigaction(SIGHUP, &action, NULL); // hang up -> could be used to reload config
	sigaction(SIGTERM, &action, NULL); // kill

	// read options:
	if (!parseConfigFile("../etc/vzmonitor.conf", gGlobalOptions,
						 gChannels, gRules)){
		printf("failed to parse config file /etc/vzmonitor.conf!\n");
		return 1;
	}
	assert(gGlobalOptions);

	// start webserver to listen for data:
	httpd_handle = MHD_start_daemon( 
		MHD_USE_SELECT_INTERNALLY, // MHD_USE_THREAD_PER_CONNECTION,
		gGlobalOptions->_port,
		NULL, NULL,
		&answer_to_connection, NULL,
		MHD_OPTION_NOTIFY_COMPLETED, &request_completed, NULL,
		MHD_OPTION_END
		);

	printf("listening on port %d\n", gGlobalOptions->_port);

	while(!gStop) {
		// check rules
		pthread_mutex_lock(&gChannelDataMutex);

		List_ShPtrRule::const_iterator it = gRules.cbegin();

		for (; it!=gRules.cend(); ++it){
			Rule *r = (*it).get();
			if (r){
				r->check();
			}
		}

		pthread_mutex_unlock(&gChannelDataMutex);
		// wait a bit:
		sleep(1);
	}

	MHD_stop_daemon(httpd_handle);

	printf("stopped listening\n" );

	delete gGlobalOptions;
	gGlobalOptions = 0;
	return 0;
}
