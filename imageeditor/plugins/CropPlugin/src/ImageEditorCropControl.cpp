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
* Crop plugin's control class.
*
*/


//  INCLUDES
#include <fbs.h>
#include <badesca.h>
#include <eikenv.h>

#include <ImageEditorUI.mbg>

#include <aknview.h>
#include <aknutils.h>
#include <AknInfoPopupNoteController.h> 
#include <crop.rsg> 
#include <bautils.h> 
#include <ConeResLoader.h> 
#include <StringLoader.h>
#include <AknIconUtils.h>

#ifdef RD_TACTILE_FEEDBACK 
#include <touchfeedback.h>
#endif /* RD_TACTILE_FEEDBACK  */

#include "ImageEditorUI.hrh"
#include "ImageEditorPluginBase.hrh"
#include "PluginInfo.h"
#include "DrawUtils.h"
#include "SystemParameters.h"
#include "JpTimer.h"
#include "ImageEditorUIDefs.h" 
#include "iepb.h"

#include "ImageEditorCropControl.h"
#include "ImageEditorError.h"

// debug log
#include "imageeditordebugutils.h"
_LIT(KCropPluginLogFile,"CropPlugin.log");

// Resource file name
_LIT (KPgnResourceFile, "crop.rsc"); 
// Separator to navi pane text
_LIT( KCropNaviTextSeparator, "x" );

//  CONSTANTS
const float KParamStep              = 0.005F;
const float KMinCropRelDistFrac     = 0.01F;
const float KCursorScalingFactor    = 0.7F;

const TInt  KMaxCropAbsoluteMin     = 75;
const TInt  KMinSourceSize          = 75;

//const TInt  KStatusPaneTextMoveIndex = 1;
const TInt  KStatusPaneTextAreaIndex = 2;

// Default value from CPreviewControlBase not used
const TInt KCropFastKeyTimerDelayInMicroseconds = 1000;

//  Used fixed point resolution for scales
//const TInt KScaleBits = 12;

//=============================================================================
CImageEditorCropControl * CImageEditorCropControl::NewL (
    const TRect& aRect,
    CCoeControl* aParent
    )
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::NewL()");

    CImageEditorCropControl * self = new (ELeave) CImageEditorCropControl;
    CleanupStack::PushL (self);
    self->ConstructL (aRect, aParent);
    CleanupStack::Pop ();   // self
    return self;
}

//=============================================================================
TInt CImageEditorCropControl::DancingAntsCallback (TAny * aPtr)
{
    ((CImageEditorCropControl *)aPtr)->OnDancingAntsCallBack();
    return KErrNone;
}

//=============================================================================
TInt CImageEditorCropControl::FastKeyCallback (TAny * aPtr)
{
    TRAPD( err, ((CImageEditorCropControl *)aPtr)->OnFastKeyCallBackL() );
    return err;
}

//=============================================================================
CImageEditorCropControl::CImageEditorCropControl () :
iState (ECropStateFirst),
iFastKeyTimerState (ETimerInactive),
iHandleEventKeys (ETrue),
iTickCount (0),
iNaviStepMultiplier (KDefaultSmallNavigationStepMultiplier),
iReadyToRender(EFalse)
{
    
}

//=============================================================================
CImageEditorCropControl::~CImageEditorCropControl ()
{
    iEditorView = NULL;
    iItem = NULL;
    iSysPars = NULL;
    
    delete iCrossHair;
    delete iCrossHairMask;
    
    delete iSecondaryCrossHair;
    delete iSecondaryCrossHairMask;
    if (iTimer)
    {
        iTimer->Cancel();
    }
    delete iTimer;

    delete iPopupController;
    delete iTooltipResize;
    delete iTooltipMove;

#ifdef DOUBLE_BUFFERED_CROP    

    delete iBufBitmap;

#endif // DOUBLE_BUFFERED_CROP

}

//=============================================================================
void CImageEditorCropControl::ConstructL (
    const TRect& /*aRect*/,
    CCoeControl* aParent
    )
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::ConstructL()");

    //    Set parent window
    SetContainerWindowL (*aParent);

    // Load MAIN CROSSHAIR image
    SDrawUtils::GetIndicatorBitmapL (
        iCrossHair, 
        iCrossHairMask,
        EMbmImageeditoruiQgn_indi_imed_cursor_super,
        EMbmImageeditoruiQgn_indi_imed_cursor_super_mask        
        );
    
   // Load  SECOND CROSSHAIR image
    SDrawUtils::GetIndicatorBitmapL (
        iSecondaryCrossHair, 
        iSecondaryCrossHairMask, 
        EMbmImageeditoruiQgn_indi_imed_cursor2_super,
        EMbmImageeditoruiQgn_indi_imed_cursor2_super_mask        
        );
    
    TSize chSize = iCrossHair->SizeInPixels();
    TSize chNewSize = TSize( ( chSize.iWidth * KCursorScalingFactor ), 
                              ( chSize.iHeight * KCursorScalingFactor ) );
    // Set secondary cursor size smaller than the main cursor
    AknIconUtils::SetSize( iSecondaryCrossHair, chNewSize );
    AknIconUtils::SetSize( iSecondaryCrossHairMask, chNewSize );
                         
    //  Create and start timer for dancing ants and fast key events
    iTimer = CPeriodic::NewL (CActive::EPriorityStandard);
    StartDancingAntsTimer();

    iPopupController = CAknInfoPopupNoteController::NewL();    
    
    TFileName resourcefile;
    resourcefile.Append(KPgnResourcePath);
    resourcefile.Append(KPgnResourceFile);
    User::LeaveIfError( CompleteWithAppPath( resourcefile ) );

    //    Read tooltip resources  
    //  (RConeResourceLoader selects the language using BaflUtils::NearestLanguageFile)
    RConeResourceLoader resLoader ( *CEikonEnv::Static() );
    CleanupClosePushL ( resLoader );
    resLoader.OpenL ( resourcefile );
    
    iTooltipResize = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_CROP_RESIZE_AREA);    
    iTooltipMove = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_CROP_MOVE_AREA);    
      

#ifdef RD_TACTILE_FEEDBACK 
    iTouchFeedBack = MTouchFeedback::Instance();
#endif /* RD_TACTILE_FEEDBACK  */
    
    CleanupStack::PopAndDestroy(); // resLoader
    
    EnableDragEvents();
       
    //    Activate control
    ActivateL();
}

//=============================================================================
void CImageEditorCropControl::Draw (const TRect & aRect) const
{

#ifndef DOUBLE_BUFFERED_CROP

    CPreviewControlBase::DrawPreviewImage (aRect);

#endif // DOUBLE_BUFFERED_CROP

    if ( iState != ECropStateMin )
    {
        //  Get graphics context
        CWindowGc & gc = SystemGc();

#ifdef DOUBLE_BUFFERED_CROP

        if ( iBufBitmap && iBufBitmap->Handle() )
        {
            gc.BitBlt(TPoint(0,0), iBufBitmap);
        }

#endif // DOUBLE_BUFFERED_CROP

        //  Compute crop rectangle inside the image area
        TRect rect = iSysPars->VisibleImageRectPrev();
        TInt w = (rect.iBr.iX - rect.iTl.iX);
        TInt h = (rect.iBr.iY - rect.iTl.iY);
        TInt ulc = (TInt) (iULC * w + 0.5) + rect.iTl.iX;
        TInt ulr = (TInt) (iULR * h + 0.5) + rect.iTl.iY;
        TInt lrc = (TInt) (iLRC * w + 0.5) + rect.iTl.iX;
        TInt lrr = (TInt) (iLRR * h + 0.5) + rect.iTl.iY;

#ifndef DOUBLE_BUFFERED_CROP

        //    Darken areas in the image that are outside crop rectangle
        gc.SetDrawMode (CGraphicsContext::EDrawModeAND);
        gc.SetPenStyle (CGraphicsContext::ENullPen);
        gc.SetPenColor (KRgbBlack);
        gc.SetBrushStyle (CGraphicsContext::EDiamondCrossHatchBrush);
        gc.SetBrushColor (KRgbWhite);

        //    Darken top
        gc.DrawRect ( TRect ( rect.iTl.iX, 0, rect.iBr.iX, ulr ) );

        //    Darken left side
        gc.DrawRect ( TRect( rect.iTl.iX, ulr, ulc, lrr ) );
        
        //    Darken right side
        gc.DrawRect ( TRect( lrc, ulr, rect.iBr.iX, lrr ) );

        //    Darken bottom
        gc.DrawRect ( TRect( rect.iTl.iX, lrr, rect.iBr.iX, Rect().iBr.iY ) );

#endif // DOUBLE_BUFFERED_CROP

        //  Draw crop rectangle with dancing antz
        DrawDancingAnts (aRect);
        
        DrawCursors( TRect( ulc, ulr, lrc, lrr ) );
                
    }
}

