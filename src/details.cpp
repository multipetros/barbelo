#include <eikedwin.h>

#include "details.h"
#include "utils.h"
#include "barbre.hrh"

CDetails::CDetails(CAP& ap) : _ap(ap)
{
}

void
CDetails::PreLayoutDynInitL()
{
}

void
CDetails::PostLayoutDynInitL()
{
	refresh();
}

void
CDetails::refresh()
{
	CEikEdwin* ed;

	// SSID
	ed = static_cast<CEikEdwin*>(ControlOrNull(EFormSSID));
	ASSERT(ed);
	ed->SetTextL(&_ap.ssid());

	// Signal
	ed = static_cast<CEikEdwin*>(ControlOrNull(EFormSignal));
	ASSERT(ed);
	TBuf<64> line;
	_LIT(fmt_sig, "-%.2d dBm");
	line.Format(fmt_sig, _ap.signal());
	ed->SetTextL(&line);

	// Encryption
	ed = static_cast<CEikEdwin*>(ControlOrNull(EFormEncryption));
	ASSERT(ed);
	_LIT(wep, "WEP");
	_LIT(wpa, "WPA");
	_LIT(wpa_psk, "WPA-PSK");
	_LIT(open, "Open");
	_LIT(x, "802.1x");
	const TDesC* sec = NULL;

	switch (_ap.security()) {
	case EWlanConnectionSecurityOpen:
		sec = &open;
		break;
	
	case EWlanConnectionSecurityWep:
		sec = &wep;
		break;

	case EWlanConnectionSecurity802d1x:
		sec = &x;
		break;

	case EWlanConnectionSecurityWpa:
		sec = &wpa;
		break;

	case EWlanConnectionSecurityWpaPsk:
		sec = &wpa_psk;
		break;
	}
	ed->SetTextL(sec);

	// Channel
	ed = static_cast<CEikEdwin*>(ControlOrNull(EFormChannel));
	ASSERT(ed);
	_LIT(fmt_chan, "%.2d");
	line.Format(fmt_chan, _ap.chan());
	ed->SetTextL(&line);

	// Mode
	ed = static_cast<CEikEdwin*>(ControlOrNull(EFormMode));
	ASSERT(ed);
	_LIT(infra, "Infrastructure");
	_LIT(adhoc, "Ad-Hoc");
	const TDesC* mode = &infra;

	if (_ap.adhoc())
		mode = &adhoc;
	ed->SetTextL(mode);

	// BSSID
	ed = static_cast<CEikEdwin*>(ControlOrNull(EFormBSSID));
	ASSERT(ed);
	Utils::mac2str(line, _ap.bssid());
	ed->SetTextL(&line);

	// Latitude
	ed = static_cast<CEikEdwin*>(ControlOrNull(EFormLatitude));
	ASSERT(ed);
	_LIT(fmt_lat, "%f");
	line.Format(fmt_lat, _ap.latitude());
	ed->SetTextL(&line);
	
	// Longitude
	ed = static_cast<CEikEdwin*>(ControlOrNull(EFormLongitude));
	ASSERT(ed);
	line.Format(fmt_lat, _ap.longitude());
	ed->SetTextL(&line);

	// First seen
	ed = static_cast<CEikEdwin*>(ControlOrNull(EFormFirstSeen));
	ASSERT(ed);
	_LIT(fmt_time, "%H:%T.%S");
	_ap.seen_first().FormatL(line, fmt_time);
	ed->SetTextL(&line);

	// Last seen
	ed = static_cast<CEikEdwin*>(ControlOrNull(EFormLastSeen));
	ASSERT(ed);
	_ap.seen_last().FormatL(line, fmt_time);
	ed->SetTextL(&line);

	// refresh screen
	// XXX flickers on N95 =(
	DrawNow();
}
