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
* Image Editor text plugin control class.
*
*/


//  INCLUDES
#include <fbs.h>
#include <badesca.h>
#include <gdi.h>
#include <eikenv.h>
#include <bitdev.h>
#include <e32math.h>

#include <aknview.h>
#include <aknutils.h>
#include <AknBidiTextUtils.h>

#include <ImageEditorUI.mbg>
#include <AknInfoPopupNoteController.h> 
#include <text.rsg> 
#include <bautils.h> 
#include <ConeResLoader.h> 
#include <gulalign.h>
#include <csxhelp/sie.hlp.hrh>

#ifdef RD_TACTILE_FEEDBACK 
#include <touchfeedback.h>
#endif /* RD_TACTILE_FEEDBACK  */

#include "ImageEditorUI.hrh"
#include "ImageEditorPluginBase.hrh"
#include "text.hrh"
#include "PluginInfo.h"
#include "JpTimer.h"
#include "ImageEditorUids.hrh"
#include "ImageEditorUiDefs.h"
#include "SystemParameters.h"

#include "ImageEditorTextControl.h"
#include "iepb.h" 

// debug log
#include "imageeditordebugutils.h"
_LIT(KTextPluginLogFile,"TextPlugin.log");

// resource file name
_LIT (KPgnResourceFile, "text.rsc");


//  CONSTANTS
const TInt KWait			    	= 1;

const TInt KPosParamStep            = 8;

const TInt KScaleParamStep      	= 10;

const TInt KDegreeMultiplier        = 1000;
const TInt KAngleParamMax		    = 359000;
const TInt KAngleParam90Deg         = 90000;
const TInt KAngleParamDef		    = 0;
const TInt KAngleParamStep		    = 2000;
const TInt KTouchScaleMaxStepCount  = 20;

const TInt KMaxColumns		    	= 100;
const TInt KMaxRows 			    = 10;

const TInt KLogicalFontInUse        = EAknLogicalFontTitleFont;

//	RESOURCE INDICES
const TInt KMainTextIndex		= 1;
const TInt KMoveTextIndex		= 2;
const TInt KResizeTextIndex		= 3;
const TInt KRotateTextIndex		= 4;

_LIT(KEmptyString, " ");

//=============================================================================
CImageEditorTextControl * CImageEditorTextControl::NewL (
	const TRect &		aRect,
	CCoeControl	*		aParent
	)
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::NewL()");

    CImageEditorTextControl * self = new (ELeave) CImageEditorTextControl;
    CleanupStack::PushL (self);
    self->ConstructL (aRect, aParent);
    CleanupStack::Pop ();   // self
    return self;
}

//=============================================================================
CImageEditorTextControl::CImageEditorTextControl () : 
iState (EInsertTextStateFirst),
iTickCount (0),
iNaviStepMultiplier (KDefaultSmallNavigationStepMultiplier),
iDisplayingOkOptionsMenu (EFalse),
iReadyToRender (EFalse),
iResLoader ( * CEikonEnv::Static() )
{
	  
}

//=============================================================================
CImageEditorTextControl::~CImageEditorTextControl ()
{
    delete iTimer;
    iTimer = NULL;
    delete iIndicator;
    iIndicator = NULL;
    delete iIndicatorMask;
    iIndicatorMask = NULL;
    iEditorView = NULL;
    iItem = NULL;
    iParent = NULL;
    iSysPars = NULL;
	delete iPopupController;
	delete iTooltipResize;
	delete iTooltipMove;
	delete iTooltipRotate;
	iResLoader.Close();
}

//=============================================================================
void CImageEditorTextControl::ConstructL (
	const TRect &		/*aRect*/,
	CCoeControl	*		aParent
	)
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::ConstructL()");

	//	Set parent window
	SetContainerWindowL (*aParent);
    iParent = aParent;
    
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
	
		iTooltipResize = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_TEXT_RESIZE);    
		iTooltipMove = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_TEXT_MOVE);    
		iTooltipRotate = CEikonEnv::Static()->AllocReadResourceL(R_TOOLTIP_TEXT_ROTATE);          
    CleanupStack::PopAndDestroy(); // resLoader	

#ifdef RD_TACTILE_FEEDBACK 
    iTouchFeedBack = MTouchFeedback::Instance();
