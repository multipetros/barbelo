#ifndef __DETAILS_H__
#define __DETAILS_H__

#include <eikdialg.h>
#include <aknform.h>

#include "ap.h"

class CDetails : public CAknForm {
public:
	CDetails(CAP& ap);

	void refresh();

protected:
	void PreLayoutDynInitL();
	void PostLayoutDynInitL();

private:
	CAP&	_ap;
};

#endif // __DETAILS_H__
