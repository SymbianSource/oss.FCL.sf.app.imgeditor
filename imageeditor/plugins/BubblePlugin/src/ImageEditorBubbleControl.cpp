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
* Talk bubble plugin control implementation.
*
*/



//  INCLUDES
#include <fbs.h>
#include <badesca.h>
#include <bautils.h>
#include <gdi.h>
#include <eikenv.h>
#include <bitdev.h>
#include <aknview.h>
#include <aknutils.h>
#include <e32math.h>

#ifdef RD_TACTILE_FEEDBACK 
#include <touchfeedback.h>
#endif /* RD_TACTILE_FEEDBACK  */

#include <ImageEditorUI.mbg>

#include <aknquerydialog.h> 
#include <avkon.rsg>
#include <AknInfoPopupNoteController.h> 
#include <bubble.rsg> 
#include <bautils.h> 
#include <ConeResLoader.h> 
#include <csxhelp/sie.hlp.hrh>
 
#include "ImageEditorUI.hrh"
#include "ImageEditorPluginBase.hrh"
#include "Bubble.hrh"
#include "JpTimer.h"
#include "PluginInfo.h"
#include "ImageEditorUids.hrh"
#include "ImageEditorUiDefs.h"  
#include "iepb.h"
#include "SystemParameters.h"
#include "ResolutionUtil.h"

#include "ImageEditorBubbleControl.h"
#include "BubbleSelectionGrid.h"

//#include "platform_security_literals.hrh"

// debug log
#include "imageeditordebugutils.h"
_LIT(KBubblePluginLogFile,"BubblePlugin.log");

//  CONSTANTS

//	TIMER WAIT
const TInt KWait					= 100;

//	Bubble POSITION
const TInt KPosParamStep			= 8;

//	Bubble SCALE
const TInt KScaleParamStep			= 50;
const TInt KTouchScaleMaxStepCount  = 20;

//	Bubble ANGLE
const TInt KDegreeMultiplier        = 1000;
const TInt KAngleParamMax			= 359000;
const TInt KAngleParam90Deg         = 90000;
const TInt KAngleParamDef			= 0;
const TInt KAngleParamStep			= 2000;

//	RESOURCE INDICES
const TInt KSelectBubbleIndex		= 0;
const TInt KMainBubbleIndex		    = 1;
const TInt KMoveBubbleIndex	    	= 2;
const TInt KResizeBubbleIndex		= 3;
const TInt KRotateBubbleIndex		= 4;

_LIT (KPgnResourceFile, "Bubble.rsc");
_LIT(KComponentName, "ImageEditorBubblePlugin");

//=============================================================================
CImageEditorBubbleControl * CImageEditorBubbleControl::NewL (
	const TRect &		aRect,
	CCoeControl	*		aParent
	)
{
    CImageEditorBubbleControl * self = new (ELeave) CImageEditorBubbleControl;
    CleanupStack::PushL (self);
    self->ConstructL (aRect, aParent);
    CleanupStack::Pop ();   // self
    return self;
}

//=============================================================================
CImageEditorBubbleControl::CImageEditorBubbleControl () : 
iState (EInsertBubbleStateMin),
iResLoader ( * ControlEnv() ),
iTickCount (0),
iNaviStepMultiplier (KDefaultSmallNavigationStepMultiplier),
iDisplayingOkOptionsMenu(EFalse),
iReadyToRender(EFalse)
{

}

//=============================================================================
CImageEditorBubbleControl::~CImageEditorBubbleControl ()
{
    delete iIndicator;
    delete iIndicatorMask;
    delete iTimer;
    iResLoader.Close();
    iSysPars = NULL;
    iEditorView = NULL;
    iItem = NULL;
    if (iPopupList)
    {
        delete iPopupList;
        iPopupList = NULL;
    }
	delete iPopupController;
	delete iTooltipResize;
	delete iTooltipMove;
	delete iTooltipRotate;
}

//=============================================================================
void CImageEditorBubbleControl::ConstructL (
	const TRect &		/*aRect*/,
	CCoeControl	*		aParent
	)
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::ConstructL()");

	//	Set parent window
	SetContainerWindowL (*aParent);

    //  Create resource utility
	TFileName resFile;
	resFile.Append(KPgnResourcePath);
	resFile.Append(KPgnResourceFile);

    User::LeaveIfError( CompleteWithAppPath( resFile ) );
    // Implementation of RConeResourceLoader uses BaflUtils::NearestLanguageFile 
    // to search for a localised resource in proper search order
    iResLoader.OpenL ( resFile );

	//	Create timer for fast key repeat
	iTimer = CJPTimer::NewL( this );

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
	
		iTooltipResize = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_BUBBLE_RESIZE);    
		iTooltipMove = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_BUBBLE_MOVE);    
		iTooltipRotate = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_BUBBLE_ROTATE);          
    CleanupStack::PopAndDestroy(); // resLoader		  

#ifdef RD_TACTILE_FEEDBACK 
    iTouchFeedBack = MTouchFeedback::Instance();
#endif /* RD_TACTILE_FEEDBACK  */
    
	//	Activate control
    ActivateL();
    
    EnableDragEvents();
    
}

//=============================================================================
void CImageEditorBubbleControl::PrepareL ()
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::PrepareL()");

    // Set busy flag untill the bitmap has been decoded
    SetBusy();

    // Main view navi pane text
    iNaviPaneText.Copy ( iItem->Parameters()[KSelectBubbleIndex] );

    // Popup list to contain the grid
    iPopupList = new (ELeave) CBubbleSelectionDialog(iBubbleFileName, iBubbleFileNameIndex);
    iPopupList->ConstructL(this);   
    
}

//=============================================================================
TBool CImageEditorBubbleControl::IsReadyToRender() const
{
    return iReadyToRender;
}