//=============================================================================
void CImageEditorCropControl::DrawCursors (const TRect& aCropRect) const
    {
    CWindowGc & gc = SystemGc();
    
    CFbsBitmap* mainCrossHair;
    CFbsBitmap* mainCrossHairMask;
    if ( iState == ECropStateMove )
        {
        mainCrossHair = iSecondaryCrossHair;
        mainCrossHairMask = iSecondaryCrossHairMask;
        }
    else
        {
        mainCrossHair = iCrossHair;
        mainCrossHairMask = iCrossHairMask;
        }
            
    //  Draw the main cross hair
	if ( mainCrossHair && mainCrossHair )
	    {       
		TInt cx = 0;
		TInt cy = 0;
		if (iState == ECropStateFirst || iState == ECropStateMove)
		    {
			cx = aCropRect.iTl.iX;
			cy = aCropRect.iTl.iY;
	    	}
		else if (iState == ECropStateSecond)
		    {
			cx = aCropRect.iBr.iX - 1;
			cy = aCropRect.iBr.iY - 1;
	    	}
		
		TSize chSize = mainCrossHair->SizeInPixels();
		gc.BitBltMasked (
			TPoint(cx - (chSize.iWidth >> 1), cy - (chSize.iHeight >> 1)), 
			mainCrossHair, 
			TRect (chSize), 
			mainCrossHairMask, 
			EFalse
			);
	    }
	    
    //  Draw the secondary cross hair
    if ( iSecondaryCrossHair && iSecondaryCrossHairMask )
        {
        
        TInt cx = 0;
        TInt cy = 0;
        if (iState == ECropStateFirst || iState == ECropStateMove)
            {
            cx = aCropRect.iBr.iX - 1;  
            cy = aCropRect.iBr.iY - 1;          
            }
        else if (iState == ECropStateSecond)
            {
            cx = aCropRect.iTl.iX;
            cy = aCropRect.iTl.iY;
            }        

        TSize chSize = iSecondaryCrossHair->SizeInPixels();            
        gc.BitBltMasked (
            TPoint(cx - (chSize.iWidth >> 1), cy - (chSize.iHeight >> 1)), 
            iSecondaryCrossHair, 
            TRect (chSize), 
            iSecondaryCrossHairMask, 
            EFalse
            );        
        }
    }
    
//=============================================================================
void CImageEditorCropControl::DrawDancingAnts (const TRect & /*aRect*/) const
{
    //  Get graphics context
    CWindowGc & gc = SystemGc();

    //  Compute crop rectangle inside the image area
    TRect rect = iSysPars->VisibleImageRectPrev();
    TInt w = (rect.iBr.iX - rect.iTl.iX);
    TInt h = (rect.iBr.iY - rect.iTl.iY);
    TInt ulc = (TInt) (iULC * w) + rect.iTl.iX;
    TInt ulr = (TInt) (iULR * h) + rect.iTl.iY;
    TInt lrc = (TInt) (iLRC * w) + rect.iTl.iX;
    TInt lrr = (TInt) (iLRR * h) + rect.iTl.iY;

    //    Crop needed rectangles
    TRect cr (ulc, ulr, lrc, lrr); 

    //  Draw the "dancing ants" rectangle
    gc.SetDrawMode (CGraphicsContext::EDrawModePEN);
    gc.SetBrushStyle (CGraphicsContext::ENullBrush);
    gc.SetBrushColor (KRgbBlack);
    
    //    Draw 
    gc.SetPenStyle (CGraphicsContext::ESolidPen);
    gc.SetPenColor ((iAntFlag) ? (KRgbBlack) : (KRgbWhite));
    gc.DrawRect (cr);
    
    //    Draw dashed line
    gc.SetPenStyle (CGraphicsContext::EDashedPen);
    gc.SetPenColor ((iAntFlag) ? (KRgbWhite) : (KRgbBlack));
    gc.DrawRect (cr);

}


//=============================================================================
void CImageEditorCropControl::SetView (CAknView * aView)
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::SetView()");

    iEditorView = aView;
}

//=============================================================================
void CImageEditorCropControl::SetSelectedUiItemL (CPluginInfo * aItem)
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::SetSelectedUiItemL()");
    iItem = aItem;
}

#ifdef DOUBLE_BUFFERED_CROP

//=============================================================================
void CImageEditorCropControl::SetImageL (CFbsBitmap * aBitmap)
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::SetImageL()");

    //    Create preview bitmap
    iPrevBitmap = aBitmap;
    TSize size = iPrevBitmap->SizeInPixels();
    TDisplayMode dmode = iPrevBitmap->DisplayMode();

    delete iBufBitmap;
    iBufBitmap = 0;
    iBufBitmap = new (ELeave) CFbsBitmap;
    User::LeaveIfError ( iBufBitmap->Create(size, dmode) );

    //    Copy bitmap data
    ClonePreviewBitmapL();
    DarkenUnselectedAreaL();
    DrawNow();    
}

#endif // DOUBLE_BUFFERED_CROP

//=============================================================================
TKeyResponse CImageEditorCropControl::OfferKeyEventL (
    const TKeyEvent &   aKeyEvent,
    TEventCode          aType
    )
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::OfferKeyEventL()");

    TKeyResponse response = EKeyWasNotConsumed;
    
    //  If busy, do not handle anything
    if ( Busy() )
    {
        response = EKeyWasConsumed;
    }

    //  EVENTKEY
    else if (EEventKey == aType && iHandleEventKeys)
    {
    
        switch (aKeyEvent.iCode)
        {

            case EKeyDownArrow:
            {
                //  Adjust crop point
                NaviDownL();
                iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
                DrawNow();
                response = EKeyWasConsumed;
                iPopupController->HideInfoPopupNote();
                break;
            }

            case EKeyUpArrow:
            {
                //  Adjust crop point
                NaviUpL();
                iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
                DrawNow();
                response = EKeyWasConsumed;
                iPopupController->HideInfoPopupNote();
                break;
            }

            case EKeyRightArrow:
            {
                //  Adjust crop point
                NaviRightL();
                iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
                DrawNow();
                response = EKeyWasConsumed;
                iPopupController->HideInfoPopupNote();
                break;
            }

            case EKeyLeftArrow:
            {
                //  Adjust crop point
                NaviLeftL();
                iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
                DrawNow();
                response = EKeyWasConsumed;
                iPopupController->HideInfoPopupNote();
                break;
            }

            case EKeyOK:
            case EKeyEnter:
            {
                StartDancingAntsTimer();

                if ( iIsCropModeManual )
                {
                    
                    if (iState == ECropStateFirst)
                    {
                        iState = ECropStateSecond;
                        iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
                        iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
                        DrawNow();
                    }
                    else if (iState == ECropStateSecond)
                    {
                        iState = ECropStateMove;
                        iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
                        iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
                        DrawNow();
                    }
                    else
                    {
                        iState = ECropStateMin;
                        iReadyToRender = ETrue;
                        iEditorView->HandleCommandL (EImageEditorResetZoom);
                        
                        if ( iULC == 0.00 && iULR == 0.00 && iLRC == 1.00 && iLRR == 1.00)
                        {
                            iEditorView->HandleCommandL (EImageEditorCancelPlugin);    
                        }
                        else
                        {
                            iEditorView->HandleCommandL (EImageEditorApplyPlugin);
                        }
                        
                    }
                    response = EKeyWasConsumed;
                }
                else
                {
                    if (iState == ECropStateFirst)
                    {
                        iState = ECropStateSecond;
                        iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
                        iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
                        DrawNow();
                    }
                    else if (iState == ECropStateSecond)
                    {
                        iState = ECropStateMove;
                        iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
                        iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
                        DrawNow();
                    }
                    else
                    {
                        ComputeCropParams();
                        iState = ECropStateMin;
                        iReadyToRender = ETrue;
                        iEditorView->HandleCommandL (EImageEditorResetZoom);
                        if ( iULC == 0.00 && iULR == 0.00 && iLRC == 1.00 && iLRR == 1.00)
                        {
                            iEditorView->HandleCommandL (EImageEditorCancelPlugin);    
                        }
                        else
                        {
                            iEditorView->HandleCommandL (EImageEditorApplyPlugin);
                        }

                    }
                    response = EKeyWasConsumed;
                }
                break;
            }

            case 0x2a: // *
            case EStdKeyIncVolume: // zoom in key
            {
            
                //    Check for minimum crop size            
                TRect virect = iSysPars->VisibleImageRect();
                TInt width = (TInt)((iLRC - iULC) * (virect.iBr.iX - virect.iTl.iX) + 0.5F);
                width = (TInt)(width * iSysPars->Scale() + 0.5);
                TInt height = (TInt)((iLRR - iULR) * (virect.iBr.iY - virect.iTl.iY) + 0.5F);
                height = (TInt)(height * iSysPars->Scale() + 0.5);

                TReal relscale = iSysPars->RelScale();
                TInt minCrop = (TInt)(KMaxCropAbsoluteMin * relscale + 0.5);
                
                if ( (width <= minCrop) || (height <= minCrop) )
                {
                    response = EKeyWasConsumed;
                }
                
                //    Store crop rectangle relative to screen
                StoreCropRelScreen();
                break;
            }

            case 0x23: // #
            case EStdKeyDecVolume: // zoom out key
            {
                //    Store crop rectangle relative to screen
                StoreCropRelScreen();
                break;
            }
            
            // Consume rotation keys to disable rotation
#ifndef LANDSCAPE_ROTATE_HOTKEYS        
            case 0x31: // 1
#else
            case 0x33: // 3            
#endif
#ifndef LANDSCAPE_ROTATE_HOTKEYS                        
            case 0x33: // 3
#else            
            case 0x39: // 9
#endif            
            {
                response = EKeyWasConsumed;
                break;
            } 

            default:
            {
                break;
            }
        }

        // If the device is really slow, it may happen that the timer
        // does not get execution time and slows down or halts completely.
        // (Ideally, this should not happen.)
        // This block is just to make sure that the cursor keeps moving
        // even if the timer halts.
        if (ETimerStarted == iFastKeyTimerState)
        {
            if (iTickCount > KDefaultFastKeyTimerMultiplyThresholdInTicks)
            {
                iNaviStepMultiplier = KDefaultBigNavigationStepMultiplier;
            }
            else
            {
                iTickCount++;
            }
        }
    }

    else if (EEventKey == aType && !iHandleEventKeys)
    {
        response = EKeyWasConsumed;
    }

    //  EVENTKEYDOWN
    else if (EEventKeyDown == aType)
    {
            
        switch (aKeyEvent.iScanCode)
        {

            case EStdKeyLeftArrow:
            case EStdKeyRightArrow:
            case EStdKeyUpArrow:
            case EStdKeyDownArrow:
            {
                iPressedKeyScanCode = aKeyEvent.iScanCode;
                StartFastKeyTimer();
                response = EKeyWasConsumed;
                break;
            }
            default:
                break;
        }
    }

    //  EVENTKEYUP
    else if (EEventKeyUp == aType)
    {
        if (aKeyEvent.iScanCode == iPressedKeyScanCode)
        {
            iHandleEventKeys = ETrue;
            iFastKeyTimerState = ETimerInactive;
            StartDancingAntsTimer();
            response = EKeyWasConsumed;
            ShowTooltip();
        }
    }

    return response;
}

