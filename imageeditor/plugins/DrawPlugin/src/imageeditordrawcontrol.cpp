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
* Image Editor plugin control class.
*
*/


//  INCLUDES
#include "imageeditordrawcontrol.h"
#include "draw.hrh"

#include "ImageEditorUI.hrh"
#include "ImageEditorPluginBase.hrh"

#include "PluginInfo.h"
#include "JpTimer.h"
#include "ImageEditorUids.hrh"
#include "ImageEditorUiDefs.h"
#include "SystemParameters.h"
#include "drawpath.h"
#include "selectionpopup.h"

#include "iepb.h" 
// debug log
#include "imageeditordebugutils.h"

#include <fbs.h>
#include <badesca.h>
#include <gdi.h>
#include <eikenv.h>
#include <bitdev.h>

#include <aknview.h>
#include <aknutils.h>

#include <ImageEditorUI.mbg>
#include <AknInfoPopupNoteController.h> 
#include <draw.rsg> 
#include <bautils.h> 
#include <ConeResLoader.h> 
#include <gulalign.h>
#include <csxhelp/sie.hlp.hrh>

// Log file
_LIT(KDrawPluginLogFile,"drawplugin.log");

//  CONSTANTS
const TInt KWait			    	= 1;
const TInt KMainTextIndex           = 0;

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CImageEditorDrawControl* CImageEditorDrawControl::NewL(
	const TRect& aRect,
	CCoeControl* aParent )
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::NewL()");

    CImageEditorDrawControl * self = new (ELeave) CImageEditorDrawControl;
    CleanupStack::PushL (self);
    self->ConstructL (aRect, aParent);
    CleanupStack::Pop ();   // self
    return self;
    }

// ---------------------------------------------------------------------------
// CImageEditorDrawControl
// ---------------------------------------------------------------------------
//
CImageEditorDrawControl::CImageEditorDrawControl() : 
iTickCount(0),
iNaviStepMultiplier(KDefaultSmallNavigationStepMultiplier),
iDisplayingOkOptionsMenu(EFalse),
iReadyToRender(EFalse),
iResLoader( *ControlEnv() )
{}

// ---------------------------------------------------------------------------
// ~CImageEditorDrawControl()
// ---------------------------------------------------------------------------
//
CImageEditorDrawControl::~CImageEditorDrawControl()
    {
    LOG(KDrawPluginLogFile, 
            "CImageEditorDrawControl::~CImageEditorDrawControl()");    
    delete iTimer;
    delete iIndicator;
    delete iIndicatorMask;
	delete iPopupController;
	delete iTooltipResize;
	iPaths.ResetAndDestroy();
    delete iNaviPaneText;	
	iResLoader.Close();
    }

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::ConstructL(
	const TRect& /*aRect*/, CCoeControl* aParent )
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::ConstructL()");
	//	Set parent window
	SetContainerWindowL(*aParent);
    
    //  Create resource utility
	TFileName resFile;
	// resource file name
	_LIT (KResourceFile, "draw.rsc");	
	resFile.Append(KPgnResourcePath);
	resFile.Append(KResourceFile);

    User::LeaveIfError( CompleteWithAppPath( resFile ) );
    // Implementation of RConeResourceLoader uses BaflUtils::NearestLanguageFile 
    // to search for a localised resource in proper search order
    iResLoader.OpenL( resFile );
    
	//	Create timer for fast key repeat
	iTimer = CJPTimer::NewL( this );

	iPopupController = CAknInfoPopupNoteController::NewL();    
	iTooltipResize = ControlEnv()->AllocReadResourceL(R_TOOLTIP_TEXT_RESIZE);           
	    
    EnableDragEvents();
        
	//	Activate control
    ActivateL();
    }

// ---------------------------------------------------------------------------
// SetView
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::SetView(CAknView* aView)
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::SetView()");
    ASSERT( aView );
    iEditorView = aView;
    }

// ---------------------------------------------------------------------------
// SetSelectedUiItemL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::SetSelectedUiItemL(CPluginInfo* aItem)
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::SetSelectedUiItemL()");
    ASSERT( aItem );
    iItem = aItem;
    delete iNaviPaneText;
    iNaviPaneText = NULL;
    iNaviPaneText = (iItem->Parameters()[KMainTextIndex]).AllocL();
    iEditorView->HandleCommandL(EImageEditorCmdRender);
    }

