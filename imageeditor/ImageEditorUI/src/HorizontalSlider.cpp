/*
* Copyright (c) 2010 Ixonos Plc.
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - Initial contribution
*
* Contributors:
* Ixonos Plc
*
* Description:  
*
*/


//	INCLUDE
#include <fbs.h>
#include <aknutils.h>
#include <ImageEditorUI.rsg>
#include <AknLayoutScalable_Avkon.cdl.h>
#include <AknLayoutScalable_Apps.cdl.h>

#include <AknIconUtils.h>
#include <ImageEditorUi.mbg>
#include "ResolutionUtil.h"
#include "ImageEditorUiDefs.h"
#include "HorizontalSlider.h"
#include "CustomControlPanics.h"

// constants
const TInt KBorderPartsNum = 9; 
const TInt KSliderPartsNum = 4; 

//=============================================================================
EXPORT_C CHorizontalSlider* CHorizontalSlider::NewL(const TRect& aRect, const CCoeControl& aControl)
    {
    CHorizontalSlider* self = CHorizontalSlider::NewLC(aRect, aControl);
    CleanupStack::Pop(self);
    return self;
    }

//=============================================================================
EXPORT_C CHorizontalSlider* CHorizontalSlider::NewLC(const TRect& aRect, const CCoeControl& aControl)
    {
    CHorizontalSlider* self = new (ELeave) CHorizontalSlider;
    CleanupStack::PushL(self);
    self->ConstructL(aRect, aControl);
    return self;
    }

//=============================================================================
EXPORT_C CHorizontalSlider::~CHorizontalSlider()
    {
    iBorders.ResetAndDestroy();
    iScrollBar.ResetAndDestroy();
	if (iIcon)
		{
	    delete iIcon;
		}
    }

//=============================================================================
void CHorizontalSlider::ConstructL(const TRect& aRect, const CCoeControl& aControl)
    {
	SetContainerWindowL(aControl);
    SetRect(aRect);

    TInt variant = 0;
    TFileName iconFile (KImageEditorUiMifFile);

	// create popup and slider graphics
    for ( TInt i = 0; i < KBorderPartsNum; ++i )
        {
        CEikImage* image = new (ELeave) CEikImage;

		image->CreatePictureFromFileL(iconFile,
										EMbmImageeditoruiQgn_graf_popup_trans_center + 2*i,
										EMbmImageeditoruiQgn_graf_popup_trans_center_mask + 2*i);

        CleanupStack::PushL( image );
        image->SetContainerWindowL( *this );
//        image->SetPictureOwnedExternally( ETrue );
        iBorders.AppendL( image );
        CleanupStack::Pop( image );
        }
    for ( TInt i = 0; i < KSliderPartsNum; ++i )
        {
        CEikImage* image = new (ELeave) CEikImage;

		image->CreatePictureFromFileL(iconFile,
										EMbmImageeditoruiQgn_graf_nslider_imed_end_left + 2*i,
										EMbmImageeditoruiQgn_graf_nslider_imed_end_left_mask + 2*i);

        CleanupStack::PushL( image );
        image->SetContainerWindowL( *this );
//        image->SetPictureOwnedExternally( ETrue );
        iScrollBar.AppendL( image );
        CleanupStack::Pop( image );
        }

	ActivateL();
    }

//=============================================================================
CHorizontalSlider::CHorizontalSlider()
    {
    // no implementation required
    }

//=============================================================================
EXPORT_C TInt CHorizontalSlider::CountComponentControls() const
    {
    TInt count = iBorders.Count();
    count += iScrollBar.Count();
	if (iIcon)
		{
		count++;
		}

    return count;
    }