//=============================================================================
void CImageEditorCropControl::SizeChanged()
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::SizeChanged()");

}

//=============================================================================
TDesC & CImageEditorCropControl::GetParam ()
{     
    LOG(KCropPluginLogFile, "CImageEditorCropControl::GetParam()");

    // Do not set crop paramater before valid area is selected,
    // otherwise preview image is cropped when zooming or panning
    if (iReadyToRender)
    {
        iParam.Copy (_L("x1 "));
        iParam.AppendNum (iCropX);
        iParam.Append (_L(" y1 "));
        iParam.AppendNum (iCropY);
        iParam.Append (_L(" x2 "));
        iParam.AppendNum (iCropX + iCropW);
        iParam.Append (_L(" y2 "));
        iParam.AppendNum (iCropY + iCropH);
    }
    return iParam;
}

//=============================================================================
void CImageEditorCropControl::SetSystemParameters ( const CSystemParameters * aPars) 
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::SetSystemParameters()");
    iSysPars = aPars;
}
 
//=============================================================================
void CImageEditorCropControl::SetCropModeL (
    const TInt      aMode,
    const float     aRatio
    )
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::SetCropModeL()");

    //  Store crop mode
    iIsCropModeManual = (aMode == 0);

    //  Store wanted aspect ratio
    iCropRatio = aRatio;
         
    //    Set initial points according to the cropping mode (aspect ratio)
    SetInitialPointsL ();
    // Shows the first tooltip when the plugin is entered
    ShowTooltip();
}

//=============================================================================
void CImageEditorCropControl::HandlePluginCommandL (const TInt aCommand)
{
    LOGFMT(KCropPluginLogFile, "CImageEditorCropControl::HandlePluginCommandL(), aCommand = %d", aCommand);

    switch (aCommand) 
    {

        //    Control focus gained, start dancing ants timer
        case EImageEditorFocusGained:
        {
            StartDancingAntsTimer();
            break;
        }

        //    Control focus lost, stop dancing ants timer
        case EImageEditorFocusLost:
        {
            if (iTimer)
            {
                iTimer->Cancel();
            }
            break;
        }

        //    Visible image area changes => update crop parameters and
        //    screen buffer
        case EImageEditorGlobalZoomChanged:
        case EImageEditorGlobalPanChanged:
        case EImageEditorGlobalRotationChanged:
        case EImageEditorPostScreenModeChange:
        {
        
            if ( aCommand == EImageEditorGlobalZoomChanged )
            {
                RestoreCropRelScreen();
                UpdateCropRectangle();
            }
            else if ( aCommand == EImageEditorPostScreenModeChange )
            {
                RestoreCropRelImage();
                UpdateCropRectangle();
            }

#ifdef DOUBLE_BUFFERED_CROP

            ClonePreviewBitmapL();
            DarkenUnselectedAreaL();
            DrawNow();

#endif // DOUBLE_BUFFERED_CROP

            iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
            DrawNow();
            break;
        }

        //    Set softkey
        case EPgnSoftkeyIdSet:
        {
            if (iState == ECropStateFirst)
            {
                iState = ECropStateSecond;
            }
            else
            {
                iState = ECropStateMove;
            }
            iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
            iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
            DrawNow();
            ShowTooltip();
            break;
        }

        //    Done softkey
        case EPgnSoftkeyIdDone:
        {
            ComputeCropParams();
            iState = ECropStateMin;
            iReadyToRender = ETrue;
            iEditorView->HandleCommandL (EImageEditorResetZoom);
            if ( iULC == 0.00 && iULR == 0.00 && iLRC == 1.00 && iLRR == 1.00)
            {
                iEditorView->HandleCommandL (EImageEditorCancelPlugin);    
            }
            else
            {
                iEditorView->HandleCommandL (EImageEditorApplyPlugin);
            }

            break;
        }

        //    Cancel softkey
        case EPgnSoftkeyIdCancel:
        {
            iEditorView->HandleCommandL (EImageEditorResetZoom);
            iEditorView->HandleCommandL (EImageEditorCancelPlugin);
            break;
        }

        //    Back softkey
        case EPgnSoftkeyIdBack:
        {
            if (iState == ECropStateSecond)
            {
                iState = ECropStateFirst;
            }
            else
            {
                iState = ECropStateSecond;
            }
            iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
            iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
            DrawNow();
            ShowTooltip();
            break;
        }

        //    Screen mode changed (portrait, portrait full, landscape, landscape full)
        case EImageEditorPreScreenModeChange:
        {
            //    Store crop rectangle relative to image
            StoreCropRelImage();
            break;
        }

        default:
        {
            break;
        }
    }
}

//=============================================================================
TInt CImageEditorCropControl::GetSoftkeyIndexL()
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::GetSoftkeyIndexL()");

    switch (iState) 
    {
        case ECropStateFirst:
        {
            return 0;
        }
        case ECropStateSecond:
        {            
            return 2;            
        }
        case ECropStateMove:
        {
            return 1;
        }
        case ECropStateMinCrop:
        {
            return 3; // empty - cancel    
        }
        default:
        {
            return -1;
        }
    }
}

//=============================================================================
TPtrC CImageEditorCropControl::GetNaviPaneTextL (
    TBool& aLeftNaviPaneScrollButtonVisibile, 
    TBool& aRightNaviPaneScrollButtonVisible )
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::GetNaviPaneTextL()");

    aLeftNaviPaneScrollButtonVisibile = EFalse;
    aRightNaviPaneScrollButtonVisible = EFalse;

    
    // Update parameters - needed for the zoom update
    ComputeCropParams();

    // Getting string from resources
    TPtrC ptr = iItem->Parameters()[KStatusPaneTextAreaIndex]; 

    //  Compute scaled width and height
    TReal width = iSysPars->Scale() * iCropW;
    TReal height = iSysPars->Scale() * iCropH;

    // Convert the width and height relative to the 
    TReal relscale = iSysPars->RelScale();
    TInt scaledWidth = (TInt)((width / relscale) + 0.5);
    TInt scaledHeight = (TInt)((height / relscale) + 0.5);
    
    iNaviPaneText.Zero();
    
    // Generate string to be added to %U
    TBuf< 20 > valueStr;
    
    valueStr.AppendNum( scaledWidth );
    valueStr.Append( KCropNaviTextSeparator );
    valueStr.AppendNum( scaledHeight );
        AknTextUtils::LanguageSpecificNumberConversion  (  valueStr );
    
    StringLoader::Format( iNaviPaneText, ptr, -1, valueStr );

    CalculateMinCrop();
    return iNaviPaneText;
}