// ---------------------------------------------------------------------------
// OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CImageEditorDrawControl::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType)
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::OfferKeyEventL()");

    TKeyResponse response = EKeyWasNotConsumed;

    //  If busy, do not handle anything
    if ( Busy() )
    {
        response = EKeyWasConsumed;
    }

    // In main state handle the OK Options menu
    else if ( aKeyEvent.iCode == EKeyOK )
        {
        iDisplayingOkOptionsMenu = ETrue;
        iEditorView->HandleCommandL (EImageEditorTryDisplayMenuBar);
        response = EKeyWasConsumed;
        }

    //  We handle only event keys
    else if (EEventKey == aType)
    {

		switch (aKeyEvent.iCode)
		{
		
			case EKeyDownArrow:
			case EKeyUpArrow:
			case EKeyRightArrow:
            case EKeyLeftArrow:
			{		
			    SetPositionOnImage( ComputeIndicatorPosition() );
		    	StoreParameters( EFalse, EFalse );			    
				response = EKeyWasConsumed;
                break;
			}
		
            case EKeyOK:
                {           
                break;
                }

		    case 0x30: // 0		    		    
		    case 0x32: // 2
		    case 0x34: // 4
		    case 0x35: // 5
		    case 0x36: // 6
		    case 0x38: // 8
			case EStdKeyIncVolume: // zoom in key
			case EStdKeyDecVolume: // zoom out key
		    {
		    	StorePosAndScaleRelScreen();
		    	break;
		    }

			default:
			{
				break;
			}
		}
	}

	//	Key pressed down, mark pressed key
	else if (aType == EEventKeyDown)
	{
		switch (aKeyEvent.iScanCode)
		{
			case EStdKeyUpArrow:
			{
				iKeyCode = 1;
			    response = EKeyWasConsumed;
				break;
			}
			case EStdKeyDownArrow:
			{
				iKeyCode = 2;
			    response = EKeyWasConsumed;
				break;
			}
			case EStdKeyLeftArrow:
			{
				iKeyCode = 3;
			    response = EKeyWasConsumed;
				break;
			}
			case EStdKeyRightArrow:
			{
				iKeyCode = 4;
			    response = EKeyWasConsumed;
				break;
			}
		}

		if ( iKeyCode != 0 )
		{
		    iNaviStepMultiplier = KDefaultSmallNavigationStepMultiplier;
		    iTickCount = 0;
			iTimer->Call( KWait );
		}
	}
	
	//	Key released, mark all keys to zero
	else if (aType == EEventKeyUp)
	{
		switch (aKeyEvent.iScanCode)
		{
			case EStdKeyUpArrow:
			case EStdKeyDownArrow:
			case EStdKeyLeftArrow:
			case EStdKeyRightArrow:
			{
				iKeyCode = 0;
			    response = EKeyWasConsumed;
			    ShowTooltip();
				break;
			}
		}
	}

    return response;
    }

// ---------------------------------------------------------------------------
// SizeChanged
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::SizeChanged()
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::SizeChanged()");
    }

// ---------------------------------------------------------------------------
// GetParam
// ---------------------------------------------------------------------------
//
TDesC& CImageEditorDrawControl::GetParam()
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::GetParam()");
    LOGDES(KDrawPluginLogFile, iParam);
	return iParam;
    }

// ---------------------------------------------------------------------------
// SetSystemParameters
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::SetSystemParameters(const CSystemParameters* aPars) 
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::SetSystemParameters()");
    ASSERT( aPars );
    iSysPars = aPars;
    }

