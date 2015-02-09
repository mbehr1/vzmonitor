#include <string>
#include <cmath>
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

double f_avgvalue(const std::string &str, const double &v)
{
    double toRet = NAN;

    double lastT = -1;
    // now determine avg of channel value from the last v seconds:
    MAP_ChannelDataList::const_iterator it = gChannelData.find(str);
    if (it != gChannelData.end()){
        const LIST_ChannelData &list = it->second;
        if (list.size()){
            const ChannelData &d = list.back();
            // first value:
            toRet = d._v;
            lastT = d._t;
            double now = f_now();
            double startT = now - v;
            if (lastT>startT) {
                // we need more values and we can't expect aequidistance!
                // TODO!
            }
        }
    } else {
        printf(" AVGVALUE didn't found any data!\n");
    }


    printf("AVGVALUE(%s, %f)=%f\n", str.c_str(), v, toRet);
    return toRet;
}

double f_minvalue(const std::string &str, const double &v)
{
    double toRet = NAN;

    // now determine min of channel value from the last v seconds:
    MAP_ChannelDataList::const_iterator it = gChannelData.find(str);
    if (it != gChannelData.end()){
        const LIST_ChannelData &list = it->second;
        if (list.size()){
            const ChannelData &d = list.back();
            // first value:
            toRet = d._v;
            double now = f_now();
            double startT = now - v;
            if (d._t>startT) {
                // we need more values:
                auto cit = list.crbegin();
                for (++cit; cit!= list.crend(); ++cit){ // skip the first(last) value
                    const ChannelData &d2 = *cit;
                    if (d2._t<startT) break;
                    if (d2._v < toRet) toRet = d2._v;
                }
            }
        }
    } else {
        printf(" MINVALUE didn't found any data!\n");
    }


    printf("MINVALUE(%s, %f)=%f\n", str.c_str(), v, toRet);
    return toRet;
}
