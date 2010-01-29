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



#include "VerticalSlider.h"
#include "CustomControlPanics.h"
#include <ImageEditorUi.mbg>
#include "ResolutionUtil.h"
#include "ImageEditorUiDefs.h"
#include <fbs.h>
#include <aknutils.h>
#include <AknIconUtils.h>


// CONSTANTS
const TInt KStandardSliderWidth     = 7;
const TInt KStandardSliderHeight    = 132;
const TInt KStandardSliderTabWidth  = 7;
const TInt KStandardSliderTabHeight = 12;

const TInt KDoubleSliderWidth       = 13;
const TInt KDoubleSliderHeight      = 244;
const TInt KDoubleSliderTabWidth    = 13;
const TInt KDoubleSliderTabHeight   = 24;

const TInt KQVGASliderWidth         = 10;
const TInt KQVGASliderHeight        = 188;
const TInt KQVGASliderTabWidth      = 10;
const TInt KQVGASliderTabHeight     = 18;

const TInt KHVGASliderWidth         = 13;
const TInt KHVGASliderHeight        = 244;
const TInt KHVGASliderTabWidth      = 13;
const TInt KHVGASliderTabHeight     = 24;

const TInt KVGASliderWidth          = 20;
const TInt KVGASliderHeight         = 376;
const TInt KVGASliderTabWidth       = 20;
const TInt KVGASliderTabHeight      = 36;

const TInt KQHDSliderWidth          = 13;
const TInt KQHDSliderHeight         = 244;
const TInt KQHDSliderTabWidth       = 13;
const TInt KQHDSliderTabHeight      = 24;

//=============================================================================
EXPORT_C CVerticalSlider* CVerticalSlider::NewL(const TRect& aRect, const CCoeControl& aControl)
    {
    CVerticalSlider* self = CVerticalSlider::NewLC(aRect, aControl);
    CleanupStack::Pop(self);
    return self;
    }

//=============================================================================
EXPORT_C CVerticalSlider* CVerticalSlider::NewLC(const TRect& aRect, const CCoeControl& aControl)
    {
    CVerticalSlider* self = new (ELeave) CVerticalSlider;
    CleanupStack::PushL(self);
    self->ConstructL(aRect, aControl);
    return self;
    }

//=============================================================================
EXPORT_C CVerticalSlider::~CVerticalSlider()
    {
    delete iSliderBg;
	delete iSliderBgMask;
	delete iSliderTab;
	delete iSliderTabMask;
    }

//=============================================================================
void CVerticalSlider::ConstructL(const TRect& /*aRect*/, const CCoeControl& aControl)
    {
	SetContainerWindowL(aControl);
	
    // Load the bitmaps
    LoadBitmapL( iSliderBg, iSliderBgMask, EMbmImageeditoruiQgn_graf_ied_vslider, EMbmImageeditoruiQgn_graf_ied_vslider_mask );
    LoadBitmapL( iSliderTab, iSliderTabMask, EMbmImageeditoruiQgn_graf_ied_vtab, EMbmImageeditoruiQgn_graf_ied_vtab_mask );
    SetBitmapSize();

	ActivateL();
    }

//=============================================================================
CVerticalSlider::CVerticalSlider()
    {
    // no implementation required
    }

//=============================================================================
EXPORT_C TInt CVerticalSlider::CountComponentControls() const
    {
    return 0;
    }

//=============================================================================
void CVerticalSlider::Draw(const TRect& /*aRect*/) const
{
    if ( iMinimumValue <= iMaximumValue )
    {
        TUint height = iMaximumValue - iMinimumValue; // height of the slider
        TUint pixelsFromMin = iPosition - iMinimumValue; // tab position from the beginning
    
        TReal factor = 0.0;
        if (iMinimumValue < iMaximumValue )
        {
            factor = (TReal) pixelsFromMin / height; // tab position from the beginning in percentage
        }
        TUint sliderTabHeight = iSliderTab->SizeInPixels().iHeight; // slider tab height

        // slider bitmap is actually a bit longer but this resolves the problem
        // where the tab is drawn outside of the slider when in maximum position
        TUint sliderBitmapHeight = iSliderBg->SizeInPixels().iHeight - sliderTabHeight;
    
        TUint tabPositionFromMinInPixels = (TUint) (factor * sliderBitmapHeight + 0.5); // calculate tab position

        // top left coordinate
        const TPoint tl = Rect().iTl;

        CWindowGc& gc = SystemGc();

        // draw actual slider using mask bitmap
        TRect bgRect(TPoint(0,0), iSliderBg->SizeInPixels());
        gc.BitBltMasked(tl, iSliderBg, bgRect, iSliderBgMask, ETrue);

        // draw the tab using mask bitmap
        TRect tabRect(TPoint(0,0),iSliderTab->SizeInPixels());
        gc.BitBltMasked(TPoint(tl.iX, tl.iY+tabPositionFromMinInPixels), iSliderTab, tabRect, iSliderTabMask, ETrue);
    }
}

//=============================================================================
EXPORT_C CCoeControl* CVerticalSlider::ComponentControl(TInt /*aIndex*/) const
    {
    return NULL;
    }

//=============================================================================
EXPORT_C TSize CVerticalSlider::MinimumSize()
	{
	TSize size = iSliderBg->SizeInPixels();
	return size;
	}

// setters

//=============================================================================
EXPORT_C void CVerticalSlider::SetMinimum(TInt aValue)
	{
	iMinimumValue = aValue;
	}

//=============================================================================
EXPORT_C void CVerticalSlider::SetMaximum(TInt aValue)
	{
	iMaximumValue = aValue;
	}

//=============================================================================
EXPORT_C void CVerticalSlider::SetStep(TUint aValue)
	{
	iStep = aValue;
	}

