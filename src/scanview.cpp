#include <aknviewappui.h>

#include "scanview.h"
#include "barbre.hrh"
#include "barbelo.rsg"

#define SIG_SCALE 25

CScanCont::CScanCont(MBarbUI& barb) : _list(NULL), _data(NULL), _barb(barb),
		_aps(NULL)
{
}

CScanCont::~CScanCont()
{
	delete _list;
	delete _data;
}

void
CScanCont::ConstructL(const TRect& rect)
{
	CreateWindowL();

	_data = new (ELeave) CDesCArrayFlat(10);

	_list = new CAknDoubleStyleListBox;
	_list->ConstructL(NULL, EAknListBoxSelectionList);
	_list->Model()->SetItemTextArray(_data);
	_list->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);

	CEikScrollBarFrame* sb = _list->CreateScrollBarFrameL();
        sb->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
                                    CEikScrollBarFrame::EAuto);

	_list->SetListBoxObserver(this);
	_list->HandleItemAdditionL();

	SizeChanged();
	SetRect(rect);
	ActivateL();
}

void
CScanCont::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
{
	if (!_aps)
		return;

	CAP* ap = (*_aps)[aListBox->CurrentItemIndex()];
	_barb.ap_details(*ap);
}

TKeyResponse
CScanCont::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	_list->OfferKeyEventL(aKeyEvent, aType);

	return EKeyWasNotConsumed;
}

void
CScanCont::SizeChanged()
{
	TPoint coordListBox;

	AknLayoutUtils::LayoutMetricsPosition(AknLayoutUtils::EMainPane,
					      coordListBox);
	_list->SetExtent(coordListBox, _list->MinimumSize());
}

TInt
CScanCont::CountComponentControls() const
{
	return 1;
}

CCoeControl*
CScanCont::ComponentControl(TInt aIndex) const
{
	switch (aIndex) {
	case 0:
		return _list;

	default:
		return NULL;
	}
}

void
CScanCont::Draw(const TRect& aRect) const
{
        CWindowGc& gc = SystemGc();
        gc.SetPenStyle(CGraphicsContext::ENullPen);
        gc.SetBrushColor(KRgbWhite);
        gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
        gc.DrawRect(aRect);
}

void
CScanCont::refresh_aps(APS& aps)
{
	_aps = &aps; // XXX

	_data->Reset();	
	for (TInt i = 0; i < aps.Count(); i++) {
		CAP* ap = aps[i];

		do_ap(*ap);

	}

	refresh();
}

void
CScanCont::do_ap(CAP& ap)
{
	_LIT(fmt, "\t%S [%d %S]\t%S\t\t");
	_LIT(wep, "WEP");
	_LIT(wpa, "WPA");
	_LIT(open, "");
	TBuf<SIG_SCALE> sig;

	// encryption
	const TDesC* sec = &open;

	switch (ap.security()) {
	case EWlanConnectionSecurityOpen:
		break;

	case EWlanConnectionSecurityWep:
		sec = &wep;
		break;

	case EWlanConnectionSecurity802d1x: /* XXX WTF? */
	case EWlanConnectionSecurityWpa:
	case EWlanConnectionSecurityWpaPsk:
		sec = &wpa;
		break;
	}
	
	// signal
	TUint8 s = dbm_to_percent(ap.signal(), SIG_SCALE);
	for (int i = 0; i < s; i++)
		sig.Append(_L("X"));

	// add data
	TBuf<100> line;
	line.Format(fmt, &ap.ssid(), ap.chan(), sec, &sig);

	_data->AppendL(line);
}

TUint8
CScanCont::dbm_to_percent(TUint8 dbm, TUint8 scale)
{
	TUint8 min = 50;
	TUint8 max = 90;
	TUint8 level;

	if (dbm > max)
		dbm = max;
	if (dbm < min)
		dbm = min;
	dbm -= min;
	max -= min;

	level = scale - ((dbm*scale)/max);

	return level;
}

void
CScanCont::refresh()
{
	TInt idx = _list->CurrentItemIndex();

	_list->HandleItemAdditionL();
	if (idx != -1)
		_list->SetCurrentItemIndexAndDraw(idx);
	SizeChanged();
	DrawNow();
}

CScanView*
CScanView::NewLC(MBarbUI& barb)
{
	CScanView* self = new CScanView(barb);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
}

CScanView::CScanView(MBarbUI& barb) : _cont(NULL), _barb(barb)
{
}

CScanView::~CScanView()
{
	delete _cont;
}

void
CScanView::ConstructL()
{
	BaseConstructL(R_BARBSCAN_VIEW);

	_cont = new CScanCont(_barb);
	_cont->SetMopParent(this);
	_cont->ConstructL(ClientRect());
	_cont->MakeVisible(EFalse);
}

TUid
CScanView::Id() const
{
	return TUid::Uid(EViewScan);	
}

void
CScanView::DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
		       const TDesC8& aCustomMessage)
{
	AppUi()->AddToStackL(*this, _cont);
	_cont->MakeVisible(ETrue);
}

void
CScanView::DoDeactivate()
{
	_cont->MakeVisible(EFalse);
	AppUi()->RemoveFromStack(_cont);
}

void
CScanView::HandleCommandL(TInt cmd)
{       
        AppUi()->HandleCommandL(cmd);
}

void
CScanView::refresh_aps(APS& ap)
{
	_cont->refresh_aps(ap);
}
