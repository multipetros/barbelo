#ifndef __MAPVIEW_H__
#define __MAPVIEW_H__

#include <aknview.h>

#include "ap.h"
#include "gps.h"
#include "barbui.h"

class CMapCont : public CCoeControl {
public:
	CMapCont(MBarbUI& barb);
	~CMapCont();

	void ConstructL(const TRect& rect);
	void SizeChanged();
	void Draw(const TRect& rect) const;
	void refresh_map(APS& ap, CGPS& gps);

private:
	static const TInt _status_sz;

	void draw_square(TReal x, TReal y) const;
	void draw_ap(CAP& ap, TBool selected) const;
        TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
				    TEventCode aType);

	APS*		_ap;
	CGPS*		_gps;
	TReal		_x;
	TReal		_y;
	TBool		_valid;
	TInt		_size;
	TReal		_scale;
	TInt		_selected;
	const CFont*	_font;
	MBarbUI&	_barb;
};

class CMapView : public CAknView {
public:
	static CMapView* NewLC(MBarbUI& barb);
	~CMapView();
	TUid Id() const;
	void refresh_map(APS& ap, CGPS& gps);

protected:
	void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
			 const TDesC8& aCustomMessage);
	void DoDeactivate();
	void HandleCommandL(TInt cmd);

private:
	CMapView(MBarbUI& barb);
	void ConstructL();

	CMapCont*	_cont;
	MBarbUI&	_barb;
};

#endif // __MAPVIEW_H__
