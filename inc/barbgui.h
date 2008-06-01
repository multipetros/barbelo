#ifndef __BARBGUI_H__
#define __BARBGUI_H__

#include <aknapp.h>
#include <akndoc.h>
#include <aknappui.h>
#include <coecntrl.h>
#include <eikedwin.h>
#include <akntabgrp.h>
#include <aknnavide.h>
#include <akntabobserver.h>
#include <aknviewappui.h>

#include "barbui.h"
#include "barbelo.h"
#include "debugview.h"
#include "scanview.h"
#include "mapview.h"
#include "details.h"

class CBarbeloApp : public CAknApplication {
public:
	TUid AppDllUid() const;

protected:
	CApaDocument* CreateDocumentL();
};

class CBarbeloDoc : public CAknDocument {
public:
	static CBarbeloDoc* NewL(CEikApplication& aApp);
	static CBarbeloDoc* NewLC(CEikApplication& aApp);

	CEikAppUi* CreateAppUiL();

private:
	CBarbeloDoc(CEikApplication& aApp);

	void ConstructL();
};

class CBarbeloUi : public CAknViewAppUi, public MAknTabObserver,
		   public MBarbUI {
public:
	CBarbeloUi();
	~CBarbeloUi();

	void ConstructL();
	void TabChangedL(TInt index);
	TKeyResponse HandleKeyEventL(const TKeyEvent& aKeyEvent,
				     TEventCode aType);

	// barbui
	void dmsg(const TDesC& msg);
	void refresh_aps(APS& ap);
	void ap_details(CAP& ap);
	void refresh_map(APS& ap, CGPS& gps);

private:
	void HandleCommandL(TInt cmd);
	void HandleStatusPaneSizeChange();
	void die();

	CAknTabGroup*	_tab;
	CBarbelo*	_barbelo;
	CDebugView*	_dv;
	CScanView*	_sv;
	CDetails*	_det;
	CMapView*	_mv;
};

#endif // __BARBGUI_H__
