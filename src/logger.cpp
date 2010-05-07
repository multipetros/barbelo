#include "logger.h"
#include "debugview.h"
#include "version.h"
#include "utils.h"

CLogger::CLogger(APS& aps) : CActive(EPriorityStandard), _logging(EFalse),
				_aps(aps), _data(NULL), _state(STATE_TIMER),
				_des(NULL, 0)
{
	CActiveScheduler::Add(this);
	_timer.CreateLocal();

	_home.Copy(_L("c:\\DATA\\warwalk"));

	if (_rfs.Connect() != KErrNone)
		ASSERT(0);

	_data = HBufC8::NewL(1024);
}

CLogger::~CLogger()
{
	stop();
	Cancel();
	_rfs.Close();

	delete _data;
}

void
CLogger::start()
{
	if (_logging)
		return;

	_logging = ETrue;
	_start.HomeTime();

	// open file
	open_file();
	TBuf<190> msg;
	TBuf<128> fname;

	_file.FullName(fname);
	msg.Format(_L("Log started %S"), &fname);
	CDebugCont::static_dmsg(fname);
	
	// go for it
	_state = STATE_TIMER;
	flush_log();
}

void
CLogger::open_file()
{
	TBuf<32> tm;
	TTime time;

	time.HomeTime();
	time.FormatL(tm, _L("%F%*N-%D-%Y"));

	TBuf<128> fullname;

	for (TInt i = 1; i < 666; i++) {
		fullname.Format(_L("%S\\Barbelo-%S-%d.xml"), &_home, &tm, i);

		if (open_file(fullname) == 0)
			break;
	}
}

TInt
CLogger::open_file(TDes& name)
{
	TInt err = _file.Create(_rfs, name, EFileWrite | EFileStream);

	if (err == KErrNone)
		return 0;

	if (err == KErrPathNotFound) {
		if (_rfs.MkDirAll(name) != KErrNone) {
			TBuf<128> e;

			e.Format(_L("Can't create dir: %S"), &name);
			CDebugCont::static_dmsg(e);

			return 0;
		}

		return open_file(name);
	}

	if (err == KErrAlreadyExists)
		return -1;

	TBuf<32> e;
	e.Format(_L("open file err: %d"), err);
	CDebugCont::static_dmsg(e);

	return 0;
}

void
CLogger::stop()
{
	if (!_logging)
		return;

	_logging = EFalse;

	if (_state == STATE_TIMER)
		Cancel();
	else
		wait_write();

	flush_log();
	wait_write();

	_file.Close();

	CDebugCont::static_dmsg(_L("Log stopped"));
}

void
CLogger::RunL()
{
	User::LeaveIfError(iStatus.Int());

	switch (_state) {
	case STATE_WRITING:
#if FP1_OR_HIGHER
		_state = STATE_FLUSHING;
		_file.Flush(iStatus);
		SetActive();
		break;

	case STATE_FLUSHING:
#else
		_file.Flush();
#endif
		_state = STATE_TIMER;
		if (_logging) {
			_timer.After(iStatus, 5 * 1000000);
			SetActive();
		}
		break;

	case STATE_TIMER:
		flush_log();
		break;

	default:
		ASSERT(0);
		break;
	}
}

TInt
CLogger::RunError(TInt aError)
{
	TBuf<32> err;

	err.Format(_L("Logger err: %d"), aError);
	CDebugCont::static_dmsg(err);

	return KErrNone;
}

void
CLogger::DoCancel()
{
	if (!IsActive())
		return;

	switch (_state) {
	case STATE_TIMER:
		_timer.Cancel();
		break;

	default:
		ASSERT(0);
		break;

	}
}

void
CLogger::time2str(TTime& time, TDes8& out)
{
	TBuf<32> t;

	time.FormatL(t, _L("%F%*E %*N %D %H:%T:%S %Y"));

	out.Copy(t);
}

void
CLogger::wait_write()
{
	while (IsActive() && _state != STATE_TIMER)
		User::WaitForRequest(iStatus);
}

void
CLogger::flush_log()
{
	ASSERT(_state = STATE_TIMER);
	// truncate file
	_file.SetSize(0);
	_data->Des().Zero();

	// xml header
	Utils::append_data(_data,
	    	   _L8("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n"));

	// root element
	TBuf8<12> ver;
	ver.Format(_L8("%.1f"), BARBELO_VERSION);

	TBuf8<32> st;
	time2str(_start, st);

	TTime now;
	TBuf8<32> et;
	now.HomeTime();
	time2str(now, et);

	TBuf8<256> root;
	root.Format(_L8("<detection-run kismet-version=\"barbelo-%S\""
			" start-time=\"%S\""
			" end-time=\"%S\">\n")
			, &ver, &st, &et);
	Utils::append_data(_data, root);

	// dump aps
	for (TInt i = 0; i < _aps.Count(); i++) {
		CAP* ap = _aps[i];

		ap->write(_data, i+1);
	}

	// root element
	Utils::append_data(_data, _L8("</detection-run>\n"));

	if (_logging) {
		_state = STATE_WRITING;

		_des.Set(_data->Des()); // Must stay in scope 
		_file.Write(_des, iStatus);
		SetActive();
	} else {
		if (_file.Write(_data->Des()) != KErrNone)
			CDebugCont::static_dmsg(_L("Can't write"));

		if (_file.Flush() != KErrNone)
			CDebugCont::static_dmsg(_L("Can't flush"));
	}
}