#endif /* RD_TACTILE_FEEDBACK  */
    
    EnableDragEvents();
        
	//	Activate control
    ActivateL();
}

//=============================================================================
void CImageEditorTextControl::SetView (CAknView * aView)
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::SetView()");
    iEditorView = aView;
}

//=============================================================================
void CImageEditorTextControl::SetSelectedUiItemL (CPluginInfo * aItem)
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::SetSelectedUiItemL()");
    iItem = aItem;
    iNaviPaneText.Copy ( KEmptyString );
    iEditorView->HandleCommandL (EImageEditorCmdRender);
}

//=============================================================================
TKeyResponse CImageEditorTextControl::OfferKeyEventL (
    const TKeyEvent &   aKeyEvent,
    TEventCode          aType
    )
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::OfferKeyEventL()");

    TKeyResponse response = EKeyWasNotConsumed;

    //  If busy, do not handle anything
    if ( Busy() )
    {
        response = EKeyWasConsumed;
    }

    // In main state handle the OK Options menu
    else if ( aKeyEvent.iCode == EKeyOK &&
        (iState == EInsertTextStateMain || iState == EInsertTextStateFirst ) )
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
				response = EKeyWasConsumed;
                break;
			}
		
            case EKeyOK:
            {
                if (iState == EInsertTextStateMove || 
                    iState == EInsertTextStateRotate  ||
                    iState == EInsertTextStateResize)
                {
                    ToMainStateL();
                    response = EKeyWasConsumed;
                }
                break;
            }
            
            case EKeyEnter:
            {   
                if (iState == EInsertTextStateMove || 
                    iState == EInsertTextStateRotate  ||
                    iState == EInsertTextStateResize)
                {   
                    iPopupController->HideInfoPopupNote();
                    ToMainStateL();
                }
                else if ( iState == EInsertTextStateMain )
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
                if ( iState == EInsertTextStateRotate )
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
		        if ( iState == EInsertTextStateRotate )
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
		    case 0x35: // 5
		    case 0x36: // 6
		    case 0x38: // 8
			case EStdKeyIncVolume: // zoom in key
			case EStdKeyDecVolume: // zoom out key
			case 0x2a: // *	
			case 0x23: // #	
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
void CImageEditorTextControl::SizeChanged()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::SizeChanged()");

}

//=============================================================================
TDesC & CImageEditorTextControl::GetParam ()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::GetParam()");
    LOGDES(KTextPluginLogFile, iParam);
	return iParam;
}

//=============================================================================
void CImageEditorTextControl::SetSystemParameters (const CSystemParameters * aPars) 
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::SetSystemParameters()");
    iSysPars = aPars;
}

//=============================================================================
void CImageEditorTextControl::SetTextL (const TDesC & aText)
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::SetTextL()");
    iText.Copy (aText);
}