//=============================================================================
void CImageEditorBubbleControl::HandleControlEventL(CCoeControl* /*aControl*/ ,TCoeEvent aEventType)
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::HandleControlEventL()");

    // When the icons have been decoded, launch the grid.
    if (aEventType == EEventStateChanged)
    {
        __ASSERT_ALWAYS( iPopupList, User::Panic( KComponentName, KErrNotReady));
		
		TBool fullscreen = CResolutionUtil::Self()->GetFullScreen();
		if (fullscreen)
		{
			iEditorView->HandleCommandL (EImageEditorMenuCmdNormalScreen);			
		}
        TInt popupOk = iPopupList->ExecuteLD(R_BUBBLE_SELECTION_DIALOG);	
        iPopupList = NULL;
        
		if (fullscreen)
		{
			iEditorView->HandleCommandL (EImageEditorMenuCmdFullScreen);
		}

        if (popupOk)
        {
            ToMoveStateL();
            SelectBubbleL();
            StoreTempParams();
            ResetBusy();
        }
        else
        {
    		LOG(KBubblePluginLogFile, "HandleControlEventL: Plugin cancelled from popup.");
            iState = EInsertBubbleStateMin;
            iEditorView->HandleCommandL (EImageEditorCancelPlugin);
        }
    }
    
    // Cancelled the grid
    else if (aEventType == EEventRequestCancel)
    {
		LOG(KBubblePluginLogFile, "HandleControlEventL: Grid cancelled.");
        iState = EInsertBubbleStateMin;
        iEditorView->HandleCommandL (EImageEditorCancelPlugin);
    }
}

//=============================================================================
void CImageEditorBubbleControl::HandleListBoxEventL(CEikListBox* /*aListBox*/, 
                                                     TListBoxEvent /*aEventType*/)
    {    
    }



//=============================================================================
void CImageEditorBubbleControl::SelectBubbleL()
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::SelectBubbleL()");

	TRect rect = iSysPars->VisibleImageRect();
	
	//	Set default position
	iX = (rect.iTl.iX + rect.iBr.iX) / 2;
	iY = (rect.iTl.iY + rect.iBr.iY) / 2;
	
	//	Set default angle
	iAngle = KAngleParamDef;

	//	Set default scale
	if ( rect.Width() > rect.Height() )
	{
		iScale = rect.Height() / 2;
		iScaleMin = rect.Height() / 10;
		iScaleMax = rect.Height();
		iScaleToHeight = ETrue;
	}
	else
	{
		iScale = rect.Width() / 2;
		iScaleMin = rect.Width() / 10;
		iScaleMax = rect.Width();
		iScaleToHeight = EFalse;
	}
	
	iParam.Copy (_L("file \""));
	iParam.Append (iBubbleFileName);
	iParam.Append (_L("\""));

	iParam.Append (_L(" bubble "));
	iParam.AppendNum (iBubbleFileNameIndex);
	iParam.Append (_L(" mask "));
	iParam.AppendNum (iBubbleFileNameIndex + 1);
	iParam.Append (_L(" load"));

	iParam.Append (_L(" x "));
	iParam.AppendNum (iX);

	iParam.Append (_L(" y "));
	iParam.AppendNum (iY);

	iParam.Append (_L(" angle "));
	iParam.AppendNum (iAngle);

	if ( iScaleToHeight )
	{
		iParam.Append (_L(" height "));
		iParam.AppendNum (iScale);
	}
	else
	{
		iParam.Append (_L(" width "));
		iParam.AppendNum (iScale);
	}

	ToMoveStateL();
	
    LOGDES(KBubblePluginLogFile, iParam);

    LOG(KBubblePluginLogFile, "SelectBubbleL: Ready to render");

    iReadyToRender = ETrue;

    iEditorView->HandleCommandL (EImageEditorCmdRender);

    LOG(KBubblePluginLogFile, "SelectBubbleL: Rendered");

}

//=============================================================================
void CImageEditorBubbleControl::SetView (CAknView * aView)
{
    iEditorView = aView;
}

//=============================================================================
void CImageEditorBubbleControl::SetSelectedUiItemL (CPluginInfo * aItem)
{
    iItem = aItem;
}

//=============================================================================
void CImageEditorBubbleControl::SetBusy ()
{
    CImageEditorControlBase::SetBusy();
    if ( iPopupList )
	{
		iPopupList->SetBusy(ETrue);
	}
}

//=============================================================================
void CImageEditorBubbleControl::ResetBusy ()
{
    // Reset busy flag only if selection grid is constructed
    if (iState != EInsertBubbleStateMin)
    {
        CImageEditorControlBase::ResetBusy();
		
		if (iPopupList)
		{
			iPopupList->SetBusy(EFalse);	
		}
		
    }
}

//=============================================================================
TKeyResponse CImageEditorBubbleControl::OfferKeyEventL (
    const TKeyEvent &   aKeyEvent,
    TEventCode          aType
    )
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::OfferKeyEventL()");
#ifdef VERBOSE
    LOGFMT(KBubblePluginLogFile, "\taType: %d", aType);
    LOGFMT(KBubblePluginLogFile, "\tiTickCount: %d", iTickCount);
