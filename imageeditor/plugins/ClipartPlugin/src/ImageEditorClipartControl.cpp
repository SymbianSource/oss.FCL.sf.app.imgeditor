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
* Control class for clipart plugin.
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

#include <ImageEditorUI.mbg>

#include <aknquerydialog.h> 
#include <avkon.rsg>

#include <AknInfoPopupNoteController.h> 
#include <clipart.rsg> 
#include <bautils.h> 
#include <ConeResLoader.h> 
#include <csxhelp/sie.hlp.hrh>

#ifdef RD_TACTILE_FEEDBACK 
#include <touchfeedback.h>
#endif /* RD_TACTILE_FEEDBACK  */

#include "ImageEditorUI.hrh"
#include "ImageEditorPluginBase.hrh"
#include "clipart.hrh"
#include "JpTimer.h"
#include "PluginInfo.h"
#include "ImageEditorUids.hrh"
#include "ImageEditorUiDefs.h"  
#include "iepb.h"
#include "SystemParameters.h"
#include "ResolutionUtil.h"

#include "ImageEditorClipartControl.h"
#include "ClipartSelectionGrid.h"

//#include "platform_security_literals.hrh"

// debug log
#include "imageeditordebugutils.h"
_LIT(KClipartPluginLogFile,"ClipartPlugin.log");

//  CONSTANTS

//	TIMER WAIT
const TInt KWait					= 100;

//	CLIPART POSITION
const TInt KPosParamStep			= 8;

//	CLIPART SCALE
const TInt KScaleParamStep			= 50;
const TInt KTouchScaleMaxStepCount  = 20;

//	CLIPART ANGLE
const TInt KDegreeMultiplier        = 1000;
const TInt KAngleParamMax			= 359000;
const TInt KAngleParam90Deg         = 90000;
const TInt KAngleParamDef			= 0;
const TInt KAngleParamStep			= 2000;

//	RESOURCE INDICES
const TInt KSelectClipartIndex		= 0;
const TInt KMainClipartIndex		= 1;
const TInt KMoveClipartIndex		= 2;
const TInt KResizeClipartIndex		= 3;
const TInt KRotateClipartIndex		= 4;

_LIT (KPgnResourceFile, "clipart.rsc");
_LIT(KComponentName, "ImageEditorClipartPlugin");

//=============================================================================
CImageEditorClipartControl * CImageEditorClipartControl::NewL (
	const TRect &		aRect,
	CCoeControl	*		aParent
	)
{
    CImageEditorClipartControl * self = new (ELeave) CImageEditorClipartControl;
    CleanupStack::PushL (self);
    self->ConstructL (aRect, aParent);
    CleanupStack::Pop ();   // self
    return self;
}

//=============================================================================
CImageEditorClipartControl::CImageEditorClipartControl () : 
iState (EInsertClipartStateMin),
iResLoader ( * ControlEnv() ),
iTickCount (0),
iNaviStepMultiplier (KDefaultSmallNavigationStepMultiplier),
iDisplayingOkOptionsMenu(EFalse),
iReadyToRender(EFalse)
{

}

//=============================================================================
CImageEditorClipartControl::~CImageEditorClipartControl ()
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
void CImageEditorClipartControl::ConstructL (
	const TRect &		/*aRect*/,
	CCoeControl	*		aParent
	)
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::ConstructL()");

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
	
		iTooltipResize = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_CLIPART_RESIZE);    
		iTooltipMove = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_CLIPART_MOVE);    
		iTooltipRotate = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_CLIPART_ROTATE);          
    CleanupStack::PopAndDestroy(); // resLoader	


#ifdef RD_TACTILE_FEEDBACK 
    iTouchFeedBack = MTouchFeedback::Instance();
#endif /* RD_TACTILE_FEEDBACK  */
    
	//	Activate control
    ActivateL();
    
    EnableDragEvents();
    
}

