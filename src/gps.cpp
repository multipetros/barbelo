#include "gps.h"
#include "debugview.h"

CGPS::CGPS(CBarbEvents& barb) : CActive(EPriorityStandard), _latitude(0.0F),
		_longitude(0.0F), _barb(barb), _state(GS_IDLE), _running(ETrue)
{
	CActiveScheduler::Add(this);
	_ss.Connect();
}

CGPS::~CGPS()
{
	Cancel();
	_ss.Close();
}


void
CGPS::start(const TDesC& ip, TInt port)
{
	_sock.Open(_ss, KAfInet, KSockStream, KProtocolInetTcp);

	TInetAddr addr;
	addr.SetFamily(KAfInet);
	addr.SetPort(port);
	addr.Input(ip);

	_state = GS_CONNECTING;
	_sock.Connect(addr, iStatus);
	SetActive();
}

void
CGPS::stop()
{
	Cancel();
	_sock.Close();
}

TReal
CGPS::latitude()
{
	return _latitude;
}

TReal
CGPS::longitude()
{
	return _longitude;
}

void
CGPS::RunL()
{
	if (!_running)
		return;

	if (iStatus != KErrNone) {
		_state = GS_IDLE;
		_barb.notify_dmsg(_L("no GPS connection"));
		// XXX resume

		return;
	}

	switch (_state) {
	case GS_IDLE:
		ASSERT(0);
		break;

	case GS_CONNECTING:
		_barb.notify_dmsg(_L("GPS connected"));
		_state = GS_RECEIVING;
		break;

	case GS_RECEIVING:
		handle_input();
		break;
	}

	ASSERT(_state == GS_RECEIVING);
	_sock.RecvOneOrMore(_buf, 0, iStatus, _len);
	SetActive();
}

void
CGPS::handle_input()
{
	_input.Append(_buf);

	TLex8 lex(_input);
	lex.Mark();

	TInt consumed = 0;
	while (!lex.Eos()) {
		if (lex.Peek() == '\n') {
			TPtrC8 line = lex.MarkedToken();

			TInt err = parse_nmea(line);
			if (err) {
				TBuf<256> sentence;
				TBuf<300> error;

				sentence.Copy(line);
				error.Format(_L("Invalid NMEA: %d [%S]"),
					     err, &sentence);
				CDebugCont::static_dmsg(error);
			}

			consumed = lex.Offset();

			lex.Inc();
			lex.Mark();
		} else
			lex.Inc();
	}

	if (consumed)
		_input.Delete(0, consumed+1);
}

TInt
CGPS::parse_nmea(TPtrC8 sentence)
{
	TUint8 checksum = 0;
	TInt i;
	TInt cmdi = -1;

	// check start of sentence
	if (sentence[0] != '$')
		return -1;

	// check checksum
	for (i = 1; i < sentence.Length(); i++) {
		if (sentence[i] == '*')
			break;

		if (cmdi == -1 && sentence[i] == ',')
			cmdi = i;

		checksum ^= sentence[i];
	}

	i++;
	if (i >= sentence.Length())
		return -2;

	TUint8 check;
	TLex8 tmp(&sentence[i]);
	tmp.Val(check, EHex);

	if (checksum != check)
		return -3;

	// parse specific sentence
	TBuf8<32> cmd;
	cmd.Copy(&sentence[1], cmdi-1);

	TPtrC8 data(&sentence[cmdi+1], i-cmdi-2);

	if (cmd.Compare(_L8("GPGGA")) == 0)
		parse_gpgga(data);

	else if (cmd.Compare(_L8("PBRB")) == 0)
		parse_pbrb(data);

	return 0;
}

void
CGPS::parse_gpgga(TPtrC8 data)
{
	TReal longitude = 0, latitude = 0;
	TUint lat_dir = 0, lon_dir = 0;

	TLex8 lex(data);
	lex.Mark();

	TInt state = 0;
	while (!lex.Eos()) {
		if (lex.Peek() == ',') {
			TPtrC8 tok = lex.MarkedToken();
			TLex8 tmp(tok);

			switch (state) {
			case 1:
				if (tmp.Val(latitude) != KErrNone)
					ASSERT(0);
				break;

			case 2:
				lat_dir = tok[0];
				break;

			case 3:
				if (tmp.Val(longitude) != KErrNone)
					ASSERT(0);
				break;

			case 4:
				lon_dir = tok[0];
				break;
			}
			state++;

			lex.Inc();
			lex.Mark();
		} else
			lex.Inc();
	}

	// convert to decimal
	latitude  = dm2dec(latitude);
	longitude = dm2dec(longitude);

	// fix sign
	if (lat_dir == 'S')
		latitude *= -1.0;

	if (lon_dir == 'W')
		longitude *= -1.0;

	// tell the world about it
	handle_position(latitude, longitude);
}

TReal
CGPS::dm2dec(TReal x)
{
	TReal deg = (TInt) (x/100.0);
	TReal min = x - deg*100.0;

	return deg + min/60.0;
}

void
CGPS::parse_pbrb(TPtrC8 data)
{
	handle_position(0, 0);
}

void
CGPS::handle_position(TReal latitude, TReal longitude)
{
	_latitude  = latitude;
	_longitude = longitude;

	_barb.notify_position();
}

TInt
CGPS::RunError(TInt aError)
{
	_barb.notify_dmsg(_L("err gps"));

	return KErrNone;
}

void
CGPS::DoCancel()
{
	_running = ETrue;

	if (!IsActive())
		return;

	switch (_state) {
	case GS_IDLE:
		ASSERT(0);
		break;

	case GS_CONNECTING:
		_sock.CancelConnect();
		break;

	case GS_RECEIVING:
		_sock.CancelRecv();
		break;
	}
}

TBool
CGPS::valid()
{
	return (_latitude != 0) || (_longitude != 0);
}
