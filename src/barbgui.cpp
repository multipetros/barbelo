#include <eikstart.h>
#include <barsread.h>

#include "barbelo.rsg"
#include "barbgui.h"
#include "scanview.h"
#include "debugview.h"
#include "barbre.hrh"

namespace {
const TUid KUidBarbeloApp = { 0xA0000666 };
};

void
CBarbeloUi::ConstructL()
{
	BaseConstructL(EAknEnableSkin);

	CAknNavigationControlContainer* c = (CAknNavigationControlContainer*)
		StatusPane()->ControlL(TUid::Uid(EEikStatusPaneUidNavi));
	CAknNavigationDecorator* navi = c->ResourceDecorator();
	_tab = static_cast<CAknTabGroup*>(navi->DecoratedControl());
	_tab->SetObserver(this);

	_sv = CScanView::NewLC(*this);
	AddViewL(_sv);

	_mv = CMapView::NewLC(*this);
	AddViewL(_mv);

	_dv = CDebugView::NewLC();
	AddViewL(_dv);

	_sv->ActivateViewL(_sv->ViewId());

	_barbelo = new CBarbelo(*this);
	_barbelo->own();
}

CBarbeloUi::CBarbeloUi() : _barbelo(NULL), _dv(NULL), _sv(NULL), _det(NULL),
			   _mv(NULL)
{
}

CBarbeloUi::~CBarbeloUi()
{
	delete _barbelo;
}

void
CBarbeloUi::HandleCommandL(TInt cmd)
{
	switch (cmd) {
	case EMenuLogStart:
		_barbelo->log_start();
		break;

	case EMenuLogStop:
		_barbelo->log_stop();
		break;

	case EEikCmdExit:
	case EAknSoftkeyExit:
		die();
		break;
	}
}

TKeyResponse
CBarbeloUi::HandleKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
{
	if (aKeyEvent.iCode == EKeyLeftArrow 
	    || aKeyEvent.iCode == EKeyRightArrow)
		return _tab->OfferKeyEventL(aKeyEvent, aType);

	return EKeyWasNotConsumed;
}

void
CBarbeloUi::TabChangedL(TInt index)
{
	switch (index) {
	case 0:
		ActivateLocalViewL(TUid::Uid(EViewScan));
		break;

	case 1:
		ActivateLocalViewL(TUid::Uid(EViewMap));
		break;

	case 2:
		ActivateLocalViewL(TUid::Uid(EViewDebug));
		break;
	}
}

void
CBarbeloUi::ap_details(CAP& ap)
{
	ASSERT(!_det);

	_det = new CDetails(ap);
	_det->ExecuteLD(R_BARBELO_AP_DETAILS);
	_det = NULL; // XXX delete?
}

void
CBarbeloUi::die()
{
	_barbelo->die();
	Exit();
}

void
CBarbeloUi::HandleStatusPaneSizeChange()
{
}

void
CBarbeloUi::dmsg(const TDesC& msg)
{
	_dv->dmsg(msg);
}

void
CBarbeloUi::refresh_aps(APS& aps)
{
	_sv->refresh_aps(aps);

	if (_det)
		_det->refresh();
}

void
CBarbeloUi::refresh_map(APS& aps, CGPS& gps)
{
	_mv->refresh_map(aps, gps);
}

CBarbeloDoc*
CBarbeloDoc::NewL(CEikApplication& aApp)
{
	CBarbeloDoc* self = NewLC(aApp);
	CleanupStack::Pop(self);

	return self;
}

CBarbeloDoc*
CBarbeloDoc::NewLC(CEikApplication& aApp)
{
	CBarbeloDoc* self = new CBarbeloDoc(aApp);
	CleanupStack::PushL(self);

	self->ConstructL();

	return self;
}

void
CBarbeloDoc::ConstructL()
{
}

CBarbeloDoc::CBarbeloDoc(CEikApplication& aApp) : CAknDocument(aApp)
{
}

CEikAppUi*
CBarbeloDoc::CreateAppUiL()
{
	return new CBarbeloUi;
}

CApaDocument*
CBarbeloApp::CreateDocumentL()
{
	return CBarbeloDoc::NewL(*this);
}

TUid
CBarbeloApp::AppDllUid() const
{
	return KUidBarbeloApp;
}


LOCAL_C CApaApplication* NewApplication()
{
	return new CBarbeloApp;
}

GLDEF_C TInt E32Main()
{
	return EikStart::RunApplication(NewApplication);
}