//=============================================================================
void CImageEditorClipartControl::PrepareL ()
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::PrepareL()");

    // Set busy flag untill the bitmap has been decoded
    SetBusy();

    // Main view navi pane text
    iNaviPaneText.Copy ( iItem->Parameters()[KSelectClipartIndex] );

    // Popup list to contain the grid
    iPopupList = new (ELeave) CClipartSelectionDialog(iClipartFileName, iClipartFileNameIndex);
    iPopupList->ConstructL(this); 
    
}

//=============================================================================
TBool CImageEditorClipartControl::IsReadyToRender() const
{
    return iReadyToRender;
}


//=============================================================================
void CImageEditorClipartControl::HandleControlEventL(CCoeControl* /*aControl*/,TCoeEvent aEventType)
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::HandleControlEventL()");

    // When the icons have been decoded, launch the grid.
    if (aEventType == EEventStateChanged)
    {
        __ASSERT_ALWAYS( iPopupList, User::Panic( KComponentName, KErrNotReady));
		
		TBool fullscreen = CResolutionUtil::Self()->GetFullScreen();
		if (fullscreen)
		{
			iEditorView->HandleCommandL (EImageEditorMenuCmdNormalScreen);			
		}
        TInt popupOk = iPopupList->ExecuteLD(R_CLIPART_SELECTION_DIALOG);	
        iPopupList = NULL;
        
		if (fullscreen)
		{
			iEditorView->HandleCommandL (EImageEditorMenuCmdFullScreen);
		}

        if (popupOk)
        {
            ToMoveStateL();
            SelectClipartL();
            StoreTempParams();
            ResetBusy();
        }
        else
        {
    		LOG(KClipartPluginLogFile, "HandleControlEventL: Plugin cancelled from popup.");
            iState = EInsertClipartStateMin;
            iEditorView->HandleCommandL (EImageEditorCancelPlugin);
        }
    }
    
    // Cancelled the grid
    else if (aEventType == EEventRequestCancel)
    {
		LOG(KClipartPluginLogFile, "HandleControlEventL: Grid cancelled.");
        iState = EInsertClipartStateMin;
        iEditorView->HandleCommandL (EImageEditorCancelPlugin);
    }
}

//=============================================================================
void CImageEditorClipartControl::HandleListBoxEventL(CEikListBox* /*aListBox*/, 
                                                     TListBoxEvent /*aEventType*/)
    {    
    }



//=============================================================================
void CImageEditorClipartControl::SelectClipartL()
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::SelectClipartL()");

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
	iParam.Append (iClipartFileName);
	iParam.Append (_L("\""));

	iParam.Append (_L(" clipart "));
	iParam.AppendNum (iClipartFileNameIndex);
	iParam.Append (_L(" mask "));
	iParam.AppendNum (iClipartFileNameIndex + 1);
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
	
    LOGDES(KClipartPluginLogFile, iParam);

    LOG(KClipartPluginLogFile, "SelectClipartL: Ready to render");

    iReadyToRender = ETrue;

    iEditorView->HandleCommandL (EImageEditorCmdRender);

    LOG(KClipartPluginLogFile, "SelectClipartL: Rendered");

}

//=============================================================================
void CImageEditorClipartControl::SetView (CAknView * aView)
{
    iEditorView = aView;
}

//=============================================================================
void CImageEditorClipartControl::SetSelectedUiItemL (CPluginInfo * aItem)
{
    iItem = aItem;
}

//=============================================================================
void CImageEditorClipartControl::SetBusy ()
{
    CImageEditorControlBase::SetBusy();
    if ( iPopupList )
	{
		iPopupList->SetBusy(ETrue);
	}
}

//=============================================================================
void CImageEditorClipartControl::ResetBusy ()
{
    // Reset busy flag only if selection grid is constructed
    if (iState != EInsertClipartStateMin)
    {
        CImageEditorControlBase::ResetBusy();
		
		if (iPopupList)
		{
			iPopupList->SetBusy(EFalse);	
		}
		
    }
}

