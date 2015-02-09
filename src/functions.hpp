#ifndef __functions_hpp_
#define __functions_hpp_

#include <string>
#include <sys/time.h>

double f_now();
double f_lasttime(const std::string &str);
double f_avgvalue(const std::string &str, const double &v);
double f_minvalue(const std::string &str, const double &v);

#endif