// ---------------------------------------------------------------------------
// HandlePluginCommandL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::HandlePluginCommandL(const TInt aCommand)
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::HandlePluginCommandL()");

    switch (aCommand) 
        {
        case EImageEditorFocusLost:
            {
            if (iTimer)
                {
                iTimer->Cancel();
                }
            break;
            }
        case EPgnSoftkeyIdOk:
            {
            iPopupController->HideInfoPopupNote();
            break;
            }
        case EPgnSoftkeyIdCancel:
            {
            iPopupController->HideInfoPopupNote();
            iEditorView->HandleCommandL (EImageEditorCancelPlugin);
            break;
            }
        case EPgnSoftkeyIdDone:
            // FALLTROUHG
        case EDrawPgnMenuCmdDone:
            {   
            iPopupController->HideInfoPopupNote();
            StoreParameters(EFalse, ETrue);
            iEditorView->HandleCommandL( EImageEditorCmdRender );//results Draw call
            iEditorView->HandleCommandL(EImageEditorApplyPlugin);
            break;
            }
        case EDrawPgnMenuCmdResize:
            {
            StoreParameters(ETrue, ETrue);
            iEditorView->HandleCommandL( EImageEditorCmdRender );//results Draw call
            SelectSizeL();         
            break;
            }
        case EDrawPgnMenuCmdColor:
            {
            StoreParameters(ETrue, ETrue);
            iEditorView->HandleCommandL( EImageEditorCmdRender );//results Draw call            
            SDrawUtils::LaunchColorSelectionPopupL(iPreview, Rect(), iRgb);
            break;
            }
        case EDrawPgnMenuCmdCancel:
            {
			iReadyToRender = EFalse;
            iEditorView->HandleCommandL(EImageEditorCancelPlugin);
            break;
            }
        
        case EImageEditorGlobalZoomChanged:
            // FALLTROUGHT
        case EImageEditorGlobalPanChanged:
            {
			RestorePosAndScaleRelScreen();
			//StoreParameters();
            StoreTempParams();
	    	iEditorView->HandleCommandL (EImageEditorCmdRender);

			//DrawNow();
	        break;
            }
        
        case EImageEditorPreScreenModeChange:
            {
			StorePosAndScaleRelImage();
        	break;
            }
        
        case EImageEditorPostScreenModeChange:
            {
			RestorePosAndScaleRelImage();
			ClipPosition();
			//StoreParameters();
            StoreTempParams();
	    	iEditorView->HandleCommandL (EImageEditorCmdRender);
			//DrawNow();
        	break;
            }

        default:
            {
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// GetSoftkeyIndexL
// ---------------------------------------------------------------------------
//
TInt CImageEditorDrawControl::GetSoftkeyIndexL()
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::GetSoftkeyIndexL()");
    // : Check needed states
    TInt state(2);
    return state;
    }

// ---------------------------------------------------------------------------
// GetContextMenuResourceId
// ---------------------------------------------------------------------------
//
TInt CImageEditorDrawControl::GetContextMenuResourceId()
    {
    return R_TEXT_CONTEXT_MENUBAR;    
    }

// ---------------------------------------------------------------------------
// GetDimmedMenuItems
// ---------------------------------------------------------------------------
//
TBitField CImageEditorDrawControl::GetDimmedMenuItems()
{
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::GetDimmedMenuItems()");

    TBitField dimmedMenuItems;
    TInt count = iItem->MenuItems().Count();

    if ( iDisplayingOkOptionsMenu )
    {
        // Dim the command EImageEditorCancelPlugin
        for ( TInt i = 0; i < count; i++)
        {
            // get the menu item id
            TInt menuItem = iItem->MenuItems().At(i).iCommandId;
            if ( menuItem == EDrawPgnMenuCmdCancel ) 
            {
                dimmedMenuItems.SetBit( i );
            }
        }
        iDisplayingOkOptionsMenu = EFalse;
    }
    
    for ( TInt i = 0; i < count; i++)
    {
        // get the menu item id
        TInt menuItem = iItem->MenuItems().At(i).iCommandId;
        if ( menuItem == EDrawPgnMenuCmdMax )
        	{
        	dimmedMenuItems.SetBit( i );
        	}    
    }
    return dimmedMenuItems;
}

// ---------------------------------------------------------------------------
// GetNaviPaneTextL
// ---------------------------------------------------------------------------
//
TPtrC CImageEditorDrawControl::GetNaviPaneTextL(
    TBool& aLeftNaviPaneScrollButtonVisibile, 
    TBool& aRightNaviPaneScrollButtonVisible )
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::GetNaviPaneTextL()");

    aLeftNaviPaneScrollButtonVisibile = EFalse;
    aRightNaviPaneScrollButtonVisible = EFalse;
    return *iNaviPaneText;
    }