//=============================================================================
void CImageEditorCropControl::ComputeCropParams()
{

    LOG(KCropPluginLogFile, "CImageEditorCropControl::ComputeCropParams()");

    TRect virect = iSysPars->VisibleImageRect();

    TInt w = (virect.iBr.iX - virect.iTl.iX);
    TInt h = (virect.iBr.iY - virect.iTl.iY);

    TInt ulc = (TInt)(iULC * w + 0.5);
    TInt ulr = (TInt)(iULR * h + 0.5);
    TInt lrc = (TInt)(iLRC * w + 0.5);
    TInt lrr = (TInt)(iLRR * h + 0.5);

    //    Set parameter struct
    w = lrc - ulc;
    h = lrr - ulr;
    iCropX = iSysPars->VisibleImageRect().iTl.iX + ulc;
    iCropY = iSysPars->VisibleImageRect().iTl.iY + ulr;
    iCropW = w;
    if (iCropX + w > virect.iBr.iX)
    {
        iCropW = virect.iBr.iX - iCropX;
    }
       iCropH = h;
    if (iCropY  + h > virect.iBr.iY)
    {
        iCropH = virect.iBr.iY - iCropY;
    }
}

//=============================================================================
void CImageEditorCropControl::NaviDownL()
{
    
    LOG(KCropPluginLogFile, "CImageEditorCropControl::NaviDownL()");

    //  MANUAL MODE
    if (iIsCropModeManual)
    {

        // FIRST: UPPER LEFT CORNER
        if (iState == ECropStateFirst)
        {
            iULR += (KParamStep * iNaviStepMultiplier);
            if (iULR > iLRR - iMinY)
            {
                iULR = iLRR - iMinY;
            }
        }

        // SECOND: LOWER RIGHT CORNER
        else if (iState == ECropStateSecond)
        {
            iLRR += (KParamStep * iNaviStepMultiplier);
            if (iLRR > 1.0)
            {
                iLRR = 1.0;
            }
        }
        // MOVE (ADDED FOR MANUAL)
        else 
        {
            float old = iLRR;
            iLRR += (KParamStep * iNaviStepMultiplier);
            if (iLRR > 1.0)
            {
                iLRR = 1.0;
            }
            iULR += (iLRR - old);
        }
    }

    //  ASPECT RATIO PRESERVING MODE
    else
    {

        // FIRST: UPPER LEFT CORNER
        if (iState == ECropStateFirst)
        {
            if ( ((iLRC - iULC) > iMinX) && ((iLRR - iULR) > iMinY) )
            {
                iULR += (KParamStep * iNaviStepMultiplier);
                if (iULR > iLRR - iMinY)
                {
                    iULR = iLRR - iMinY;
                }
                ComputePreservedULC();
                if (iULC > iLRC - iMinX)
                {
                    iULC = iLRC - iMinX;
                }
            }
        }

        // SECOND: LOWER RIGHT CORNER
        else if (iState == ECropStateSecond)
        {
            iLRR += (KParamStep * iNaviStepMultiplier);
            if (iLRR > 1.0)
            {
                iLRR = 1.0;
            }
            ComputePreservedLRC();
            if (iLRC > 1.0)
            {
                iLRC = 1.0;
                ComputePreservedLRR();
            }
        }

        // MOVE (ONLY WITH ASPECT RATIO PRESERVING)
        // NOWADAYS ALSO WITH MANUAL
        else 
        {
            float old = iLRR;
            iLRR += (KParamStep * iNaviStepMultiplier);
            if (iLRR > 1.0)
            {
                iLRR = 1.0;
            }
            iULR += (iLRR - old);
        }
    }
    ComputeCropParams();

#ifdef DOUBLE_BUFFERED_CROP

    if (iState == ECropStateSecond || iState == ECropStateMove)
    {
        ClonePreviewBitmapL();    
    }

    DarkenUnselectedAreaL();
    DrawNow();

#endif // DOUBLE_BUFFERED_CROP

}

//=============================================================================
void CImageEditorCropControl::NaviUpL()
{

    LOG(KCropPluginLogFile, "CImageEditorCropControl::NaviUpL()");

    //  MANUAL MODE
    if (iIsCropModeManual)
    {

        // FIRST: UPPER LEFT CORNER
        if (iState == ECropStateFirst)
        {
            iULR -= (KParamStep * iNaviStepMultiplier);
            if (iULR < 0.0)
            {
                iULR = 0.0;
            }
        }

        // SECOND: LOWER RIGHT CORNER
        else if (iState == ECropStateSecond)
        {
            iLRR -= (KParamStep * iNaviStepMultiplier);
            if (iLRR < iULR + iMinY)
            {
                iLRR = iULR + iMinY;
            }
        }  
        // MOVE (ADDED FOR MANUAL)
        else 
        {
            float old = iULR;
            iULR -= (KParamStep * iNaviStepMultiplier);
            if (iULR < 0.0)
            {
                iULR = 0.0;
            }
            iLRR -= (old - iULR);
        } 
    }
    
    //  ASPECT RATIO PRESERVING MODE
    else
    {

        // FIRST: UPPER LEFT CORNER
        if (iState == ECropStateFirst)
        {
            iULR -= (KParamStep * iNaviStepMultiplier);
            if (iULR < 0.0)
            {
                iULR = 0.0;
            }
            ComputePreservedULC();
            if (iULC < 0.0)
            {
                iULC = 0.0;
                ComputePreservedULR();
            }
        }

        // SECOND: LOWER RIGHT CORNER
        else if (iState == ECropStateSecond)
        {
            if ( ((iLRC - iULC) > iMinX) && ((iLRR - iULR) > iMinY) )
            {
                iLRR -= (KParamStep * iNaviStepMultiplier);
                if ((iLRR - iULR) < iMinY)
                {
                    iLRR = iULR + iMinY;
                }
                ComputePreservedLRC();
                if ((iLRC - iULC) < iMinX)
                {
                    iLRC = iULC + iMinX;
                }
            }
        }

        // MOVE (ONLY WITH ASPECT RATIO PRESERVING)
        // NOWADAYS ALSO WITH MANUAL
        else 
        {
            float old = iULR;
            iULR -= (KParamStep * iNaviStepMultiplier);
            if (iULR < 0.0)
            {
                iULR = 0.0;
            }
            iLRR -= (old - iULR);
        }
    }
    ComputeCropParams();

#ifdef DOUBLE_BUFFERED_CROP

    if (iState == ECropStateFirst || iState == ECropStateMove)
    {
        ClonePreviewBitmapL();    
    }

    DarkenUnselectedAreaL();
    DrawNow();

#endif // DOUBLE_BUFFERED_CROP

}

//=============================================================================
void CImageEditorCropControl::NaviRightL()
{

    LOG(KCropPluginLogFile, "CImageEditorCropControl::NaviRightL()");

    //  MANUAL MODE
    if (iIsCropModeManual)
    {

        // FIRST: UPPER LEFT CORNER
        if (iState == ECropStateFirst)
        {
            iULC += (KParamStep * iNaviStepMultiplier);
            if (iULC > iLRC - iMinX)
            {
                iULC = iLRC - iMinX;
            }
        }

        // SECOND: LOWER RIGHT CORNER
        else if (iState == ECropStateSecond)
        {
            iLRC += (KParamStep * iNaviStepMultiplier);
            if (iLRC > 1.0)
            {
                iLRC = 1.0;
            }
        }
        // MOVE (ADDED FOR MANUAL)
        else if (iState == ECropStateMove)
        {
            float old = iLRC;
            iLRC += (KParamStep * iNaviStepMultiplier);
            if (iLRC > 1.0)
            {
                iLRC = 1.0;
            }
            iULC += (iLRC - old);
        }
    }

    //  ASPECT RATIO PRESERVING MODE
    else
    {

        // MOVE (ONLY WITH ASPECT RATIO PRESERVING)
        // NOWADAYS ALSO WITH MANUAL
        if (iState == ECropStateMove)
        {
            float old = iLRC;
            iLRC += (KParamStep * iNaviStepMultiplier);
            if (iLRC > 1.0)
            {
                iLRC = 1.0;
            }
            iULC += (iLRC - old);
        }
    }
    ComputeCropParams();

#ifdef DOUBLE_BUFFERED_CROP

    if (iState == ECropStateSecond || iState == ECropStateMove)
    {
        ClonePreviewBitmapL();    
    }

    DarkenUnselectedAreaL();
    DrawNow();

#endif // DOUBLE_BUFFERED_CROP

}

//=============================================================================
void CImageEditorCropControl::NaviLeftL()
{

    LOG(KCropPluginLogFile, "CImageEditorCropControl::NaviLeftL()");

    //  MANUAL MODE
    if (iIsCropModeManual)
    {

        // FIRST: UPPER LEFT CORNER
        if (iState == ECropStateFirst)
        {
            iULC -= (KParamStep * iNaviStepMultiplier);
            if (iULC < 0.0)
            {
                iULC = 0.0;
            }
        }

        // SECOND: LOWER RIGHT CORNER
        else if (iState == ECropStateSecond)
        {
            iLRC -= (KParamStep * iNaviStepMultiplier);
            if (iLRC < iULC + iMinX)
            {
                iLRC = iULC + iMinX;
            }
        }
        // MOVE (ADDED FOR MANUAL)
        else if (iState == ECropStateMove)
        {
            float old = iULC;
            iULC -= (KParamStep * iNaviStepMultiplier);
            if (iULC < 0.0)
            {
                iULC = 0.0;
            }
            iLRC -= (old - iULC);
        }
    }

    //  ASPECT RATIO PRESERVING MODE
    else
    {

        // MOVE (ONLY WITH ASPECT RATIO PRESERVING)
        // NOWADAYS ALSO WITH MANUAL
        if (iState == ECropStateMove)
        {
            float old = iULC;
            iULC -= (KParamStep * iNaviStepMultiplier);
            if (iULC < 0.0)
            {
                iULC = 0.0;
            }
            iLRC -= (old - iULC);
        }
    }
    ComputeCropParams();

#ifdef DOUBLE_BUFFERED_CROP

    if (iState == ECropStateFirst || iState == ECropStateMove)
    {
        ClonePreviewBitmapL();    
    }

    DarkenUnselectedAreaL();
    DrawNow();

#endif // DOUBLE_BUFFERED_CROP

}

