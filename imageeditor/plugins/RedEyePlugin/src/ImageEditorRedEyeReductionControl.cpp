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
* Control class for RedEyeReduction plugin.
*
*/


//  INCLUDES
#include <fbs.h>
#include <badesca.h>
#include <gdi.h>
#include <eikenv.h>
#include <bitdev.h>
#include <eikmenup.h>
#include <coehelp.h>

#include <aknview.h>
#include <aknutils.h>

#include <ImageEditorUI.mbg>


#include <AknInfoPopupNoteController.h> 
#include <redeyereduction.rsg> 
#include <bautils.h> 
#include <ConeResLoader.h> 
#include <csxhelp/sie.hlp.hrh>

#ifdef RD_TACTILE_FEEDBACK 
#include <touchfeedback.h>
#endif /* RD_TACTILE_FEEDBACK  */

#include "ImageEditorUI.hrh"
#include "ImageEditorPluginBase.hrh"
#include "RedEyeReduction.hrh"
#include "DrawUtils.h"
#include "PluginInfo.h"
#include "SystemParameters.h"
#include "ImageEditorUids.hrh"

#include "ImageEditorRedEyeReductionControl.h"
#include "ImageEditorUIDefs.h" 
#include "iepb.h"
#include "definitions.def"

// 	DEBUG LOG
#include "imageeditordebugutils.h"
_LIT(KRedEyeReductionPluginLogFile, "RedEyeReductionPlugin.log");

//  CONSTANTS
const TReal KPosParamMin        = 0.0;
const TReal KPosParamMax        = 1.0;
const TReal KPosParamStep       = 0.01;
const TReal KPosParamDef        = 0.5;

const TReal KRadiusParamMin      = 0.0;
const TReal KRadiusParamMax      = 0.4;
const TReal KRadiusParamStep     = 0.0025;
const TReal KRadiusParamDef      = 0.05;

const TInt KMoveSelectionIndex    = 0;
const TInt KResizeSelectionIndex  = 1;

// Default values from CPreviewControlBase not used
const TInt KRedEyeFastKeyTimerDelayInMicroseconds = 500000;
const TInt KRedEyeFastKeyTimerIntervalInMicroseconds = 50000;
const TInt KRedEyeFastKeyTimerMultiplyThresholdInTicks = 9;
const TInt KRedEyeBigNavigationStepMultiplier = 3;

// Touch constants
const TInt KTouchScaleMaxStepCount  = 50;

// Panic category
_LIT (KComponentName, "ImageEditorRedEyeReduction" );

// Resource file name
_LIT (KPgnResourceFile, "redeyereduction.rsc"); 

//=============================================================================
TInt CImageEditorRedEyeReductionControl::DancingAntzCallback (TAny * aPtr)
{
	((CImageEditorRedEyeReductionControl *)aPtr)->OnDancingAntzCallBack();
	return KErrNone;
}

//=============================================================================
TInt CImageEditorRedEyeReductionControl::FastKeyCallback (TAny * aPtr)
{
	TRAPD( err, ((CImageEditorRedEyeReductionControl *)aPtr)->OnFastKeyCallBackL() );
	return err;
}

//=============================================================================
CImageEditorRedEyeReductionControl * CImageEditorRedEyeReductionControl::NewL (
	const TRect &		aRect,
	CCoeControl	*		aParent
	)
{
    CImageEditorRedEyeReductionControl * self = new (ELeave) CImageEditorRedEyeReductionControl;
    CleanupStack::PushL (self);
    self->ConstructL (aRect, aParent);
    CleanupStack::Pop ();   // self
    return self;
}

//=============================================================================
CImageEditorRedEyeReductionControl::CImageEditorRedEyeReductionControl () :
iState( ERedEyeReductionStateMin ),
iX( KPosParamDef ),
iY( KPosParamDef ),
iR( KRadiusParamDef ),
iHandleEventKeys (ETrue),
iTickCount (0),
iNaviStepMultiplier (KDefaultSmallNavigationStepMultiplier),
iReadyToRender(ETrue),
iDoNotUpdateParameters(EFalse),
iCursorDragEnabled( EFalse )
{
    
}

//=============================================================================
CImageEditorRedEyeReductionControl::~CImageEditorRedEyeReductionControl ()
{
    delete iCrossHair;
    delete iCrossHairMask;
    iUndoArray.Close();
    if( iTimer )
    {
        iTimer->Cancel();
    }
    delete iTimer;
    iVisibleImageRectPrev = NULL;
    iVisibleImageRect = NULL;
    iEditorView = NULL;
    iItem = NULL;
    
    delete iPopupController;
	delete iTooltipLocate;
	delete iTooltipOutline;

}

//=============================================================================
void CImageEditorRedEyeReductionControl::ConstructL (
	const TRect &		/*aRect*/,
	CCoeControl	*		aParent
	)
{
	LOG_INIT(KRedEyeReductionPluginLogFile);
	LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::ConstructL" );

	//	Set parent window
	SetContainerWindowL (*aParent);

    //	Create timer for fast keys and dancing ants
    iTimer = CPeriodic::NewL (CActive::EPriorityStandard);

	//	Create CROSSHAIR image
    SDrawUtils::GetIndicatorBitmapL( 
        iCrossHair, 
        iCrossHairMask, 
        EMbmImageeditoruiQgn_indi_imed_cursor_super, // cross-hair
        EMbmImageeditoruiQgn_indi_imed_cursor_super_mask
        );

	iPopupController = CAknInfoPopupNoteController::NewL();    
	
	TFileName resourcefile;
	resourcefile.Append(KPgnResourcePath);
	resourcefile.Append(KPgnResourceFile);
    User::LeaveIfError( CompleteWithAppPath( resourcefile ) );

    //	Read tooltip resources  
    //  (RConeResourceLoader selects the language using BaflUtils::NearestLanguageFile)
    RConeResourceLoader resLoader ( *CEikonEnv::Static() );
    CleanupClosePushL ( resLoader );
	resLoader.OpenL ( resourcefile );
	
	iTooltipLocate = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_REDEYE_LOCATE);    
	iTooltipOutline = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_REDEYE_OUTLINE);    
      
    CleanupStack::PopAndDestroy(); // resLoader