//=============================================================================
void CImageEditorTextControl::HandlePluginCommandL (const TInt aCommand)
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::HandlePluginCommandL()");

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
            ToMainStateL();
            break;
        }
        case EPgnSoftkeyIdCancel:
        {
            iPopupController->HideInfoPopupNote();
            if (iState == EInsertTextStateMain ||
                iState == EInsertTextStateFirst)
                {
                // Cancel plugin
                iState = EInsertTextStateMin;

                //  When plug-in is cancelled, delete text buffer.
                iEditorView->HandleCommandL (EImageEditorCancelPlugin);
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
        case EPgnSoftkeyIdDone:
        case ETextPgnMenuCmdDone:
        {   
            iPopupController->HideInfoPopupNote();
            ToMainStateL();
            iState = EInsertTextStateMin;
            iEditorView->HandleCommandL (EImageEditorApplyPlugin);
            break;
        }
        case ETextPgnMenuCmdMove:
        {
            ToMoveStateL();
            break;
        }
        case ETextPgnMenuCmdResize:
        {
            ToResizeStateL();
            break;
        }
        case ETextPgnMenuCmdRotate:
        {
            ToRotateStateL();
            break;
        }
        case ETextPgnMenuCmdColor:
        {                                                               
            if ( SDrawUtils::LaunchColorSelectionPopupL (iPreview, 
                                                         Rect(), 
                                                         iRgb) )
            {
            	StoreParameters();
                iEditorView->HandleCommandL (EImageEditorCmdRender);
            }
            break;
        }
        case ETextPgnMenuCmdFont:
        {
            // Not implemented
            break;
        }
        case ETextPgnMenuCmdCancel:
        {
			iReadyToRender = EFalse;
            iState = EInsertTextStateMin;
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
			StoreParameters();
            StoreTempParams();
	    	iEditorView->HandleCommandL (EImageEditorCmdRender);

			DrawNow();
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
			StoreParameters();
            StoreTempParams();
	    	iEditorView->HandleCommandL (EImageEditorCmdRender);
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
TInt CImageEditorTextControl::GetSoftkeyIndexL()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::GetSoftkeyIndexL()");

    TInt state = 0;

    if (iState == EInsertTextStateFirst)
        {
        state = 0;  // Options - Cancel
        }
    else if (iState == EInsertTextStateMove || 
             iState == EInsertTextStateRotate  ||
             iState == EInsertTextStateResize)
        {
        state = 1; // Ok - Cancel
        }
    else
        {
        state = 2; // Options - Done 
        }

    return state;

}

//=============================================================================
TInt CImageEditorTextControl::GetContextMenuResourceId()
{
return R_TEXT_CONTEXT_MENUBAR;    
}

//=============================================================================
TBitField CImageEditorTextControl::GetDimmedMenuItems()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::GetDimmedMenuItems()");

    TBitField dimmedMenuItems;
    TInt count = iItem->MenuItems().Count();

    if ( iDisplayingOkOptionsMenu )
    {
        // Dim the command EImageEditorCancelPlugin
        for ( TInt i = 0; i < count; i++)
        {
            // get the menu item id
            TInt menuItem = iItem->MenuItems().At(i).iCommandId;
            if ( menuItem == ETextPgnMenuCmdCancel ) 
            {
                dimmedMenuItems.SetBit( i );
            }
        }
        iDisplayingOkOptionsMenu = EFalse;
    }

    return dimmedMenuItems;
}

//=============================================================================
TPtrC CImageEditorTextControl::GetNaviPaneTextL (
    TBool& aLeftNaviPaneScrollButtonVisibile, 
    TBool& aRightNaviPaneScrollButtonVisible )
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::GetNaviPaneTextL()");

    aLeftNaviPaneScrollButtonVisibile = EFalse;
    aRightNaviPaneScrollButtonVisible = EFalse;
    return iNaviPaneText;
}