#endif

    TKeyResponse response = EKeyWasNotConsumed;

    //  If busy, do not handle anything
    if ( Busy() )
    {
        response = EKeyWasConsumed;
    }

    // In main state handle the OK Options menu
    else if ( aKeyEvent.iCode == EKeyOK &&
        (iState == EInsertBubbleStateMain || iState == EInsertBubbleStateFirst ) )
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
            // Just consume keys
			case EKeyDownArrow:
			case EKeyUpArrow:
			case EKeyRightArrow:
            case EKeyLeftArrow:
			{
				response = EKeyWasConsumed;
                break;
			}
			
            case EKeyOK:
            {
                if (iState == EInsertBubbleStateMove || 
                    iState == EInsertBubbleStateRotate  ||
                    iState == EInsertBubbleStateResize)
                {
                    ToMainStateL();
					DrawNow();
                    response = EKeyWasConsumed;
                }
                break;
            }
            
            case EKeyEnter:
            {   
                if (iState == EInsertBubbleStateMove || 
                    iState == EInsertBubbleStateRotate  ||
                    iState == EInsertBubbleStateResize)
                {   
                    ToMainStateL();
                    DrawNow();
                }
                else if ( iState == EInsertBubbleStateMain )
                {
                    // Show context sensitive menu
        			iEditorView->HandleCommandL( EImageEditorOpenContextMenu );
                }
                response = EKeyWasConsumed;
                break;                
    			    
            }
            
            case 0x31: // 1		    			    
		    {
		        // Rotate only in rotate state
                if ( iState == EInsertBubbleStateRotate )
				{
                    iAngle -= KAngleParam90Deg;
                    StoreParameters();
    	            TRAP_IGNORE( iEditorView->HandleCommandL (EImageEditorCmdRender) );
				}
	    		break;
		    }
		    
		    case 0x33: // 3		    			    
		    {   
		        // Rotate only in rotate state
		        if ( iState == EInsertBubbleStateRotate )
				{
                    iAngle += KAngleParam90Deg;
                    StoreParameters();
    	            TRAP_IGNORE( iEditorView->HandleCommandL (EImageEditorCmdRender) );
				}				
	    		break;
		    }
		    
		    case 0x30: // 0	    		    
		    case 0x32: // 2
		    case 0x34: // 4		    
		    case 0x36: // 6
		    case 0x38: // 8
		    case EStdKeyDecVolume: // zoom out key
		    case 0x23: // #			    
		    {
	    		StorePosAndScaleRelScreen();
				break;
		    }
		    
		    case 0x35: // 5
		    case 0x2a: // *		    
			case EStdKeyIncVolume: // zoom in key
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
			default:
			{
				iKeyCode = 0;
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
			default:
			{
				iKeyCode = 0;
				break;
			}
		}
	}

    return response;
}

//=============================================================================
TDesC & CImageEditorBubbleControl::GetParam ()
{
	return iParam;
}

//=============================================================================
void CImageEditorBubbleControl::SetSystemParameters (const CSystemParameters * aPars) 
{
    iSysPars = aPars;
}

//=============================================================================
void CImageEditorBubbleControl::HandlePluginCommandL (const TInt aCommand)
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::HandlePluginCommandL()");

    switch (aCommand) 
    {
        case EPgnSoftkeyIdCancel:
        {
            iPopupController->HideInfoPopupNote();
        	if (iState == EInsertBubbleStateMin)
        	{
        		// ignore if the plugin is not ready yet
        	}
            else if (iState == EInsertBubbleStateMain ||
                     iState == EInsertBubbleStateFirst)
            {
            	if ( !CImageEditorControlBase::Busy() )

        		{
					LOG(KBubblePluginLogFile, "HandleControlEventL: Plug-in cancelled.");
					
	                //  Cancel plugin
	                iState = EInsertBubbleStateMin;
                	iEditorView->HandleCommandL (EImageEditorCancelPlugin);
        		
        		}
            }
            else
            {
                // Return to plugin main view
                RestoreTempParams();
				StoreParameters();                
				ToMainStateL();
                iEditorView->HandleCommandL (EImageEditorCmdRender);
            }
            break;
        }
        case EPgnSoftkeyIdOk:
        {
            iPopupController->HideInfoPopupNote();
        	if (iState != EInsertBubbleStateMin)
        	{
	            ToMainStateL(); 
	            DrawNow();        		
        	}
            break;
        }
        case EPgnSoftkeyIdDone:
        case EBubblePgnMenuCmdDone:
        {
        	if (iState != EInsertBubbleStateMin)
        	{
            	iState = EInsertBubbleStateMin;
            	iEditorView->HandleCommandL (EImageEditorApplyPlugin);
        	}
            break;
        }
        case EBubblePgnMenuCmdMove:
        {
            ToMoveStateL();
            DrawDeferred();
            break;
        }
        case EBubblePgnMenuCmdResize:
        {
            ToResizeStateL();
            DrawDeferred();
            break;
        }
        case EBubblePgnMenuCmdRotate:
        {
            ToRotateStateL();
            DrawDeferred();
            break;
        }
        case EBubblePgnMenuCmdCancel:
        {
			LOG(KBubblePluginLogFile, "HandleControlEventL: Plug-in cancelled from menu.");
            iState = EInsertBubbleStateMin;
            iEditorView->HandleCommandL (EImageEditorCancelPlugin);
            break;
        }

        case EImageEditorPreGlobalZoomChange:
        case EImageEditorPreGlobalPanChange:
        {
        	StorePosAndScaleRelScreen();
        	break;
        }
        
        case EImageEditorGlobalZoomChanged:
        case EImageEditorGlobalPanChanged:
        {
        
			RestorePosAndScaleRelScreen();
			ClipPosition();
			LOGFMT(KBubblePluginLogFile, "\tiX = %d", iX);
			LOGFMT(KBubblePluginLogFile, "\tiY = %d", iY);

			StoreParameters();
            StoreTempParams();
	    	iEditorView->HandleCommandL (EImageEditorCmdRender);

			DrawNow();
	        break;
        }

        case EImageEditorPreScreenModeChange:
        {
        	if (iState != EInsertBubbleStateMin)
        	{
				StorePosAndScaleRelImage();
        	}
        	break;
        }
        case EImageEditorPostScreenModeChange:
        {
        	if (iState != EInsertBubbleStateMin)
        	{
				RestorePosAndScaleRelImage();
				ClipPosition();
				StoreParameters();
	            StoreTempParams();
		    	iEditorView->HandleCommandL (EImageEditorCmdRender);
				DrawNow();
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
TInt CImageEditorBubbleControl::GetSoftkeyIndexL()
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::GetSoftkeyIndexL()");

    TInt state = 0;

	switch (iState)
	{
		case EInsertBubbleStateFirst:
		{
			state = 0; // Options - Cancel
			break;
		}
		case EInsertBubbleStateMove:
		case EInsertBubbleStateRotate:
		case EInsertBubbleStateResize:
		{
			state = 1; // Ok - Cancel
			break;
		}
		case EInsertBubbleStateMain:
		{
			state = 2; // Options - Done
			break;
		}
		case EInsertBubbleStateMin:
		default:
		{
			state = 3; // Empty 
			break;
		}
		
	}


    return state;
}

//=============================================================================
TInt CImageEditorBubbleControl::GetContextMenuResourceId()
{
return R_BUBBLE_CONTEXT_MENUBAR;    
}

//=============================================================================
TBitField CImageEditorBubbleControl::GetDimmedMenuItems()
{

    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::GetDimmedMenuItems()");
    
	TBitField dimmedMenuItems;
    TInt count = iItem->MenuItems().Count();

    if ( iDisplayingOkOptionsMenu )
    {
        // Dim the command EBubblePgnMenuCmdCancel
        for ( TInt i = 0; i < count; i++)
        {
            // get the menu item id
            TInt menuItem = iItem->MenuItems().At(i).iCommandId;
            if ( menuItem == EBubblePgnMenuCmdCancel ) 
            {
                dimmedMenuItems.SetBit( i );
            }
        }
        iDisplayingOkOptionsMenu = EFalse;
    }

    return dimmedMenuItems;
}

//=============================================================================
TPtrC CImageEditorBubbleControl::GetNaviPaneTextL (
    TBool& aLeftNaviPaneScrollButtonVisibile, 
    TBool& aRightNaviPaneScrollButtonVisible )
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::GetNaviPaneTextL()");

    aLeftNaviPaneScrollButtonVisibile = EFalse;
    aRightNaviPaneScrollButtonVisible = EFalse;
    return iNaviPaneText;
}

//=============================================================================
void CImageEditorBubbleControl::Draw (const TRect & aRect) const
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::Draw()");

    CPreviewControlBase::DrawPreviewImage (aRect);

    if ( iIndicator && iIndicator->Handle() && 
         iIndicatorMask && iIndicatorMask->Handle() )
    {
        CWindowGc & gc = SystemGc();
        
		gc.SetPenStyle (CGraphicsContext::ENullPen);
		gc.SetBrushStyle (CGraphicsContext::ENullBrush);

        gc.BitBltMasked ( 
            ComputeIndicatorPosition(),
            iIndicator, 
            TRect (iIndicator->SizeInPixels()), 
            iIndicatorMask, 
            EFalse
            );
    }
}

