#ifndef __DEBUGVIEW_H__
#define __DEBUGVIEW_H__

#include <aknview.h>
#include <eikedwin.h>

class CDebugCont : public CCoeControl {
public:
	CDebugCont();
	~CDebugCont();

	void ConstructL(const TRect& rect);
	void dmsg(const TDesC& msg);
	static void static_dmsg(const TDesC& msg);
	void SizeChanged();

private:
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,
				    TEventCode aType);
	CCoeControl* ComponentControl(TInt aIndex) const;
	TInt CountComponentControls() const;

	CEikEdwin*      	_dbg;
	static CDebugCont*	_hack;
};

class CDebugView : public CAknView {
public:
	static CDebugView* NewLC();
	~CDebugView();

        TUid Id() const;
	void dmsg(const TDesC& msg);

protected:
        void DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,                                                const TDesC8& aCustomMessage);
        void DoDeactivate();
	void HandleCommandL(TInt cmd);

private:
        CDebugView();

        void ConstructL();

	CDebugCont*	_cont;
};

#endif // __DEBUGVIEW_H__