//=============================================================================
void CImageEditorTextControl::Draw (const TRect & aRect) const
{
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
void CImageEditorTextControl::NaviDown()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::NaviDown()");

    switch (iState) 
    {
        case EInsertTextStateMove:
        {
            iY += (KPosParamStep * iNaviStepMultiplier);
            if ( iY > iSysPars->VisibleImageRect().iBr.iY )
            {
            	iY = iSysPars->VisibleImageRect().iBr.iY;
            }
    	    break;
        }
        case EInsertTextStateResize:
        {
            iScale -= (KScaleParamStep * iNaviStepMultiplier);
            if (iScale < iScaleMin)
            {
                iScale = iScaleMin;
            }
    	    break;
        }
        case EInsertTextStateRotate:
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
void CImageEditorTextControl::NaviUp()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::NaviUp()");
    
    switch (iState) 
    {
        case EInsertTextStateMove:
        {
            iY -= (KPosParamStep * iNaviStepMultiplier);
            if ( iY < iSysPars->VisibleImageRect().iTl.iY )
            {
            	iY = iSysPars->VisibleImageRect().iTl.iY;
            }
    	    break;
        }
        case EInsertTextStateResize:
        {
            iScale += (KScaleParamStep * iNaviStepMultiplier);
            if (iScale > iScaleMax)
            {
                iScale = iScaleMax;
            }
    	    break;
        }
        case EInsertTextStateRotate:
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
void CImageEditorTextControl::NaviRight()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::NaviRight()");

    switch (iState) 
    {
        case EInsertTextStateMove:
        {
            iX += (KPosParamStep * iNaviStepMultiplier);
            if ( iX > iSysPars->VisibleImageRect().iBr.iX )
            {
            	iX = iSysPars->VisibleImageRect().iBr.iX;
            }
    	    break;
        }
        case EInsertTextStateResize:
        {
            iScale += (KScaleParamStep * iNaviStepMultiplier);
            if (iScale > iScaleMax)
            {
                iScale = iScaleMax;
            }
    	    break;
        }
        case EInsertTextStateRotate:
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
void CImageEditorTextControl::NaviLeft()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::NaviLeft()");

    switch (iState) 
    {
        case EInsertTextStateMove:
        {
            iX -= (KPosParamStep * iNaviStepMultiplier);
            if ( iX < iSysPars->VisibleImageRect().iTl.iX )
            {
            	iX = iSysPars->VisibleImageRect().iTl.iX;
            }
    	    break;
        }
        case EInsertTextStateResize:
        {
            iScale -= (KScaleParamStep * iNaviStepMultiplier);
            if (iScale < iScaleMin)
            {
                iScale = iScaleMin;
            }
    	    break;
        }
        case EInsertTextStateRotate:
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
void CImageEditorTextControl::ToMoveStateL()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::ToMoveStateL()");

    iState = EInsertTextStateMove;
    iNaviPaneText.Copy ( iItem->Parameters()[KMoveTextIndex] );
    StoreTempParams();
    iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
    iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
    LoadIndicatorL (
        EMbmImageeditoruiQgn_indi_imed_move_super, 
        EMbmImageeditoruiQgn_indi_imed_move_super_mask);
    DrawNow();
    ShowTooltip();
}

//=============================================================================
void CImageEditorTextControl::ToResizeStateL()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::ToResizeStateL()");

    iState = EInsertTextStateResize;
    iNaviPaneText.Copy ( iItem->Parameters()[KResizeTextIndex] );
    StoreTempParams();
    iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
    iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
    LoadIndicatorL (
        EMbmImageeditoruiQgn_indi_imed_resize_super, 
        EMbmImageeditoruiQgn_indi_imed_resize_super_mask
        );
    DrawNow();
    ShowTooltip();
}

//=============================================================================
void CImageEditorTextControl::ToRotateStateL()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::ToRotateStateL()");

    iState = EInsertTextStateRotate;
    iNaviPaneText.Copy ( iItem->Parameters()[KRotateTextIndex] );
    StoreTempParams();
    iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
    iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
    LoadIndicatorL (
        EMbmImageeditoruiQgn_indi_imed_rotate_left_super,
        EMbmImageeditoruiQgn_indi_imed_rotate_left_super_mask
        );
    DrawNow();
    ShowTooltip();
}

//=============================================================================
void CImageEditorTextControl::ToMainStateL()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::ToMainStateL()");

    //  Delete old indicator
    delete iIndicator;
    iIndicator = 0;
    delete iIndicatorMask;
    iIndicatorMask = 0;

    iState = EInsertTextStateMain;
    iNaviPaneText.Copy ( iItem->Parameters()[KMainTextIndex] );
    iEditorView->HandleCommandL (EImageEditorUpdateSoftkeys);
    iEditorView->HandleCommandL (EImageEditorUpdateNavipane); 
    DrawNow();
}

//=============================================================================
void CImageEditorTextControl::StoreTempParams()
    {
    LOG(KTextPluginLogFile, "CImageEditorTextControl::StoreTempParams()");

    iTempX = iX;
    iTempY = iY;
    iTempScale = iScale;
    iTempAngle = iAngle;
    }

//=============================================================================
void CImageEditorTextControl::RestoreTempParams()
    {
    LOG(KTextPluginLogFile, "CImageEditorTextControl::RestoreTempParams()");

    iX = iTempX;
    iY = iTempY;
    iScale = iTempScale;
    iAngle = iTempAngle;
    }

//=============================================================================
void CImageEditorTextControl::GetHelpContext(TCoeHelpContext& aContext) const
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::GetHelpContext()");

    aContext.iMajor = TUid::Uid(UID_IMAGE_EDITOR);
    aContext.iContext = KSIE_HLP_EDIT_TEXT;
}

