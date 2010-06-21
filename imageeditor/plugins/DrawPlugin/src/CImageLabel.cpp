/*
 ============================================================================
 Name		: CImageLabel.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CCImageLabel implementation
 ============================================================================
 */
#include "TimeAO.h"
#include "CImageLabel.h"
#include <W32STD.H>
#include <akniconutils.h> 
#include <EIKENV.H> //For iEikonEnv
#include <GDI.H>
// ---------------------------------------------------------------------------
// CImageLabel()
// ---------------------------------------------------------------------------
//
CImageLabel::CImageLabel()
	{
	// No implementation required
	}
// ---------------------------------------------------------------------------
// ~CImageLabel()
// ---------------------------------------------------------------------------
//
CImageLabel::~CImageLabel()
	{
	if (iBitmap != NULL)
		{
		delete iBitmap;
		iBitmap = NULL;
		}
	if (iBitmapMask != NULL)
		{
		delete iBitmapMask;
		iBitmap = NULL;
		}
	if (iTimeAO != NULL)
		{
		delete iTimeAO;
		iTimeAO = NULL;
		}
	}
// ---------------------------------------------------------------------------
// NewLC()
// ---------------------------------------------------------------------------
//
CImageLabel* CImageLabel::NewLC(CCoeControl *aParent, const TRect& aRect,
		TOOLTIP_LAYOUT aLabelLayout)
	{
	CImageLabel* self = new (ELeave) CImageLabel();
	CleanupStack::PushL(self);
	self->ConstructL(aParent, aRect, aLabelLayout);
	return self;
	}
// ---------------------------------------------------------------------------
// NewL()
// ---------------------------------------------------------------------------
//
CImageLabel* CImageLabel::NewL(CCoeControl *aParent, const TRect& aRect,
		TOOLTIP_LAYOUT aLabelLayout)
	{
	CImageLabel* self = CImageLabel::NewLC(aParent, aRect, aLabelLayout);
	CleanupStack::Pop(); // self;
	return self;
	}
// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
//
void CImageLabel::ConstructL(CCoeControl *aParent, const TRect& aRect,
		TOOLTIP_LAYOUT aLabelLayout)
	{
	SetContainerWindowL(*aParent);
	SetRect(aRect);

	iState = ENone;
	iLabelLayout = aLabelLayout;
	iBitmap = NULL;
	iBitmapMask = NULL;

	iTooltipStartPoint = TPoint(0, 0);
	CalculateImagePoint();
	CalculateBoundingRectStartPoint();
	CalculateZones();

	iTimeAO = CTimeAO::NewL();
	iTimeAO->SetObserver(this);
	
	iIsHandlePointerNotifyProcessing = EFalse;
	}
// ---------------------------------------------------------------------------
// HandlePointerEventL()
// ---------------------------------------------------------------------------
//
void CImageLabel::HandlePointerEventL(const TPointerEvent& aPointerEvent)
	{
	switch (aPointerEvent.iType)
		{
		case TPointerEvent::EButton1Down:
			{
			if(iIsHandlePointerNotifyProcessing)
				{
				//Ignore button down event when HandlePointerNotify is processing.
				break;
				}
			iState = ESelected;
			DrawNow();
			iTimeAO->StartL();
			
			break;
			}
		case TPointerEvent::EButton1Up:
			{
			iTimeAO->Cancel();
			//If the statte is moveing or selected.change the state to ENone 
			
			if (iState == EMoving || iState == ESelectedWithTooltip)
				{
				iState = ENone;
				DrawNow();
				}
			else
				{
				iState = ENone;
				DrawNow();
				//If not on process start it
				if(!iIsHandlePointerNotifyProcessing)
					{
					iIsHandlePointerNotifyProcessing = ETrue;
					//To avoid pop up dialog two times by quick double chick
					iHandlePointerObserver->HandlePointerNotify(this);
					iIsHandlePointerNotifyProcessing = EFalse;
					}
				}
			break;
			}
		default:
			break;
		}
	}
// ---------------------------------------------------------------------------
// SizeChanged()
// ---------------------------------------------------------------------------
//
void CImageLabel::SizeChanged()
	{
	DrawNow();
	}