//=============================================================================
void CImageEditorCropControl::SetInitialPointsL ()
{

    LOG(KCropPluginLogFile, "CImageEditorCropControl::SetInitialPointsL()");

    const TSize vps = iSysPars->ViewPortRect().Size();
    if (vps.iWidth <= KMinSourceSize || vps.iHeight <= KMinSourceSize)
    {
          iULC = 0.00F;
        iULR = 0.00F;
        iLRC = 1.00F;
        iLRR = 1.00F;
        iState = ECropStateMinCrop;
    }
  
    //  MANUAL
    else if (iIsCropModeManual)
    {
        iULC = 0.02F;
        iULR = 0.02F;
        iLRC = 0.98F;
        iLRR = 0.98F;
    }

    //  ASPECT RATIO PRESERVED
    else
    {

        TRect virect = iSysPars->VisibleImageRect();
        TInt width = (virect.iBr.iX - virect.iTl.iX);
        TInt height = (virect.iBr.iY - virect.iTl.iY);
        
        float current = (float)width / (float)height;
    
        if (iCropRatio > current)
        {
            float tmp = 0.5F * (current / iCropRatio);
            iULC = 0.0F;
            iULR = 0.5F - tmp;
            iLRC = 1.0F;
            iLRR = 0.5F + tmp;
        }
        else
        {
            float tmp = 0.5F * (iCropRatio / current);
            iULC = 0.5F - tmp;
            iULR = 0.0F;
            iLRC = 0.5F + tmp;
            iLRR = 1.0F;
        }
    }

    ComputeCropParams();
    StoreCropRelScreen();

#ifdef DOUBLE_BUFFERED_CROP

    DarkenUnselectedAreaL();
    DrawNow();

#endif // DOUBLE_BUFFERED_CROP

}

//=============================================================================
void CImageEditorCropControl::StartDancingAntsTimer()
{

    LOG(KCropPluginLogFile, "CImageEditorCropControl::StartDancingAntsTimer()");

    if (iTimer)
    {
        iTimer->Cancel();
        iTimer->Start(
            TTimeIntervalMicroSeconds32 (KDancingAntzTimerDelayInMicroseconds),
            TTimeIntervalMicroSeconds32 (KDancingAntzTimerIntervalInMicroseconds),
            TCallBack (DancingAntsCallback, this)
            );
    }
}

//=============================================================================
void CImageEditorCropControl::StartFastKeyTimer()
{

    LOG(KCropPluginLogFile, "CImageEditorCropControl::StartFastKeyTimer()");

    iNaviStepMultiplier = KDefaultSmallNavigationStepMultiplier;
    iTickCount = 0;
    iFastKeyTimerState = ETimerStarted;

    if (iTimer)
    {
        iTimer->Cancel();
        iTimer->Start(
            TTimeIntervalMicroSeconds32 (KCropFastKeyTimerDelayInMicroseconds),
            TTimeIntervalMicroSeconds32 (KDefaultFastKeyTimerIntervalInMicroseconds),
            TCallBack (FastKeyCallback, this)
            );
    }
}

//=============================================================================
void CImageEditorCropControl::OnDancingAntsCallBack()
{
    iAntFlag = !iAntFlag;
    ActivateGc();
    DrawDancingAnts (Rect());
    DeactivateGc();
}

//=============================================================================
void CImageEditorCropControl::OnFastKeyCallBackL()
{
    if (iTickCount > KDefaultFastKeyTimerMultiplyThresholdInTicks)
    {
        iNaviStepMultiplier = KDefaultBigNavigationStepMultiplier;
    }
    else
    {
        iTickCount++;
    }

    // If first time here, reset the tick counter.
    if (ETimerStarted == iFastKeyTimerState)
    {
        iFastKeyTimerState = ETimerRunning;
        iTickCount = 0;
    }

    iHandleEventKeys = EFalse;

    switch (iPressedKeyScanCode)
    {
        case EStdKeyDownArrow:
        {
			NaviDownL();
            break;
        }
        case EStdKeyUpArrow:
        {
			NaviUpL();
            break;
        }
        case EStdKeyLeftArrow:
        {
			NaviLeftL();
            break;
        }
        case EStdKeyRightArrow:
        {
            NaviRightL();
            break;
        }
        default:
            break;
    }
    iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
    DrawNow();
}

//=============================================================================
TBool CImageEditorCropControl::IsReadyToRender() const
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::IsReadyToRender()");

    // Always ready to render. However selected crop are is not set in GetParam()¨
    // until closing the plug-in.
    return ETrue;
}

//=============================================================================
void CImageEditorCropControl::CalculateMinCrop() 
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::CalculateMinCrop()");
    
    TInt im_width = (iSysPars->VisibleImageRect().iBr.iX - iSysPars->VisibleImageRect().iTl.iX);
    im_width = (TInt)(im_width * iSysPars->Scale() + 0.5);
    TInt im_height = (iSysPars->VisibleImageRect().iBr.iY - iSysPars->VisibleImageRect().iTl.iY);
    im_height = (TInt)(im_height * iSysPars->Scale() + 0.5);
    TInt im_maxdim = (im_width < im_height) ? (im_height) : (im_width); 

    TInt im_min_crop_rel = (TInt)(KMinCropRelDistFrac * im_maxdim + 0.5F);
    TInt im_min_crop_abs = KMaxCropAbsoluteMin;
    TInt im_maxcrop = (im_min_crop_rel > im_min_crop_abs) ? 
        (im_min_crop_rel) : (im_min_crop_abs);
    
    TReal relscale = iSysPars->RelScale();
	TInt image_maxcrop_prev = (TInt)(im_maxcrop * relscale + 0.5);
    
    iMinX = (float)image_maxcrop_prev / (float)im_width;
    iMinY = (float)image_maxcrop_prev / (float)im_height;
}

//=============================================================================
void CImageEditorCropControl::ComputePreservedULC() 
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::ComputePreservedULC()");

    TRect virect = iSysPars->VisibleImageRect();
    TInt viwidth = (virect.iBr.iX - virect.iTl.iX);
    TInt viheight = (virect.iBr.iY - virect.iTl.iY);
    iULC = (iLRC * viwidth - iCropRatio * ((iLRR - iULR) * viheight)) / viwidth;
}

//=============================================================================
void CImageEditorCropControl::ComputePreservedLRC() 
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::ComputePreservedLRC()");

    TRect virect = iSysPars->VisibleImageRect();
    TInt viwidth = (virect.iBr.iX - virect.iTl.iX);
    TInt viheight =(virect.iBr.iY - virect.iTl.iY);
    iLRC = (iULC * viwidth + iCropRatio * ((iLRR - iULR) * viheight)) / viwidth;
}

//=============================================================================
void CImageEditorCropControl::ComputePreservedULR()
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::ComputePreservedULR()");

    TRect virect = iSysPars->VisibleImageRect();
    TInt viwidth = (virect.iBr.iX - virect.iTl.iX);
    TInt viheight =(virect.iBr.iY - virect.iTl.iY);
    iULR = (iLRR * viheight - (((iLRC - iULC) * viwidth) / iCropRatio)) / viheight;
}

//=============================================================================
void CImageEditorCropControl::ComputePreservedLRR()
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::ComputePreservedLRR()");

    TRect virect = iSysPars->VisibleImageRect();
    TInt viwidth = (virect.iBr.iX - virect.iTl.iX);
    TInt viheight =(virect.iBr.iY - virect.iTl.iY);
    iLRR = (iULR * viheight + (((iLRC - iULC) * viwidth) / iCropRatio)) / viheight;
}

#ifdef DOUBLE_BUFFERED_CROP

//=============================================================================
void CImageEditorCropControl::ClonePreviewBitmapL() 
{

    LOG(KCropPluginLogFile, "CImageEditorCropControl::ClonePreviewBitmapL()");

    TBitmapUtil bmptls (iPrevBitmap);
    bmptls.Begin(TPoint(0,0));

    TSize size = iPrevBitmap->SizeInPixels();
    TDisplayMode dmode = iPrevBitmap->DisplayMode();

    TInt bufsize = size.iHeight * iPrevBitmap->ScanLineLength (size.iWidth, dmode);
    TUint8 * ps = (TUint8*)( iPrevBitmap->DataAddress() ); 
    TUint8 * pd = (TUint8*)( iBufBitmap->DataAddress() ); 
    Mem::Copy (pd, ps, bufsize);

    bmptls.End();
}

