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
* Utils class for helping in drawing.
*
*/


//	CAknTextQueryDialog 
#include <aknquerydialog.h>
#include <aknwaitdialog.h>
#include <eikprogi.h>
#include <bautils.h> 
#include <AknUtils.h>
#include <AknIconUtils.h>
#include <AknInfoPopupNoteController.h>
#include <gulalign.h>

#include <ImageEditorUI.rsg>
#include "ResolutionUtil.h"
#include "ImageEditorUiDefs.h"
#include "DrawUtils.h"
#include "ColorSelectionGrid.h"
#include "colorselectionpopup.h"
#include "CMultiLineQueryDialog.h"

//=============================================================================
EXPORT_C void SDrawUtils::DrawGuide (
    CGraphicsContext &  aGc,
    const TRect &       aRect,
    const TGuideType    aType,
    const TRgb &        aColor
    )
{

    //  Set graphics context parameters
	aGc.SetPenStyle (CGraphicsContext::ESolidPen);
	aGc.SetPenColor (aColor);
    aGc.SetBrushStyle (CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor (aColor);

    switch (aType)
    {
        case EGuideTypeUp:
        {
            TInt colstart = ((aRect.iBr.iX + aRect.iTl.iX) >> 1) + 1;
            TInt colstop = colstart + 1; 
            for (TInt i = aRect.iTl.iY; i <= aRect.iBr.iY; ++i, --colstart, ++colstop)
            {
				aGc.DrawLine (TPoint (colstart, i), TPoint (colstop, i));
            }
            break;
        }
        case EGuideTypeDown:
        {
            TInt colstart = ((aRect.iBr.iX + aRect.iTl.iX) >> 1) + 1;
            TInt colstop = colstart + 1; 
            for (TInt i = aRect.iBr.iY; i >= aRect.iTl.iY; --i, --colstart, ++colstop)
            {
				aGc.DrawLine (TPoint (colstart, i), TPoint (colstop, i));
            }
            break;
        }
        case EGuideTypeLeft:
        {
            TInt rowstart = ((aRect.iBr.iY + aRect.iTl.iY) >> 1) + 1;
            TInt rowstop = rowstart + 1; 
            for (TInt i = aRect.iTl.iX; i <= aRect.iBr.iX; ++i, --rowstart, ++rowstop)
            {
				aGc.DrawLine (TPoint (i, rowstart), TPoint (i, rowstop));
            }
            break;
        }
        case EGuideTypeRight:
        {
            TInt rowstart = ((aRect.iBr.iY + aRect.iTl.iY) >> 1) + 1;
            TInt rowstop = rowstart + 1; 
            for (TInt i = aRect.iBr.iX; i >= aRect.iTl.iX; --i, --rowstart, ++rowstop)
            {
				aGc.DrawLine (TPoint (i, rowstart), TPoint (i, rowstop));
            }
            break;
        }
        default:
        {
            break;
        }

    }
}

//=============================================================================
EXPORT_C void SDrawUtils::DrawBeveledRect (
    CGraphicsContext &  aGc,
    const TRect &       aRect,
    const TRgb &        aColor,
    const TRgb &        aLight,
    const TRgb &        aShadow
    )
{
    //  Set graphics context parameters
	aGc.SetPenStyle (CGraphicsContext::ESolidPen);
	aGc.SetPenColor (aShadow);
    aGc.SetBrushStyle (CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor (aColor);
    
    //  Draw shadow + rectangle area
    aGc.DrawRect (aRect);

	//	Get corners
	TInt ulc = aRect.iTl.iX;
	TInt ulr = aRect.iTl.iY;
	TInt lrc = aRect.iBr.iX;
	TInt lrr = aRect.iBr.iY;

    //  Draw light
	aGc.SetPenColor (aLight);
	aGc.DrawLine ( TPoint(lrc - 1, ulr + 1), TPoint(lrc - 1, lrr));
	aGc.DrawLine ( TPoint(ulc + 1, lrr - 1), TPoint(lrc - 1, lrr - 1));

}

//=============================================================================
EXPORT_C void SDrawUtils::DrawFlatRect (
    CGraphicsContext &  aGc,
    const TRect &       aRect,
    const TRgb &        aColor,
    const TRgb &        aLight,
    const TRgb &        aShadow
    )
{
    //  Set graphics context parameters
	aGc.SetPenStyle (CGraphicsContext::ESolidPen);
	aGc.SetPenColor (aShadow);
    aGc.SetBrushStyle (CGraphicsContext::ESolidBrush);
	aGc.SetBrushColor (aColor);

	//	Get corners
	TInt ulc = aRect.iTl.iX;
	TInt ulr = aRect.iTl.iY;
	TInt lrc = aRect.iBr.iX;
	TInt lrr = aRect.iBr.iY;
    
    //  Draw shadow + rectangle area
    aGc.DrawRect (TRect (ulc, ulr, lrc - 1, lrr - 1));

    //  Draw light
	aGc.SetPenColor (aLight);
	aGc.DrawLine ( TPoint(lrc - 1, ulr), TPoint(lrc - 1, lrr));
	aGc.DrawLine ( TPoint(ulc, lrr - 1), TPoint(lrc, lrr - 1));
	aGc.DrawLine ( TPoint(ulc + 1, ulr + 1), TPoint(lrc - 2, ulr + 1));
	aGc.DrawLine ( TPoint(ulc + 1, ulr + 2), TPoint(ulc + 1, lrr - 2));
}

//=============================================================================
EXPORT_C TInt SDrawUtils::LaunchQueryDialogL (const TDesC & aPrompt)
{
	CAknQueryDialog * dlg = 
		new (ELeave) CAknQueryDialog ( const_cast<TDesC&>(aPrompt) );

	return dlg->ExecuteLD (R_IMAGE_EDITOR_CONFIRMATION_QUERY);
}

//=============================================================================
EXPORT_C TInt SDrawUtils::LaunchQueryDialogOkOnlyL (const TDesC & aPrompt)
{
	CAknQueryDialog * dlg = 
		new (ELeave) CAknQueryDialog ( const_cast<TDesC&>(aPrompt) );

	return dlg->ExecuteLD (R_IMAGE_EDITOR_CONFIRMATION_QUERY_OK_ONLY);
}

//=============================================================================
EXPORT_C TInt SDrawUtils::LaunchTextQueryDialogL (
    TDes &			aData,
	const TDesC &	aPrompt
    )
{
	//	Create a query dialog
    CAknTextQueryDialog * dlg = 
		new (ELeave) CAknTextQueryDialog (aData);

	// Enable predictive input
	dlg->SetPredictiveTextInputPermitted(ETrue);

	//	Execute query dialog
    return dlg->ExecuteLD (R_IMAGE_EDITOR_TEXT_QUERY, aPrompt);
}

//=============================================================================
EXPORT_C TInt SDrawUtils::LaunchMultiLineTextQueryDialogL (
    TDes &			aData,
	const TDesC &	aPrompt
    )
{
	//	Create a query dialog
    CMultiLineQueryDialog* dlg = CMultiLineQueryDialog::NewL( aData );
    
	// Enable predictive input
	dlg->SetPredictiveTextInputPermitted( ETrue );

	//	Execute query dialog
    return dlg->ExecuteLD ( R_IMAGE_EDITOR_TEXT_QUERY, aPrompt );
}

//=============================================================================
EXPORT_C TInt SDrawUtils::LaunchListQueryDialogL (
	MDesCArray *	aTextItems,
	const TDesC &	aPrompt
	)
{
	//	Selected text item index
	TInt index (-1);

	//	Create a new list dialog
    CAknListQueryDialog * dlg = new (ELeave) CAknListQueryDialog (&index);

	//	Prepare list query dialog
	dlg->PrepareLC (R_IMAGE_EDITOR_LIST_QUERY);

	//	Set heading
	dlg->QueryHeading()->SetTextL (aPrompt);

	//	Set text item array
	dlg->SetItemTextArray (aTextItems);	

	//	Set item ownership
	dlg->SetOwnershipType (ELbmDoesNotOwnItemArray);

	//	Execute
	if (dlg->RunLD())
	{
		return index;
	}
	else
	{
		return -1;
	}
}

//=============================================================================
EXPORT_C void SDrawUtils::LaunchWaitNoteL (
    CAknWaitDialog** 			aSelfPtr,
	TInt						aResourceID,
	const TDesC &				aPrompt,
    MProgressDialogCallback*	aCallback
	)
{
    (*aSelfPtr) = new (ELeave) CAknWaitDialog ( (CEikDialog**)aSelfPtr, ETrue);
    (*aSelfPtr)->SetTextL ( aPrompt );
    (*aSelfPtr)->ExecuteLD (aResourceID);
    if (aCallback)
    {
        (*aSelfPtr)->SetCallback (aCallback);
    }
}

//=============================================================================
EXPORT_C void SDrawUtils::LaunchProgressNoteL (
    CAknProgressDialog** 		aSelfPtr,
	TInt						aResourceID,
	const TDesC &				aPrompt,
	TInt						aFinalValue,
    MProgressDialogCallback*	aCallback
	)
{
    (*aSelfPtr) = new (ELeave) CAknWaitDialog ( (CEikDialog**)aSelfPtr, ETrue);
    (*aSelfPtr)->PrepareLC(aResourceID);
    (*aSelfPtr)->GetProgressInfoL()->SetFinalValue (aFinalValue);
    (*aSelfPtr)->SetTextL ( aPrompt );
    if (aCallback)
    {
        (*aSelfPtr)->SetCallback (aCallback);
    }
    (*aSelfPtr)->RunLD();
}

//=============================================================================
EXPORT_C TInt SDrawUtils::LaunchColorSelectionGridL (TRgb & aColor)
{

	CColorSelectionGrid * grid = new (ELeave) CColorSelectionGrid;
	CleanupStack::PushL(grid);

    CAknPopupList * popup_list = 
        CAknPopupList::NewL (grid, R_AVKON_SOFTKEYS_SELECT_BACK__SELECT);
    CleanupStack::PushL (popup_list); 

    grid->ConstructL (popup_list, CEikListBox::ELeftDownInViewRect);
    
	//	Create dialog prompt
	HBufC * prompt = 
		grid->ControlEnv()->AllocReadResourceLC (R_SELECT_COLOUR);

    popup_list->SetTitleL ( prompt->Des() );
    
    CleanupStack::PopAndDestroy(); // prompt

    TInt popup_ok = popup_list->ExecuteLD();
    CleanupStack::Pop();    // popupList
    if (popup_ok)
    {
        //  Get the selected item descriptor from the grid
        TInt index = grid->CurrentDataIndex();
        TPtrC ptr = grid->Model()->MatchableTextArray()->MdcaPoint (index);

        //  Extract the color information from the descriptor
        TInt i = ptr.Locate (TChar('\t'));
        if( i != KErrNotFound )
        {
            TBuf<32> colordes;
            colordes.Copy ( ptr.Mid ( i + 1 ) );
            TLex val (colordes);
            val.SkipSpace();
            TInt color = 0;
            val.Val (color);
            aColor.SetRed (color & 0xFF);
            aColor.SetGreen ((color >> 8) & 0xFF);
            aColor.SetBlue ((color >> 16) & 0xFF);
        }
    }
    CleanupStack::PopAndDestroy();	// grid

    return popup_ok;
}

//=============================================================================
EXPORT_C TInt SDrawUtils::LaunchColorSelectionPopupL (const CFbsBitmap* aPreview, 
                                                            TRect aRect, 
                                                            TRgb & aColor)
    {
    TInt dialog_ok = CColorSelectionDialog::RunDlgLD( aPreview, aRect, aColor); 
    
    return dialog_ok;
    }

//=============================================================================
EXPORT_C TInt SDrawUtils::GetIndicatorBitmapL (
    CFbsBitmap *&	aChBitmap,
    CFbsBitmap *&	aChMask,
	TInt			aBmpIndex,
	TInt			aMaskIndex
    )
{

    // Get the icon file name
    TFileName iconFile (KImageEditorUiMifFile);
    User::LeaveIfError( CompleteWithAppPath(iconFile) );

    // Create bitmap and mask
    AknIconUtils::CreateIconL (
        aChBitmap,
        aChMask,
        iconFile,
        aBmpIndex,
        aMaskIndex
        );

    //  Get the screen mode from the Resolution Util
    TInt screenMode = CResolutionUtil::Self()->GetScreenMode();
    TSize bitmapSize;

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
            bitmapSize = TSize (KStandardCrosshairWidth, KStandardCrosshairHeight);
            break;
        }

        // big screen sizes
        case CResolutionUtil::EQVGA:
        case CResolutionUtil::EQVGAFullScreen:
        case CResolutionUtil::EQVGALandscape:
        case CResolutionUtil::EQVGALandscapeFullScreen:
        {
            bitmapSize = TSize (KQVGACrosshairWidth, KQVGACrosshairHeight);
            break;
        }

        case CResolutionUtil::EDouble:
        case CResolutionUtil::EDoubleFullScreen:
        case CResolutionUtil::EDoubleLandscape:
        case CResolutionUtil::EDoubleLandscapeFullScreen:
        {
            bitmapSize = TSize (KDoubleCrosshairWidth, KDoubleCrosshairHeight);
            break;
        }
        
        // HVGA
        case CResolutionUtil::EHVGA:
        case CResolutionUtil::EHVGAFullScreen:
        case CResolutionUtil::EHVGALandscape:
        case CResolutionUtil::EHVGALandscapeFullScreen:
        {
            bitmapSize = TSize (KHVGACrosshairWidth, KHVGACrosshairHeight);
            break;
        }
        
        // VGA
        case CResolutionUtil::EVGA:
        case CResolutionUtil::EVGAFullScreen:
        case CResolutionUtil::EVGALandscape:
        case CResolutionUtil::EVGALandscapeFullScreen:
        {
            bitmapSize = TSize (KVGACrosshairWidth, KVGACrosshairHeight);
            break;
        }
        
        // QHD
        case CResolutionUtil::EQHD:
        case CResolutionUtil::EQHDFullScreen:
        case CResolutionUtil::EQHDLandscape:
        case CResolutionUtil::EQHDLandscapeFullScreen:
        {
            bitmapSize = TSize (KVGACrosshairWidth, KVGACrosshairHeight);
            break;
        }
        
        default:
        {
            bitmapSize = TSize (KStandardCrosshairWidth, KStandardCrosshairHeight);
            break;
        }
    }
    
    // Set size for scalable icons - MUST BE CALLED BEFORE ICON IS USABLE
    AknIconUtils::SetSize( aChBitmap, bitmapSize );

    return KErrNone;
}

