#include <eikstart.h>
#include <barsread.h>
#include <rconnmon.h>
#include <wlanmgmtclient.h>
#include <lbs.h>

#include <barbelo.rsg>
#include "barbelo.h"

CBarbelo::CBarbelo(MBarbUI& ui) : _ui(ui), _wifi(*this), _gps(*this),
				  _pos_changed(EFalse), _logger(_aps)
{
}

CBarbelo::~CBarbelo()
{
	_aps.ResetAndDestroy();
}

void
CBarbelo::notify_wifiscan(CWlanScanInfo& scanInfo)
{
	// find the BSSID
	TWlanBssid bssid;
	scanInfo.Bssid(bssid);

	CAP* ap = find_ap(bssid);
	if (!ap) {
		ap = new CAP(bssid);
		_aps.Append(ap);
	} 

	// set params
	TUint8 len;
	const TUint8* data;
	if (scanInfo.InformationElement(0, len, &data) == KErrNone)
		ap->set_ssid(data, len);

	TUint8 signal = scanInfo.RXLevel();
	ap->set_signal(signal);
	ap->set_security(scanInfo.SecurityMode());
	if (scanInfo.InformationElement(3, len, &data) == KErrNone)
		ap->set_chan(*data);

	ap->set_capability(scanInfo.Capability());

	if (ap->set_location(_gps, signal))
		_pos_changed = ETrue;
}

void
CBarbelo::notify_wifiscan_done()
{
	// notify UI
	_ui.refresh_aps(_aps);

	if (_pos_changed)
		notify_position();

	_pos_changed = EFalse;
}

CAP*
CBarbelo::find_ap(TWlanBssid& bssid)
{
#if FP1_OR_HIGHER
	TInt idx = _aps.Find(bssid, CAP::find_bssid);
#else
	TInt idx = KErrNotFound;
	for (TInt i = 0; i < _aps.Count(); ++i) {
		CAP* ap = _aps[i];
		if (CAP::find_bssid(&bssid, *ap)) {
			idx = i;
			break;
		}
	}
#endif
	if (idx == KErrNotFound)
		return NULL;

	return _aps[idx];
}

void
CBarbelo::notify_dmsg(const TDesC& msg)
{
	_ui.dmsg(msg);
}

void
CBarbelo::own()
{
	_wifi.start();
	_gps.start(_L("127.0.0.1"), 666);
}

void
CBarbelo::die()
{
	_wifi.stop();
	_gps.stop();
	log_stop();

	_ui.dmsg(_L("die"));
}

void
CBarbelo::notify_position()
{
	_ui.refresh_map(_aps, _gps);
}

void
CBarbelo::log_start()
{
	_logger.start();
}

void
CBarbelo::log_stop()
{
	_logger.stop();
}