//=============================================================================
void CImageEditorCropControl::DarkenUnselectedAreaL() 
{

    LOG(KCropPluginLogFile, "CImageEditorCropControl::DarkenUnselectedAreaL()");
    
    if (iBufBitmap)  
    {
        //  Compute crop rectangle inside the image area
        TRect rect = iSysPars->VisibleImageRectPrev();
        TInt w = (rect.iBr.iX - rect.iTl.iX);
        TInt h = (rect.iBr.iY - rect.iTl.iY);
        TInt ulc = (TInt) (iULC * w + 0.5) + rect.iTl.iX;
        TInt ulr = (TInt) (iULR * h + 0.5) + rect.iTl.iY;
        TInt lrc = (TInt) (iLRC * w  + 0.5) + rect.iTl.iX;
        TInt lrr = (TInt) (iLRR * h + 0.5) + rect.iTl.iY;

        //    Darken areas in the image that are outside crop rectangle
        CFbsBitmapDevice * bitmapDevice = CFbsBitmapDevice::NewL (iBufBitmap); 
        CleanupStack::PushL (bitmapDevice);

        //    Create bitmap graphics context
        CFbsBitGc * bitmapContext = 0;
        User::LeaveIfError (bitmapDevice->CreateContext (bitmapContext));
        CleanupStack::PushL (bitmapContext) ;

        
        bitmapContext->SetDrawMode (CGraphicsContext::EDrawModeAND);
        bitmapContext->SetPenStyle (CGraphicsContext::ENullPen);
        bitmapContext->SetPenColor (KRgbBlack);
        bitmapContext->SetBrushStyle (CGraphicsContext::EDiamondCrossHatchBrush);
        bitmapContext->SetBrushColor (KRgbWhite);

        //    Darken top
        bitmapContext->DrawRect ( TRect ( rect.iTl.iX, 0, rect.iBr.iX, ulr ) );

        //    Darken left side
        bitmapContext->DrawRect ( TRect( rect.iTl.iX, ulr, ulc, lrr ) );
        
        //    Darken right side
        bitmapContext->DrawRect ( TRect( lrc, ulr, rect.iBr.iX, lrr ) );

        //    Darken bottom
        bitmapContext->DrawRect ( TRect( rect.iTl.iX, lrr, rect.iBr.iX, Rect().iBr.iY ) );

        CleanupStack::PopAndDestroy(2); // bitmapContext, bitmapDevice 
    }
}

#endif // DOUBLE_BUFFERED_CROP

//=============================================================================
void CImageEditorCropControl::UpdateCropRectangle()
{

    LOG(KCropPluginLogFile, "CImageEditorCropControl::UpdateCropRectangle()");

    if ( iOldCropRectPrev == TRect (0,0,0,0) )
    {
        return;
    }
    if (iULC < 0.0)
    {
        iULC = 0.0;
        if ( !iIsCropModeManual )
        {
            ComputePreservedLRR();
        }
    }
    if (iULR < 0.0)
    {
        iULR = 0.0;
        if ( !iIsCropModeManual )
        {
            ComputePreservedLRC();
        }
    }
    if (iLRC > 1.0)
    {
        iLRC = 1.0;
        if ( !iIsCropModeManual )
        {
            ComputePreservedLRR();
        }
    }
    if (iLRR > 1.0)
    {
        iLRR = 1.0;
        if ( !iIsCropModeManual )
        {
            ComputePreservedLRC();
        }
    }
}

//=============================================================================
void CImageEditorCropControl::StoreCropRelScreen()
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::StoreCropRelScreen()");

    TRect viprect = iSysPars->VisibleImageRectPrev();
    
    TInt vipwidth = (viprect.iBr.iX - viprect.iTl.iX);
    TInt vipheight = (viprect.iBr.iY - viprect.iTl.iY);
    iOldCropRectPrev.iTl.iX = viprect.iTl.iX + (TInt)(iULC * vipwidth + 0.5);
    iOldCropRectPrev.iTl.iY = viprect.iTl.iY + (TInt)(iULR * vipheight + 0.5);
    iOldCropRectPrev.iBr.iX = viprect.iTl.iX + (TInt)(iLRC * vipwidth + 0.5);
    iOldCropRectPrev.iBr.iY = viprect.iTl.iY + (TInt)(iLRR * vipheight + 0.5);
}

//=============================================================================
void CImageEditorCropControl::RestoreCropRelScreen()
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::RestoreCropRelScreen()");

    TRect viprect = iSysPars->VisibleImageRectPrev();
    TInt width = (viprect.iBr.iX - viprect.iTl.iX);
    TInt height = (viprect.iBr.iY - viprect.iTl.iY);
    iULC = (float)(iOldCropRectPrev.iTl.iX - viprect.iTl.iX) / (float)width;
    iULR = (float)(iOldCropRectPrev.iTl.iY - viprect.iTl.iY) / (float)height;
    iLRC = (float)(iOldCropRectPrev.iBr.iX - viprect.iTl.iX) / (float)width;
    iLRR = (float)(iOldCropRectPrev.iBr.iY - viprect.iTl.iY) / (float)height;
}

//=============================================================================
void CImageEditorCropControl::StoreCropRelImage()
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::StoreCropRelImage()");

    TReal relscale = iSysPars->RelScale();
    TRect virect = iSysPars->VisibleImageRect();
    virect.iTl.iX = (TInt)((virect.iTl.iX / relscale) + 0.5);
    virect.iTl.iY = (TInt)((virect.iTl.iY / relscale) + 0.5);
    virect.iBr.iX = (TInt)((virect.iBr.iX / relscale) + 0.5);
    virect.iBr.iY = (TInt)((virect.iBr.iY / relscale) + 0.5);

    TInt viwidth = (virect.iBr.iX - virect.iTl.iX);
    TInt viheight = (virect.iBr.iY - virect.iTl.iY);

    iOldCropRectPrev.iTl.iX = virect.iTl.iX + (TInt)(iULC * viwidth + 0.5);
    iOldCropRectPrev.iTl.iY = virect.iTl.iY + (TInt)(iULR * viheight + 0.5);
    iOldCropRectPrev.iBr.iX = virect.iTl.iX + (TInt)(iLRC * viwidth + 0.5);
    iOldCropRectPrev.iBr.iY = virect.iTl.iY + (TInt)(iLRR * viheight + 0.5);
    
}

//=============================================================================
void CImageEditorCropControl::RestoreCropRelImage()
{
    LOG(KCropPluginLogFile, "CImageEditorCropControl::RestoreCropRelImage()");

    TReal relscale = iSysPars->RelScale();
    TRect virect = iSysPars->VisibleImageRect();
    virect.iTl.iX = (TInt)(virect.iTl.iX / relscale + 0.5);
    virect.iTl.iY = (TInt)(virect.iTl.iY / relscale + 0.5);
    virect.iBr.iX = (TInt)(virect.iBr.iX / relscale + 0.5);
    virect.iBr.iY = (TInt)(virect.iBr.iY / relscale + 0.5);

    TInt width = (virect.iBr.iX - virect.iTl.iX);
    TInt height = (virect.iBr.iY - virect.iTl.iY);

    iULC = (float)(iOldCropRectPrev.iTl.iX - virect.iTl.iX) / width;
    iULR = (float)(iOldCropRectPrev.iTl.iY - virect.iTl.iY) / height;
    iLRC = (float)(iOldCropRectPrev.iBr.iX - virect.iTl.iX) / width;
    iLRR = (float)(iOldCropRectPrev.iBr.iY - virect.iTl.iY) / height;

}

