#ifndef __channel_data_hpp_
#define __channel_data_hpp_

#include <list>
#include <map>
#include <string>

class ChannelData
{
public:
    ChannelData( const double &t, const double &v) : _t(t), _v(v) {};
    double _t;
    double _v;
};

typedef std::list<ChannelData> LIST_ChannelData;

typedef std::map<std::string, LIST_ChannelData> MAP_ChannelDataList;

extern MAP_ChannelDataList gChannelData;

#endif
