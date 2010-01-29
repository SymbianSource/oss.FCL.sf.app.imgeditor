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
#include <aknview.h>
#include <aknlists.h> 
#include <barsread.h>
#include <akniconarray.h> 
#include <gulicon.h>
#include <aknutils.h>

#include "ColorSelectionGrid.h"
#include "ResolutionUtil.h"

const TInt KColorGridRows = 8;
const TInt KColorGridCols = 14;

const TInt KStandardGridSizeWidth = 12;
const TInt KStandardGridSizeHeight = 12;
const TInt KQVGAGridSizeWidth = 15;
const TInt KQVGAGridSizeHeight = 15;
const TInt KDoubleGridSizeWidth = 24;
const TInt KDoubleGridSizeHeight = 24;
const TInt KHVGAGridSizeWidth = 20;
const TInt KHVGAGridSizeHeight = 20;
const TInt KVGAGridSizeWidth = 31;
const TInt KVGAGridSizeHeight = 26;
const TInt KQHDGridSizeWidth = 22;
const TInt KQHDGridSizeHeight = 22;

const TInt KStandardIconSizeWidth = 9;
const TInt KStandardIconSizeHeight = 9;
const TInt KQVGAIconSizeWidth = 11;
const TInt KQVGAIconSizeHeight = 11;
const TInt KDoubleIconSizeWidth = 18;
const TInt KDoubleIconSizeHeight = 18;
const TInt KHVGAIconSizeWidth = 15;
const TInt KHVGAIconSizeHeight = 15;
const TInt KVGAIconSizeWidth = 26;
const TInt KVGAIconSizeHeight = 22;
const TInt KQHDIconSizeWidth = 17;
const TInt KQHDIconSizeHeight = 17;



//=============================================================================
EXPORT_C CColorSelectionGrid::CColorSelectionGrid () 
: CAknGrid ()
{

}

//=============================================================================
EXPORT_C CColorSelectionGrid::~CColorSelectionGrid ()
{

}

//=============================================================================
EXPORT_C void CColorSelectionGrid::ConstructL (
    const CCoeControl *     aParent,  
    TInt                    aFlags 
    )
{
    //  Construct base instance
    CAknGrid::ConstructL (aParent, aFlags);

    //  Get the screen mode from the Resolution Util
    TInt screenMode = CResolutionUtil::Self()->GetScreenMode();

    switch (screenMode)
    {

    case CResolutionUtil::EDouble:
    case CResolutionUtil::EDoubleFullScreen:
    case CResolutionUtil::EDoubleLandscape:
    case CResolutionUtil::EDoubleLandscapeFullScreen:
        {
        iGridSize.SetSize(
            KDoubleGridSizeWidth, 
            KDoubleGridSizeHeight);

        iIconSize.SetSize(
            KDoubleIconSizeWidth,
            KDoubleIconSizeHeight);
        break;

        }
    // QVGA
    case CResolutionUtil::EQVGA:
    case CResolutionUtil::EQVGAFullScreen:
    case CResolutionUtil::EQVGALandscape:
    case CResolutionUtil::EQVGALandscapeFullScreen:
        {
        iGridSize.SetSize(
            KQVGAGridSizeWidth, 
            KQVGAGridSizeHeight);

        iIconSize.SetSize(
            KQVGAIconSizeWidth,
            KQVGAIconSizeHeight);

        break;
        }
    // HVGA
    case CResolutionUtil::EHVGA:
    case CResolutionUtil::EHVGAFullScreen:
    case CResolutionUtil::EHVGALandscape:
    case CResolutionUtil::EHVGALandscapeFullScreen:
        {
        iGridSize.SetSize(
            KHVGAGridSizeWidth, 
            KHVGAGridSizeHeight);

        iIconSize.SetSize(
            KHVGAIconSizeWidth,
            KHVGAIconSizeHeight);

        break;
        }
    // VGA    
    case CResolutionUtil::EVGA:
    case CResolutionUtil::EVGAFullScreen:
    case CResolutionUtil::EVGALandscape:
    case CResolutionUtil::EVGALandscapeFullScreen:
        {
        iGridSize.SetSize(
            KVGAGridSizeWidth, 
            KVGAGridSizeHeight);

        iIconSize.SetSize(
            KVGAIconSizeWidth,
            KVGAIconSizeHeight);

        break;
        } 
    // QHD    
    case CResolutionUtil::EQHD:
    case CResolutionUtil::EQHDFullScreen:
    case CResolutionUtil::EQHDLandscape:
    case CResolutionUtil::EQHDLandscapeFullScreen:
        {
        iGridSize.SetSize(
            KQHDGridSizeWidth, 
            KQHDGridSizeHeight);

        iIconSize.SetSize(
            KQHDIconSizeWidth,
            KQHDIconSizeHeight);

        break;
        }       
    default:  
        {
        iGridSize.SetSize(
            KStandardGridSizeWidth, 
            KStandardGridSizeHeight);

        iIconSize.SetSize(
            KStandardIconSizeWidth,
            KStandardIconSizeHeight);

        break;
        }
    }
    //  Set grid layout
    SetLayoutL (
        EFalse, 
        ETrue, 
        ETrue, 
        KColorGridCols, 
        KColorGridRows, 
        iGridSize);
    
    // Set scroll bar invisible
    ScrollBarFrame()->
        SetScrollBarVisibilityL( CEikScrollBarFrame::EOff, 
                                 CEikScrollBarFrame::EOff );  
                                 
    SetPrimaryScrollingType (CAknGridView::EScrollStops);
    SetSecondaryScrollingType (CAknGridView::EScrollStops);
    
    SetCurrentDataIndex (0);
        
	//	Set up the icons
	SetupGridIconsL ();
	
	// Draw the grid
	SizeChanged();
}