//=============================================================================
void CImageEditorTextControl::PrepareL ()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::PrepareL()");
    
	//	Get current view port
	TRect rect = iSysPars->VisibleImageRect();
	
	//	Set default position
	iX = (rect.iTl.iX + rect.iBr.iX) / 2;
	iY = (rect.iTl.iY + rect.iBr.iY) / 2;
	
	//	Set default angle
	iAngle = KAngleParamDef;

	//	Scale to width
	iScale = ( rect.Height() - 1 ) / 8;
	iScaleMin = ( rect.Height() - 1 ) / 20;
	iScaleMax = ( rect.Height() - 1 ) / 3;

	//	Set default color to white
	iRgb.SetRed (255);
	iRgb.SetGreen (255);
	iRgb.SetBlue (255);

	//	Set position
	iParam.Append (_L("x "));
	iParam.AppendNum (iX);

	iParam.Append (_L(" y "));
	iParam.AppendNum (iY);

	//	Set angle
	iParam.Append (_L(" angle "));
	iParam.AppendNum (iAngle);

	//	Create screen font	
    const CFont * font = AknLayoutUtils::FontFromId(KLogicalFontInUse);
	iParam.Append (_L(" font "));
	iParam.AppendNum ((TInt)font);

	//	Set maximum text image width and height
	LOGFMT (KTextPluginLogFile, "fontwidth %d", font->TextWidthInPixels(_L("W") ));
	iParam.Append (_L(" textwidth "));
	iParam.AppendNum ( KMaxColumns * font->TextWidthInPixels(_L("W") ));
	iParam.Append (_L(" textheight "));
	iParam.AppendNum ( KMaxRows * font->HeightInPixels() );

	//	Set text
	iParam.Append (_L(" text \""));
	iParam.Append (iText);
	iParam.Append (_L("\""));

	//	Set scale
	iParam.Append (_L(" height "));
	iParam.AppendNum (iScale);
	
	iReadyToRender = ETrue;
    iEditorView->HandleCommandL (EImageEditorCmdRender);
    
    ToMoveStateL();
}

//=============================================================================
void CImageEditorTextControl::LoadIndicatorL (
    TInt    aBitmapInd,
    TInt    aMaskInd
    ) 
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::LoadIndicatorL()");

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

//=============================================================================
TPoint CImageEditorTextControl::ComputeIndicatorPosition() const
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
	    
	TInt s = (iScale * vpprect.Height()) / vprect.Height();
	y = ((iY - vprect.iTl.iY) * vpprect.Height()) / vprect.Height();
	y += vpprect.iTl.iY;
	y -= iIndicator->SizeInPixels().iHeight / 4;
	y += s / 2;

    // Does not work if multiple lines
    TInt tw = AknLayoutUtils::FontFromId(KLogicalFontInUse)->TextWidthInPixels(iText.Left(KMaxColumns));     
    TInt th = AknLayoutUtils::FontFromId(KLogicalFontInUse)->HeightInPixels();

	x = ((iX  - vprect.iTl.iX) * vpprect.Width()) / vprect.Width();
	x += vpprect.iTl.iX;
	x -= iIndicator->SizeInPixels().iWidth / 2;
	x -= (tw * s) / (2 * th);

    return TPoint (x,y);
}

//=============================================================================
TBool CImageEditorTextControl::IsReadyToRender () const
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::IsReadyToRender()");

	return iReadyToRender;
}

//=============================================================================
void CImageEditorTextControl::StoreParameters()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::StoreParameters()");

	iParam.Copy (_L("x "));
	iParam.AppendNum (iX);
	iParam.Append (_L(" y "));
	iParam.AppendNum (iY);
	iParam.Append (_L(" angle "));
	iParam.AppendNum (iAngle);
	iParam.Append (_L(" red "));
	iParam.AppendNum ( iRgb.Red() );
	iParam.Append (_L(" green "));
	iParam.AppendNum ( iRgb.Green() );
	iParam.Append (_L(" blue "));
	iParam.AppendNum ( iRgb.Blue() );
	iParam.Append (_L(" height "));
	iParam.AppendNum (iScale);
}

//=============================================================================
void CImageEditorTextControl::TimerCallBack()
{
    LOG(KTextPluginLogFile, "CImageEditorTextControl::TimerCallBack()");

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
	    StoreParameters();
	    TRAP_IGNORE( iEditorView->HandleCommandL (EImageEditorCmdRender) );
		iTimer->Call (KWait);			
	}
}

