#ifndef __BARBEVENTS_H__
#define __BARBEVENTS_H__

#include <wlanscaninfo.h>

class CBarbEvents {
public:
	virtual ~CBarbEvents() {}

	virtual void notify_wifiscan(CWlanScanInfo& si) = 0;
	virtual void notify_wifiscan_done() = 0;
	virtual void notify_dmsg(const TDesC& msg) = 0;
	virtual void notify_position() = 0;
};

#endif // __BARBEVENTS_H__