//=============================================================================
TKeyResponse CImageEditorClipartControl::OfferKeyEventL (
    const TKeyEvent &   aKeyEvent,
    TEventCode          aType
    )
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::OfferKeyEventL()");
#ifdef VERBOSE
    LOGFMT(KClipartPluginLogFile, "\taType: %d", aType);
    LOGFMT(KClipartPluginLogFile, "\tiTickCount: %d", iTickCount);
#endif

    TKeyResponse response = EKeyWasNotConsumed;

    //  If busy, do not handle anything
    if ( Busy() )
    {
        response = EKeyWasConsumed;
    }

    // In main state handle the OK Options menu
    else if ( aKeyEvent.iCode == EKeyOK &&
        (iState == EInsertClipartStateMain || iState == EInsertClipartStateFirst ) )
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
            // additional keycodes which are generated when holding navigation key 
            case 14:
            case 15:
            case 16:
            case 17:
			{
				response = EKeyWasConsumed;
                break;
			}
			
            case EKeyOK:
            {
                if (iState == EInsertClipartStateMove || 
                    iState == EInsertClipartStateRotate  ||
                    iState == EInsertClipartStateResize)
                {
                    ToMainStateL();
					DrawNow();
                    response = EKeyWasConsumed;
                }
                break;
            }
            
            case EKeyEnter:
            {   
                if (iState == EInsertClipartStateMove || 
                    iState == EInsertClipartStateRotate  ||
                    iState == EInsertClipartStateResize)
                {                       
                    ToMainStateL();
                    DrawNow();
                }
                else if ( iState == EInsertClipartStateMain )
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
                if ( iState == EInsertClipartStateRotate )
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
		        if ( iState == EInsertClipartStateRotate )
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
TDesC & CImageEditorClipartControl::GetParam ()
{
	return iParam;
}

//=============================================================================
void CImageEditorClipartControl::SetSystemParameters (const CSystemParameters * aPars) 
{
    iSysPars = aPars;
}

//=============================================================================
void CImageEditorClipartControl::HandlePluginCommandL (const TInt aCommand)
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::HandlePluginCommandL()");

    switch (aCommand) 
    {
        case EPgnSoftkeyIdCancel:
        {   
            iPopupController->HideInfoPopupNote();
        	if (iState == EInsertClipartStateMin)
        	{
        		// ignore if the plugin is not ready yet
        	}
            else if (iState == EInsertClipartStateMain ||
                     iState == EInsertClipartStateFirst)
            {
            	if ( !CImageEditorControlBase::Busy() )

        		{
					LOG(KClipartPluginLogFile, "HandleControlEventL: Plug-in cancelled.");
					
	                //  Cancel plugin
	                iState = EInsertClipartStateMin;
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
        	if (iState != EInsertClipartStateMin)
        	{
	            ToMainStateL(); 
	            DrawNow();        		
        	}
            break;
        }
        case EPgnSoftkeyIdDone:
        case EClipartPgnMenuCmdDone:
        {
        	if (iState != EInsertClipartStateMin)
        	{
            	iState = EInsertClipartStateMin;
            	iEditorView->HandleCommandL (EImageEditorApplyPlugin);
        	}
            break;
        }
        case EClipartPgnMenuCmdMove:
        {
            ToMoveStateL();
            DrawDeferred();
            break;
        }
        case EClipartPgnMenuCmdResize:
        {
            ToResizeStateL();
            DrawDeferred();
            break;
        }
        case EClipartPgnMenuCmdRotate:
        {
            ToRotateStateL();
            DrawDeferred();
            break;
        }
        case EClipartPgnMenuCmdCancel:
        {
			LOG(KClipartPluginLogFile, "HandleControlEventL: Plug-in cancelled from menu.");
            iState = EInsertClipartStateMin;
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
			LOGFMT(KClipartPluginLogFile, "\tiX = %d", iX);
			LOGFMT(KClipartPluginLogFile, "\tiY = %d", iY);

			StoreParameters();
            StoreTempParams();
	    	iEditorView->HandleCommandL (EImageEditorCmdRender);

			DrawNow();
	        break;
        }

        case EImageEditorPreScreenModeChange:
        {
        	if (iState != EInsertClipartStateMin)
        	{
				StorePosAndScaleRelImage();
        	}
        	break;
        }
        case EImageEditorPostScreenModeChange:
        {
        	if (iState != EInsertClipartStateMin)
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
TInt CImageEditorClipartControl::GetSoftkeyIndexL()
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::GetSoftkeyIndexL()");

    TInt state = 0;

	switch (iState)
	{
		case EInsertClipartStateFirst:
		{
			state = 0; // Options - Cancel
			break;
		}
		case EInsertClipartStateMove:
		case EInsertClipartStateRotate:
		case EInsertClipartStateResize:
		{
			state = 1; // Ok - Cancel
			break;
		}
		case EInsertClipartStateMain:
		{
			state = 2; // Options - Done
			break;
		}
		case EInsertClipartStateMin:
		default:
		{
			state = 3; // Empty 
			break;
		}
		
	}


    return state;
}

//=============================================================================
TInt CImageEditorClipartControl::GetContextMenuResourceId()
{
return R_CLIPART_CONTEXT_MENUBAR;    
}

//=============================================================================
TBitField CImageEditorClipartControl::GetDimmedMenuItems()
{

    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::GetDimmedMenuItems()");
    
	TBitField dimmedMenuItems;
    TInt count = iItem->MenuItems().Count();

    if ( iDisplayingOkOptionsMenu )
    {
        // Dim the command EClipartPgnMenuCmdCancel
        for ( TInt i = 0; i < count; i++)
        {
            // get the menu item id
            TInt menuItem = iItem->MenuItems().At(i).iCommandId;
            if ( menuItem == EClipartPgnMenuCmdCancel ) 
            {
                dimmedMenuItems.SetBit( i );
            }
        }
        iDisplayingOkOptionsMenu = EFalse;
    }

    return dimmedMenuItems;
}

//=============================================================================
TPtrC CImageEditorClipartControl::GetNaviPaneTextL (
    TBool& aLeftNaviPaneScrollButtonVisibile, 
    TBool& aRightNaviPaneScrollButtonVisible )
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::GetNaviPaneTextL()");

    aLeftNaviPaneScrollButtonVisibile = EFalse;
    aRightNaviPaneScrollButtonVisible = EFalse;
    return iNaviPaneText;
}

//=============================================================================
void CImageEditorClipartControl::Draw (const TRect & aRect) const
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::Draw()");

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
void CImageEditorClipartControl::NaviDown()
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::NaviDown()");

    switch (iState) 
    {

        case EInsertClipartStateMove:
        {
            iY += (KPosParamStep * iNaviStepMultiplier);
            if ( iY > iSysPars->VisibleImageRect().iBr.iY )
            {
            	iY = iSysPars->VisibleImageRect().iBr.iY - 1;
            }
    	    break;
        }

        case EInsertClipartStateResize:
        {
            iScale -= KScaleParamStep;
            if (iScale < iScaleMin)
            {
            	iScale = iScaleMin;
            }
    	    break;
        }

        case EInsertClipartStateRotate:
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
void CImageEditorClipartControl::NaviUp()
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::NaviUp()");

    switch (iState) 
    {

        case EInsertClipartStateMove:
        {
            iY -= (KPosParamStep * iNaviStepMultiplier);
            if ( iY < iSysPars->VisibleImageRect().iTl.iY )
            {
            	iY = iSysPars->VisibleImageRect().iTl.iY;
            }
    	    break;
        }

        case EInsertClipartStateResize:
        {
            iScale += KScaleParamStep;
            if (iScale > iScaleMax)
            {
            	iScale = iScaleMax;
            }
    	    break;
        }

        case EInsertClipartStateRotate:
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
void CImageEditorClipartControl::NaviRight()
{
    
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::NaviRight()");

    switch (iState) 
    {

        case EInsertClipartStateMove:
        {
            iX += (KPosParamStep * iNaviStepMultiplier);
            if ( iX >= iSysPars->VisibleImageRect().iBr.iX )
            {
            	iX = iSysPars->VisibleImageRect().iBr.iX - 1;
            }
    	    break;
        }

        case EInsertClipartStateResize:
        {
            iScale += KScaleParamStep;
            if (iScale > iScaleMax)
            {
            	iScale = iScaleMax;
            }
    	    break;
        }

        case EInsertClipartStateRotate:
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
void CImageEditorClipartControl::NaviLeft()
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::NaviLeft()");

    switch (iState) 
    {

        case EInsertClipartStateMove:
        {
            iX -= (KPosParamStep * iNaviStepMultiplier);
            if ( iX < iSysPars->VisibleImageRect().iTl.iX )
            {
            	iX = iSysPars->VisibleImageRect().iTl.iX;
            }
            break;
        }

        case EInsertClipartStateResize:
        {
            iScale -= KScaleParamStep;
            if (iScale < iScaleMin)
            {
            	iScale = iScaleMin;
            }
    	    break;
        }

        case EInsertClipartStateRotate:
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
void CImageEditorClipartControl::ToMoveStateL()
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::ToMoveStateL()");

    iState = EInsertClipartStateMove;
    iNaviPaneText.Copy ( iItem->Parameters()[KMoveClipartIndex] );
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
void CImageEditorClipartControl::ToResizeStateL()
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::ToResizeStateL()");

    iState = EInsertClipartStateResize;
    iNaviPaneText.Copy ( iItem->Parameters()[KResizeClipartIndex] );
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
void CImageEditorClipartControl::ToRotateStateL()
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::ToRotateStateL()");

    iState = EInsertClipartStateRotate;
    iNaviPaneText.Copy ( iItem->Parameters()[KRotateClipartIndex] );
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
void CImageEditorClipartControl::ToMainStateL()
{

    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::ToMainStateL()");
    
	//  Delete old indicator
    delete iIndicator;
    iIndicator = 0;
    delete iIndicatorMask;
    iIndicatorMask = 0;

    iState = EInsertClipartStateMain;
    iPopupController->HideInfoPopupNote();
    iNaviPaneText.Copy ( iItem->Parameters()[KMainClipartIndex] );
    iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
    iEditorView->HandleCommandL (EImageEditorUpdateNavipane); 
}

//=============================================================================
void CImageEditorClipartControl::StoreTempParams()
    {
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::StoreTempParams()");
	iTempX = iX;
	iTempY = iY;
    iTempScale = iScale;
    iTempAngle = iAngle;
    }

//=============================================================================
void CImageEditorClipartControl::RestoreTempParams()
    {
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::RestoreTempParams()");
	iX = iTempX;
	iY = iTempY;
	iScale = iTempScale;
	iAngle = iTempAngle;
    }

//=============================================================================
void CImageEditorClipartControl::GetHelpContext(TCoeHelpContext& aContext) const
{
    aContext.iMajor = TUid::Uid(UID_IMAGE_EDITOR);
    aContext.iContext = KSIE_HLP_EDIT_CLIPART;
}

//=============================================================================
void CImageEditorClipartControl::LoadIndicatorL (
    TInt    aBitmapInd,
    TInt    aMaskInd
    ) 
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::LoadIndicatorL()");

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
TPoint CImageEditorClipartControl::ComputeIndicatorPosition() const
{   
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::ComputeIndicatorPosition()");
	
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

	LOGFMT (KClipartPluginLogFile, "ComputeIndicatorPosition --- (iX == %d)", iX);
	LOGFMT (KClipartPluginLogFile, "ComputeIndicatorPosition --- (iY == %d)", iY);
	LOGFMT (KClipartPluginLogFile, "ComputeIndicatorPosition --- (x == %d)", x);
	LOGFMT (KClipartPluginLogFile, "ComputeIndicatorPosition --- (y == %d)", y);

    return TPoint (x,y);
}

//=============================================================================
void CImageEditorClipartControl::StoreParameters()
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::StoreParameters()");
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
    LOGDES(KClipartPluginLogFile, iParam);
}

//=============================================================================
void CImageEditorClipartControl::TimerCallBack()
{
    LOGFMT(KClipartPluginLogFile, "CImageEditorClipartControl::TimerCallBack() (iTickCount == %d)", iTickCount);

    if (iTickCount > KDefaultFastKeyTimerMultiplyThresholdInTicks)
    {
        LOG(KClipartPluginLogFile, "CImageEditorClipartControl::TimerCallBack: switching to big steps");
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
	    LOG(KClipartPluginLogFile, "\tTimerCallBack: Render...");
	    StoreParameters();
	    TRAP_IGNORE( iEditorView->HandleCommandL (EImageEditorCmdRender) );

		iTimer->Call (KWait);			
	}
}

//=============================================================================
void CImageEditorClipartControl::StorePosAndScaleRelScreen()
{

    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::StorePosAndScaleRelScreen()");

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

	LOGFMT(KClipartPluginLogFile, "\tiPosXOld = %d", iPosXOld);
	LOGFMT(KClipartPluginLogFile, "\tiPosYOld = %d", iPosYOld);

	iParam.Copy(_L("nop"));	

}

//=============================================================================
void CImageEditorClipartControl::RestorePosAndScaleRelScreen()
{
	LOG(KClipartPluginLogFile, "CImageEditorClipartControl::RestorePosAndScaleRelScreen()");

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
void CImageEditorClipartControl::StorePosAndScaleRelImage()
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::StorePosAndScaleRelImage()");

	TReal relscale = iSysPars->RelScale();

	iPosXOld = (TInt)((iX / relscale) + 0.5);	
	iPosYOld = (TInt)((iY / relscale) + 0.5);
	iScaleOld = (TInt)(iScale / relscale + 0.5);

	iParam.Copy(_L("nop"));	
}

//=============================================================================
void CImageEditorClipartControl::RestorePosAndScaleRelImage()
{
    LOG(KClipartPluginLogFile, "CImageEditorClipartControl::RestorePosAndScaleRelImage()");

	TReal relscale = iSysPars->RelScale();

	iX = (TInt)(iPosXOld * relscale + 0.5);
	iY = (TInt)(iPosYOld * relscale + 0.5);
	iScale = TInt(iScaleOld * relscale + 0.5);
}

//=============================================================================
void CImageEditorClipartControl::ClipPosition()
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
void CImageEditorClipartControl::HandlePointerEventL(
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
				if ( iState == EInsertClipartStateMove )
				    {	
				    iPopupController->HideInfoPopupNote();				   		    
				    SetClipPosition( aPointerEvent.iPosition );
				    }			
			    else if ( iState == EInsertClipartStateRotate )
				    {
				    iPopupController->HideInfoPopupNote();
				    // Store current position. Rotating is handled in drag-event
				    // is pen position has changed
				    iPointerPosition = aPointerEvent.iPosition;				    
				    }			
				else if ( iState == EInsertClipartStateResize )
				    {
				    iPopupController->HideInfoPopupNote();
				    iPointerPosition = aPointerEvent.iPosition;
				    }    
#ifdef RD_TACTILE_FEEDBACK
				if ( iTouchFeedBack )
					{
					iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
					RDebug::Printf( "ImageEditor::ImageEditorClipartControl: ETouchFeedback" );
					}
#endif /* RD_TACTILE_FEEDBACK  */
				    
				break;
				}
			case TPointerEvent::EDrag:
				{
				if ( iState == EInsertClipartStateMove )
				    {
				    SetClipPosition( aPointerEvent.iPosition );
				    iPointerPosition = aPointerEvent.iPosition;
				    }				
				else if ( iState == EInsertClipartStateRotate )
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
				else if ( iState == EInsertClipartStateResize )
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
				if ( iState == EInsertClipartStateMain )
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
void CImageEditorClipartControl::SetClipPosition( TPoint aPointedPosition )
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
void CImageEditorClipartControl::ShowTooltip()
    {   
    iPopupController->HideInfoPopupNote();
    
    if ( iState != EInsertClipartStateMove && 
	     iState != EInsertClipartStateResize &&
		 iState != EInsertClipartStateRotate )
	    {
	    return;
		}
    
    TPoint iconPosition = ComputeIndicatorPosition();
      
    if ( iState == EInsertClipartStateMove )
        {
        SDrawUtils::ShowToolTip( iPopupController,
                                 this,
                                 iconPosition,
                                 EHRightVBottom,
                                 *iTooltipMove );
        }
    // resize
    else if ( iState == EInsertClipartStateResize )
        {
        SDrawUtils::ShowToolTip( iPopupController,
                                 this,
                                 iconPosition,
                                 EHRightVBottom,
                                 *iTooltipResize );
        }
    // rotate
    else if ( iState == EInsertClipartStateRotate ) 
        {
        SDrawUtils::ShowToolTip( iPopupController,
                                 this,
                                 iconPosition,
                                 EHRightVBottom,
                                 *iTooltipRotate );
        }
    
    }    

//=============================================================================
TBool CImageEditorClipartControl::CalculateResize( TPoint aStartPoint, 
                                                   TPoint aEndPoint )
    {
    // Whether clipart is resized in this function or not
    TBool clipartResized( EFalse );
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
       	clipartResized = ETrue;
        }
    if (iScale < iScaleMin)
        {
        iScale = iScaleMin;
        clipartResized = ETrue;
        }
    
    if ( oldScale != iScale )    
        {
        clipartResized = ETrue;
        }
        
    return clipartResized;
    
    }
    
//=============================================================================
TBool CImageEditorClipartControl::CalculateRotation( TPoint aStartPoint, 
                                                     TPoint aEndPoint )
    {    
    TBool angleChanged( EFalse );
    TInt oldAngle = iAngle;
    
    // Get system parameters
   	TRect visibleImageRect( iSysPars->VisibleImageRect() );
   	TRect visibleImageRectPrev( iSysPars->VisibleImageRectPrev() );
   	
   	// Calculate clipart real center point on the screen (physical coordinates)	
   	TReal posFactorX( TReal( iX - visibleImageRect.iTl.iX ) /
   	                            visibleImageRect.Width() );
   	TInt clipartCenterX = posFactorX * visibleImageRectPrev.Width() + 
   	                                   visibleImageRectPrev.iTl.iX;
   	
   	TReal posFactorY( TReal( iY - visibleImageRect.iTl.iY ) / 
   	                            visibleImageRect.Height() );
   	TInt clipartCenterY = posFactorY * visibleImageRectPrev.Height() + 
   	                                   visibleImageRectPrev.iTl.iY;
   	
    // Calculate start and end positions of the movement assuming that
    // clipart centre is in origo.
    // Note! y-axis is mirrored on screen coordinates compared to standard 2-d
    // co-ordinates->mirror y-axis to ease the calculation
   	TPoint startPos( ( aStartPoint.iX - clipartCenterX ), 
                       ( clipartCenterY - aStartPoint.iY ) );    
    TPoint endPos( ( aEndPoint.iX - clipartCenterX ), 
                     ( clipartCenterY - aEndPoint.iY ) );

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

