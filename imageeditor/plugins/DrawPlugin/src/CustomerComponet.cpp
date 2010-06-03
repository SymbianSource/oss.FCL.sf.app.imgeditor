/*
 ============================================================================
 Name		: CustomerComponet.cpp
 Author	  : henry
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CCustomerComponet implementation
 ============================================================================
 */

#include "CustomerComponet.h"
#include "gdi.h"
#include <eikdef.h>
#include "imageditorfilterdraw.mbg"
#include <eikenv.h>
#include <akniconutils.h>
#include <aknnotewrappers.h>

// ---------------------------------------------------------------------------
// Default constructer
// ---------------------------------------------------------------------------
//
CCustomerComponet::CCustomerComponet()
	{
	// No implementation required
	}

CCustomerComponet::~CCustomerComponet()
	{
	if (iMenu)
		{
		delete iMenu;
		iMenu = NULL;
		}
	if (iRight)
		{
		delete iRight;
		iRight = NULL;
		}
	if (iLeft)
		{
		delete iLeft;
		iLeft = NULL;
		}
	if (iTop)
		{
		delete iTop;
		iTop = NULL;
		}
	if (iBottom)
		{
		delete iBottom;
		iBottom = NULL;
		}
	if (iMenuMask)
		{
		delete iMenuMask;
		iMenuMask = NULL;
		}
	if (iRightMask)
		{
		delete iRightMask;
		iRightMask = NULL;
		}
	if (iLeftMask)
		{
		delete iLeftMask;
		iLeftMask = NULL;
		}
	if (iTopMask)
		{
		delete iTopMask;
		iTopMask = NULL;
		}
	if (iBottomMask)
		{
		delete iBottomMask;
		iBottomMask = NULL;
		}
	}
// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CCustomerComponet* CCustomerComponet::NewLC(TRect & aRect, CCoeControl* aParent)
	{
	CCustomerComponet* self = new (ELeave) CCustomerComponet();
	CleanupStack::PushL(self);
	self->ConstructL(aRect, aParent);
	return self;
	}
// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CCustomerComponet* CCustomerComponet::NewL(TRect & aRect, CCoeControl* aParent)
	{
	CCustomerComponet* self = CCustomerComponet::NewLC(aRect, aParent);
	CleanupStack::Pop(); // self;
	return self;
	}
// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
//
void CCustomerComponet::ConstructL(TRect & aRect, CCoeControl* aParent)
	{
	SetContainerWindowL(*aParent);
	EnableDragEvents();
	SetRect(aRect);
	TRgb color = KRgbBlue;
	//Set transparent effect
	if (KErrNone == Window().SetTransparencyAlphaChannel())
		{
		color.SetAlpha(0);
		}
	Window().SetBackgroundColor(color);

	iDrawDirection = EFalse;
	
	//Load pictures and mask
	_LIT(iconFileName, "\\resource\\apps\\imageditorfilterdraw.mbm");
	TFileName iconFile(iconFileName);
	//Menu
	AknIconUtils::CreateIconL(iMenu, iMenuMask, iconFileName,
			EMbmImageditorfilterdraw15a, EMbmImageditorfilterdrawMenumask);
	//Right
	AknIconUtils::CreateIconL(iRight, iRightMask, iconFileName,
			EMbmImageditorfilterdrawRight, EMbmImageditorfilterdrawRightmask);
	//Left
	AknIconUtils::CreateIconL(iLeft, iLeftMask, iconFileName,
			EMbmImageditorfilterdrawLeft, EMbmImageditorfilterdrawLeftmask);
	//Top
	AknIconUtils::CreateIconL(iTop, iTopMask, iconFileName,
			EMbmImageditorfilterdrawTop, EMbmImageditorfilterdrawTopmask);
	//Bottom
	AknIconUtils::CreateIconL(iBottom, iBottomMask, iconFileName,
			EMbmImageditorfilterdrawBottom, EMbmImageditorfilterdrawBottommask);
	
	ActivateL();
	}