//=============================================================================
void CHorizontalSlider::Draw(const TRect& /*aRect*/) const
	{
	// determine popup border layouts
    AknLayoutUtils::LayoutControl( iBorders[0], Rect(), AknLayoutScalable_Avkon::bg_tb_trans_pane_g1().LayoutLine() );
    AknLayoutUtils::LayoutControl( iBorders[1], Rect(), AknLayoutScalable_Avkon::bg_tb_trans_pane_g2().LayoutLine() );
    AknLayoutUtils::LayoutControl( iBorders[2], Rect(), AknLayoutScalable_Avkon::bg_tb_trans_pane_g3().LayoutLine() );
    AknLayoutUtils::LayoutControl( iBorders[3], Rect(), AknLayoutScalable_Avkon::bg_tb_trans_pane_g4().LayoutLine() );
    AknLayoutUtils::LayoutControl( iBorders[4], Rect(), AknLayoutScalable_Avkon::bg_tb_trans_pane_g5().LayoutLine() );
    AknLayoutUtils::LayoutControl( iBorders[5], Rect(), AknLayoutScalable_Avkon::bg_tb_trans_pane_g6().LayoutLine() );
    AknLayoutUtils::LayoutControl( iBorders[6], Rect(), AknLayoutScalable_Avkon::bg_tb_trans_pane_g7().LayoutLine() );
    AknLayoutUtils::LayoutControl( iBorders[7], Rect(), AknLayoutScalable_Avkon::bg_tb_trans_pane_g8().LayoutLine() );
    AknLayoutUtils::LayoutControl( iBorders[8], Rect(), AknLayoutScalable_Avkon::bg_tb_trans_pane_g9().LayoutLine() );
    // Images need to be resized separately, because the standard LayoutImage
    // tries to preserve the aspect ratio and we don't want that.
    //
    for ( TInt i = 0; i < iBorders.Count(); ++i )
        {
        AknIconUtils::SetSize(
            const_cast<CFbsBitmap*>( iBorders[i]->Bitmap() ),
            iBorders[i]->Size(), EAspectRatioNotPreserved );
        }

	// determine icon layout
    AknLayoutUtils::LayoutControl( iIcon,  Rect(), AknLayoutScalable_Apps::popup_imed_adjust2_window_g1().LayoutLine() );
    AknIconUtils::SetSize(
        const_cast<CFbsBitmap*>( iIcon->Bitmap() ),
        iIcon->Size(), EAspectRatioNotPreserved );

	// determine caption layout
    TAknLayoutText layoutText;
    layoutText.LayoutText( Rect(), AknLayoutScalable_Apps::popup_imed_adjust2_window_t1() );
    TRgb color = layoutText.Color();
    layoutText.DrawText( SystemGc(), iText, ETrue, color );

	// determine scrollbar layouts
    TAknWindowComponentLayout l = AknLayoutScalable_Apps::slider_imed_adjust_pane();
    TAknWindowLineLayout lineLayout = l.LayoutLine();
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect(Rect(), lineLayout);
    AknLayoutUtils::LayoutControl( iScrollBar[0], layoutRect.Rect(), AknLayoutScalable_Apps::slider_imed_adjust_pane_g1().LayoutLine() );
    AknLayoutUtils::LayoutControl( iScrollBar[1], layoutRect.Rect(), AknLayoutScalable_Apps::slider_imed_adjust_pane_g2().LayoutLine() );
    AknLayoutUtils::LayoutControl( iScrollBar[2], layoutRect.Rect(), AknLayoutScalable_Apps::slider_imed_adjust_pane_g3().LayoutLine() );
    AknLayoutUtils::LayoutControl( iScrollBar[3], layoutRect.Rect(), AknLayoutScalable_Apps::slider_imed_adjust_pane_g4().LayoutLine() );
    for ( TInt i = 0; i < iScrollBar.Count(); ++i )
        {
        AknIconUtils::SetSize(
            const_cast<CFbsBitmap*>( iScrollBar[i]->Bitmap() ),
            iScrollBar[i]->Size(), EAspectRatioNotPreserved );
        }

	// determine scrollbar marker layout
    if ( iMinimumValue <= iMaximumValue )
	    {
        TUint length = iMaximumValue - iMinimumValue; // length of the slider
        TUint pixelsFromMin = iPosition - iMinimumValue; // tab position from the beginning
    
        TReal factor = 0.0;
        if (iMinimumValue < iMaximumValue )
        {
            factor = (TReal) pixelsFromMin / length; // tab position from the beginning in percentage
        }

        TUint sliderTabWidth = iScrollBar[KSliderPartsNum - 1]->Size().iWidth; // slider tab width

        // slider bitmap is actually a bit longer but this resolves the problem
        // where the tab is drawn outside of the slider when in maximum position
        TUint sliderBitmapWidth = layoutRect.Rect().Width() - sliderTabWidth;
    
        TInt tabPositionFromMinInPixels = (TInt) ((factor * sliderBitmapWidth + 0.5) - (sliderBitmapWidth / 2)); // calculate tab position

		TRect markerRect = layoutRect.Rect();
		markerRect.iTl.iX += tabPositionFromMinInPixels;

	    AknLayoutUtils::LayoutControl( iScrollBar[KSliderPartsNum - 1], markerRect, AknLayoutScalable_Apps::slider_imed_adjust_pane_g4().LayoutLine() );

        AknIconUtils::SetSize(
            const_cast<CFbsBitmap*>( iScrollBar[KSliderPartsNum - 1]->Bitmap() ),
            iScrollBar[KSliderPartsNum - 1]->Size(), EAspectRatioNotPreserved );
	    }
	}