//=============================================================================
void CImageEditorBubbleControl::NaviDown()
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::NaviDown()");

    switch (iState) 
    {

        case EInsertBubbleStateMove:
        {
            iY += (KPosParamStep * iNaviStepMultiplier);
            if ( iY > iSysPars->VisibleImageRect().iBr.iY )
            {
            	iY = iSysPars->VisibleImageRect().iBr.iY - 1;
            }
    	    break;
        }

        case EInsertBubbleStateResize:
        {
            iScale -= KScaleParamStep;
            if (iScale < iScaleMin)
            {
            	iScale = iScaleMin;
            }
    	    break;
        }

        case EInsertBubbleStateRotate:
        {
            iAngle -= (KAngleParamStep * iNaviStepMultiplier) % KAngleParamMax;
    	    break;
        }

        default:
        {
    	    break;
        }
    }
}

//=============================================================================
void CImageEditorBubbleControl::NaviUp()
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::NaviUp()");

    switch (iState) 
    {

        case EInsertBubbleStateMove:
        {
            iY -= (KPosParamStep * iNaviStepMultiplier);
            if ( iY < iSysPars->VisibleImageRect().iTl.iY )
            {
            	iY = iSysPars->VisibleImageRect().iTl.iY;
            }
    	    break;
        }

        case EInsertBubbleStateResize:
        {
            iScale += KScaleParamStep;
            if (iScale > iScaleMax)
            {
            	iScale = iScaleMax;
            }
    	    break;
        }

        case EInsertBubbleStateRotate:
        {
            iAngle += (KAngleParamStep * iNaviStepMultiplier) % KAngleParamMax;
    	    break;
        }

        default:
        {
    	    break;
        }
    }
}

//=============================================================================
void CImageEditorBubbleControl::NaviRight()
{
    
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::NaviRight()");

    switch (iState) 
    {

        case EInsertBubbleStateMove:
        {
            iX += (KPosParamStep * iNaviStepMultiplier);
            if ( iX >= iSysPars->VisibleImageRect().iBr.iX )
            {
            	iX = iSysPars->VisibleImageRect().iBr.iX - 1;
            }
    	    break;
        }

        case EInsertBubbleStateResize:
        {
            iScale += KScaleParamStep;
            if (iScale > iScaleMax)
            {
            	iScale = iScaleMax;
            }
    	    break;
        }

        case EInsertBubbleStateRotate:
        {
            iAngle += (KAngleParamStep * iNaviStepMultiplier) % KAngleParamMax;
            break;
        }

        default:
        {
    	    break;
        }
    }
}

