#include <string>
#include <cmath>
#include "functions.hpp"
#include "channel_data.hpp"
#include "config_options.hpp"

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
        print(LOG_INFO, " LASTTIME didn't found any data!");
    }
    print(LOG_VERBOSE, "LASTTIME (%s)=%f", str.c_str(), toRet);
    return toRet;
}

double f_avgvalue(const std::string &str, const double &v)
{
    double toRet = NAN;
	if (v<=0.0) return toRet;

    double lastT = -1;
    // now determine avg of channel value from the last v seconds:
    MAP_ChannelDataList::const_iterator it = gChannelData.find(str);
    if (it != gChannelData.end()){
        const LIST_ChannelData &list = it->second;
		if (list.size()>1){
            double now = f_now();
            double startT = now - v;
			double lastV=0.0;
			lastT = now;
			toRet = 0;
			auto dit = list.crbegin();
			while (dit != list.crend()) {
				const double &t = (*dit)._t;
				if (t>startT) {
					// average with lastT-t share:
					lastV = (*dit)._v;
					toRet += (lastT-t)*lastV;
					lastT = t;
				} else { // t<=startT:
					// average with lastT-startT share:
					toRet += (lastT-startT)*((*dit)._v);
					lastT = startT;
					break;
				}
				++dit;
			}
			if (lastT > startT){
				// some data missing. fill with lastV;
				toRet += (lastT - startT) * lastV;
			}
			toRet /= v;
		} else if (list.size()==1) {
			const ChannelData &d = list.back();
			// first value as avg (we assume no difference from previous value:
			toRet = d._v;
		}
    } else {
        print(LOG_INFO, " AVGVALUE didn't found any data!");
    }


	print(LOG_VERBOSE, "AVGVALUE(%s, %f)=%f", str.c_str(), v, toRet);
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
        print(LOG_INFO, " MINVALUE didn't found any data!");
    }


    print(LOG_VERBOSE, "MINVALUE(%s, %f)=%f", str.c_str(), v, toRet);
    return toRet;
}

double f_maxvalue(const std::string &str, const double &v)
{
	double toRet = NAN;

	// now determine max of channel value from the last v seconds:
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
					if (d2._v > toRet) toRet = d2._v;
				}
			}
		}
	} else {
		print(LOG_INFO, " MAXVALUE didn't found any data!");
	}


	print(LOG_VERBOSE, "MAXVALUE(%s, %f)=%f", str.c_str(), v, toRet);
	return toRet;
}