#ifdef RD_TACTILE_FEEDBACK 
    iTouchFeedBack = MTouchFeedback::Instance();
#endif /* RD_TACTILE_FEEDBACK  */
    
	//	Activate control
    ActivateL();
    
    EnableDragEvents();
}

//=============================================================================
void CImageEditorRedEyeReductionControl::SetView (CAknView * aView)
{
    LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::SetView" );
    
    iEditorView = aView;
}

//=============================================================================
void CImageEditorRedEyeReductionControl::SetSelectedUiItemL (CPluginInfo * aItem)
{
    LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::SetSelectedUiItemL" );

    iItem = aItem;
}

//=============================================================================
void CImageEditorRedEyeReductionControl::PrepareL ()
{
    LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::PrepareL" );

#ifdef __TOUCH_ONLY_DEVICE_RER_PLUGIN_ACTION__         
    // The whole crosshair set state is skipped totally in touch-only devices
    ToResizeStateL();
#else        
    ToMoveStateL();
#endif    
                
    // Store parameters for the initial filter. 
    // This is a special case. For the other filters, the parameters
    // are stored implicitly when calling 
    iEditorView->HandleCommandL (EImageEditorStoreParameters);
}

//=============================================================================
TKeyResponse CImageEditorRedEyeReductionControl::OfferKeyEventL (
    const TKeyEvent &   aKeyEvent,
    TEventCode          aType
    )
{
    TKeyResponse response = EKeyWasNotConsumed;

    //  If busy, do not handle anything
    if ( Busy() )
    {
        response = EKeyWasConsumed;
    }

    //  EEventKey
    else if (EEventKey == aType && iHandleEventKeys)
    {
        
		switch (aKeyEvent.iCode)
		{

			case EKeyDownArrow:
			{
                //  Adjust crop point
                NaviDown();
                DrawNow();
                response = EKeyWasConsumed;
                break;
			}

			case EKeyUpArrow:
			{
                //  Adjust crop point
                NaviUp();
                DrawNow();
                response = EKeyWasConsumed;
                break;
			}

			case EKeyRightArrow:
			{
                //  Adjust crop point
                NaviRight();
                DrawNow();
				response = EKeyWasConsumed;
                break;
			}

            case EKeyLeftArrow:
			{
                //  Adjust crop point
                NaviLeft();
                DrawNow();
				response = EKeyWasConsumed;
                break;
			}

            case EKeyOK:
            case EKeyEnter:
            {
                KeyOkL();
                DrawNow();
                response = EKeyWasConsumed;
                break;
            }

			default:
			{
				break;
			}
		}
    }

    else if (EEventKey == aType && !iHandleEventKeys)
    {
        response = EKeyWasConsumed;
    }

    //  EEventKeyDown
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

    //  EEventKeyUp
    else if (EEventKeyUp == aType)
    {
        if (aKeyEvent.iScanCode == iPressedKeyScanCode)
        {
            iHandleEventKeys = ETrue;
            if (iState == ERedEyeReductionStateResizeSelection)
            {
                StartDancingAntzTimer();
            }
            else if( iTimer )
            {
                iTimer->Cancel();
            }
            response = EKeyWasConsumed;
            
            ShowTooltip();
        }
    }

    return response;
}
    
//=============================================================================
void CImageEditorRedEyeReductionControl::SizeChanged()
{
}

//=============================================================================
TRect CImageEditorRedEyeReductionControl::GetParam ()
{
	LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::GetParam" );
	if (iUndoArray.Count() == 0 || iDoNotUpdateParameters)
	{
		LOG( KRedEyeReductionPluginLogFile, "\tiRedEyeRect: (0,0), (0,0)" );
		return TRect();
	}
	else
	{
		LOGFMT4( KRedEyeReductionPluginLogFile, "\tiRedEyeRect: (%d,%d), (%d,%d)", iRedEyeRect.iTl.iX, iRedEyeRect.iTl.iY, iRedEyeRect.iBr.iX, iRedEyeRect.iBr.iY );
		ComputeParams();
		return iRedEyeRect;
	}
}

//=============================================================================
void CImageEditorRedEyeReductionControl::SetSystemParameters (const CSystemParameters * aPars) 
{
    LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::SetSystemParameters" );

    iVisibleImageRect = &(aPars->VisibleImageRect()); 
    iVisibleImageRectPrev = &(aPars->VisibleImageRectPrev());
	iSysPars = aPars;

    __ASSERT_ALWAYS( iVisibleImageRectPrev && iVisibleImageRectPrev, User::Panic(KComponentName, KErrNotReady) );
    LOGFMT4( KRedEyeReductionPluginLogFile, "\tiVisibleImageRect: (%d,%d), (%d,%d)", iVisibleImageRect->iTl.iX, iVisibleImageRect->iTl.iY, iVisibleImageRect->iBr.iX, iVisibleImageRect->iBr.iY );
    LOGFMT2( KRedEyeReductionPluginLogFile, "\tVisible image rect size: %d,%d", (iVisibleImageRect->iBr.iX - iVisibleImageRect->iTl.iX) + 1, (iVisibleImageRect->iBr.iY - iVisibleImageRect->iTl.iY) + 1 );
    LOGFMT2( KRedEyeReductionPluginLogFile, "\tSize(): %d,%d", Size().iWidth, Size().iHeight );
    LOGFMT4( KRedEyeReductionPluginLogFile, "\tiVisibleImageRectPrev: (%d,%d), (%d,%d)", iVisibleImageRectPrev->iTl.iX, iVisibleImageRectPrev->iTl.iY, iVisibleImageRectPrev->iBr.iX, iVisibleImageRectPrev->iBr.iY );
    LOGFMT2( KRedEyeReductionPluginLogFile, "\tiVisibleImageRectPrev size: %d,%d", iVisibleImageRectPrev->Width(), iVisibleImageRectPrev->Height() );
}