//=============================================================================
void CImageEditorCropControl::HandlePointerEventL(
                                           const TPointerEvent &aPointerEvent )
    {        
    if( AknLayoutUtils::PenEnabled() )
        {
        TBool redraw( ETrue );
        switch( aPointerEvent.iType )
            {
            case TPointerEvent::EButton1Down:
                {
                iPopupController->HideInfoPopupNote();
                iTouchDragEnabled = EFalse;
                // Store tapped position
                iTappedPosition = aPointerEvent.iPosition;
                
                if ( iState == ECropStateFirst || iState == ECropStateSecond )
                    {
                    // Check first if minimum crop area is tapped (priority 1)
                    if ( IsMinCropAreaTapped( aPointerEvent.iPosition ) )
                        {
                        // enables immediate rectangle moving
                        redraw = ETrue;
                        iTouchDragEnabled = ETrue; 
                        iState = ECropStateMove;
                        iEditorView->HandleCommandL ( 
                                                EImageEditorUpdateSoftkeys );
                        }
                    // Check if corners are tapped (priority 2)                        
                    else if ( IsCursorTapped( aPointerEvent.iPosition ) )
                        {
#ifdef RD_TACTILE_FEEDBACK
						if ( iTouchFeedBack )
							{
							iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
							RDebug::Printf( "ImageEditor::ImageEditorCropControl: ETouchFeedback 1" );
							}
#endif /* RD_TACTILE_FEEDBACK  */
                        
                        // Just enable dragging, no need to change position
                        // here
                        iTouchDragEnabled = ETrue;
                        redraw = EFalse;
                        }    
                    else if ( IsOppositeCornerTapped( 
                                                aPointerEvent.iPosition ) )
                        {
#ifdef RD_TACTILE_FEEDBACK
						if ( iTouchFeedBack )
							{
							iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
							RDebug::Printf( "ImageEditor::ImageEditorCropControl: ETouchFeedback 2" );
							}
#endif /* RD_TACTILE_FEEDBACK  */
                        
                        iTouchDragEnabled = ETrue;
                        if ( iState == ECropStateFirst )
                            {
                            iState = ECropStateSecond;    
                            }
                        else
                            {
                            iState = ECropStateFirst;
                            }
                        iEditorView->HandleCommandL ( 
                                                EImageEditorUpdateSoftkeys );
                        redraw = ETrue;
                        }
                    // Change to move state                        
                    else if ( IsCropAreaTapped( aPointerEvent.iPosition ) )
                        {
                        // enables immediate rectangle moving
                        redraw = ETrue;
                        iTouchDragEnabled = ETrue; 
                        iState = ECropStateMove;
                        iEditorView->HandleCommandL ( 
                                                EImageEditorUpdateSoftkeys );
                        }                    
                    }
                else if ( iState == ECropStateMove )
                    {
                    TInt corner;
                    
                    if ( IsMinCropAreaTapped( aPointerEvent.iPosition ) )
                        {
                        // enables immediate rectangle moving
                        redraw = ETrue;
                        iTouchDragEnabled = ETrue; 
                        iState = ECropStateMove;
                        //iEditorView->HandleCommandL ( 
                        //                        EImageEditorUpdateSoftkeys );
                        }                              
                    // Moving back to first or second state from Move state
                    // by pressing top-left or bottom-right corner
                    else if( IsCornerTapped( aPointerEvent.iPosition , corner) )
                        {
#ifdef RD_TACTILE_FEEDBACK
						if ( iTouchFeedBack )
							{
							iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
							RDebug::Printf( "ImageEditor::ImageEditorCropControl: ETouchFeedback 3" );
							}
#endif /* RD_TACTILE_FEEDBACK  */
                        if ( corner == ETLCorner )
                            {
                            // enables immediate dragging
                            redraw = ETrue;
                            iTouchDragEnabled = ETrue; 
                            iState = ECropStateFirst;
                            iEditorView->HandleCommandL ( 
                                                    EImageEditorUpdateSoftkeys );
                            }
                        else if ( corner == EBRCorner )
                            {
                            redraw = ETrue;
                            iTouchDragEnabled = ETrue;
                            iState = ECropStateSecond;
                            iEditorView->HandleCommandL ( 
                                                    EImageEditorUpdateSoftkeys );
                            }
                        }                    
                    else if ( IsCropAreaTapped( aPointerEvent.iPosition ) )
                        {
                        redraw = EFalse;
                        iTouchDragEnabled = ETrue;
                        iTappedPosition = aPointerEvent.iPosition;
                        }                        
                    }
                break;
                }
            case TPointerEvent::EDrag:
                {
                if ( iState == ECropStateFirst && 
                     iTouchDragEnabled )
                    {   
                    SetTLPosition( iTappedPosition, aPointerEvent.iPosition );
                    iTappedPosition = aPointerEvent.iPosition;
                    }
                else if ( iState == ECropStateSecond && 
                          iTouchDragEnabled )
                    {     
                    SetBRPosition( iTappedPosition, aPointerEvent.iPosition );                                    
                    iTappedPosition = aPointerEvent.iPosition;
                    }    
                else if ( iState == ECropStateMove && iTouchDragEnabled )
                    {
                    MoveCropArea( iTappedPosition, aPointerEvent.iPosition );
                    iTappedPosition = aPointerEvent.iPosition;
                    }
                redraw = ETrue;    
                break;        
                }
            case TPointerEvent::EButton1Up:
                {
                iTouchDragEnabled = EFalse;  
                redraw = EFalse;    
                ShowTooltip();
                break;
                }
                        
            default:
                {
                break;    
                }    
            }
        
        if ( redraw )
            {
            ComputeCropParams();
            
#ifdef DOUBLE_BUFFERED_CROP

            if ( iState == ECropStateFirst || 
                 iState == ECropStateSecond || 
                 iState == ECropStateMove )
                {
                ClonePreviewBitmapL();    
                }
            DarkenUnselectedAreaL();

#endif // DOUBLE_BUFFERED_CROP
                            
            iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
            DrawNow();
            }
        CCoeControl::HandlePointerEventL( aPointerEvent );        
        }
    }

//=============================================================================    
void CImageEditorCropControl::SetTLPosition( TPoint aOldPosition, 
                                             TPoint aNewPosition )
    {        
    // Get system parameters
    TRect visibleImageRectPrev( iSysPars->VisibleImageRectPrev() );
    
    //Set new x value
    iULC += TReal( aNewPosition.iX - aOldPosition.iX ) /
              ( visibleImageRectPrev.iBr.iX - visibleImageRectPrev.iTl.iX );
    
    //Set new y value
    iULR += TReal( aNewPosition.iY - aOldPosition.iY  ) /
              ( visibleImageRectPrev.iBr.iY - visibleImageRectPrev.iTl.iY );
    
    // check the limits 
    if (iULR < 0.0)
        {
        iULR = 0.0;
        }    
    else if (iULR > iLRR - iMinY)
        {
        iULR = iLRR - iMinY;
        }  
    
    if ( !iIsCropModeManual )
        {
        // To preserve selected aspect ratio
        ComputePreservedULC();
        }      
                         
    if (iULC < 0.0)
        {
        iULC = 0.0;
        }
    else if (iULC > iLRC - iMinX)
        {
        iULC = iLRC - iMinX;
        }  
    
    if ( !iIsCropModeManual )
        {
        // To preserve selected aspect ratio
        ComputePreservedULR();
        }                       
    }

//=============================================================================
void CImageEditorCropControl::SetBRPosition( TPoint aOldPosition, 
                                             TPoint aNewPosition )
    {
    
    // Get system parameters
    TRect visibleImageRectPrev( iSysPars->VisibleImageRectPrev() );
    
    //Set new x value
    iLRC += TReal( aNewPosition.iX - aOldPosition.iX ) /
              ( visibleImageRectPrev.iBr.iX - visibleImageRectPrev.iTl.iX );
    
    //Set new y value
    iLRR += TReal( aNewPosition.iY - aOldPosition.iY ) /
              ( visibleImageRectPrev.iBr.iY - visibleImageRectPrev.iTl.iY ); 
    
    // check the limits 
    if (iLRR < iULR + iMinY)
        {
        iLRR = iULR + iMinY;
        }
    else if (iLRR > 1.0)
        {
        iLRR = 1.0;
        }
        
    if ( !iIsCropModeManual )
        {
        // To preserve selected aspect ratio
        ComputePreservedLRC();
        }  
                 
    if (iLRC < iULC + iMinX)
        {
        iLRC = iULC + iMinX;
        }
    else if (iLRC > 1.0)
        {
        iLRC = 1.0;
        } 
        
    if ( !iIsCropModeManual )
        {
        // To preserve selected aspect ratio
        ComputePreservedLRR();
        }    
    }
                