// ---------------------------------------------------------------------------
// Draw
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::Draw(const TRect& aRect) const
    {
    CWindowGc & gc = SystemGc();
    if( IsReadyToRender() )
        {
        CPreviewControlBase::DrawPreviewImage(aRect);
        }

    if( iPaths.Count() )
        {
        ASSERT( iSysPars ); 
        for( TInt i(0); i<iPaths.Count(); ++i)
            {
            CDrawPath* path = iPaths[i];
            CArrayFix<TPoint>* pathPoints = path->ItemArray();
            gc.SetPenStyle( CGraphicsContext::ESolidPen );
            gc.SetPenColor( path->Color() );
            gc.SetPenSize( ScaledLineSize( path->Size() ) );
            gc.DrawPolyLine( pathPoints );
            }
        }
    
  /*  
   * : Too slow cause we need to redraw preview image everytime
   * if( iIndicator && iIndicator->Handle() && 
         iIndicatorMask && iIndicatorMask->Handle() )
        {                
        gc.BitBltMasked ( 
            ComputeIndicatorPosition(),
            iIndicator, 
            TRect (iIndicator->SizeInPixels()), 
            iIndicatorMask, 
            EFalse
            );
        }*/       	
    }

// ---------------------------------------------------------------------------
// NaviDown
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::NaviDown()
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::NaviDown()");
    }

// ---------------------------------------------------------------------------
// NaviUp
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::NaviUp()
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::NaviUp()");
    }

// ---------------------------------------------------------------------------
// NaviRight
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::NaviRight()
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::NaviRight()");
    }

// ---------------------------------------------------------------------------
// NaviLeft
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::NaviLeft()
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::NaviLeft()");
    }

// ---------------------------------------------------------------------------
// SelectSizeL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::SelectSizeL()
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::SelectSizeL()");
    // Symmetric line width / height
    TInt size(iSize.iWidth);
    CSelectionDialog::RunDlgLD( iPreview, Rect(), size );
    iSize = TSize(size,size);
    }

// ---------------------------------------------------------------------------
// StoreTempParams
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::StoreTempParams()
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::StoreTempParams()");
    }

// ---------------------------------------------------------------------------
// RestoreTempParams
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::RestoreTempParams()
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::RestoreTempParams()");
    }

// ---------------------------------------------------------------------------
// GetHelpContext
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::GetHelpContext(TCoeHelpContext& aContext) const
{
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::GetHelpContext()");

    aContext.iMajor = TUid::Uid(UID_IMAGE_EDITOR);
    aContext.iContext = KSIE_HLP_EDIT_TEXT;
}

// ---------------------------------------------------------------------------
// PrepareL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::PrepareL()
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::PrepareL()");
    
	//	Get current view port
	TRect rect = iSysPars->VisibleImageRectPrev();
	
	//	Set default position
	iX = (rect.iTl.iX + rect.iBr.iX) / 2;
	iY = (rect.iTl.iY + rect.iBr.iY) / 2;
	
	//	Set default color to white
	iRgb = KRgbWhite;

	// Set default size
	TInt defaultSize(8);
	iSize = TSize(defaultSize, defaultSize);
    iReadyToRender = ETrue;

    iEditorView->HandleCommandL( EImageEditorCmdRender );        
    }

// ---------------------------------------------------------------------------
// LoadIndicatorL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::LoadIndicatorL(
    TInt aBitmapInd, TInt aMaskInd ) 
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::LoadIndicatorL()");

    //  Delete old indicator
    delete iIndicator;
    iIndicator = 0;
    delete iIndicatorMask;
    iIndicatorMask = 0;

    //  Load new indicator
	SDrawUtils::GetIndicatorBitmapL (
		iIndicator,
		iIndicatorMask,
		aBitmapInd,
		aMaskInd
		);
    }