// ---------------------------------------------------------------------------
// Draw
// ---------------------------------------------------------------------------
//
void CCustomerComponet::Draw(const TRect& aRect) const
	{
	CWindowGc& gc = SystemGc();
	gc.Clear(aRect);
	//Create rect for showing pictures.
	TRect menuRect, topRect, bottomRect, rightRect, leftRect;

	menuRect = aRect;
	//get the width of the rect
	TInt wx = aRect.iBr.iX - aRect.iTl.iX;
	// get the height of the screen
	TInt hy = aRect.iBr.iY - aRect.iTl.iY;
	//Set top rect.
	topRect.SetRect(TPoint(wx / 3 + aRect.iTl.iX, aRect.iTl.iY), TPoint(
			(wx / 3) * 2 + aRect.iTl.iX, hy / 3 + aRect.iTl.iY));
	//Set bottom rect
	bottomRect.SetRect(TPoint((wx/3)+aRect.iTl.iX,(hy/3)*2+aRect.iTl.iY),
			TPoint((wx/3)*2+aRect.iTl.iX,aRect.iBr.iY));
	//set right rect.
	rightRect.SetRect(TPoint(aRect.iTl.iX,hy/3+aRect.iTl.iY), TPoint(wx/3+aRect.iTl.iX,(hy/3)*2+aRect.iTl.iY));

	//set left  rect.
	leftRect.SetRect(TPoint((wx/3)*2+aRect.iTl.iX,hy/3+aRect.iTl.iY),
			TPoint(aRect.iBr.iX,(hy/3)*2+aRect.iTl.iY));
	//Set set size for pictures
	
	AknIconUtils::SetSize(iMenu, aRect.Size(), EAspectRatioPreserved);
	AknIconUtils::SetSize(iMenuMask, aRect.Size(), EAspectRatioPreserved);

	AknIconUtils::SetSize(iTop, topRect.Size(), EAspectRatioPreserved);
	AknIconUtils::SetSize(iTopMask, topRect.Size(), EAspectRatioPreserved);

	AknIconUtils::SetSize(iBottom, bottomRect.Size(), EAspectRatioPreserved);
	AknIconUtils::SetSize(iBottomMask, bottomRect.Size(), EAspectRatioPreserved);

	AknIconUtils::SetSize(iRight, rightRect.Size(), EAspectRatioPreserved);
	AknIconUtils::SetSize(iRightMask, rightRect.Size(), EAspectRatioPreserved);

	AknIconUtils::SetSize(iLeft, leftRect.Size(), EAspectRatioPreserved);
	AknIconUtils::SetSize(iLeftMask, leftRect.Size(), EAspectRatioPreserved);
	
	//Draw menu
	gc.DrawBitmapMasked(menuRect, iMenu, TRect(TPoint(0, 0),
			iMenu->SizeInPixels()), iMenuMask, ETrue);
	//if the user press the menu bottom show direction rect.
	if (iDrawDirection)
		{
		gc.DrawBitmapMasked(topRect, iTop, TRect(TPoint(0, 0),
				iTop->SizeInPixels()), iTopMask, ETrue);

		gc.DrawBitmapMasked(bottomRect, iBottom, TRect(TPoint(0, 0),
				iBottom->SizeInPixels()), iBottomMask, ETrue);
		gc.DrawBitmapMasked(leftRect, iRight, TRect(TPoint(0, 0),
				iRight->SizeInPixels()), iRightMask, ETrue);
		gc .DrawBitmapMasked(rightRect, iLeft, TRect(TPoint(0, 0),
				iLeft->SizeInPixels()), iLeftMask, ETrue);
		}
	}
// ---------------------------------------------------------------------------
// SetRect
// ---------------------------------------------------------------------------
//
void CCustomerComponet::SetRect(TRect& aRect)
	{
	DrawNow(aRect);
	}
// ---------------------------------------------------------------------------
// SizeChanged()
// ---------------------------------------------------------------------------
//
void CCustomerComponet::SizeChanged()
	{
//Nothing done here.
	}
// ---------------------------------------------------------------------------
// HandlePointerEventL()
// ---------------------------------------------------------------------------
//
void CCustomerComponet::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	switch (aPointerEvent.iType)
		{
		
		case TPointerEvent::EButton1Down:
			{
			iDrawDirection = ETrue;
			iButtonFlag = ETrue;
			iPointerObserver->HandlePointerNotify(this);
			DrawNow();
			}
			break;
		case TPointerEvent::EButton1Up:
			{
			iDrawDirection = EFalse;
			iButtonFlag = EFalse;
			iPointerObserver->HandlePointerNotify(this);
			DrawNow();
			}
			break;
		case TPointerEvent::EDrag:
			{
			DrawNow();
			}
			break;
		default:
			break;
		}
	}
// ---------------------------------------------------------------------------
// SetHandlePointerObserver()
// ---------------------------------------------------------------------------
//
void CCustomerComponet::SetHandlePointerObserver(
		MHandlePointerObserver* aHandlePointerObserver)
	{
	iPointerObserver = aHandlePointerObserver;

	}
// ---------------------------------------------------------------------------
// GetButtonType()
// ---------------------------------------------------------------------------
//
TBool CCustomerComponet::GetButtonType()
	{
	return iButtonFlag;
	}
