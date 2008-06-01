#ifndef __BARBELO_H__
#define __BARBELO_H__

#include <wlanmgmtclient.h>

#include "barbui.h"
#include "wifiscan.h"
#include "gps.h"
#include "ap.h"
#include "logger.h"

class CBarbelo : public CBarbEvents {
public:
	CBarbelo(MBarbUI& ui);
	~CBarbelo();

	void own();
	void die();
	CAP* find_ap(TWlanBssid& x);

	// events
	void notify_wifiscan(CWlanScanInfo& si);
	void notify_wifiscan_done();
	void notify_dmsg(const TDesC& msg);
	void notify_position();
	void log_start();
	void log_stop();

private:
	MBarbUI&	_ui;
	CWifiScan	_wifi;
	APS		_aps;
	CGPS		_gps;
	TBool		_pos_changed;
	CLogger		_logger;
};

#endif // __BARBELO_H__
