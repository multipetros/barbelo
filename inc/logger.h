#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <e32std.h>
#include <e32base.h>

#include "ap.h"

class CLogger : public CActive {
public:
	CLogger(APS& ap);
	~CLogger();

	void start();
	void stop();
	static void time2str(TTime& time, TDes8& out);

protected:
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();

private:
	enum State {
		STATE_TIMER = 0,
		STATE_WRITING,
		STATE_FLUSHING
	};

	void flush_log();
	void open_file();
	TInt open_file(TDes& name);
	void wait_write();

	TBool		_logging;
	RTimer		_timer;
	APS&		_aps;
	TBuf<32>	_home;
	RFs		_rfs;
	RFile		_file;
	TTime		_start;
	HBufC8*		_data;
	State		_state;
	TPtr8		_des;
};

#endif // __LOGGER_H__
