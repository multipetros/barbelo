#include "ap.h"
#include "logger.h"
#include "utils.h"

TBool
CAP::find_bssid(const TWlanBssid* bssid, const CAP& cap)
{
	return bssid->Compare(cap._bssid) == 0;
}

CAP::CAP(const TWlanBssid& bssid) : _bssid(bssid), _signal_max(666)
{
	_seen_first.HomeTime();
	_seen_last.HomeTime();

	_latitude = _longitude = 0;
}

void
CAP::set_ssid(const TUint8* data, TUint8 len)
{
	TBuf8<64> lame;

	lame.Append(data, len);
	_ssid.Copy(lame);
}

void
CAP::set_signal(TUint8 signal)
{
	_signal = signal;
	_seen_last.HomeTime();
}

TDes&
CAP::ssid()
{
	return _ssid;
}

void
CAP::set_chan(TUint8 chan)
{
	_chan = chan;
}

TUint8
CAP::chan()
{
	return _chan;
}

void
CAP::set_security(TWlanConnectionSecurityMode mode)
{
	_security = mode;
}

TWlanConnectionSecurityMode
CAP::security()
{
	return _security;
}

TUint8
CAP::signal()
{
	return _signal;
}

TWlanBssid&
CAP::bssid()
{
	return _bssid;
}

void
CAP::set_capability(TUint16 cap)
{
	_capability = cap;
}

TBool
CAP::adhoc()
{
	return !(_capability & 1);
}

TTime&
CAP::seen_first()
{
	return _seen_first;
}

TTime&
CAP::seen_last()
{
	return _seen_last;
}

TBool
CAP::set_location(CGPS& gps, TUint8 signal)
{
	if (!gps.valid())
		return EFalse;

	// units are negative
	if (signal > _signal_max)
		return EFalse;

	_signal_max = signal;
	_latitude  = gps.latitude();
	_longitude = gps.longitude();

	return ETrue;
}

TReal
CAP::latitude()
{
	return _latitude;
}

TReal
CAP::longitude()
{
	return _longitude;
}

void
CAP::write(HBufC8*& out, TInt num)
{
	// root
	TBuf8<32> fs, ls;

	CLogger::time2str(_seen_first, fs);
	CLogger::time2str(_seen_last, ls);

	TBuf8<32> type;
	if (adhoc())
		type.Append(_L8("ad-hoc"));
	else
		type.Append(_L8("infrastructure"));

	TBuf8<32> wep;
	if (security() != EWlanConnectionSecurityOpen)
		wep.Append(_L8("true"));
	else
		wep.Append(_L8("false"));

	TBuf8<32> cloaked;
	if (_ssid.Length() > 0)
		cloaked.Append(_L8("false"));
	else
		cloaked.Append(_L8("true"));

	TBuf8<256> root;
	root.Format(_L8("<wireless-network number=\"%d\""
			" type=\"%S\""
			" wep=\"%S\""
			" cloaked=\"%S\""
			" first-time=\"%S\""
			" last-time=\"%S\">\n")
		    , num, &type, &wep, &cloaked, &fs, &ls);
	Utils::append_data(out, root);

	// details
	type.Copy(_ssid);
	root.Format(_L8("<SSID>%S</SSID>\n"), &type);
	Utils::append_data(out, root);

	TBuf<32> tmp;
	Utils::mac2str(tmp, _bssid);
	type.Copy(tmp);
	root.Format(_L8("<BSSID>%S</BSSID>\n"), &type);
	Utils::append_data(out, root);

	root.Format(_L8("<channel>%d</channel>\n"), _chan);
	Utils::append_data(out, root);
	
	switch (_security) {
	case EWlanConnectionSecurityOpen:
		Utils::append_data(out, _L8("<encryption>None</encryption>\n"));
		break;

	case EWlanConnectionSecurityWpaPsk:
		Utils::append_data(out, _L8("<encryption>PSK</encryption>\n"));
		/* fall-through */

	case EWlanConnectionSecurityWpa:
		Utils::append_data(out, _L8("<encryption>WPA</encryption>\n"));
		/* fall-through */

	case EWlanConnectionSecurityWep:
		Utils::append_data(out, _L8("<encryption>WEP</encryption>\n"));
		break;

	case EWlanConnectionSecurity802d1x:
		Utils::append_data(out,
			_L8("<encryption>802d.1x</encryption>\n"));
		break;

	default:
		Utils::append_data(out,
			_L8("<encryption>Unknown</encryption>\n"));
		break;
	}

	// GPS
	if (_longitude != 0 && _latitude != 0) {
		Utils::append_data(out, _L8("<gps-info unit=\"english\">\n"));

		root.Format(_L8("<min-lat>%f</min-lat>\n"), _latitude);
		Utils::append_data(out, root);

		root.Format(_L8("<min-lon>%f</min-lon>\n"), _longitude);
		Utils::append_data(out, root);

		root.Format(_L8("<max-lat>%f</max-lat>\n"), _latitude);
		Utils::append_data(out, root);

		root.Format(_L8("<max-lon>%f</max-lon>\n"), _longitude);
		Utils::append_data(out, root);

		// other crap
		Utils::append_data(out, _L8(
			       "<min-alt>0.0</min-alt>\n"
			       "<min-spd>0.0</min-spd>\n"
			       "<max-alt>0.0</max-alt>\n"
			       "<max-spd>0.0</max-spd>\n"
				));

		Utils::append_data(out, _L8("</gps-info>\n"));
	}

	// other crap, so that parsers don't barf
	Utils::append_data(out, _L8(
	                "<maxrate>0.0</maxrate>\n"
	                "<maxseenrate>0</maxseenrate>\n"
			"<carrier>IEEE 802.11b</carrier>\n"
			"<packets>\n"
			"<LLC>0</LLC>\n"
			"<data>0</data>\n"
			"<crypt>0</crypt>\n"
			"<weak>0</weak>\n"
			"<total>0</total>\n"
			"</packets>\n"
			"<datasize>0</datasize>\n"
			));

	// barbelo stuff
	Utils::append_data(out, _L8("<barbelo>\n"));
	root.Format(_L8("<signal>%d</signal>\n"), _signal_max);
	Utils::append_data(out, root);
	Utils::append_data(out, _L8("</barbelo>\n"));

	// end root element
	Utils::append_data(out, _L8("</wireless-network>\n"));
}