// ---------------------------------------------------------------------------
// ComputeIndicatorPosition
// ---------------------------------------------------------------------------
//
TPoint CImageEditorDrawControl::ComputeIndicatorPosition() const
	{
	TRect vprect = iSysPars->VisibleImageRect();
	TRect vpprect = iSysPars->VisibleImageRectPrev();
    
    TInt x( 0 );
	TInt y( 0 );
	// check if there is no indicator
	if ( !iIndicator )
	    {
	    return TPoint ( x, y );
	    }
	    
	y = ((iY - vprect.iTl.iY) * vpprect.Height()) / vprect.Height();
	y += vpprect.iTl.iY;
	//y -= iIndicator->SizeInPixels().iHeight; /// 4;

	x = ((iX  - vprect.iTl.iX) * vpprect.Width()) / vprect.Width();
	x += vpprect.iTl.iX;
	//x -= iIndicator->SizeInPixels().iWidth / 2;

    return TPoint (x,y);
	}

// ---------------------------------------------------------------------------
// IsReadyToRender
// ---------------------------------------------------------------------------
//
TBool CImageEditorDrawControl::IsReadyToRender() const
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::IsReadyToRender()");
	return iReadyToRender;
    }

// ---------------------------------------------------------------------------
// StoreParameters
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::StoreParameters(TBool aLastItem, TBool aDone)
    {
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::StoreParameters()");
    RDebug::Print(_L("CImageEditorDrawControl::StoreParameters color[%d]"), 
    	iRgb.Internal());    

	iParam.Copy(_L("x "));
	iParam.AppendNum (iX);
	iParam.Append(_L(" y "));
	iParam.AppendNum (iY);
	
	if( aLastItem )
		{	
	    iParam.Append(_L(" size "));
	    iParam.AppendNum (iSize.iHeight);
	    iParam.Append(_L(" color "));
	    iParam.AppendNum ( iRgb.Value() );  		
		iParam.Append(_L(" lastItem "));
		}	
    if( aDone )
        {   
        iParam.Append(_L("done"));
        }
    }

// ---------------------------------------------------------------------------
// TimerCallBack
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::TimerCallBack()
{
    LOG(KDrawPluginLogFile, "CImageEditorDrawControl::TimerCallBack()");

    if (iTickCount > KDefaultFastKeyTimerMultiplyThresholdInTicks)
    {
        iNaviStepMultiplier = KDefaultBigNavigationStepMultiplier;
    }
    else
    {
        iTickCount++;
    }

	if (iKeyCode)
	{

		switch (iKeyCode)
		{
			case 1:
			{
	            NaviUp();
				break;
			}
			case 2:
			{
	            NaviDown();
				break;
			}
			case 3:
			{
				NaviLeft();
				break;
			}
			case 4:
			{
	            NaviRight();
				break;
			}
	        default:
	            break;
		}
	    //StoreParameters();
	    TRAP_IGNORE( iEditorView->HandleCommandL(EImageEditorCmdRender) );
		iTimer->Call (KWait);			
	}
}