//=============================================================================
void CImageEditorTextControl::StorePosAndScaleRelScreen()
{

    LOG(KTextPluginLogFile, "CImageEditorClipartControl::StorePosAndScaleRelScreen()");

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
	if (iScale == iScaleMax)
	{
		iScaleOld = iScaleMax;
	}
	else if (iScale == iScaleMin)
	{
		iScaleOld = iScaleMin;
	}
	else
	{
		TInt dimold_pix = (TInt)((iScale / relscale) + 0.5); 
		if ( viwidth > viheight )
		{
			iScaleOld = (TInt)((TReal)(dimold_pix * vipheight) / viheight + 0.5);
		}
		else
		{
			iScaleOld = (TInt)((TReal)(dimold_pix * vipwidth) / viwidth + 0.5);
		}
	}


	//	Position
	TInt xCurrent = (TInt)((iX / relscale) + 0.5); 
	TInt yCurrent = (TInt)((iY / relscale) + 0.5); 
	iPosXOld = viprect.iTl.iX + (TReal)((xCurrent - virect.iTl.iX) * vipwidth) / viwidth;			
	iPosYOld = viprect.iTl.iY + (TReal)((yCurrent - virect.iTl.iY) * vipheight) / viheight;			

	LOGFMT(KTextPluginLogFile, "\tiPosXOld = %d", iPosXOld);
	LOGFMT(KTextPluginLogFile, "\tiPosYOld = %d", iPosYOld);

	iParam.Copy(_L("nop"));	
}

//=============================================================================
void CImageEditorTextControl::RestorePosAndScaleRelScreen()
{
	LOG(KTextPluginLogFile, "CImageEditorClipartControl::RestorePosAndScaleRelScreen()");

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
	if (iScale == iScaleMax)
	{
		iScale = iScaleOld;
	}
	else if (iScale == iScaleMin)
	{
		iScale = iScaleOld;
	}
	else
	{
		if ( viwidth > viheight )
		{
			iScale = (TInt)((iScaleOld * viheight) / vipheight + 0.5);
		}
		else
		{
			iScale = (TInt)((iScaleOld * viwidth) / vipwidth + 0.5);
		}
		iScale = (TInt)(iScale * relscale + 0.5); 

	}


	//	Position
	iX = (TInt)(virect.iTl.iX + (TReal)((iPosXOld - viprect.iTl.iX) * viwidth) / vipwidth + 0.5);
	iY = (TInt)(virect.iTl.iY + (TReal)((iPosYOld - viprect.iTl.iY) * viheight) / vipheight + 0.5);

}


//=============================================================================
void CImageEditorTextControl::StorePosAndScaleRelImage()
{
    LOG(KTextPluginLogFile, "CImageEditorClipartControl::StorePosAndScaleRelImage()");
	TReal relscale = iSysPars->RelScale();
	iPosXOld = (TInt)((iX / relscale) + 0.5);
	iPosYOld = (TInt)((iY / relscale) + 0.5);
	iScaleOld = (TInt)(iScale / relscale + 0.5);
	iParam.Copy(_L("nop"));	
}

//=============================================================================
void CImageEditorTextControl::RestorePosAndScaleRelImage()
{
    LOG(KTextPluginLogFile, "CImageEditorClipartControl::RestorePosAndScaleRelImage()");
	TReal relscale = iSysPars->RelScale();
	iX = (TInt)(iPosXOld * relscale + 0.5);
	iY = (TInt)(iPosYOld * relscale + 0.5);
	iScale = (TInt)(iScaleOld * relscale + 0.5);
}