//=============================================================================
void CImageEditorBubbleControl::NaviLeft()
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::NaviLeft()");

    switch (iState) 
    {

        case EInsertBubbleStateMove:
        {
            iX -= (KPosParamStep * iNaviStepMultiplier);
            if ( iX < iSysPars->VisibleImageRect().iTl.iX )
            {
            	iX = iSysPars->VisibleImageRect().iTl.iX;
            }
            break;
        }

        case EInsertBubbleStateResize:
        {
            iScale -= KScaleParamStep;
            if (iScale < iScaleMin)
            {
            	iScale = iScaleMin;
            }
    	    break;
        }

        case EInsertBubbleStateRotate:
        {
            iAngle -= (KAngleParamStep * iNaviStepMultiplier) % KAngleParamMax;
    	    break;
        }

        default:
        {
    	    break;
        }
    }
}

//=============================================================================
void CImageEditorBubbleControl::ToMoveStateL()
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::ToMoveStateL()");

    iState = EInsertBubbleStateMove;
    iNaviPaneText.Copy ( iItem->Parameters()[KMoveBubbleIndex] );
    StoreTempParams();
    iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
    iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
    LoadIndicatorL (
        EMbmImageeditoruiQgn_indi_imed_move_super, 
        EMbmImageeditoruiQgn_indi_imed_move_super_mask
        );
    ShowTooltip();    
}

//=============================================================================
void CImageEditorBubbleControl::ToResizeStateL()
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::ToResizeStateL()");

    iState = EInsertBubbleStateResize;
    iNaviPaneText.Copy ( iItem->Parameters()[KResizeBubbleIndex] );
    StoreTempParams();
    iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
    iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
    LoadIndicatorL (
        EMbmImageeditoruiQgn_indi_imed_resize_super, 
        EMbmImageeditoruiQgn_indi_imed_resize_super_mask
        );
    ShowTooltip();    
}

//=============================================================================
void CImageEditorBubbleControl::ToRotateStateL()
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::ToRotateStateL()");

    iState = EInsertBubbleStateRotate;
    iNaviPaneText.Copy ( iItem->Parameters()[KRotateBubbleIndex] );
    StoreTempParams();
    iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
    iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
    LoadIndicatorL (
        EMbmImageeditoruiQgn_indi_imed_rotate_left_super, 
        EMbmImageeditoruiQgn_indi_imed_rotate_left_super_mask
        );
    ShowTooltip();    
}

//=============================================================================
void CImageEditorBubbleControl::ToMainStateL()
{

    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::ToMainStateL()");
    
	//  Delete old indicator
    delete iIndicator;
    iIndicator = 0;
    delete iIndicatorMask;
    iIndicatorMask = 0;

    iState = EInsertBubbleStateMain;
    iPopupController->HideInfoPopupNote();
    iNaviPaneText.Copy ( iItem->Parameters()[KMainBubbleIndex] );
    iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
    iEditorView->HandleCommandL (EImageEditorUpdateNavipane); 
}

//=============================================================================
void CImageEditorBubbleControl::StoreTempParams()
    {
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::StoreTempParams()");
	iTempX = iX;
	iTempY = iY;
    iTempScale = iScale;
    iTempAngle = iAngle;
    }

//=============================================================================
void CImageEditorBubbleControl::RestoreTempParams()
    {
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::RestoreTempParams()");
	iX = iTempX;
	iY = iTempY;
	iScale = iTempScale;
	iAngle = iTempAngle;
    }

//=============================================================================
void CImageEditorBubbleControl::GetHelpContext(TCoeHelpContext& aContext) const
{
    aContext.iMajor = TUid::Uid(UID_IMAGE_EDITOR);
    aContext.iContext = KSIE_HLP_EDIT_CLIPART;
}

//=============================================================================
void CImageEditorBubbleControl::LoadIndicatorL (
    TInt    aBitmapInd,
    TInt    aMaskInd
    ) 
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::LoadIndicatorL()");

    //  Delete old indicator
    delete iIndicator;
    iIndicator = 0;
    delete iIndicatorMask;
    iIndicatorMask = 0;

	SDrawUtils::GetIndicatorBitmapL (
		iIndicator,
		iIndicatorMask,
		aBitmapInd,
		aMaskInd
		);
}

//=============================================================================
TPoint CImageEditorBubbleControl::ComputeIndicatorPosition() const
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::ComputeIndicatorPosition()");
	
	TInt x( 0 );
	TInt y( 0 );
	// check if there is no indicator
	if ( !iIndicator )
	    {
	    return TPoint ( x, y );
	    }
	    
	TRect vprect = iSysPars->VisibleImageRect();
	TRect vpprect = iSysPars->VisibleImageRectPrev();

	TInt s = 0;
	if (iScaleToHeight)
	{
	 	s = (iScale * vpprect.Height()) / vprect.Height();
	
	}
	else
	{
	 	s = (iScale * vpprect.Width()) / vprect.Width();
	}
	
	x = ((iX - vprect.iTl.iX)* vpprect.Width()) / vprect.Width();
	x += vpprect.iTl.iX;
	x -= iIndicator->SizeInPixels().iWidth / 2;
	x -= s / 2;

	y = ((iY - vprect.iTl.iY) * vpprect.Height()) / vprect.Height();
	y += vpprect.iTl.iY;
	y -= iIndicator->SizeInPixels().iHeight / 2;
	y += s / 2;

	LOGFMT (KBubblePluginLogFile, "ComputeIndicatorPosition --- (iX == %d)", iX);
	LOGFMT (KBubblePluginLogFile, "ComputeIndicatorPosition --- (iY == %d)", iY);
	LOGFMT (KBubblePluginLogFile, "ComputeIndicatorPosition --- (x == %d)", x);
	LOGFMT (KBubblePluginLogFile, "ComputeIndicatorPosition --- (y == %d)", y);

    return TPoint (x,y);
}