//=============================================================================
EXPORT_C void CVerticalSlider::SetStepAmount(TUint8 aValue)
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
EXPORT_C void CVerticalSlider::SetPosition(TInt aValue)
	{
	__ASSERT_ALWAYS( aValue >= iMinimumValue, Panic(EVerticalSliderPanicIndexUnderflow) );
	__ASSERT_ALWAYS( aValue <= iMaximumValue, Panic(EVerticalSliderPanicIndexOverflow) );

	iPosition = aValue;
	}

// getters

//=============================================================================
EXPORT_C TInt CVerticalSlider::Minimum() const
	{
	return iMinimumValue;
	}

//=============================================================================
EXPORT_C TInt CVerticalSlider::Maximum() const
	{
	return iMaximumValue;
	}

//=============================================================================
EXPORT_C TInt CVerticalSlider::Step() const
	{
	return iStep;
	}

//=============================================================================
EXPORT_C TInt CVerticalSlider::Position() const
	{
	return iPosition;
	}

//=============================================================================
EXPORT_C void CVerticalSlider::Increment()
	{
	iPosition += iStep;
	if(iPosition > iMaximumValue)
		{
		iPosition = iMaximumValue;
		}
	}

//=============================================================================
EXPORT_C void CVerticalSlider::Decrement()
	{
	iPosition -= iStep;
	if(iPosition < iMinimumValue)
		{
		iPosition = iMinimumValue;
		}
	}


//=============================================================================
void CVerticalSlider::LoadBitmapL( CFbsBitmap*& aBitmap, CFbsBitmap*& aMask, TInt aBitmapId, TInt aMaskId ) const
{

    TFileName iconFile( KImageEditorUiMifFile );
    User::LeaveIfError( CompleteWithAppPath(iconFile) );

    // Get ids for bitmap and mask
    AknIconUtils::CreateIconL(
        aBitmap,
        aMask,
        iconFile,
        aBitmapId,
        aMaskId
        );


}

//=============================================================================
void CVerticalSlider::SetBitmapSize() const
{

    __ASSERT_ALWAYS( iSliderBg && iSliderTab, Panic(EVerticalSliderPanicBitmapsNotLoaded) );

    //  Get the screen mode from the Resolution Util
    TInt screenMode = CResolutionUtil::Self()->GetScreenMode();

    TSize sliderSize;
    TSize tabSize;

    switch( screenMode )
    {
        // small screen sizes
        case CResolutionUtil::EStandard:
        case CResolutionUtil::EStandardFullScreen:
        case CResolutionUtil::EStandardLandscape:
        case CResolutionUtil::EStandardLandscapeFullScreen:
        case CResolutionUtil::ESquare:
        case CResolutionUtil::ESquareFullScreen:
        case CResolutionUtil::ESquareRotated:
        case CResolutionUtil::ESquareRotatedFullScreen:
        {
            sliderSize = TSize (KStandardSliderWidth, KStandardSliderHeight);
            tabSize = TSize (KStandardSliderTabWidth, KStandardSliderTabHeight);
            break;
        }

        // QVGA
        case CResolutionUtil::EQVGA:
        case CResolutionUtil::EQVGAFullScreen:
        case CResolutionUtil::EQVGALandscape:
        case CResolutionUtil::EQVGALandscapeFullScreen:
        {
            sliderSize = TSize (KQVGASliderWidth, KQVGASliderHeight);
            tabSize = TSize (KQVGASliderTabWidth, KQVGASliderTabHeight);
            break;
        }
       
        case CResolutionUtil::EDouble:
        case CResolutionUtil::EDoubleFullScreen:
        case CResolutionUtil::EDoubleLandscape:
        case CResolutionUtil::EDoubleLandscapeFullScreen:
        {
            sliderSize = TSize (KDoubleSliderWidth, KDoubleSliderHeight);
            tabSize = TSize (KDoubleSliderTabWidth, KDoubleSliderTabHeight);
            break;
        }
        
        // HVGA
        case CResolutionUtil::EHVGA:
        case CResolutionUtil::EHVGAFullScreen:
        case CResolutionUtil::EHVGALandscape:
        case CResolutionUtil::EHVGALandscapeFullScreen:
        {
            sliderSize = TSize (KHVGASliderWidth, KHVGASliderHeight);
            tabSize = TSize (KHVGASliderTabWidth, KHVGASliderTabHeight);
            break;
        }
        
        // VGA
        case CResolutionUtil::EVGA:
        case CResolutionUtil::EVGAFullScreen:
        case CResolutionUtil::EVGALandscape:
        case CResolutionUtil::EVGALandscapeFullScreen:
        {
            sliderSize = TSize (KVGASliderWidth, KVGASliderHeight);
            tabSize = TSize (KVGASliderTabWidth, KVGASliderTabHeight);
            break;
        }
        
        // QHD
        case CResolutionUtil::EQHD:
        case CResolutionUtil::EQHDFullScreen:
        case CResolutionUtil::EQHDLandscape:
        case CResolutionUtil::EQHDLandscapeFullScreen:
        {
            sliderSize = TSize (KQHDSliderWidth, KQHDSliderHeight);
            tabSize = TSize (KQHDSliderTabWidth, KQHDSliderTabHeight);
            break;
        }
        
        default:
        {
            sliderSize = TSize (KStandardSliderWidth, KStandardSliderHeight);
            tabSize = TSize (KStandardSliderTabWidth, KStandardSliderTabHeight);
            break;
        }
    }
    
    // Set size for scalable icons - MUST BE CALLED BEFORE ICON IS USABLE
    AknIconUtils::SetSize( iSliderBg, sliderSize );
    AknIconUtils::SetSize( iSliderTab, tabSize );

}

// End of File