// ---------------------------------------------------------------------------
// Draw()
// ---------------------------------------------------------------------------
//
void CImageLabel::Draw(const TRect& aRect) const
	{
	// Get the standard graphics context
	CWindowGc& gc = SystemGc();

	TRgb Rgb(KRgbGray);
	Rgb.SetAlpha(180);
	gc.SetBrushColor(Rgb);
	gc.SetBrushStyle(CWindowGc::ESolidBrush);
	gc.SetPenStyle(CGraphicsContext::ESolidPen);
	gc.SetPenColor(KRgbDarkGray);
	gc.SetPenSize(TSize(2,2));

	switch (iState)
		{
		case ENone:
			{
		//do nothing	
			}
			break;
		case EMoving:
			{
			//Draw the bounding rect
			TSize cornorSize(8, 8);
			gc.DrawRoundRect(TRect(iBoundingRectStartPoint, iBoundingSize),
					cornorSize);
			break;
			}
		case ESelected:
			{
			//Draw the bouding rect
			TSize cornorSize(8, 8);
			gc.DrawRoundRect(TRect(iBoundingRectStartPoint, iBoundingSize),
					cornorSize);
			break;
			}
		case ESelectedWithTooltip:
			{
			//Draw the bounding rect
			TSize cornorSize(8, 8);
			gc.DrawRoundRect(TRect(iBoundingRectStartPoint, iBoundingSize),
					cornorSize);

			CFont* pFont = NULL;
			TFontSpec fontSpec = iEikonEnv->TitleFont()->FontSpecInTwips();
			fontSpec.iHeight *= 0.535;
			iCoeEnv->ScreenDevice()->GetNearestFontInTwips(pFont, fontSpec);

			//Draw the tooltips
			gc.UseFont(pFont);
			gc.SetPenStyle(CGraphicsContext::ESolidPen);
			gc.SetPenColor(KRgbGreen);
			gc.DrawText(iTooltip, iTooltipStartPoint);
			iCoeEnv->ScreenDevice()->ReleaseFont(pFont);
			gc.DiscardFont();
			break;
			}
		default:
			break;
		}

	//Draw svg picture
	if (iBitmap != NULL)
		{
		gc.BitBltMasked(iImageStartPoint, iBitmap, TRect(TPoint(0, 0), TSize(
				iBoundingSize)), iBitmapMask, ETrue);
		}

	}
// ---------------------------------------------------------------------------
// Notify()
// ---------------------------------------------------------------------------
//
void CImageLabel::Notify()
	{
	//if the state is not moving set selected with tooltips
	if (iState != EMoving)
		iState = ESelectedWithTooltip;
	DrawNow();
	}
// ---------------------------------------------------------------------------
// SetImage()
// ---------------------------------------------------------------------------
//
void CImageLabel::SetImage(const TDesC& aMbmPackage, TInt aIndex)
	{
	//Load pictures
	AknIconUtils::CreateIconL(iBitmap, iBitmapMask, aMbmPackage, aIndex, aIndex
			+ 1);
	AknIconUtils::SetSize(iBitmap, iImageSize);
	AknIconUtils::SetSize(iBitmapMask, iImageSize);

	ActivateL();
	}
// ---------------------------------------------------------------------------
// SetTooltip()
// ---------------------------------------------------------------------------
//
void CImageLabel::SetTooltip(const TDesC& aTooltip)
	{
	iTooltip.Copy(aTooltip);
	CalculateTooltipPoint();
	}
// ---------------------------------------------------------------------------
// ResetControl()
// ---------------------------------------------------------------------------
//
void CImageLabel::ResetControl()
	{
	CalculateImagePoint();
	CalculateBoundingRectStartPoint();
	CalculateTooltipPoint();
	CalculateZones();
	}
// ---------------------------------------------------------------------------
// CalculateImagePoint()
// ---------------------------------------------------------------------------
//
void CImageLabel::CalculateImagePoint()
	{
	// landscape 
	if (iLabelLayout == ELabelVertical)
		{
		iImageStartPoint.iX = Rect().iTl.iX + 2;
		iImageStartPoint.iY = Rect().iBr.iY - (Rect().iBr.iX - Rect().iTl.iX)
				+ 2;
		}
	//portrait
	else if (iLabelLayout ==ELabelHorizontal )
		{
		iImageStartPoint.iX = Rect().iBr.iX - (Rect().iBr.iY - Rect().iTl.iY)
				+ 2;
		iImageStartPoint.iY = Rect().iTl.iY + 2;
		}
	}