//=============================================================================
void CImageEditorBubbleControl::StoreParameters()
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::StoreParameters()");
	iParam.Copy (_L("x "));
	iParam.AppendNum (iX);
	iParam.Append (_L(" y "));
	iParam.AppendNum (iY);
	iParam.Append (_L(" angle "));
	iParam.AppendNum (iAngle);

	if ( iScaleToHeight )
	{
		iParam.Append (_L(" height "));
		iParam.AppendNum (iScale);
	}
	else
	{
		iParam.Append (_L(" width "));
		iParam.AppendNum (iScale);
	}
    LOGDES(KBubblePluginLogFile, iParam);
}

//=============================================================================
void CImageEditorBubbleControl::TimerCallBack()
{
    LOGFMT(KBubblePluginLogFile, "CImageEditorBubbleControl::TimerCallBack() (iTickCount == %d)", iTickCount);

    if (iTickCount > KDefaultFastKeyTimerMultiplyThresholdInTicks)
    {
        LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::TimerCallBack: switching to big steps");
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
	    LOG(KBubblePluginLogFile, "\tTimerCallBack: Render...");
	    StoreParameters();
	    TRAP_IGNORE( iEditorView->HandleCommandL (EImageEditorCmdRender) );

		iTimer->Call (KWait);			
	}
}

//=============================================================================
void CImageEditorBubbleControl::StorePosAndScaleRelScreen()
{

    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::StorePosAndScaleRelScreen()");

	TReal relscale = iSysPars->Scale();
	TRect virect = iSysPars->VisibleImageRect();
	virect.iTl.iX = (TInt)((virect.iTl.iX / relscale) + 0.5);
	virect.iTl.iY = (TInt)((virect.iTl.iY / relscale) + 0.5);
	virect.iBr.iX = (TInt)((virect.iBr.iX / relscale) + 0.5);
	virect.iBr.iY = (TInt)((virect.iBr.iY / relscale) + 0.5);
	
	TInt viwidth = virect.iBr.iX - virect.iTl.iX;
	TInt viheight = virect.iBr.iY - virect.iTl.iY;
	
	TRect viprect = iSysPars->VisibleImageRectPrev();

	TInt vipwidth = viprect.iBr.iX - viprect.iTl.iX;
	TInt vipheight = viprect.iBr.iY - viprect.iTl.iY;

	//	Scale
	TInt dimold_pix = (TInt)((iScale / relscale) + 0.5); 
	if ( iScaleToHeight )
	{
		iScaleOld = (TReal)(dimold_pix * vipheight) / viheight;
	}
	else
	{
		iScaleOld = (TReal)(dimold_pix * vipwidth) / viwidth;
	}

	//	Position
	TInt xCurrent = (TInt)((iX / relscale) + 0.5); 
	TInt yCurrent = (TInt)((iY / relscale) + 0.5); 
	iPosXOld = viprect.iTl.iX + (TReal)((xCurrent - virect.iTl.iX) * vipwidth) / viwidth;			
	iPosYOld = viprect.iTl.iY + (TReal)((yCurrent - virect.iTl.iY) * vipheight) / viheight;			

	LOGFMT(KBubblePluginLogFile, "\tiPosXOld = %d", iPosXOld);
	LOGFMT(KBubblePluginLogFile, "\tiPosYOld = %d", iPosYOld);

	iParam.Copy(_L("nop"));	

}

//=============================================================================
void CImageEditorBubbleControl::RestorePosAndScaleRelScreen()
{
	LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::RestorePosAndScaleRelScreen()");

	TReal relscale = iSysPars->Scale();

	TRect virect = iSysPars->VisibleImageRect();
	virect.iTl.iX = (TInt)((virect.iTl.iX / relscale) + 0.5);
	virect.iTl.iY = (TInt)((virect.iTl.iY / relscale) + 0.5);
	virect.iBr.iX = (TInt)((virect.iBr.iX / relscale) + 0.5);
	virect.iBr.iY = (TInt)((virect.iBr.iY / relscale) + 0.5);

	TInt viwidth = virect.iBr.iX - virect.iTl.iX;
	TInt viheight = virect.iBr.iY - virect.iTl.iY;
	
	TRect viprect = iSysPars->VisibleImageRectPrev();
	TInt vipwidth = viprect.iBr.iX - viprect.iTl.iX;
	TInt vipheight = viprect.iBr.iY - viprect.iTl.iY;

	//	Scale
	if (iScaleToHeight)
	{
		iScale = (TInt)((iScaleOld * viheight) / vipheight + 0.5);
	}
	else
	{
		iScale = (TInt)((iScaleOld * viwidth) / vipwidth + 0.5);
	}
	iScale = (TInt)(iScale * relscale + 0.5); 

	//	Position
	iX = (TInt)(virect.iTl.iX + (TReal)((iPosXOld - viprect.iTl.iX) * viwidth) / vipwidth + 0.5);
	iY = (TInt)(virect.iTl.iY + (TReal)((iPosYOld - viprect.iTl.iY) * viheight) / vipheight + 0.5);

}

//=============================================================================
void CImageEditorBubbleControl::StorePosAndScaleRelImage()
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::StorePosAndScaleRelImage()");

	TReal relscale = iSysPars->RelScale();

	iPosXOld = (TInt)((iX / relscale) + 0.5);	
	iPosYOld = (TInt)((iY / relscale) + 0.5);
	iScaleOld = (TInt)(iScale / relscale + 0.5);

	iParam.Copy(_L("nop"));	
}

