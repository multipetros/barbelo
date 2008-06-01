#ifndef __GPS_H__
#define __GPS_H__

#include <e32std.h>
#include <in_sock.h>

#include "barbevents.h"

class CGPS : public CActive {
public:
	CGPS(CBarbEvents& barb);
	~CGPS();

	void start(const TDesC& ip, TInt port);
	void stop();

	TReal latitude();
	TReal longitude();
	TBool valid();

protected:
	void RunL();
	TInt RunError(TInt aError);
	void DoCancel();

private:
	void handle_input();
	TInt parse_nmea(TPtrC8 sentence);
	void handle_position(TReal latitude, TReal longitude);
	void parse_gpgga(TPtrC8 data);
	void parse_pbrb(TPtrC8 data);
	TReal dm2dec(TReal x);

	enum GS_STATE {
		GS_IDLE = 1,
		GS_CONNECTING,
		GS_RECEIVING
	};

	RSocketServ	_ss;	
	RSocket		_sock;
	TReal		_latitude;
	TReal		_longitude;
	CBarbEvents&	_barb;
	GS_STATE	_state;
	TBuf8<64>	_buf;
	TSockXfrLength	_len;
	TBuf8<128>	_input;
	TBool		_running;
};

#endif // __GPS_H__
