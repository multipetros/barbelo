#ifndef __SCANVIEW_H__
#define __SCANVIEW_H__

#include <aknview.h>
#include <aknlists.h>

#include "ap.h"
#include "barbui.h"

class CScanCont : public CCoeControl, public MEikListBoxObserver {
public:
	CScanCont(MBarbUI& barb);
	~CScanCont();

        void ConstructL(const TRect& rect);
	void Draw(const TRect& aRect) const;
	void refresh_aps(APS& ap);

protected:
	void HandleListBoxEventL(CEikListBox*, TListBoxEvent);
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
				    TEventCode aType);
	void SizeChanged();
	TInt CountComponentControls() const;
	CCoeControl* ComponentControl(TInt aIndex) const;

private:
	void refresh();
	void do_ap(CAP& ap);
	TUint8 dbm_to_percent(TUint8 dbm, TUint8 scale);

	CAknDoubleStyleListBox*	_list;
	CDesCArrayFlat*		_data;
	MBarbUI&		_barb;
	APS*			_aps;
};

class CScanView : public CAknView {
public:
	static CScanView* NewLC(MBarbUI& barb);
	~CScanView();

	TUid Id() const;
	void refresh_aps(APS& ap);

protected:
        void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
			 const TDesC8& aCustomMessage);
	void DoDeactivate();
	void HandleCommandL(TInt cmd);

private:
	CScanView(MBarbUI& barb);

	void ConstructL();

	CScanCont*	_cont;
	MBarbUI&	_barb;
};

#endif // __SCANVIEW_H__