// ---------------------------------------------------------------------------
// CalculateBoundingRectStartPoint()
// calcuate related start point
// ---------------------------------------------------------------------------
//
void CImageLabel::CalculateBoundingRectStartPoint()
	{
	// landscape 
	if (iLabelLayout == ELabelVertical)
		{
		iBoundingRectStartPoint.iX = Rect().iTl.iX + 1;
		iBoundingRectStartPoint.iY = Rect().iBr.iY - (Rect().iBr.iX
				- Rect().iTl.iX) + 1;
		}
	//portrait
	else if (iLabelLayout == ELabelHorizontal)
		{
		iBoundingRectStartPoint.iX = Rect().iBr.iX - (Rect().iBr.iY
				- Rect().iTl.iY) + 1;
		iBoundingRectStartPoint.iY = Rect().iTl.iY + 1;
		}

	}
// ---------------------------------------------------------------------------
// CalculateTooltipPoint()
// Where show the tooltips.
// ---------------------------------------------------------------------------
//
void CImageLabel::CalculateTooltipPoint()
	{
	// landscape 
	if (iLabelLayout == ELabelVertical)
		{
		CFont* pFont = NULL;
		TFontSpec fontSpec = iEikonEnv->TitleFont()->FontSpecInTwips();
		fontSpec.iHeight *= 0.535;
		iCoeEnv->ScreenDevice()->GetNearestFontInTwips(pFont, fontSpec);
		int tooltipLength = pFont->TextWidthInPixels(iTooltip);
		iCoeEnv->ScreenDevice()->ReleaseFont(pFont);

		iTooltipStartPoint.iX = Rect().iTl.iX
				+ (Rect().Width() - tooltipLength) / 2;
		iTooltipStartPoint.iY = Rect().iBr.iY - (Rect().iBr.iX - Rect().iTl.iX);
		} 
	//portrait
	else if (iLabelLayout ==ELabelHorizontal )
		{
		CFont* pFont = NULL;
		TFontSpec fontSpec = iEikonEnv->TitleFont()->FontSpecInTwips();
		fontSpec.iHeight *= 0.535;
		iCoeEnv->ScreenDevice()->GetNearestFontInTwips(pFont, fontSpec);
		int tooltipLength = pFont->TextWidthInPixels(iTooltip);
		int tooltipHeight = pFont->HeightInPixels();
		iCoeEnv->ScreenDevice()->ReleaseFont(pFont);

		iTooltipStartPoint.iX = Rect().iTl.iX + (iBoundingRectStartPoint.iX
				- Rect().iTl.iX - tooltipLength) / 2;
		iTooltipStartPoint.iY = Rect().iBr.iY - (Rect().Height()
				- tooltipHeight) / 2;
		}
	}
// ---------------------------------------------------------------------------
// CalculateZones()
// ---------------------------------------------------------------------------
//

void CImageLabel::CalculateZones()
	{
	// landscape 
	if (iLabelLayout == ELabelVertical)
		{
		iBoundingSize.iHeight = Rect().iBr.iX - Rect().iTl.iX - 2;
		iBoundingSize.iWidth = iBoundingSize.iHeight - 2;

		iImageSize.iHeight = iBoundingSize.iHeight - 4;
		iImageSize.iWidth = iBoundingSize.iWidth - 4;
		}
	//portrait
	else if (iLabelLayout ==ELabelHorizontal)
		{
		iBoundingSize.iHeight = Rect().iBr.iY - Rect().iTl.iY - 2;
		iBoundingSize.iWidth = iBoundingSize.iHeight - 2;

		iImageSize.iHeight = iBoundingSize.iHeight - 4;
		iImageSize.iWidth = iBoundingSize.iWidth - 4;
		}
	}
// ---------------------------------------------------------------------------
// LabelIsMoving()
// ---------------------------------------------------------------------------
//

void CImageLabel::LabelIsMoving()
	{
	iState = EMoving;
	}
// ---------------------------------------------------------------------------
// SetHandlePointerObserver()
// ---------------------------------------------------------------------------
//
void CImageLabel::SetHandlePointerObserver(
		MHandlePointerObserver* aHandlePointerObserver)
	{
	iHandlePointerObserver = aHandlePointerObserver;
	}