//=============================================================================    
TBool CImageEditorCropControl::IsCursorTapped( TPoint aTappedPosition ) const
    {   
    TInt corner;
    
    if ( IsCornerTapped( aTappedPosition, corner ) )
        {
        if ( corner == ETLCorner && iState == ECropStateFirst )
            {
            return ETrue;
            }
        else if ( corner == EBRCorner && iState == ECropStateSecond )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

//=============================================================================    
TBool CImageEditorCropControl::IsOppositeCornerTapped( 
                                                TPoint aTappedPosition ) const
    {    
    TInt corner;
    
    if ( IsCornerTapped( aTappedPosition, corner ) )
        {
        if ( corner == ETLCorner && iState == ECropStateSecond )
            {
            return ETrue;
            }
        else if ( corner == EBRCorner && iState == ECropStateFirst )
            {
            return ETrue;
            }
        }
    return EFalse;
    }  

//============================================================================= 
TBool CImageEditorCropControl::IsCropAreaTapped( TPoint aTappedPosition ) const
    {
    TRect rect = iSysPars->VisibleImageRectPrev();
    TInt w = ( rect.iBr.iX - rect.iTl.iX );
    TInt h = ( rect.iBr.iY - rect.iTl.iY );
    TInt ulc = ( TInt ) ( iULC * w + 0.5 ) + rect.iTl.iX;
    TInt ulr = ( TInt ) ( iULR * h + 0.5 ) + rect.iTl.iY;
    TInt lrc = ( TInt ) ( iLRC * w + 0.5 ) + rect.iTl.iX;
    TInt lrr = ( TInt) ( iLRR * h + 0.5 ) + rect.iTl.iY;
    
    TRect areaRect ( TPoint( ulc, ulr ), TPoint( lrc, lrr ) );
    
    return areaRect.Contains( aTappedPosition );
    
    }
    
//============================================================================= 
TBool CImageEditorCropControl::IsMinCropAreaTapped( TPoint aTappedPosition ) const
    {
    TRect rect = iSysPars->VisibleImageRectPrev();
    TInt w = ( rect.iBr.iX - rect.iTl.iX );
    TInt h = ( rect.iBr.iY - rect.iTl.iY );
    TInt ulc = ( TInt ) ( iULC * w + 0.5 ) + rect.iTl.iX;
    TInt ulr = ( TInt ) ( iULR * h + 0.5 ) + rect.iTl.iY;
    TInt lrc = ( TInt ) ( iLRC * w + 0.5 ) + rect.iTl.iX;
    TInt lrr = ( TInt) ( iLRR * h + 0.5 ) + rect.iTl.iY;
    
    TPoint middlePoint = TPoint( ulc + ( lrc - ulc ) / 2,
                                 ulr + ( lrr - ulr ) / 2 );
                                     
    TReal relscale = iSysPars->RelScale();
    TInt minCrop = (TInt)(KMaxCropAbsoluteMin * relscale + 0.5);
                                
    TRect areaRect ( TPoint( middlePoint.iX - minCrop, middlePoint.iY - minCrop ), 
                     TPoint( middlePoint.iX + minCrop, middlePoint.iY + minCrop ) );
    
    // restrict min crop area inside total cropping area
    if( areaRect.iTl.iX < ulc )
        {
        areaRect.iTl.iX = ulc;
        }
    if( areaRect.iTl.iY < ulr )
        {
        areaRect.iTl.iY = ulr;
        }
    if( areaRect.iBr.iX > lrc )
        {
        areaRect.iBr.iX = lrc;
        }
    if( areaRect.iBr.iY > lrr )
        {
        areaRect.iBr.iY = lrr;
        }
        
    return areaRect.Contains( aTappedPosition );
    
    }
                
//============================================================================= 
TBool CImageEditorCropControl::IsCornerTapped( TPoint aTappedPosition, 
                                               TInt& aTappedCorner ) const
    {
    //  Compute crop rectangle inside the image area
    TRect rect = iSysPars->VisibleImageRectPrev();
    TInt w = ( rect.iBr.iX - rect.iTl.iX );
    TInt h = ( rect.iBr.iY - rect.iTl.iY );
    TInt ulc = ( TInt ) ( iULC * w + 0.5 ) + rect.iTl.iX;
    TInt ulr = ( TInt ) ( iULR * h + 0.5 ) + rect.iTl.iY;
    TInt lrc = ( TInt ) ( iLRC * w + 0.5 ) + rect.iTl.iX;
    TInt lrr = ( TInt ) ( iLRR * h + 0.5 ) + rect.iTl.iY;  
    
    TBool topLeftTapped = EFalse;
    TBool bottomRightTapped = EFalse;
    
    TSize cursorSize = iCrossHair->SizeInPixels();    
    TSize tripleCursorSize( 3 * cursorSize.iWidth, 
                            3 * cursorSize.iHeight );
    
    // Case1, upper left corner
    TInt cx = ulc;
    TInt cy = ulr;
    TRect cursorRect( TPoint( cx - ( tripleCursorSize.iWidth / 2 ), 
                              cy - ( tripleCursorSize.iHeight / 2 ) ), 
                              tripleCursorSize );
    
    // Calculate an estimate for the distance to top-left corner
    // Change more accurate implementation if needed in future
    // (this is fast)
     TInt distTL = Abs( cx - aTappedPosition.iX ) +
                   Abs( cy - aTappedPosition.iY );
                                        
    if ( cursorRect.Contains( aTappedPosition ) )                          
        {
        topLeftTapped = ETrue;
        }
        
    // Case2, lower right corner    
    cx = lrc - 1;
    cy = lrr - 1;
    cursorRect = TRect( TPoint( cx - ( tripleCursorSize.iWidth / 2 ), 
                              cy - ( tripleCursorSize.iHeight / 2 ) ), 
                              tripleCursorSize );
    
    // Calculate an estimate for the distance to bottom-right corner
    // Change more accurate implementation if needed in future
    // (this is fast)
     TInt distBR = Abs( cx - aTappedPosition.iX ) +
                   Abs( cy - aTappedPosition.iY );
                                            
    if ( cursorRect.Contains( aTappedPosition ) )                          
        {
        bottomRightTapped = ETrue;
        }  
    
    // Check tapped corner
    if ( topLeftTapped || bottomRightTapped )
        {
        if( topLeftTapped && !bottomRightTapped )
            {
            aTappedCorner = ETLCorner;
            }
        else if( !topLeftTapped && bottomRightTapped )
            {
            aTappedCorner = EBRCorner;
            }
        // Tapping happened inside of both corners. Check the closest.    
        else
            {
            if( distTL < distBR )
                {
                aTappedCorner = ETLCorner;
                }
            else
                {
                aTappedCorner = EBRCorner;
                }
            }
        return ETrue;    
        }
        
    // no top-left nor bottom-righ corner tapped    
    aTappedCorner = EInvalidCorner;
    return EFalse;
    }
        
//============================================================================= 
void CImageEditorCropControl::MoveCropArea( TPoint aOldPosition, 
                                            TPoint aNewPosition )
    {
    TRect visibleImageRectPrev( iSysPars->VisibleImageRectPrev() );
    
    // Change can be positive or negative
    TInt xChange ( aNewPosition.iX - aOldPosition.iX );
    TInt yChange ( aNewPosition.iY - aOldPosition.iY );
    
    // X-components    
    // store old x-values so they can be used if crop rect is trying to be
    // moved outside the visible area    
    TReal ulcOld = iULC;
    iULC += TReal( xChange ) / ( visibleImageRectPrev.iBr.iX - 
                                 visibleImageRectPrev.iTl.iX );
    TReal lrcOld = iLRC;
    iLRC += TReal( xChange ) / ( visibleImageRectPrev.iBr.iX - 
                                 visibleImageRectPrev.iTl.iX );
    
    // if limits are reached, move as much as possible (both sides)
    if ( iULC < 0.0 )
        {
        iULC = 0.0;
        iLRC = lrcOld;
        // ulc min limit reached,  move lrc as much as it was possible
        // to move ulc
        iLRC += ( iULC - ulcOld );
        }
    else if ( iLRC > 1.0 )
        {
        iLRC = 1.0;
        iULC = ulcOld;
        iULC += ( iLRC - lrcOld );
        }
    
    // Y-components        
    TReal ulrOld = iULR;
    iULR += TReal( yChange ) / ( visibleImageRectPrev.iBr.iY - 
                                 visibleImageRectPrev.iTl.iY );
    TReal lrrOld = iLRR;
    iLRR += TReal( yChange ) / ( visibleImageRectPrev.iBr.iY - 
                                 visibleImageRectPrev.iTl.iY );
    
    if ( iULR < 0.0 )
        {
        iULR = 0.0;
        iLRR = lrrOld;
        iLRR += ( iULR - ulrOld ); 
        }    
    else if ( iLRR > 1.0 )
        {
        iLRR = 1.0;
        iULR = ulrOld;
        iULR += ( iLRR - lrrOld );
        }            
    }
  
//=============================================================================
void CImageEditorCropControl::ShowTooltip()
    {
    iPopupController->HideInfoPopupNote();
    
    // Calculate visible image rect corner positions
    TRect rect = iSysPars->VisibleImageRectPrev();
    TInt w = ( rect.iBr.iX - rect.iTl.iX );
    TInt h = ( rect.iBr.iY - rect.iTl.iY );
    TInt ulc = ( TInt ) ( iULC * w + 0.5 ) + rect.iTl.iX;
    TInt ulr = ( TInt ) ( iULR * h + 0.5 ) + rect.iTl.iY;
    TInt lrc = ( TInt ) ( iLRC * w + 0.5 ) + rect.iTl.iX;
    TInt lrr = ( TInt ) ( iLRR * h + 0.5 ) + rect.iTl.iY;  
    
    TSize cursorSize = iCrossHair->SizeInPixels();    
        
    // user is setting upper left corner
    if ( iState == ECropStateFirst )
        {    
        TPoint middlePoint = TPoint( ulc, ulr );                                   
        
        SDrawUtils::ShowToolTip( iPopupController,
                                 this,
                                 middlePoint,
                                 EHLeftVBottom,
                                 *iTooltipResize );    
        }
    // user is setting lower right corner    
    else if ( iState == ECropStateSecond )
        {
        TPoint middlePoint = TPoint( lrc - 1 ,
                                     lrr - 1 );

        SDrawUtils::ShowToolTip ( iPopupController,
                                  this, 
                                  middlePoint,
                                  EHRightVBottom, 
                                  *iTooltipResize );                          
        }    
    // in move state
    else if ( iState == ECropStateMove )
        {            
        TPoint middlePoint = TPoint( ulc + ( lrc - ulc ) / 2,
                                     ulr + ( lrr - ulr ) / 2 );

        SDrawUtils::ShowToolTip ( iPopupController,
                                  this, 
                                  middlePoint,
                                  EHCenterVCenter, 
                                  *iTooltipMove );       
        }
    }
   
    
// End of file