// ---------------------------------------------------------------------------
// StorePosAndScaleRelScreen
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::StorePosAndScaleRelScreen()
    {
    LOG(KDrawPluginLogFile, 
            "CImageEditorClipartControl::StorePosAndScaleRelScreen()");
	iParam.Copy(_L("nop"));	
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::RestorePosAndScaleRelScreen()
    {
	LOG(KDrawPluginLogFile, 
	        "CImageEditorClipartControl::RestorePosAndScaleRelScreen()");
    }


// ---------------------------------------------------------------------------
// StorePosAndScaleRelImage
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::StorePosAndScaleRelImage()
    {
    TReal relscale = iSysPars->RelScale();    
    LOGFMT(KDrawPluginLogFile,
            "CImageEditorClipartControl::StorePosAndScaleRelImage():%g",
            relscale);
    }

// ---------------------------------------------------------------------------
// RestorePosAndScaleRelImage
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::RestorePosAndScaleRelImage()
    {
    TReal relscale = iSysPars->RelScale();
    LOGFMT(KDrawPluginLogFile,
            "CImageEditorClipartControl::RestorePosAndScaleRelImage():%g",
            relscale);
    }

// ---------------------------------------------------------------------------
// ClipPosition
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::ClipPosition()
{
    if ( iX < iSysPars->VisibleImageRect().iTl.iX )
    {
    	iX = iSysPars->VisibleImageRect().iTl.iX;
    }
    else if ( iX > iSysPars->VisibleImageRect().iBr.iX )
    {
    	iX = iSysPars->VisibleImageRect().iBr.iX;
    }
    
    if ( iY < iSysPars->VisibleImageRect().iTl.iY )
    {
    	iY = iSysPars->VisibleImageRect().iTl.iY;
    }
    else if ( iY > iSysPars->VisibleImageRect().iBr.iY )
    {
    	iY = iSysPars->VisibleImageRect().iBr.iY;
    }
}

// ---------------------------------------------------------------------------
// HandlePointerEventL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::HandlePointerEventL(
    const TPointerEvent &aPointerEvent)
    {
    RDebug::Print(_L("CImageEditorDrawControl::HandlePointerEventL pen[%d]"), 
    	AknLayoutUtils::PenEnabled());        
    if( AknLayoutUtils::PenEnabled() && !Busy())
		{
		RDebug::Print(_L("CImageEditorDrawControl::iType [%d]"), 
			aPointerEvent.iType);
		TBool lastItem( EFalse );
		switch( aPointerEvent.iType )
			{
			case TPointerEvent::EButton1Down:
				{
				iReadyToRender = EFalse;

				CDrawPath* lastPath = CDrawPath::NewL();
				CleanupStack::PushL( lastPath );
				lastPath->SetColor( iRgb );
				lastPath->SetSize( iSize );								
                SetPositionOnImage( aPointerEvent.iPosition );
                lastPath->AddItemL( aPointerEvent.iPosition );
                iPaths.AppendL( lastPath );
                CleanupStack::Pop( lastPath );
                break;
				}
			case TPointerEvent::EDrag:
				{  
				if( iPaths.Count() )
    				{
    				CDrawPath* path = iPaths[iPaths.Count()-1];
    				// store current position for next round
                    iPointerPosition = aPointerEvent.iPosition;    
    			    SetPositionOnImage( aPointerEvent.iPosition );
    			    path->AddItemL( aPointerEvent.iPosition );
    				}
				break;		
				}
			case TPointerEvent::EButton1Up:
				{
				iReadyToRender = ETrue;
				ShowTooltip();
				lastItem = ETrue;
				break;
				}
						
			default:
				{
				break;	
				}	
			}
			
    	StoreParameters( lastItem, EFalse ); 
	    iEditorView->HandleCommandL( EImageEditorCmdRender );//results Draw call
		CCoeControl::HandlePointerEventL( aPointerEvent );
		}
    }

// ---------------------------------------------------------------------------
// SetPositionOnImage
// ---------------------------------------------------------------------------
//    
void CImageEditorDrawControl::SetPositionOnImage( TPoint aPointedPosition )
    {    
    ASSERT( iSysPars );
    // Get system parameters
   	TRect visibleImageRect( iSysPars->VisibleImageRect() );
   	TRect visibleImageRectPrev( iSysPars->VisibleImageRectPrev() );

   	TInt xPosFactorDivider( visibleImageRectPrev.Width() );
   	TInt yPosFactorDivider( visibleImageRectPrev.Height() );
    
   	LOGFMT2(KDrawPluginLogFile, "CImageEditorDrawControl::vir w:%d h:%d", 
   	        visibleImageRect.Width(), visibleImageRect.Height());
    LOGFMT2(KDrawPluginLogFile, "CImageEditorDrawControl::virp w:%d h:%d", 
            visibleImageRectPrev.Width(), visibleImageRectPrev.Height());
   	   	
   	// Dividing by zero will cause panic -> check
   	if ( xPosFactorDivider == 0 || yPosFactorDivider == 0 )
   	    {
   	    //	Set default position
    	iX = (visibleImageRect.iTl.iX + visibleImageRect.iBr.iX) / 2;
    	iY = (visibleImageRect.iTl.iY + visibleImageRect.iBr.iY) / 2;
   	    }
   	else
   	    {
   	    // Calculate relative position on the screen
   	    TReal xPositionFactor 
   	             ( TReal( aPointedPosition.iX - visibleImageRectPrev.iTl.iX ) /
	             xPosFactorDivider );
	                        
    	TReal yPositionFactor 
    	         ( TReal( aPointedPosition.iY - visibleImageRectPrev.iTl.iY ) /
		         yPosFactorDivider );
        LOGFMT2(KDrawPluginLogFile, "CImageEditorDrawControl::pfx:%g pfy:%g", 
                xPositionFactor, yPositionFactor);		
		// Calculate position on visible image		

   	    iX = visibleImageRect.iTl.iX + 
   	        visibleImageRect.Width() * xPositionFactor + 0.5;	    
    	
    	iY = visibleImageRect.iTl.iY + 
    	    visibleImageRect.Height() * yPositionFactor + 0.5;    	
   	    }    

    LOGFMT4(KDrawPluginLogFile, "CImageEditorDrawControl::Draw x:%d iX:%d y:%d iY:%d", 
            aPointedPosition.iX, iX, aPointedPosition.iY, iY);
	// Check that not out of bounds    
    //ClipPosition();          
    }
    
