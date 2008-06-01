#include "barbelo.h"
#include "wifiscan.h"

CWifiScan::CWifiScan(CBarbEvents& barb) 
		: CActive(EPriorityStandard), _barb(barb),
		  _si(NULL), _cli(NULL), _state(WS_NONE), _running(false)
{
	CActiveScheduler::Add(this);
	_timer.CreateLocal();
}

CWifiScan::~CWifiScan()
{
	Cancel();

	delete _si;
	delete _cli;
}

void
CWifiScan::RunL()
{
	if (!_running) {
		if (_state == WS_SCAN)
			CActiveScheduler::Stop();
		return;
	}

	User::LeaveIfError(iStatus.Int());
	
	if (_state == WS_TIMER) {
		do_scan();
		return;
	}

	ASSERT(_state == WS_SCAN);
	for (_si->First(); !_si->IsDone(); _si->Next())
		_barb.notify_wifiscan(*_si);

	_barb.notify_wifiscan_done();

	do_timer();
}

void
CWifiScan::DoCancel()
{
	if (!IsActive())
		return;

	switch (_state) {
	case WS_SCAN:
		// XXX how do we cancel request?
		break;

	case WS_TIMER:
		_timer.Cancel();
		break;

	case WS_NONE:
		break;
	}
}

TInt
CWifiScan::RunError(TInt aError)
{
	_barb.notify_dmsg(_L("err"));

	return KErrNone;
}

void
CWifiScan::start()
{
	if (!_si)
        	_si = CWlanScanInfo::NewL();
	if (!_cli)
        	_cli = CWlanMgmtClient::NewL();

	_running = true;
	do_scan();
}

void
CWifiScan::do_scan()
{
	_state = WS_SCAN;
	_cli->GetScanResults(iStatus, *_si);
	SetActive();
}

void
CWifiScan::do_timer()
{
	_state = WS_TIMER;
	// N95 seems to refresh each 5s.  But you gotta do 2 requests.  WTF.  Or
	// more likely, I gotta learn how to sample =D.
	_timer.After(iStatus, 5 * 500000);
	SetActive();
}

void
CWifiScan::stop()
{
	_running = false;

	// XXX can't cancel scan
	if (IsActive() && _state == WS_SCAN)
		CActiveScheduler::Start();

	Cancel();
}