//=============================================================================
EXPORT_C void CColorSelectionGrid::SizeChanged()
{
	CAknGrid::SizeChanged();
	SetupGrid();
}

//=============================================================================
void CColorSelectionGrid::SetupGridIconsL ()
{
	//	Create icon array
	CArrayPtr<CGulIcon>* icons = 
		new (ELeave) CAknIconArray (KColorGridRows * KColorGridCols);
	CleanupStack::PushL(icons);

    //  Get reference to grid item text array
    MDesCArray * array = Model()->ItemTextArray();
    CDesCArray * cArray = static_cast<CDesCArray *>(array);

	//	Create icon bitmaps and add them to the array
	TInt index = 0;
	for (TInt i = 0; i < KColorGridRows; ++i)
	{
		for (TInt j = 0; j < KColorGridCols; ++j)
		{
            //  Create grid item bitmap    
            CFbsBitmap * bitmap = new (ELeave) CFbsBitmap;
            CleanupStack::PushL( bitmap );
            User::LeaveIfError (bitmap->Create (iIconSize, EColor64K) );

            //  Compute color for bitmap
            TRgb color (255, 255, 255);
			if (j == 0)
			{
				color = HSLtoRGB (0.0, 0.0, i * (1.0 / 7.0));

			}
			else
			{
				color = HSLtoRGB (i * (360.0 / 8.0), 1.0, j * (1.0 / 14.0));
			}
             
			//	Fill the bitmap with the correct color
			TUint16 color_64k = (TUint16)
				(0 | 
				(((TUint8)color.Red() >> 3) << 11) | 
				(((TUint8)color.Green() >> 2) << 5) | 
				((TUint8)color.Blue() >> 3));

            bitmap->LockHeapLC();
			TUint16 * pd = (TUint16 *)bitmap->DataAddress();
			TSize size = bitmap->SizeInPixels();
			TInt w = (bitmap->ScanLineLength(size.iWidth, EColor64K) >> 1) * size.iHeight;
			for (TInt k = w; k != 0; --k)
			{
				*pd++ = color_64k;
			}
            CleanupStack::PopAndDestroy(); // LockHeapLC()

            //  Add item to the grid
            icons->AppendL ( CGulIcon::NewL (bitmap, 0) );

            CleanupStack::Pop(); // bitmap

            //  Create grid item text
            TBuf<256> buf;
            buf.AppendNum (index);
            buf.Append(KAknStripTabs);
            buf.AppendNum ((TInt)color.Value());
            cArray->AppendL (buf);
			++index; 
		}
	}
	CleanupStack::Pop(icons);	// icons

	//	Set icons to the grid
	ItemDrawer()->FormattedCellData()->SetIconArray(icons);

    //  Handle item addition
    HandleItemAdditionL();
}

//=============================================================================
void CColorSelectionGrid::SetupGrid ()
{
	// Setup text foreground and background colours to default
	AknListBoxLayouts::SetupStandardGrid (*this);

	// Get local copies of data we will need	
	CFormattedCellListBoxItemDrawer * itemDrawer = this->ItemDrawer();
	TInt cell_w = ColumnWidth(); 
	TInt cell_h = ItemHeight();
	
	// Set up graphics sub-cells
	AknListBoxLayouts::SetupFormGfxCell (
		*this,						// the grid
		itemDrawer,					// the grid's drawer
		0,							// index of the graphic within item strings
		0,							// left position
		0,							//	top position
		0,							// right - unused
		0,							// bottom - unused
		cell_w ,					// width of graphic
		cell_h,						// height of graphic
		TPoint (0, 0),				// start position
		TPoint (cell_w, cell_h)		// end position
		);
}

//=============================================================================
TRgb CColorSelectionGrid::HSLtoRGB (
	const TReal aH, 
	const TReal aS, 
	const TReal aL
	) const
{
	TRgb rgb;

	if (aS == 0.0)
	{
		rgb.SetRed ((TInt)(aL * 255.0));
		rgb.SetGreen((TInt)(aL * 255.0));
		rgb.SetBlue((TInt)(aL * 255.0));
	}
	else
	{
		TReal rm1, rm2;
		if (aL <= 0.5)
		{
			rm2 = aL + aL * aS;
		}
		else
		{
			rm2 = aL + aS - aL * aS;
		}
		rm1 = 2.0 * aL - rm2;   
		
		rgb.SetRed (ToRGB (rm1, rm2, aH + 120.0));
		rgb.SetGreen (ToRGB (rm1, rm2, aH));
		rgb.SetBlue (ToRGB (rm1, rm2, aH - 120.0));
	}

	return rgb;
}

//=============================================================================
TInt CColorSelectionGrid::ToRGB (
	TReal aRM1, 
	TReal aRM2, 
	TReal aRH
	) const
{
  if (aRH > 360.0)
  {
	  aRH -= 360.0;
  }
  else if (aRH < 0.0)
  {
	  aRH += 360.0;
  }
 
  if (aRH < 60.0)
  {	
	  aRM1 = aRM1 + (aRM2 - aRM1) * aRH / 60.0;
  }
  else if (aRH < 180.0)
  {
	  aRM1 = aRM2;
  }
  else if (aRH < 240.0) 
  {
	  aRM1 = aRM1 + (aRM2 - aRM1) * (240.0f - aRH) / 60.0;
  }
  return (TInt)(aRM1 * 255.0);
}

// End of File
