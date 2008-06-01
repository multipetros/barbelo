#ifndef __BARBUI_H__
#define __BARBUI_H__

#include "ap.h"

class MBarbUI {
public:
	virtual ~MBarbUI() {}

	virtual void dmsg(const TDesC& msg) = 0;
	virtual void refresh_aps(APS& ap) = 0;
	virtual void ap_details(CAP& ap) = 0;
	virtual void refresh_map(APS& ap, CGPS& gps) = 0;
};

#endif // __BARBUI_H__