//=============================================================================
EXPORT_C CCoeControl* CHorizontalSlider::ComponentControl(TInt aIndex) const
    {
    if ( aIndex < iBorders.Count() )
        {
        return iBorders[aIndex];
        }

    if ( aIndex < iBorders.Count() + iScrollBar.Count())
        {
        return iScrollBar[aIndex - iBorders.Count()];
        }
    return iIcon;
    }

// setters

//=============================================================================
EXPORT_C void CHorizontalSlider::SetIcon(CEikImage* aIcon)
	{
	iIcon = aIcon;
	}

//=============================================================================
EXPORT_C void CHorizontalSlider::SetCaption(const TDesC& aText)
	{
	iText = aText;
	}

//=============================================================================
EXPORT_C void CHorizontalSlider::SetMinimum(TInt aValue)
	{
	iMinimumValue = aValue;
	}

//=============================================================================
EXPORT_C void CHorizontalSlider::SetMaximum(TInt aValue)
	{
	iMaximumValue = aValue;
	}

//=============================================================================
EXPORT_C void CHorizontalSlider::SetStep(TUint aValue)
	{
	iStep = aValue;
	}

//=============================================================================
EXPORT_C void CHorizontalSlider::SetStepAmount(TUint8 aValue)
	{
	iNumberOfSteps = aValue;
	
	if(aValue == 0)
		{
		iStep = 0;
		}
	else
		{
		iStep = (iMaximumValue-iMinimumValue) / aValue;
		}
	}

//=============================================================================
EXPORT_C void CHorizontalSlider::SetPosition(TInt aValue)
	{
	__ASSERT_ALWAYS( aValue >= iMinimumValue, Panic(EHorizontalSliderPanicIndexUnderflow) );
	__ASSERT_ALWAYS( aValue <= iMaximumValue, Panic(EHorizontalSliderPanicIndexOverflow) );

	iPosition = aValue;
	}

// getters

//=============================================================================
EXPORT_C TInt CHorizontalSlider::Minimum() const
	{
	return iMinimumValue;
	}

//=============================================================================
EXPORT_C TInt CHorizontalSlider::Maximum() const
	{
	return iMaximumValue;
	}

//=============================================================================
EXPORT_C TInt CHorizontalSlider::Step() const
	{
	return iStep;
	}

//=============================================================================
EXPORT_C TInt CHorizontalSlider::Position() const
	{
	return iPosition;
	}

//=============================================================================
EXPORT_C void CHorizontalSlider::Increment()
	{
	iPosition += iStep;
	if(iPosition > iMaximumValue)
		{
		iPosition = iMaximumValue;
		}
	}

//=============================================================================
EXPORT_C void CHorizontalSlider::Decrement()
	{
	iPosition -= iStep;
	if(iPosition < iMinimumValue)
		{
		iPosition = iMinimumValue;
		}
	}

// End of File