//=============================================================================
void CImageEditorTextControl::ClipPosition()
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
void CImageEditorTextControl::HandlePointerEventL(
                                            const TPointerEvent &aPointerEvent)
    {            
    if( AknLayoutUtils::PenEnabled() )
		{
		TBool render = ETrue;	
		switch( aPointerEvent.iType )
			{
			case TPointerEvent::EButton1Down:
				{
				if ( iState == EInsertTextStateMove )
				    {	
				    iPopupController->HideInfoPopupNote();				   		    
				    SetTextPosition( aPointerEvent.iPosition );
				    }			
				else if ( iState == EInsertTextStateRotate )
				    {
				    iPopupController->HideInfoPopupNote();
				    // Store current position. Rotating is handled in drag-event
				    // is pen position has changed
				    iPointerPosition = aPointerEvent.iPosition;
				    }
			    else if ( iState == EInsertTextStateResize )
				    {
				    iPopupController->HideInfoPopupNote();
				    iPointerPosition = aPointerEvent.iPosition;
				    }    
				    
#ifdef RD_TACTILE_FEEDBACK
				if ( iTouchFeedBack )
					{
					iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
					RDebug::Printf( "ImageEditor::ImageEditorTextControl: ETouchFeedback" );
					}
#endif /* RD_TACTILE_FEEDBACK  */
				
				break;
				}
			case TPointerEvent::EDrag:
				{
				if ( iState == EInsertTextStateMove )
				    {
				    SetTextPosition( aPointerEvent.iPosition );
				    // store current position for next round
                    iPointerPosition = aPointerEvent.iPosition;
				    }				
				else if ( iState == EInsertTextStateRotate )
				    {
				    if ( CalculateRotation( iPointerPosition, aPointerEvent.iPosition ) )
        			    {
        			    // store current position for next round
                        iPointerPosition = aPointerEvent.iPosition;
        			    }
        			else
      			        {
                        render = EFalse;  			        
    			        }    			        				    
				    }				
				else if ( iState == EInsertTextStateResize )
				    {
				    if ( CalculateResize( iPointerPosition, aPointerEvent.iPosition ) )
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
				if ( iState == EInsertTextStateMain )
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
void CImageEditorTextControl::SetTextPosition( TPoint aPointedPosition )
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
void CImageEditorTextControl::ShowTooltip()
    {   
    iPopupController->HideInfoPopupNote();

    TPoint iconPosition = ComputeIndicatorPosition();
    TRect iconRect( iconPosition.iX, iconPosition.iY, 
                    iconPosition.iX, iconPosition.iY );

    if ( iState == EInsertTextStateMove )
        {
        SDrawUtils::ShowToolTip( iPopupController,
                                 this,
                                 iconPosition,
                                 EHRightVTop,
                                 *iTooltipMove );
        }
    // resize
    else if ( iState == EInsertTextStateResize )
        {                                     
        SDrawUtils::ShowToolTip( iPopupController,
                                 this,
                                 iconPosition,
                                 EHRightVTop, 
                                 *iTooltipResize );                                 
        }
    // rotate
    else if ( iState == EInsertTextStateRotate ) 
        {
        SDrawUtils::ShowToolTip( iPopupController,
                                 this,
                                 iconPosition,
                                 EHRightVTop,
                                 *iTooltipRotate );
        }
    
    }

//=============================================================================
TBool CImageEditorTextControl::CalculateRotation( TPoint aStartPoint, 
                                                 TPoint aEndPoint )
    {    
    TBool angleChanged( EFalse );
    TInt oldAngle = iAngle;
    
    // Get system parameters
   	TRect visibleImageRect( iSysPars->VisibleImageRect() );
   	TRect visibleImageRectPrev( iSysPars->VisibleImageRectPrev() );
   	
   	// Calculate Text real center point on the screen (physical coordinates)	
   	TReal posFactorX( TReal( iX - visibleImageRect.iTl.iX ) /
   	                            visibleImageRect.Width() );
   	TInt textCenterX = posFactorX * visibleImageRectPrev.Width() + 
   	                                   visibleImageRectPrev.iTl.iX;
   	
   	TReal posFactorY( TReal( iY - visibleImageRect.iTl.iY ) / 
   	                            visibleImageRect.Height() );
   	TInt textCenterY = posFactorY * visibleImageRectPrev.Height() + 
   	                                   visibleImageRectPrev.iTl.iY;
   	
    // Calculate start and end positions of the movement assuming that
    // text centre is in origo.
    // Note! y-axis is mirrored on screen coordinates compared to standard 2-d
    // co-ordinates->mirror y-axis to ease the calculation
   	TPoint startPos( ( aStartPoint.iX - textCenterX ), 
                       ( textCenterY - aStartPoint.iY ) );    
    TPoint endPos( ( aEndPoint.iX - textCenterX ), 
                     ( textCenterY - aEndPoint.iY ) );

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

//=============================================================================
TBool CImageEditorTextControl::CalculateResize( TPoint aStartPoint, 
                                                TPoint aEndPoint )
    {
        
    // Whether bubble is resized in this function or not
    TBool textResized( EFalse );
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
       	textResized = ETrue;
        }
    if (iScale < iScaleMin)
        {
        iScale = iScaleMin;
        textResized = ETrue;
        }
    
    if ( oldScale != iScale )    
        {
        textResized = ETrue;
        }
        
    return textResized;
                
    }   
    
    
// End of File
