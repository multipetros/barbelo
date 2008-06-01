#ifndef __UTILS_H__
#define __UTILS_H__

#include <e32std.h>

class Utils {
public:
	static void mac2str(TDes& out, TDes8& in);
	static void append_data(HBufC8*& buf, const TDesC8& in);
};

#endif // __UTILS_H__
