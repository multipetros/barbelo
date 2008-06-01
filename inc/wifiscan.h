#ifndef __WIFISCAN_H__
#define __WIFISCAN_H__

#include <wlanmgmtclient.h>

#include "barbevents.h"

class CWifiScan : public CActive {
public:
	CWifiScan(CBarbEvents& barb);
	~CWifiScan();

	void start();
	void stop();

protected:
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

private:
	enum State {
		WS_NONE = 0,
		WS_SCAN,
		WS_TIMER,
	};

	void do_scan();
	void do_timer();

	CBarbEvents&		_barb;
	CWlanScanInfo*		_si;
	CWlanMgmtClient*	_cli;
	State			_state;
	RTimer			_timer;
	bool			_running;
};

#endif // __WIFISCAN_H__