//=============================================================================
EXPORT_C void SDrawUtils::ShowToolTip ( 
                                CAknInfoPopupNoteController* aPopupController,
                                CCoeControl* aCallingControl, 
                                TRect aPositionRect,
                                const TDesC& aText )
    {
    TPoint controlPosition( 0, 0 );
    if ( AknsUtils::GetControlPosition( aCallingControl, 
                                    controlPosition ) != KErrNone )
        {
        controlPosition = aCallingControl->PositionRelativeToScreen();
        }
    
    // add calling control's relative position to rect coordinates    
    TRect positionRect( TPoint( aPositionRect.iTl.iX + controlPosition.iX,
                                aPositionRect.iTl.iY + controlPosition.iY ), 
                                aPositionRect.Size() );        
    if( aPopupController )
        {
        aPopupController->SetTextL( aText );
        aPopupController->SetTimePopupInView( KTimeTooltipInView );  
        aPopupController->SetTooltipModeL( ETrue );
        aPopupController->SetPositionByHighlight( positionRect );
        aPopupController->ShowInfoPopupNote();
        }
    
    }

//=============================================================================
EXPORT_C void SDrawUtils::ShowToolTip ( 
                                CAknInfoPopupNoteController* aPopupController,
                                CCoeControl* aCallingControl, 
                                TPoint aPosition,
                                TGulAlignmentValue aAlignment, 
                                const TDesC& aText )
    {
    TPoint controlPosition( 0, 0 );
    if ( AknsUtils::GetControlPosition( aCallingControl, 
                                    controlPosition ) != KErrNone )
        {
        controlPosition = aCallingControl->PositionRelativeToScreen();
        }
    
    // add calling control's relative position to coordinates     
    TPoint finalPosition = TPoint( aPosition.iX + controlPosition.iX, 
                                   aPosition.iY + controlPosition.iY );
    if( aPopupController )
        {
        aPopupController->SetTextL( aText );
        aPopupController->SetTimePopupInView( KTimeTooltipInView );  
        aPopupController->SetTooltipModeL( ETrue );
        aPopupController->SetPositionAndAlignment( finalPosition, aAlignment );
        aPopupController->ShowInfoPopupNote();
        }    
    }
// End of File
