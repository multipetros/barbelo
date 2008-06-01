#include "utils.h"

void
Utils::mac2str(TDes& out, TDes8& in)
{
	TBuf<2> tmp;

        _LIT(fmt_mac, "%.2X");

	out.Zero();
	for (TInt i = 0; i < 6; i++) {
		tmp.Format(fmt_mac, in[i]);

		if (tmp.Length() == 1)
			out.Append('0');

		out.Append(tmp);

		if (i < 5)
			out.Append(':');
	}
}

// I'm sorry, but but Symbian got their philosophy wrong.  Pushing such code to
// users does not improve performance, security or robustness - it does the
// opposite.  -sorbo.
void
Utils::append_data(HBufC8*& buf, const TDesC8& in)
{
	TInt remaining = buf->Des().MaxLength() - buf->Des().Length();

	if (remaining <= in.Length()) {

		TInt size = buf->Des().MaxLength();

		size += in.Length() - remaining;
		size += 1024;

		buf = buf->ReAlloc(size);
	}

	buf->Des().Append(in);
}
