#include <aknviewappui.h>
#include <aknutils.h>

#include "mapview.h"
#include "barbre.hrh"
#include "barbelo.rsg"
#include "debugview.h"

const TInt CMapCont::_status_sz = 50;

void
CMapCont::ConstructL(const TRect& rect)
{
	CreateWindowL();

	_font = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont);

	SizeChanged();
	SetRect(rect);
	ActivateL();
}

CMapCont::CMapCont(MBarbUI& barb) : _ap(NULL), _gps(NULL), _x(0), _y(0),
			_valid(EFalse), _size(8), _scale(100000),
			_selected(0), _font(NULL), _barb(barb)
{
}

CMapCont::~CMapCont()
{
}

void
CMapCont::SizeChanged()
{
}

void
CMapCont::Draw(const TRect& recto) const
{
	// clear background
	CWindowGc& gc = SystemGc();
	TRect rect(Rect());

	gc.SetPenStyle(CGraphicsContext::ENullPen);
	gc.SetBrushColor(KRgbWhite);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
	gc.Clear(rect);

	if (!_ap || !_gps)
		return;

	// set origin & draw me
	gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetBrushStyle(CGraphicsContext::ESolidBrush);

	TRgb color = KRgbBlue;
	if (!_valid)
		color = KRgbRed;

	gc.SetPenColor(color);
	gc.SetBrushColor(color);
	draw_square(_x, _y);

	// draw APs
	for (TInt i = 0; i < _ap->Count(); i++) {
		CAP* ap = (*_ap)[i];

		draw_ap(*ap, i == _selected);
	}
}

void
CMapCont::draw_ap(CAP& ap, TBool selected) const
{
	CWindowGc& gc = SystemGc();
	TReal lat = ap.latitude();
	TReal lon = ap.longitude();

	if ((lat == 0) && (lon == 0))
		return;

	TRgb color;	
	if (selected) {
		color = KRgbGreen;

		gc.UseFont(_font);
		gc.SetBrushColor(KRgbWhite);
		gc.SetPenColor(KRgbBlack);

		TRect canvas = Rect();
		TRect rect(10, canvas.Height() - _status_sz,
			   canvas.Width(), canvas.Height());
		gc.DrawText(ap.ssid(), rect, rect.Height()/3);
		gc.DiscardFont();
	} else
		color = KRgbBlack;

	gc.SetPenColor(color);
	gc.SetBrushColor(color);
	draw_square(lat, lon);
}

void
CMapCont::draw_square(TReal x, TReal y) const
{
	CWindowGc& gc = SystemGc();
	TRect canvas(0, 0, Rect().Width(), Rect().Height() - _status_sz);

	// cordinates relative to position
	x -= _x;
	y -= _y;

	// scale
	x *= _scale;
	y *= _scale;

	// origin
	TInt ox = canvas.Width() / 2;
	TInt oy = canvas.Height() / 2;

	// plane cordinates
	ox += (TInt) x;
	oy += (TInt) y;

	TInt off = _size/2;
	TRect square(ox - off, oy - off, ox + off, oy + off);
	gc.DrawRect(square);
}

void
CMapCont::refresh_map(APS& aps, CGPS& gps)
{
	// XXX
	_ap  = &aps;
	_gps = &gps;

	_valid = _gps->valid();
	if (_valid) {
		_x = _gps->latitude();
		_y = _gps->longitude();
	}

	DrawNow();
}

TKeyResponse
CMapCont::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
	if (aType != EEventKey)
		return EKeyWasNotConsumed;

	TBool refresh = EFalse;

	switch (aKeyEvent.iScanCode) {
	case EStdKeyUpArrow:
		_selected--;

		if (_selected < 0)
			_selected = 0;

		refresh = ETrue;
		break;

	case EStdKeyDownArrow:
		_selected++;

		if (_selected >= _ap->Count())
			_selected = _ap->Count() - 1;

		if (_selected < 0)
			_selected = 0;

		refresh = ETrue;
		break;

	// XXX portability
	case EStdKeyDevice3:
		if (_ap->Count() > 0) {
			CAP* ap = (*_ap)[_selected];

			_barb.ap_details(*ap);
		}
		break;

	case EStdKeyHash:
		_scale *= 10.0;
		refresh = ETrue;
		break;

	// *
	case 0x2a:
		_scale /= 10.0;
		refresh = ETrue;
		break;
	}

	if (refresh)
		DrawNow();

	return EKeyWasNotConsumed;
}

CMapView*
CMapView::NewLC(MBarbUI& barb)
{
	CMapView* self = new CMapView(barb);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);

	return self;
}

CMapView::CMapView(MBarbUI& barb) : _cont(NULL), _barb(barb)
{
}

CMapView::~CMapView()
{
	delete _cont;
}

void
CMapView::ConstructL()
{
	BaseConstructL(R_BARBMAP_VIEW);

	_cont = new CMapCont(_barb);
        _cont->SetMopParent(this);
        _cont->ConstructL(ClientRect());
        _cont->MakeVisible(EFalse);
}

TUid
CMapView::Id() const
{
	return TUid::Uid(EViewMap);
}

void
CMapView::DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                      const TDesC8& aCustomMessage)
{
	AppUi()->AddToStackL(*this, _cont);
	_cont->MakeVisible(ETrue);
}

void
CMapView::DoDeactivate()
{
	_cont->MakeVisible(EFalse);
	AppUi()->RemoveFromStack(_cont);
}

void
CMapView::HandleCommandL(TInt cmd)
{
        AppUi()->HandleCommandL(cmd);
}

void
CMapView::refresh_map(APS& ap, CGPS& gps)
{
	_cont->refresh_map(ap, gps);
}
