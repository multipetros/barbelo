#ifndef __AP_H__
#define __AP_H__

#include <e32std.h>
#include <wlanscaninfo.h>

#include "gps.h"

class CAP {
public:
	typedef TBuf<64>	SSID;

	CAP(const TWlanBssid& bssid);
	static TBool find_bssid(const TWlanBssid* bssid, const CAP& cap);

	void write(HBufC8*& out, TInt num);
	void set_ssid(const TUint8*, TUint8 len);
	void set_signal(TUint8 signal);
	void set_security(TWlanConnectionSecurityMode mode);
	void set_chan(TUint8 chan);
	void set_capability(TUint16 capability);
	TBool set_location(CGPS& gps, TUint8 signal);

	TDes& ssid();
	TWlanConnectionSecurityMode security();
	TUint8 chan();
	TUint8 signal();
	TWlanBssid& bssid();
	TBool adhoc();
	TTime& seen_first();
	TTime& seen_last();
	TReal latitude();
	TReal longitude();

private:
	TWlanBssid			_bssid;
	TUint8				_signal;
	TUint8				_signal_max;
	SSID				_ssid;
	TWlanConnectionSecurityMode	_security;
	TUint8				_chan;
	TUint16				_capability;
	TTime				_seen_first;
	TTime				_seen_last;
	TReal				_latitude;
	TReal				_longitude;
};

typedef RPointerArray<CAP> APS;

#endif // __AP_H__
