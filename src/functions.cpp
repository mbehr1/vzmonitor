#include <string>
#include "functions.hpp"
#include "channel_data.hpp"

double f_now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + ((double)tv.tv_usec / 1e6);
}

double f_lasttime(const std::string &str)
{
    double toRet = 0.0;
    // search for channel with alias str the last time (assuming sorted!)
    MAP_ChannelDataList::const_iterator it = gChannelData.find(str);
    if (it != gChannelData.end()){
        const LIST_ChannelData &list = it->second;
        if (list.size()){
            const ChannelData &d = list.back();
            toRet = d._t;
        }
    } else {
        printf(" LASTTIME didn't found any data!\n");
    }
    printf("LASTTIME (%s)=%f\n", str.c_str(), toRet);
    return toRet;
}