//=============================================================================
void CImageEditorBubbleControl::RestorePosAndScaleRelImage()
{
    LOG(KBubblePluginLogFile, "CImageEditorBubbleControl::RestorePosAndScaleRelImage()");

	TReal relscale = iSysPars->RelScale();

	iX = (TInt)(iPosXOld * relscale + 0.5);
	iY = (TInt)(iPosYOld * relscale + 0.5);
	iScale = TInt(iScaleOld * relscale + 0.5);
}

//=============================================================================
void CImageEditorBubbleControl::ClipPosition()
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

//=============================================================================
void CImageEditorBubbleControl::HandlePointerEventL(
                                            const TPointerEvent &aPointerEvent)
    {        
    if( AknLayoutUtils::PenEnabled() )
		{	
		//  If busy, do not handle anything
        if ( Busy() )
            {
            return;
            }	
            
        TBool render = ETrue;            
		switch( aPointerEvent.iType )
			{
			case TPointerEvent::EButton1Down:
				{
				if ( iState == EInsertBubbleStateMove )
				    {	
				    iPopupController->HideInfoPopupNote();				   		    
				    SetBubblePosition( aPointerEvent.iPosition );
				    }
				else if ( iState == EInsertBubbleStateRotate )
				    {
				    iPopupController->HideInfoPopupNote();
				    // Store current position. Rotating is handled in drag-event
				    // is pen position has changed
				    iPointerPosition = aPointerEvent.iPosition;
				    }			
				else if ( iState == EInsertBubbleStateResize )
				    {
				    iPopupController->HideInfoPopupNote();
				    iPointerPosition = aPointerEvent.iPosition;
				    }    
#ifdef RD_TACTILE_FEEDBACK
				if ( iTouchFeedBack )
					{
					iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
					RDebug::Printf( "ImageEditor::ImageEditorBubbleControl: ETouchFeedback" );
					}
#endif /* RD_TACTILE_FEEDBACK  */
				
				break;
				}
			case TPointerEvent::EDrag:
				{
				if ( iState == EInsertBubbleStateMove )
				    {
				    SetBubblePosition( aPointerEvent.iPosition );
				    iPointerPosition = aPointerEvent.iPosition;
				    TRect clientRect;
				    CResolutionUtil::Self()->GetClientRect( clientRect );
				    TPoint tempPoint = iPointerPosition;
				    tempPoint.iY += iY;
				    if ( !clientRect.Contains( tempPoint ) )
				    	{
				    	ShowTooltip();
				    	}
				    }
				else if ( iState == EInsertBubbleStateRotate )
				    {
				    if ( CalculateRotation( iPointerPosition, 
				                            aPointerEvent.iPosition ) )
				        {
				        // store current position for next round
				        iPointerPosition = aPointerEvent.iPosition;
				        }
				    else
				        {
				        render = EFalse;
				        }				       
				    }
				else if ( iState == EInsertBubbleStateResize )
				    {
				    if ( CalculateResize( iPointerPosition, 
				                          aPointerEvent.iPosition ) )
				        {
				        // store current position for next round
				        iPointerPosition = aPointerEvent.iPosition;
				        }
				    else
				        {
				        render = EFalse;
				        }				 			    
				    }    			
				
				break;		
				}
			case TPointerEvent::EButton1Up:
				{	
				if ( iState == EInsertBubbleStateMain )
    			    {
    			    // Show context sensitive menu
    			    iEditorView->HandleCommandL( EImageEditorOpenContextMenu );
    			    }
				else
				    {
				    ShowTooltip();
				    }						 		 
				break;
				}
						
			default:
				{
				break;	
				}	
			}
			
    	StoreParameters();
    	
    	if ( render )
    	    {
	        TRAP_IGNORE( iEditorView->HandleCommandL (EImageEditorCmdRender) );
    	    }
		
		CCoeControl::HandlePointerEventL( aPointerEvent );

		}
    }
    
//=============================================================================    
void CImageEditorBubbleControl::SetBubblePosition( TPoint aPointedPosition )
    {    
    
    // Get system parameters
   	TRect visibleImageRect( iSysPars->VisibleImageRect() );
   	TRect visibleImageRectPrev( iSysPars->VisibleImageRectPrev() );
   	
   	TInt xPosFactorDivider
   	            ( visibleImageRectPrev.iBr.iX - visibleImageRectPrev.iTl.iX );
   	TInt yPosFactorDivider
   	            ( visibleImageRectPrev.iBr.iY - visibleImageRectPrev.iTl.iY );
   	
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
		
		// Calculate position on visible image		                                
   	    iX = visibleImageRect.iTl.iX + 
   	         ( visibleImageRect.iBr.iX - visibleImageRect.iTl.iX ) * 
   	         xPositionFactor;	    
    	
    	iY = visibleImageRect.iTl.iY + 
	         ( visibleImageRect.iBr.iY - visibleImageRect.iTl.iY ) * 
	         yPositionFactor;		  
   	    }    
	
	// Check that not out of bounds
    ClipPosition();          
   
    }
    
//=============================================================================    
void CImageEditorBubbleControl::ShowTooltip()
    {   
    iPopupController->HideInfoPopupNote();
    
    if ( iState != EInsertBubbleStateMove && 
	     iState != EInsertBubbleStateResize &&
		 iState != EInsertBubbleStateRotate )
	    {
	    return;
		}
    
    TPoint iconPosition = ComputeIndicatorPosition();

    if ( iState == EInsertBubbleStateMove )
        {
        SDrawUtils::ShowToolTip( iPopupController,
                                 this,
                                 iconPosition,
                                 EHRightVBottom,
                                 *iTooltipMove );
        }
    // resize
    else if ( iState == EInsertBubbleStateResize )
        {
        SDrawUtils::ShowToolTip( iPopupController,
                                 this,
                                 iconPosition,
                                 EHRightVBottom,
                                 *iTooltipResize );
        }
    // rotate
    else if ( iState == EInsertBubbleStateRotate ) 
        {
        SDrawUtils::ShowToolTip( iPopupController,
                                 this,
                                 iconPosition,
                                 EHRightVBottom,
                                 *iTooltipRotate );
        }
    
    }    

