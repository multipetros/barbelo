#include <aknviewappui.h>
#include <barsread.h>

#include "debugview.h"
#include "barbre.hrh"
#include "barbelo.rsg"

CDebugCont* CDebugCont::_hack = NULL;

void
CDebugCont::ConstructL(const TRect& rect)
{
	CreateWindowL();

        TResourceReader reader;
        iCoeEnv->CreateResourceReaderLC(reader, R_BARBELO_DBG);

        _dbg = new CEikEdwin;
        _dbg->SetContainerWindowL(*this);
        _dbg->ConstructFromResourceL(reader);
        CleanupStack::PopAndDestroy(); // reader
        CEikScrollBarFrame* sb = _dbg->CreatePreAllocatedScrollBarFrameL();
        sb->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,
                                    CEikScrollBarFrame::EAuto);

	SizeChanged();
	SetRect(rect);
	ActivateL();
}

CDebugCont::CDebugCont() : _dbg(NULL)
{
	_hack = this;
}

CDebugCont::~CDebugCont()
{
	delete _dbg;
}

void
CDebugCont::static_dmsg(const TDesC& msg)
{
	if (_hack)
		_hack->dmsg(msg);
}

void
CDebugCont::SizeChanged()
{
        TSize size = Size();

        CEikScrollBarFrame* sb = _dbg->ScrollBarFrame();
        if (sb)
                size.iWidth -= sb->VerticalScrollBar()->ScrollBarBreadth();

        _dbg->SetExtent(TPoint(0, 0), size);
}

TKeyResponse
CDebugCont::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{       
        if (aType != EEventKey)
                return EKeyWasNotConsumed;

        switch (aKeyEvent.iScanCode) {
                case EStdKeyUpArrow:
                        _dbg->MoveDisplayL(TCursorPosition::EFLineUp);
                        break;

                case EStdKeyDownArrow:
                        _dbg->MoveDisplayL(TCursorPosition::EFLineDown);
                        break;
        }

        return EKeyWasNotConsumed;
//      return _dbg->OfferKeyEventL(aKeyEvent, aType);
}

CCoeControl*
CDebugCont::ComponentControl(TInt aIndex) const
{       
        switch (aIndex)
        {
        case 0:
                return _dbg;
                
        default:
                return NULL;
        }
}

TInt
CDebugCont::CountComponentControls() const
{       
        return 1;
}

void
CDebugCont::dmsg(const TDesC& msg)
{
        CPlainText *text;
        text = _dbg->Text();
        text->InsertL(text->DocumentLength(), msg);
        text->InsertL(text->DocumentLength(), _L("\f"));

        _dbg->SetCursorPosL(text->DocumentLength(), EFalse);
        _dbg->ClearSelectionL();
        _dbg->UpdateScrollBarsL();
        _dbg->NotifyNewFormatL();
}

CDebugView*
CDebugView::NewLC()
{       
        CDebugView* self = new CDebugView;
        CleanupStack::PushL(self);
        self->ConstructL();
        CleanupStack::Pop(self);

        return self;
}

CDebugView::CDebugView() : _cont(NULL)
{
}

CDebugView::~CDebugView()
{
	delete _cont;
}

void
CDebugView::ConstructL()
{
	BaseConstructL(R_BARBDEBUG_VIEW);

	_cont = new CDebugCont;
	_cont->SetMopParent(this);
	_cont->ConstructL(ClientRect());
	_cont->MakeVisible(EFalse);
}

TUid
CDebugView::Id() const
{       
        return TUid::Uid(EViewDebug);
}

void
CDebugView::DoActivateL(const TVwsViewId& aPrevViewId, TUid aCustomMessageId,
                        const TDesC8& aCustomMessage)
{
	AppUi()->AddToStackL(*this, _cont);
	_cont->MakeVisible(ETrue);
}

void
CDebugView::DoDeactivate()
{
	_cont->MakeVisible(EFalse);
	AppUi()->RemoveFromStack(_cont);
}

void
CDebugView::dmsg(const TDesC& msg)
{
	_cont->dmsg(msg);
}

void
CDebugView::HandleCommandL(TInt cmd)
{
	AppUi()->HandleCommandL(cmd);
}