//=============================================================================
void CImageEditorRedEyeReductionControl::HandlePluginCommandL (const TInt aCommand)
{
    LOGFMT( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::HandlePluginCommandL: %d", aCommand );

    switch (aCommand) 
    {
        case EImageEditorFocusGained:
        {
            if (iState == ERedEyeReductionStateResizeSelection)
            {
                StartDancingAntzTimer();
            }
            break;
        }
        case EImageEditorFocusLost:
        {
            if (iTimer)
            {
                iTimer->Cancel();
            }
            break;
        }
        case EPgnSoftkeyIdCancel:
        {
            CancelPluginL();
            break;
        }
        case EPgnSoftkeyIdDone:
        {        
            iDoNotUpdateParameters = ETrue;
            iEditorView->HandleCommandL (EImageEditorApplyPlugin);
            break;
        }
        case ERedEyeReductionPgnMenuCmdSet:
        case EPgnSoftkeyIdSet:
        {
            KeyOkL();
            DrawNow();
            break;
        }
        case ERedEyeReductionPgnMenuCmdReduceRed:
        case EPgnSoftkeyIdReduceRed:
        {
            KeyOkL();
            DrawNow();
            break;
        }
        case ERedEyeReductionPgnMenuCmdUndo:
        {
            UndoL();
            iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
            break;
        }
        case ERedEyeReductionPgnMenuCmdCancel:
        {
            CancelPluginL();
            break;
        }
        case EImageEditorPreGlobalZoomChange:
        case EImageEditorPreGlobalPanChange:
		{
			iDoNotUpdateParameters = ETrue;
			break;
		}
        case EImageEditorGlobalZoomChanged:
        case EImageEditorGlobalPanChanged:
        {
			iDoNotUpdateParameters = EFalse;
			DrawNow();
	        break;
        }
        
    	case EImageEditorPreScreenModeChange:
    	{
    		TReal relscale = iSysPars->RelScale();
			TRect virect = *iVisibleImageRect;
			virect.iTl.iX = (TInt)((virect.iTl.iX / relscale) + 0.5);
			virect.iTl.iY = (TInt)((virect.iTl.iY / relscale) + 0.5);
			virect.iBr.iX = (TInt)((virect.iBr.iX / relscale) + 0.5);
			virect.iBr.iY = (TInt)((virect.iBr.iY / relscale) + 0.5);
			iLockPoint.iX = virect.iTl.iX + iX * virect.Width();
			iLockPoint.iY = virect.iTl.iY + iY * virect.Height();
    		break;
    	}
    	case EImageEditorPostScreenModeChange:
    	{
    		TReal relscale = iSysPars->RelScale();
			TRect virect = *iVisibleImageRect;
			virect.iTl.iX = (TInt)((virect.iTl.iX / relscale) + 0.5);
			virect.iTl.iY = (TInt)((virect.iTl.iY / relscale) + 0.5);
			virect.iBr.iX = (TInt)((virect.iBr.iX / relscale) + 0.5);
			virect.iBr.iY = (TInt)((virect.iBr.iY / relscale) + 0.5);
			iX = (TReal)(iLockPoint.iX - virect.iTl.iX) / virect.Width();
			iY = (TReal)(iLockPoint.iY - virect.iTl.iY) / virect.Height();
			ClipPosition();
			DrawNow();
    		break;
    	}
        
        default:
        {
            break;
        }
    }
}

//=============================================================================
TInt CImageEditorRedEyeReductionControl::GetSoftkeyIndexL()
{
    
    TInt index = -1;
  if ( iState == ERedEyeReductionStateResizeSelection )
        {
        if ( !iUndoArray.Count() )
            {
            index = 3;
            }
        else
            {
            index = 1;
            }            
        }
    else if ( iState == ERedEyeReductionStateMoveCrosshair )
        {
        if ( !iUndoArray.Count() )
            {
            index = 0;
            }
        else
            {
            index = 2;
            }  
        }
   
  
       
    LOGFMT( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::GetSoftkeyIndexL: %d", index );

    return index;
}

//=============================================================================
TBitField CImageEditorRedEyeReductionControl::GetDimmedMenuItems()
{
	LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::GetDimmedMenuItems" );

    TBitField dimmedMenuItems;
    TInt count = iItem->MenuItems().Count();

    switch (iState) 
    {
    case ERedEyeReductionStateMoveCrosshair:
        {
            // Dim the command ERedEyeReductionPgnMenuCmdReduceRed, and also 
            // ERedEyeReductionPgnMenuCmdUndo if selecting the first eye.
            for ( TInt i = 0; i < count; i++)
            {
                // get the menu item id
                TInt menuItem = iItem->MenuItems().At(i).iCommandId;
                if ( menuItem == ERedEyeReductionPgnMenuCmdReduceRed ||
                    ( iUndoArray.Count() == 0 && menuItem == ERedEyeReductionPgnMenuCmdUndo) 
                    )
                {
                    dimmedMenuItems.SetBit( i );
                }
            }
            break;
        }
    case ERedEyeReductionStateResizeSelection:
        {
            // Dim the command ERedEyeReductionPgnMenuCmdSet.
            for ( TInt i = 0; i < count; i++)
            {
                // get the menu item id
                TInt menuItem = iItem->MenuItems().At(i).iCommandId;
                
#ifdef __TOUCH_ONLY_DEVICE_RER_PLUGIN_ACTION__ 
                if ( menuItem == ERedEyeReductionPgnMenuCmdSet ||
                   ( iUndoArray.Count() == 0 && menuItem == ERedEyeReductionPgnMenuCmdUndo))
                {
                    dimmedMenuItems.SetBit( i );
                }                
#else
                if ( menuItem == ERedEyeReductionPgnMenuCmdSet )
                {
                    dimmedMenuItems.SetBit( i );
                }
#endif                
                
            }
            break;
        }
    default:
        {
            ASSERT( EFalse );
            break;
        }
    }
    return dimmedMenuItems;
}

//=============================================================================
TPtrC CImageEditorRedEyeReductionControl::GetNaviPaneTextL (
    TBool& aLeftNaviPaneScrollButtonVisibile, 
    TBool& aRightNaviPaneScrollButtonVisible )
{
    aLeftNaviPaneScrollButtonVisibile = EFalse;
    aRightNaviPaneScrollButtonVisible = EFalse;
    return iNaviPaneText;
}

//=============================================================================
void CImageEditorRedEyeReductionControl::Draw (const TRect & aRect) const
{
    LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::Draw()" );

    CPreviewControlBase::DrawPreviewImage (aRect);

#ifdef VERBOSE
    LOGFMT4( KRedEyeReductionPluginLogFile, "\tiVisibleImageRect: (%d,%d), (%d,%d)", iVisibleImageRect->iTl.iX, iVisibleImageRect->iTl.iY, iVisibleImageRect->iBr.iX, iVisibleImageRect->iBr.iY );
    LOGFMT2( KRedEyeReductionPluginLogFile, "\tVisible image rect size: %d,%d", (iVisibleImageRect->iBr.iX - iVisibleImageRect->iTl.iX) + 1, (iVisibleImageRect->iBr.iY - iVisibleImageRect->iTl.iY) + 1 );
    LOGFMT2( KRedEyeReductionPluginLogFile, "\tSize(): %d,%d", Size().iWidth, Size().iHeight );
    LOGFMT4( KRedEyeReductionPluginLogFile, "\tiVisibleImageRectPrev: (%d,%d), (%d,%d)", iVisibleImageRectPrev->iTl.iX, iVisibleImageRectPrev->iTl.iY, iVisibleImageRectPrev->iBr.iX, iVisibleImageRectPrev->iBr.iY );
    LOGFMT2( KRedEyeReductionPluginLogFile, "\tiVisibleImageRectPrev size: %d,%d", iVisibleImageRectPrev->Width(), iVisibleImageRectPrev->Height() );
#endif

    //  In move state draw the crosshair
    if ( iState == ERedEyeReductionStateMoveCrosshair )
    {
		//  Get graphics context
		CWindowGc & gc = SystemGc();

		gc.SetPenStyle (CGraphicsContext::ENullPen);
		gc.SetBrushStyle (CGraphicsContext::ENullBrush);

        TSize chSize = iCrossHair->SizeInPixels();
		TPoint center = CursorLocation();
        gc.BitBltMasked (
            TPoint(center.iX - (chSize.iWidth >> 1), center.iY - (chSize.iHeight >> 1)), 
            iCrossHair, 
            TRect (chSize), 
            iCrossHairMask, 
            EFalse
            );
    }

    // In resize state draw the selection circle
    else if ( iState == ERedEyeReductionStateResizeSelection )
    {
		DrawDancingAntz(Rect());
    }
}

//=============================================================================
void CImageEditorRedEyeReductionControl::DrawDancingAntz (const TRect & /*aRect*/) const
{
	//  Get graphics context
    CWindowGc & gc = SystemGc();

    //  Compute center point of the area
    TPoint center = CursorLocation();

    //  Compute selection rectangle
    TRect rect ( SelectionRect( center ) ); 

    //  Draw the "dancing ants" ellipse
	gc.SetDrawMode (CGraphicsContext::EDrawModePEN);
    gc.SetBrushStyle (CGraphicsContext::ENullBrush);
    gc.SetBrushColor (KRgbBlack);
	
	//	Draw 
    gc.SetPenStyle (CGraphicsContext::ESolidPen);
	gc.SetPenColor ((iAntFlag) ? (KRgbBlack) : (KRgbWhite));
    gc.DrawEllipse (rect);
    
	//	Draw dashed line
	gc.SetPenStyle (CGraphicsContext::EDashedPen);
	gc.SetPenColor ((iAntFlag) ? (KRgbWhite) : (KRgbBlack));
    gc.DrawEllipse (rect);

}

//=============================================================================
void CImageEditorRedEyeReductionControl::UndoL()
    {
    TInt count = iUndoArray.Count();

    LOGFMT( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::UndoL(): iUndoArray.Count() == %d", count );

    switch (iState) 
        {
        case ERedEyeReductionStateMoveCrosshair:
            {
            if ( count > 0 )
                {
    			// Remove the latest selected red eye area
                // (It would be possible to set the cursor back to the point 
                //  which was the location of the previous eye, but that 
                // depends also on the zoom and pan values, which are not 
                // available for plugins)
                iUndoArray.Remove( count - 1 );

    			// Undo the filter
    			iEditorView->HandleCommandL (EImageEditorUndoFilter);
                
    			if ( iUndoArray.Count() == 0)
			        {
                    // One filter is added already when opening the plugin. If 
                    // this initial filter is undone, we need to create a new one.
    				iEditorView->HandleCommandL (EImageEditorStoreParameters);
    				iEditorView->HandleCommandL (EImageEditorAddFilterToEngine);

			        }
                else
                    {
                    // Restore parameters
                    iRedEyeRect = iUndoArray[ count - 2 ];
                    }
                }
            break;
            }
        case ERedEyeReductionStateResizeSelection:
            {
        
#ifdef __TOUCH_ONLY_DEVICE_RER_PLUGIN_ACTION__  
            // in touch-only device do the same thing here in resize state as 
            // in non-touch device in move state->remove/undo filter 
            if ( count > 0 )
                {
                    		
            iUndoArray.Remove( count - 1 );

    		// Undo the filter
    		iEditorView->HandleCommandL (EImageEditorUndoFilter);
            
    			if ( iUndoArray.Count() == 0)
    		        {
                    // One filter is added already when opening the plugin. If this
                    // initial filter is undone, we need to create a new one.
    				iEditorView->HandleCommandL (EImageEditorStoreParameters);
    				iEditorView->HandleCommandL (EImageEditorAddFilterToEngine);

    		        }
                else
                    {
                    // Restore parameters
                    iRedEyeRect = iUndoArray[ count - 2 ];
                    }
                }

#else 
                
            // In the resize state consider that undo means
            // going back to the move crosshair state (non-touch)
            ToMoveStateL();
#endif                                   
                                   
            break;
            }
        
        default:
            {
            ASSERT( EFalse );
            break;
            }
        }

    DrawNow();
    }
    

//=============================================================================
void CImageEditorRedEyeReductionControl::NaviDown()
{
    switch (iState) 
    {
    case ERedEyeReductionStateMoveCrosshair:
        {
            iY += (KPosParamStep * iNaviStepMultiplier);
            if (iY > KPosParamMax)
            {
                iY = KPosParamMax;
            }
            break;
        }
    case ERedEyeReductionStateResizeSelection:
        {
            iR -= (KRadiusParamStep * iNaviStepMultiplier);
            if (iR < KRadiusParamMin)
            {
                iR = KRadiusParamMin;
            }
            break;
        }
    default:
        {
            ASSERT( EFalse );
            break;
        }
    }
}

//=============================================================================
void CImageEditorRedEyeReductionControl::NaviUp()
{
    switch (iState) 
    {
    case ERedEyeReductionStateMoveCrosshair:
        {
            iY -= (KPosParamStep * iNaviStepMultiplier);
            if (iY < KPosParamMin)
            {
                iY = KPosParamMin;
            }
            break;
        }
    case ERedEyeReductionStateResizeSelection:
        {
            iR += (KRadiusParamStep * iNaviStepMultiplier);
            if (iR > KRadiusParamMax)
            {
                iR = KRadiusParamMax;
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
void CImageEditorRedEyeReductionControl::NaviRight()
{
    switch (iState) 
    {
	case ERedEyeReductionStateMoveCrosshair:
        {
            iX += (KPosParamStep * iNaviStepMultiplier);
            if (iX > KPosParamMax)
            {
                iX = KPosParamMax;
            }
            break;
        }
	case ERedEyeReductionStateResizeSelection:
        {
            iR += (KRadiusParamStep * iNaviStepMultiplier);
            if (iR > KRadiusParamMax)
            {
                iR = KRadiusParamMax;
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
void CImageEditorRedEyeReductionControl::NaviLeft()
{
    switch (iState) 
    {
    case ERedEyeReductionStateMoveCrosshair:
        {
            iX -= (KPosParamStep * iNaviStepMultiplier);
            if (iX < KPosParamMin)
            {
                iX = KPosParamMin;
            }
    	    break;
        }
    case ERedEyeReductionStateResizeSelection:
        {
            iR -= (KRadiusParamStep * iNaviStepMultiplier);
            if (iR < KRadiusParamMin)
            {
                iR = KRadiusParamMin;
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
void CImageEditorRedEyeReductionControl::KeyOkL()
{
    switch (iState) 
    {
    case ERedEyeReductionStateMoveCrosshair:
        {
            ToResizeStateL();
            break;
        }
    case ERedEyeReductionStateResizeSelection:
        {
            if ( iUndoArray.Count() > 0)
            {
			    AddFilterToEngineL();
            }
			else
			{

                // Update the filter parameters
                ComputeParams();

                // Store coodinates
                iUndoArray.Append( iRedEyeRect );

				// Render 
				iEditorView->HandleCommandL (EImageEditorCmdRender);
			}

			
#ifdef __TOUCH_ONLY_DEVICE_RER_PLUGIN_ACTION__ 
            // Loop only inside Resize state in touch-only devices
            ToResizeStateL();            

#else
            
            // Go back to move state (non-touch)                
            ToMoveStateL();
#endif                
			                          
            break;
        }

    case ERedEyeReductionStateDone:
        {
            if ( iUndoArray.Count() > 0)
            {
			    AddFilterToEngineL();
            }
			else
            {            	
                // Update the filter parameters
                ComputeParams();

                // Store coodinates
                iUndoArray.Append( iRedEyeRect );
                                
                // Render 
            	iEditorView->HandleCommandL (EImageEditorCmdRender);
			
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
void CImageEditorRedEyeReductionControl::CancelPluginL()
{
    LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::CancelPluginL" );

    iState = ERedEyeReductionStateMin;

    // Undo all the filters exept for the first one,
    // which is cancelled by the framework.
    TInt count = iUndoArray.Count();
    while ( count > 1) 
    {
        iEditorView->HandleCommandL (EImageEditorUndoFilter);
        count--;
    }

    iEditorView->HandleCommandL (EImageEditorCancelPlugin);
}

//=============================================================================
void CImageEditorRedEyeReductionControl::ToMoveStateL()
{
	LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::ToMoveState" );

	// Stop dancing antz timer
	if (iTimer)
	{
		iTimer->Cancel();
	}

    // Update navi pane text
    iState = ERedEyeReductionStateMoveCrosshair;
    iNaviPaneText.Copy ( iItem->Parameters()[KMoveSelectionIndex] );
    iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
    // Update softkeys
    iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);

	ShowTooltip();
}

//=============================================================================
void CImageEditorRedEyeReductionControl::ToResizeStateL()
{
	LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::ToResizeState" );

    // Start the timer
    StartDancingAntzTimer();

    // Reset the selection radius
    iR = KRadiusParamDef;

    // Update navi pane text
    iState = ERedEyeReductionStateResizeSelection;
    iNaviPaneText.Copy ( iItem->Parameters()[KResizeSelectionIndex] );
    iEditorView->HandleCommandL (EImageEditorUpdateNavipane);

    // Update softkeys
    iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);

	ShowTooltip();
}

//=============================================================================
TPoint CImageEditorRedEyeReductionControl::CursorLocation() const
{
    __ASSERT_ALWAYS( iVisibleImageRectPrev, User::Panic(KComponentName, KErrNotReady) );
	TInt x = (TInt) (iX * (iVisibleImageRectPrev->Width())) + iVisibleImageRectPrev->iTl.iX;
	TInt y = (TInt) (iY * (iVisibleImageRectPrev->Height())) + iVisibleImageRectPrev->iTl.iY;
    return TPoint( x, y );
}

//=============================================================================
TRect CImageEditorRedEyeReductionControl::SelectionRect( const TPoint& aCenter ) const
{
    __ASSERT_ALWAYS( iVisibleImageRectPrev, User::Panic(KComponentName, KErrNotReady) );

    // Radius iR is relative to the real image's x dimension.
    // To obtain a circe, y-dimenson must be scaled accordingly.
    TInt vpWidth = (iVisibleImageRect->iBr.iX - iVisibleImageRect->iTl.iX);
    TInt vpHeight = (iVisibleImageRect->iBr.iY - iVisibleImageRect->iTl.iY);
    TReal xyRatio = (TReal) vpHeight / vpWidth;

    //  Compute rectangle inside the image area
    TInt width = iVisibleImageRectPrev->Width();
    TInt height = iVisibleImageRectPrev->Height();

    TInt ulc = aCenter.iX - (TInt)( iR * width + 0.5 );           // upper left corner x
    TInt ulr = aCenter.iY - (TInt)( iR * height / xyRatio + 0.5); // upper left corner y
    TInt lrc = aCenter.iX + (TInt)( iR * width + 0.5 );           // lower right corner x
    TInt lrr = aCenter.iY + (TInt)( iR * height / xyRatio + 0.5); // lower right corner y

    return TRect ( ulc, ulr, lrc, lrr );
}

//=============================================================================
void CImageEditorRedEyeReductionControl::ComputeParams()
{
    LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::ComputeParams()" );

    __ASSERT_ALWAYS( iVisibleImageRect, User::Panic(KComponentName, KErrNotReady) );

    // Compute the coordinates

    TInt vpWidth = iVisibleImageRect->iBr.iX - iVisibleImageRect->iTl.iX;
    TInt vpHeight = iVisibleImageRect->iBr.iY - iVisibleImageRect->iTl.iY;

    TInt r      = (TInt) (iR * vpWidth + 0.5);
    TInt ulc    = (TInt) (iX * vpWidth + 0.5 ) - r;
    TInt ulr    = (TInt) (iY * vpHeight + 0.5 ) - r;
    TInt side   = (TInt) (2 * iR * vpWidth + 0.5);

    // Set the filter parameters.
    // The coordinates iX and iR are relative to the viewport image,
    // whereas the filter parameters are relative to the source image.
   	iRedEyeRect.iTl.iX = iVisibleImageRect->iTl.iX + ulc;
    iRedEyeRect.iTl.iY = iVisibleImageRect->iTl.iY + ulr;
    iRedEyeRect.iBr.iX = iRedEyeRect.iTl.iX + side;
    iRedEyeRect.iBr.iY = iRedEyeRect.iTl.iY + side;

    iReadyToRender = ETrue;   
}

//=============================================================================
void CImageEditorRedEyeReductionControl::AddFilterToEngineL()
{
    LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::AddFilterToEngineL" );

    // Update the filter parameters
    ComputeParams();

    // Store coodinates for undo
    iUndoArray.Append( iRedEyeRect );

   	// Force adding the filter to the engine. This includes setting the undo point.
	iEditorView->HandleCommandL (EImageEditorAddFilterToEngine);
	iEditorView->HandleCommandL (EImageEditorStoreParameters);

    // Render 
	iEditorView->HandleCommandL (EImageEditorCmdRender);
}

//=============================================================================
void CImageEditorRedEyeReductionControl::StartDancingAntzTimer()
{
    LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::StartDancingAntzTimer()" );

    iNaviStepMultiplier = KDefaultSmallNavigationStepMultiplier;

	if (iTimer)
	{
        iTimer->Cancel();
		iTimer->Start(
			TTimeIntervalMicroSeconds32 (KDancingAntzTimerDelayInMicroseconds),
			TTimeIntervalMicroSeconds32 (KDancingAntzTimerIntervalInMicroseconds),
			TCallBack (DancingAntzCallback, this)
			);
	}
}

//=============================================================================
void CImageEditorRedEyeReductionControl::StartFastKeyTimer()
{
    LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::StartFastKeyTimer()" );

    iNaviStepMultiplier = KDefaultSmallNavigationStepMultiplier;
    iTickCount = 0;

	if (iTimer)
	{
		iTimer->Cancel();
		iTimer->Start(
			TTimeIntervalMicroSeconds32 (KRedEyeFastKeyTimerDelayInMicroseconds),
			TTimeIntervalMicroSeconds32 (KRedEyeFastKeyTimerIntervalInMicroseconds),
			TCallBack (FastKeyCallback, this)
			);
	}
}

//=============================================================================
void CImageEditorRedEyeReductionControl::OnDancingAntzCallBack()
{
	iAntFlag = !iAntFlag;
	ActivateGc();
	DrawDancingAntz (Rect());
	DeactivateGc();
}

//=============================================================================
void CImageEditorRedEyeReductionControl::OnFastKeyCallBackL()
{
#ifdef VERBOSE
    LOGFMT2( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::OnFastKeyCallBackL: iNaviStepMultiplier: %d, iTickCount: %d", iNaviStepMultiplier, iTickCount );
#endif

    if (iTickCount > KRedEyeFastKeyTimerMultiplyThresholdInTicks)
    {
        iNaviStepMultiplier = KRedEyeBigNavigationStepMultiplier;
        LOG( KRedEyeReductionPluginLogFile, "CImageEditorRedEyeReductionControl::OnFastKeyCallBackL: switching to big steps" );
    }
    else
    {
        iTickCount++;
    }

    iHandleEventKeys = EFalse;

	switch (iPressedKeyScanCode)
	{
		case EStdKeyDownArrow:
		{
            NaviDown();
			break;
		}
		case EStdKeyUpArrow:
		{
            NaviUp();
			break;
		}
		case EStdKeyLeftArrow:
		{
			NaviLeft();
			break;
		}
		case EStdKeyRightArrow:
		{
            NaviRight();
			break;
		}
        default:
            break;
	}
	DrawNow();
}

//=============================================================================
TBool CImageEditorRedEyeReductionControl::IsReadyToRender() const
{
    return iReadyToRender;
}

//=============================================================================
void CImageEditorRedEyeReductionControl::GetHelpContext(TCoeHelpContext& aContext) const
{
    aContext.iMajor = TUid::Uid(UID_IMAGE_EDITOR);
    aContext.iContext = KSIE_HLP_REDEYE;
}

//=============================================================================
void CImageEditorRedEyeReductionControl::ClipPosition()
{
    if (iX < KPosParamMin)
    {
        iX = KPosParamMin;
    }
    else if (iX > KPosParamMax)
    {
        iX = KPosParamMax;
    }

    if (iY < KPosParamMin)
    {
        iY = KPosParamMin;
    }
    else if (iY > KPosParamMax)
    {
        iY = KPosParamMax;
    }
}

//=============================================================================
void CImageEditorRedEyeReductionControl::HandlePointerEventL(
                                            const TPointerEvent &aPointerEvent)
    {        
    if( AknLayoutUtils::PenEnabled() )
		{		
		TBool drawNow ( EFalse );
		switch( aPointerEvent.iType )
			{
			case TPointerEvent::EButton1Down:
				{
				iPopupController->HideInfoPopupNote();
			    // Enable dragging if in cursor area tapped 
			    // (both move and resize cases)
			    if ( CursorTapped( aPointerEvent.iPosition ) )
			        {			        
			        iCursorDragEnabled = ETrue;
			        drawNow = EFalse;
			        }
			    else if ( iState == ERedEyeReductionStateResizeSelection )
			        {			        
			        // Tapping outside cursor area just stores the pointer
			        // position
			        if ( !CursorTapped( aPointerEvent.iPosition ) )
			            {
			            iPointerPosition = aPointerEvent.iPosition;
			            drawNow = EFalse;
			            }    
			        }
				
#ifdef RD_TACTILE_FEEDBACK
				if ( iTouchFeedBack )
					{
					iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
					RDebug::Printf( "ImageEditor::ImageEditorRedEyeReductionControl: ETouchFeedback" );
					}
#endif /* RD_TACTILE_FEEDBACK  */
			    
				break;
				}
			case TPointerEvent::EDrag:
				{
				// Sets the new cursor position when drag is enabled
				// (i.e. EButton1Down has happened in cursor area)
                if ( ( iState == ERedEyeReductionStateMoveCrosshair || 
                       iState ==ERedEyeReductionStateResizeSelection ) 
                       && iCursorDragEnabled )
                    {
                    SetCursorPosition( aPointerEvent.iPosition );
			    	// prevents user to move cursor outside image area
			    	ClipPosition();	
			    	drawNow = ETrue;	    
                    }
                    
                // Selection cursor can be grown or made smaller moving
                // pen up and down outside the cursor area
                if ( iState == ERedEyeReductionStateResizeSelection && 
                     !iCursorDragEnabled )
                    {                    
                    if ( CalculateResize( iPointerPosition, 
				                          aPointerEvent.iPosition ) )
                        {
                        iPointerPosition = aPointerEvent.iPosition;
                        drawNow = ETrue;
                        }
                    else
                        {
                        drawNow = EFalse;
                        }    
                    }    
                                                            
				break;		
				}
			case TPointerEvent::EButton1Up:
				{
				// Sets crosshair cursor to a new position
				if ( iState == ERedEyeReductionStateMoveCrosshair )
				    {
				    SetCursorPosition( aPointerEvent.iPosition );
			    	ClipPosition();
			    	drawNow = ETrue;
				    }
			
				ShowTooltip();
				    
				iCursorDragEnabled = EFalse;    
				break;
				}
						
			default:
				{
				break;	
				}	
			}
	
		CCoeControl::HandlePointerEventL( aPointerEvent );
    
        if ( drawNow ) 
            {
            DrawNow();    
            }	
		}
    }

//=============================================================================    
void CImageEditorRedEyeReductionControl::SetCursorPosition( TPoint aNewPosition )
    {    
    //Set new x value
    iX = TReal( aNewPosition.iX - iVisibleImageRectPrev->iTl.iX ) /
			  ( iVisibleImageRectPrev->iBr.iX - iVisibleImageRectPrev->iTl.iX );
    
    //Set new y value
    iY = TReal( aNewPosition.iY - iVisibleImageRectPrev->iTl.iY  ) /
			  ( iVisibleImageRectPrev->iBr.iY - iVisibleImageRectPrev->iTl.iY ); 
    }


//=============================================================================    
TRect CImageEditorRedEyeReductionControl::CrossHairCursorRect() const
    {   
     
    TSize chSize = iCrossHair->SizeInPixels();
	TPoint center = CursorLocation();
    TPoint topLeftCorner(center.iX - (chSize.iWidth >> 1), 
                         center.iY - (chSize.iHeight >> 1));
    
    TRect moveCursorRect( topLeftCorner, chSize );
    
    return moveCursorRect;
    
    }
    
//=============================================================================
TBool CImageEditorRedEyeReductionControl::CursorTapped( TPoint aTappedPosition ) const
    {
    TBool cursorTapped( EFalse );
    TRect cursorRect;
    
    if ( iState == ERedEyeReductionStateMoveCrosshair )
        {
		cursorRect = CrossHairCursorRect();
		
		TPoint newTLCorner = TPoint( cursorRect.iTl.iX - ( cursorRect.Width() / 2 ),
		                             cursorRect.iTl.iY - ( cursorRect.Height() / 2 ) );		                             
		TPoint newBRCorner = TPoint( cursorRect.iBr.iX + ( cursorRect.Width() / 2 ),
		                             cursorRect.iBr.iY + ( cursorRect.Height() / 2 ));
		
		// Make tappable rect double sized to crosshair rect
		TRect doubleCursorRect = TRect( newTLCorner, newBRCorner );
		
		cursorTapped = doubleCursorRect.Contains( aTappedPosition );
		}
	else if ( iState == ERedEyeReductionStateResizeSelection )
	    {
		cursorRect = SelectionRect( CursorLocation() );
		cursorTapped = cursorRect.Contains( aTappedPosition );
		}
		
    return cursorTapped;
    } 

//=============================================================================    
void CImageEditorRedEyeReductionControl::ShowTooltip()
    {   
    iPopupController->HideInfoPopupNote();
    
    if ( iState == ERedEyeReductionStateMoveCrosshair )
        { 
        TPoint center = CursorLocation();
        SDrawUtils::ShowToolTip( iPopupController,
                                 this,
                                 center,
                                 EHLeftVBottom,
                                 *iTooltipLocate );
        }
    else if ( iState == ERedEyeReductionStateResizeSelection )
        {
        TRect circleRect ( SelectionRect( CursorLocation() ) );
        TPoint center( circleRect.Center() );
        
        SDrawUtils::ShowToolTip( iPopupController,
                                 this,
                                 center,
                                 EHLeftVBottom,
                                 *iTooltipOutline );
        }
    }

//=============================================================================
TBool CImageEditorRedEyeReductionControl::CalculateResize( TPoint aStartPoint, 
                                                  TPoint aEndPoint )
    {
    // Whether bubble is resized in this function or not
    TBool radiusChanged( EFalse );
    // Store old scale value
    TReal oldRadius = iR;
    TReal changeThreshold( 0.001 );  
    
    // Get system parameters
    TRect visibleImageRectPrev( iSysPars->VisibleImageRectPrev() );
            
    // Compute change on the screen
    TInt deltaX = aEndPoint.iX - aStartPoint.iX;
    TInt deltaY = aEndPoint.iY - aStartPoint.iY;
   	
   	// Use bigger dimension
	TInt maxChangeInPixels;
	if ( visibleImageRectPrev.Height() > visibleImageRectPrev.Width() )
	    {
	    maxChangeInPixels = visibleImageRectPrev.Height();
	    }
	else
	    {
	    maxChangeInPixels = visibleImageRectPrev.Width();
	    }
	  
	TInt oneStepInPixels =  maxChangeInPixels / KTouchScaleMaxStepCount;
	TReal scaleStep = ( KRadiusParamMax - KRadiusParamMin ) / KTouchScaleMaxStepCount;

    // Relates to second and fourth corners. Defines how steep/gentle the 
    // moving angle has to be in order to scale.
    TInt slopeAngleFactor = 3;
    
    // The first quarter (movement towards upper-right corner)
    if( ( deltaX > 0 && deltaY <= 0 ) || ( deltaX >= 0 && deltaY < 0 ) )
        {
        // use bigger value
        if (Abs( deltaX ) >= Abs( deltaY) )
            {
            iR += scaleStep * ( Abs( deltaX ) / oneStepInPixels );
            }
        else
            {
            iR += scaleStep * ( Abs( deltaY ) / oneStepInPixels );
            }				                             
        }
    // The second (movement towards lower-right corner)  	
    else if( ( deltaX > 0 && deltaY >= 0 ) || ( deltaX >= 0 && deltaY > 0 ) )
        {
        if( deltaX > slopeAngleFactor * deltaY )
            {			                
	        iR += scaleStep * ( Abs( deltaX ) / oneStepInPixels );    
            }
	        			               
        else if ( slopeAngleFactor * deltaX < deltaY )
            {			              
	        iR -= scaleStep * ( Abs( deltaY ) / oneStepInPixels ); 
	        }
        }    
    // The third (movement towards lower-left corner)
    else if( ( deltaX < 0 && deltaY >= 0 ) || ( deltaX <= 0 && deltaY > 0 ) )
        {
        if (Abs( deltaX ) >= Abs( deltaY) )
            {
            iR -= scaleStep * ( Abs( deltaX ) / oneStepInPixels );    
            }
        else
            {
            iR -= scaleStep * ( Abs( deltaY ) / oneStepInPixels ); 
            }
        }
    // The fourth (movement towards upper-left corner)
    else if( ( deltaX < 0 && deltaY <= 0 ) || ( deltaX <= 0 && deltaY < 0 ) )
        {
        if( slopeAngleFactor * Abs( deltaX ) < Abs( deltaY ) )
            {
            iR += scaleStep * ( Abs( deltaY ) / oneStepInPixels ); 
            }
        else if ( Abs( deltaX ) > slopeAngleFactor * Abs( deltaY ) )
            {
            iR -= scaleStep * ( Abs( deltaX ) / oneStepInPixels );    
	        }
        }
    
    // Check the limits        
    if (iR > KRadiusParamMax)
        {
        iR = KRadiusParamMax;
        radiusChanged = ETrue;
        }
    
    if (iR < KRadiusParamMin)
        {
        iR = KRadiusParamMin;
        radiusChanged = ETrue;
        }
                                
    if ( Abs( oldRadius - iR ) > changeThreshold )
        {
        radiusChanged = ETrue;
        }
        
    return radiusChanged;
    
    }    
// End of File