//=============================================================================
TBool CImageEditorBubbleControl::CalculateResize( TPoint aStartPoint, 
                                                  TPoint aEndPoint )
    {
    // Whether bubble is resized in this function or not
    TBool bubbleResized( EFalse );
    // Store old scale value
    TInt oldScale = iScale;    
    
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
	TInt scaleStep = ( iScaleMax - iScaleMin ) / KTouchScaleMaxStepCount + 1;

    // Relates to second and fourth corners. Defines how steep/gentle the 
    // moving angle has to be in order to scale.
    TInt slopeAngleFactor = 3;
    
    // The first quarter (movement towards upper-right corner)
    if( ( deltaX > 0 && deltaY <= 0 ) || ( deltaX >= 0 && deltaY < 0 ) )
        {
        // use bigger value
        if (Abs( deltaX ) >= Abs( deltaY) )
            {
            iScale += scaleStep * ( Abs( deltaX ) / oneStepInPixels );
            }
        else
            {
            iScale += scaleStep * ( Abs( deltaY ) / oneStepInPixels );
            }				                             
        }
    // The second (movement towards lower-right corner)  	
    else if( ( deltaX > 0 && deltaY >= 0 ) || ( deltaX >= 0 && deltaY > 0 ) )
        {
        if( deltaX > slopeAngleFactor * deltaY )
            {			                
	        iScale += scaleStep * ( Abs( deltaX ) / oneStepInPixels );    
            }
	        			               
        else if ( slopeAngleFactor * deltaX < deltaY )
            {			              
	        iScale -= scaleStep * ( Abs( deltaY ) / oneStepInPixels ); 
	        }
        }    
    // The third (movement towards lower-left corner)
    else if( ( deltaX < 0 && deltaY >= 0 ) || ( deltaX <= 0 && deltaY > 0 ) )
        {
        if (Abs( deltaX ) >= Abs( deltaY) )
            {
            iScale -= scaleStep * ( Abs( deltaX ) / oneStepInPixels );    
            }
        else
            {
            iScale -= scaleStep * ( Abs( deltaY ) / oneStepInPixels ); 
            }
        }
    // The fourth (movement towards upper-left corner)
    else if( ( deltaX < 0 && deltaY <= 0 ) || ( deltaX <= 0 && deltaY < 0 ) )
        {
        if( slopeAngleFactor * Abs( deltaX ) < Abs( deltaY ) )
            {
            iScale += scaleStep * ( Abs( deltaY ) / oneStepInPixels ); 
            }
        else if ( Abs( deltaX ) > slopeAngleFactor * Abs( deltaY ) )
            {
            iScale -= scaleStep * ( Abs( deltaX ) / oneStepInPixels );    
	        }
        }
    
    // Check the limits        
    if (iScale > iScaleMax)
        {
       	iScale = iScaleMax;
       	bubbleResized = ETrue;
        }
    if (iScale < iScaleMin)
        {
        iScale = iScaleMin;
        bubbleResized = ETrue;
        }
    
    if ( oldScale != iScale )    
        {
        bubbleResized = ETrue;
        }
        
    return bubbleResized;
    
    }
    
//=============================================================================
TBool CImageEditorBubbleControl::CalculateRotation( TPoint aStartPoint, 
                                                    TPoint aEndPoint )
    {
    TBool angleChanged( EFalse );
    TInt oldAngle = iAngle;
        
    // Get system parameters
   	TRect visibleImageRect( iSysPars->VisibleImageRect() );
   	TRect visibleImageRectPrev( iSysPars->VisibleImageRectPrev() );
   	
   	// Calculate TalkBubble real center point on the screen 
   	// (physical coordinates)	
   	TReal posFactorX( TReal( iX - visibleImageRect.iTl.iX ) /
   	                            visibleImageRect.Width() );
   	TInt bubbleCenterX = posFactorX * visibleImageRectPrev.Width() + 
   	                                   visibleImageRectPrev.iTl.iX;
   	
   	TReal posFactorY( TReal( iY - visibleImageRect.iTl.iY ) / 
   	                            visibleImageRect.Height() );
   	TInt bubbleCenterY = posFactorY * visibleImageRectPrev.Height() + 
   	                                   visibleImageRectPrev.iTl.iY;
   	
    // Calculate start and end positions of the movement assuming that
    // clipart centre is in origo.
    // Note! y-axis is mirrored on screen coordinates compared to standard 2-d
    // co-ordinates->mirror y-axis to ease the calculation
   	TPoint startPos( ( aStartPoint.iX - bubbleCenterX ), 
                       ( bubbleCenterY - aStartPoint.iY ) );    
    TPoint endPos( ( aEndPoint.iX - bubbleCenterX ), 
                     ( bubbleCenterY - aEndPoint.iY ) );

    TReal angleInRadStart;
    TReal angleInRadEnd;
    
    // Calculate start and end angles in radians
    TInt err1 = Math::ATan( angleInRadStart, startPos.iY, startPos.iX );
    TInt err2 = Math::ATan( angleInRadEnd, endPos.iY, endPos.iX );
    
    if( !err1 && !err2 )
        {
        // Calculate change in angle and convert it to degrees
        TReal changeInDegrees = 
                       ( angleInRadEnd - angleInRadStart ) * KRadToDeg;
        
        iAngle -= ( KDegreeMultiplier * TInt( changeInDegrees ) ) 
                  % KAngleParamMax;
        }
    
    if ( iAngle != oldAngle )
        {
        angleChanged = ETrue;
        }
        
    return angleChanged;      
    }
        
// End of file