// ---------------------------------------------------------------------------
// GetPositionOnView
// ---------------------------------------------------------------------------
// 
TPoint CImageEditorDrawControl::GetPositionOnView()
    {
    ASSERT( iSysPars );
    // Get system parameters 
    TRect visibleImageRect( iSysPars->VisibleImageRect() );
    TRect visibleImageRectPrev( iSysPars->VisibleImageRectPrev() );

    TInt xPosFactorDivider( visibleImageRectPrev.Width() );
    TInt yPosFactorDivider( visibleImageRectPrev.Height() );
    
    // Calculate relative position on the screen
    TReal xPositionFactor( TReal( iX + visibleImageRectPrev.iTl.iX ) /
             xPosFactorDivider );
                        
    TReal yPositionFactor( TReal( iY + visibleImageRectPrev.iTl.iY ) /
             yPosFactorDivider );    

    LOGFMT2(KDrawPluginLogFile, 
            "CImageEditorDrawControl::GetPositionOnView xf:%g yf:%g", 
            xPositionFactor , yPositionFactor );    
    
    LOGFMT2(KDrawPluginLogFile, 
            "CImageEditorDrawControl::GetPositionOnView xf:%d yf:%d", 
            xPosFactorDivider , yPosFactorDivider );     
    
    LOGFMT2(KDrawPluginLogFile, 
            "CImageEditorDrawControl::GetPositionOnView w:%d h:%d", 
            iX , iY );  
    
    TInt xPosition(iX + visibleImageRectPrev.iTl.iX);
    TInt yPosition(iY + visibleImageRectPrev.iTl.iY);  
    
    LOGFMT3(KDrawPluginLogFile, 
            "CImageEditorDrawControl::GetPositionOnView w:%d h:%d S:%g", 
            xPosition, yPosition, iSysPars->RelScale());  
    
    return TPoint(xPosition, yPosition);
    }

// ---------------------------------------------------------------------------
// ShowTooltip
// ---------------------------------------------------------------------------
//    
void CImageEditorDrawControl::ShowTooltip()
    {   
    iPopupController->HideInfoPopupNote();

    TPoint iconPosition = ComputeIndicatorPosition();
    TRect iconRect( iconPosition.iX, iconPosition.iY, 
                    iconPosition.iX, iconPosition.iY );

                              
    /*SDrawUtils::ShowToolTip( iPopupController,
                             this,
                             iconPosition,
                             EHRightVTop, 
                             *iTooltipResize );                                 
    */
    
    }

// ---------------------------------------------------------------------------
// ScaledLineSize
// ---------------------------------------------------------------------------
//    
TSize CImageEditorDrawControl::ScaledLineSize( TSize aOriginalSize ) const 
    {
    TInt sizew(aOriginalSize.iWidth * iSysPars->RelScale() + 0.5);
    TInt sizeh(aOriginalSize.iHeight * iSysPars->RelScale() + 0.5);
    if( sizew < 1 )
        {
        sizew++;
        }
    if( sizeh < 1 )
        {
        sizeh++;
        }

    LOGFMT3(KDrawPluginLogFile, 
            "CImageEditorDrawControl::ScaledLineSize w:%d h:%d S:%g", 
            sizew, sizeh, iSysPars->RelScale());    
    return TSize(sizew , sizeh);
    }
        
// End of File
