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



//  INCLUDES
#include <fbs.h>
#include <f32file.h>
#include <badesca.h>
#include <gdi.h>
#include <eikenv.h>
#include <bitdev.h>
#include <bautils.h>

#include <aknview.h>
#include <aknutils.h>
#include <ConeResLoader.h>

#ifdef RD_TACTILE_FEEDBACK 
#include <touchfeedback.h>
#endif /* RD_TACTILE_FEEDBACK  */

#include "ImageEditorUI.hrh"
#include "ImageEditorPluginBase.hrh"
#include "Frame.hrh"
#include "PluginInfo.h"
#include "ImageEditorError.h"
#include "ResolutionUtil.h"
#include <frame.rsg>

#include "ImageEditorFrameControl.h"
#include "ImageEditorFramePlugin.h"
#include "ImageEditorFramePlugin.pan"

//#include "platform_security_literals.hrh"


//	CONSTANTS
const TInt KCurrentFrameIndex           = 1;
const TInt KFrameChangeThreshold        = 30;
const TInt KDirectionChangeThreshold    = 7;

_LIT (KPgnResourceFile, "frame.rsc");
_LIT (KFrameWild, "*.mbm");

//=============================================================================
CImageEditorFrameControl * CImageEditorFrameControl::NewL (
	const TRect &		aRect,
	CCoeControl	*		aParent
	)
{
    CImageEditorFrameControl * self = new (ELeave) CImageEditorFrameControl;
    CleanupStack::PushL (self);
    self->ConstructL (aRect, aParent);
    CleanupStack::Pop ();   // self
    return self;
}

//=============================================================================
CImageEditorFrameControl::CImageEditorFrameControl () :
iState (EFrameControlStateMin)
{
    
}

//=============================================================================
CImageEditorFrameControl::~CImageEditorFrameControl ()
{
    iMultiplicities.Close();

    if (iFrameFileArray)
        {
        iFrameFileArray->Reset();
        delete iFrameFileArray;
        iFrameFileArray = NULL;
        }
    iEditorView = NULL;
    iItem = NULL;
}

//=============================================================================
void CImageEditorFrameControl::ConstructL (
	const TRect &		/*aRect*/,
	CCoeControl	*		aParent
	)
{

	//	Set parent window
	SetContainerWindowL (*aParent);
   	
   	iFrameFileArray = new (ELeave) CDesCArrayFlat (8);

#ifdef RD_TACTILE_FEEDBACK 
    iTouchFeedBack = MTouchFeedback::Instance();
#endif /* RD_TACTILE_FEEDBACK  */
    
	//	Activate control
    ActivateL();
    
    EnableDragEvents();
}

//=============================================================================
void CImageEditorFrameControl::SetView (CAknView * aView)
{
    iEditorView = aView;
}

//=============================================================================
void CImageEditorFrameControl::SetSelectedUiItemL (CPluginInfo * aItem)
{
    iItem = aItem;
}

//=============================================================================
void CImageEditorFrameControl::PrepareL ()
{
	iState = EFrameControlStateInitializing;

    //	Find frames
    FindFramesL();

	//	Select the first frame to be drawn
    iCurrentFrameIndex = 0;
    SetNaviPaneTextL();
    
    //	Load the first frame synchronously to avoid timing problems
    SelectFrameL();
    
    iState = EFrameControlStateIdle;
}

//=============================================================================
TKeyResponse CImageEditorFrameControl::OfferKeyEventL (
    const TKeyEvent &   aKeyEvent,
    TEventCode          aType
    )
{
    TKeyResponse response = EKeyWasNotConsumed;

    //  If busy, do not handle anything
    if ( iState < EFrameControlStateIdle )
    {
        response = EKeyWasConsumed;
    }
    else if (aType != EEventKey  )
    {
        switch (aKeyEvent.iScanCode)
		{   
		    // Just consume these keys (No EEventKey type event delivered for 
		    // these keys so this must be done here)
		    case EStdKeyRightShift:
		    case EStdKeyLeftShift:
			{
				response = EKeyWasConsumed;
                break;
			}
		}
    }
    //  We handle only event keys
    else if (EEventKey == aType)
    {

		TBool rotated = CResolutionUtil::Self()->GetLandscape();

		switch (aKeyEvent.iCode)
		{

			// Just consume these keys
		    case 0x31: // 1
		    case 0x33: // 3
		    case 0x35: // 5
            case 0x37: // 7
		    case 0x39: // 9
		    case 0x2a: // *
		    case 0x23: // #
			case EKeyDownArrow:
			case EKeyUpArrow:
			case EStdKeyIncVolume: // zoom in key
			case EStdKeyDecVolume: // zoom out key
			{
				response = EKeyWasConsumed;
                break;
			}

		    case 0x32: // 2
			{
				if (rotated)
				{
                	//  Switch to next frame
	                NaviLeftL();					
				}
				response = EKeyWasConsumed;	
                break;
			}
			case 0x38: // 8
			{
				if (rotated)
				{
                	//  Switch to next frame
	                NaviRightL();					
				}
				response = EKeyWasConsumed;	
                break;
			}

   		    case 0x34:   		    
   		    {
  				if (!rotated)
				{
		            //  Switch to previous frame
		            NaviLeftL();
				}
				response = EKeyWasConsumed;
                break;
			}

		    case 0x36:
			{
				if (!rotated)
				{
                	//  Switch to next frame
	                NaviRightL();					
				}
				response = EKeyWasConsumed;	
                break;
			}

			
			case EKeyRightArrow:
			{
				NaviRightL();					
				response = EKeyWasConsumed;	
                break;			
			}	           

            case EKeyLeftArrow:
            {
            	NaviLeftL();					
				response = EKeyWasConsumed;	
                break;			
            }
            
            case EKeyOK:
            case EKeyEnter:
            {
				iEditorView->HandleCommandL (EImageEditorApplyPlugin);
                response = EKeyWasConsumed;
                break;
            }

			default:
			{
				break;
			}
		}
	}
    return response;
}

//=============================================================================
void CImageEditorFrameControl::SizeChanged()
{

}

//=============================================================================
TDesC & CImageEditorFrameControl::GetParam ()
{
	return iParameter;
}

//=============================================================================
void CImageEditorFrameControl::HandlePluginCommandL (const TInt aCommand)
{
    switch (aCommand) 
    {
        case EPgnSoftkeyIdCancel:
        {
            iEditorView->HandleCommandL (EImageEditorCancelPlugin);
            break;
        }
        case EPgnSoftkeyIdOk:
        {
            iEditorView->HandleCommandL (EImageEditorApplyPlugin);
            break;
        }
        default:
        {
            break;
        }
    }
}

//=============================================================================
TInt CImageEditorFrameControl::GetSoftkeyIndexL()
{
    return 0;
}

//=============================================================================
TPtrC CImageEditorFrameControl::GetNaviPaneTextL (
    TBool& aLeftNaviPaneScrollButtonVisibile, 
    TBool& aRightNaviPaneScrollButtonVisible )
{
    if ( iFrameCount > 1)
    {
        aLeftNaviPaneScrollButtonVisibile = ETrue;
        aRightNaviPaneScrollButtonVisible = ETrue;
    }
    else
    {
        aLeftNaviPaneScrollButtonVisibile = EFalse;
        aRightNaviPaneScrollButtonVisible = EFalse;
    }
    return iNaviPaneText;
}

//=============================================================================
void CImageEditorFrameControl::Draw (const TRect & aRect) const
{
    CPreviewControlBase::DrawPreviewImage (aRect);
}

//=============================================================================
void CImageEditorFrameControl::NaviRightL()
{
    LOG(KFramePluginLogFile, "CImageEditorFrameControl::NaviRightL()");

	// Switch to the next frame bitmap
    iCurrentFrameIndex++;  
    if (iCurrentFrameIndex > iFrameCount - 1 )
    {
        // loop back to first
        iCurrentFrameIndex = 0;
    }
    SetNaviPaneTextL();
    SelectFrameL();
}

//=============================================================================
void CImageEditorFrameControl::NaviLeftL()
{
    LOG(KFramePluginLogFile, "CImageEditorFrameControl::NaviLeftL()");

	// Switch to the previous frame bitmap
    iCurrentFrameIndex--;
    if ( iCurrentFrameIndex < 0 )
    {
        // loop to last
        iCurrentFrameIndex = iFrameCount - 1;
    }
    SetNaviPaneTextL();
    SelectFrameL();
}

//=============================================================================
void CImageEditorFrameControl::FindFramesL()
{
    LOG(KFramePluginLogFile, "CImageEditorFrameControl::FindFrames()");

    //	Read resource
	TFileName resourcefile;
	resourcefile.Append(KPgnResourcePath);
	resourcefile.Append(KPgnResourceFile);

    User::LeaveIfError( CompleteWithAppPath( resourcefile ) );

    RConeResourceLoader resLoader( *ControlEnv() );
    resLoader.OpenL ( resourcefile );
    CleanupClosePushL(resLoader);

    CDesCArrayFlat* array = ControlEnv()->ReadDesCArrayResourceL(R_FRAME_DIRS);
    CleanupStack::PushL(array);

    TFileName pathList;
    _LIT(KPathSeparator, ";");

    for (TInt k = 0; k < array->Count(); ++k)
        {
        TPtrC path = (*array)[k];
        LOGFMT(KFramePluginLogFile, "\tSearch path: %S", &path);
        pathList.Append(path);
        pathList.Append(KPathSeparator);
        }

	//  Create a file finder
    TFindFile fileFinder (ControlEnv()->FsSession()); 
    CDir * fileList = 0; 

    //  Find files by wild card and directory
    TInt err = fileFinder.FindWildByPath (
		KFrameWild, 
		&pathList, 
		fileList
		);

    User::LeaveIfError(err);

    CleanupStack::PopAndDestroy(2); // resLoader, array

	//	Go through all drives
    while (err == KErrNone)
    {
        CleanupStack::PushL (fileList);

		//	Check all plug-in candidates
        for (TInt i = 0; i < fileList->Count(); ++i) 
        {

            //	Create a full file name for a frame file
            TParse fullentry;
            TPtrC name = (*fileList)[i].iName;
            const TDesC* related = &(fileFinder.File());
            fullentry.Set ( name, related, 0 );
            TPtrC fullname (fullentry.FullName());

            //  Store frame file names
            iFrameFileArray->AppendL (fullname);

            //  Check the amount of frames 
            TInt err_bmp = KErrNone;
            TInt err_bmp_mask = KErrNone;
            TInt j = 0;
            while (KErrNone == err_bmp && KErrNone == err_bmp_mask)
            {
                CFbsBitmap * tmp = new (ELeave) CFbsBitmap;
                CleanupStack::PushL (tmp);
                err_bmp = tmp->Load (fullname, j);
                CleanupStack::PopAndDestroy(); // tmp

                tmp = new (ELeave) CFbsBitmap;
                CleanupStack::PushL (tmp);
                err_bmp_mask = tmp->Load (fullname, j + 1);
                CleanupStack::PopAndDestroy(); // tmp

                if (KErrNone == err_bmp && KErrNone == err_bmp_mask)
                {
                    ++iFrameCount;
                }
                j += 2;
            }
            User::LeaveIfError( iMultiplicities.Append (j - 1) );
        }
    
		CleanupStack::PopAndDestroy(); /// fileList

		//	Try once again
        err = fileFinder.FindWild (fileList);

    }

}

//=============================================================================
void CImageEditorFrameControl::SelectFrameL ()
{
    LOG(KFramePluginLogFile, "CImageEditorFrameControl::SelectFrameL ()");

    //  Select frame
    TInt    file = 0;
    TInt    frame = 0;
    TInt    index = 0;
    TBool   bContinue = ETrue;
    for (TInt i = 0 ; (i < iFrameFileArray->Count()) && bContinue; ++i)
    {
        for (TInt j = 0 ; j < iMultiplicities[i]; j += 2)
        {
            if (index == iCurrentFrameIndex)
            {
                file = i;
                frame = j;
                bContinue = EFalse;
                break;
            }
            ++index;
        }
    }

	iParameter.Copy ( _L("file \""));
	iParameter.Append ( (*iFrameFileArray)[file] );
	iParameter.Append ( _L("\" frame ") );
	iParameter.AppendNum ( frame );
	iParameter.Append ( _L(" mask ") );
	iParameter.AppendNum ( frame + 1 );
	iParameter.Append ( _L(" load"));

	iEditorView->HandleCommandL (EImageEditorCmdRender);
   
}

//=============================================================================
void CImageEditorFrameControl::SetNaviPaneTextL()
{
    // Update the navi pane text with the frame index and count
    // (make sure this is not called before the view exists)
    if ( iEditorView && iItem )
    {
        // .loc file parameter format strings
        _LIT(KParameter1, "%0N");
        _LIT(KParameter2, "%1N");

        // Getting string from resources
        TPtrC ptr = iItem->Parameters()[KCurrentFrameIndex]; 

        // Finding format patterns
        TInt pos1 = ptr.Find(KParameter1);
        TInt pos2 = ptr.Find(KParameter2);
        
        if(pos1 == KErrNotFound || pos2 == KErrNotFound)
        {
            User::Leave(KSIEEInternal);
        }

        // Clear navi pane 
        iNaviPaneText.Zero();

        // Add new text and replace format strings with current frame
        // index and frame count
	    iNaviPaneText.Append(ptr.Left(pos1));
        iNaviPaneText.AppendNum (  iCurrentFrameIndex + 1 );
        iNaviPaneText.Append (ptr.Mid(pos1 + 3, pos2 - (pos1 + 3)));
        iNaviPaneText.AppendNum ( iFrameCount );
	    iNaviPaneText.Append (ptr.Mid(pos2 + 3));
        
        AknTextUtils::LanguageSpecificNumberConversion  (  iNaviPaneText );
        
        iEditorView->HandleCommandL (EImageEditorUpdateNavipane);
    }
}

//=============================================================================
void CImageEditorFrameControl::HandlePointerEventL(
                                        const TPointerEvent &aPointerEvent )
    {        
    if( AknLayoutUtils::PenEnabled() )
		{	
			
		switch( aPointerEvent.iType )
			{
			case TPointerEvent::EButton1Down:
				{
                // Initialize change values
                iPreviousChange = ENoDirection;
			    iOneDirectionalChange = ETrue;
		        // Store positions
		        iPointerPosition = aPointerEvent.iPosition;
                iInitialPointerPosition = iPointerPosition;
                
#ifdef RD_TACTILE_FEEDBACK
				if ( iTouchFeedBack )
					{
					iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
					RDebug::Printf( "ImageEditor::ImageEditorFrameControl: ETouchFeedback" );
					}
#endif /* RD_TACTILE_FEEDBACK  */
                
				break;
				}
			case TPointerEvent::EDrag:
				{
				// Moving direction is from right to left
				// KDirectionChangeThreshold is here instead of 0, because 
				// touch screen tends to give drag events though finger is
				// kept still on the screen.
				if( ( iPointerPosition.iX - aPointerEvent.iPosition.iX ) > 
				                                KDirectionChangeThreshold )
				    {
				    if( iPreviousChange == ENextFrame )
				        {
				        iOneDirectionalChange = EFalse;
				        }
				    iPreviousChange = EPreviousFrame;
				    }				   
				// Moving direction is from left to right    
				else if(( aPointerEvent.iPosition.iX - iPointerPosition.iX ) > 
				                                KDirectionChangeThreshold )
				    {
				    if (iPreviousChange == EPreviousFrame)
				        {
				        iOneDirectionalChange = EFalse;
				        }
				    iPreviousChange = ENextFrame;
				    }
				
				// store current pen position    
			    iPointerPosition = aPointerEvent.iPosition;
				break;		
				}
			case TPointerEvent::EButton1Up:
			    {			    
			    iFinalPointerPosition = aPointerEvent.iPosition;			    
			    // pen has been moved only one direction (x-wise) 
			    // after button down event
			    if( iOneDirectionalChange )
			        {
			        TInt xChange = iFinalPointerPosition.iX - 
			                       iInitialPointerPosition.iX;
                    TInt yChange = iFinalPointerPosition.iY - 
			                       iInitialPointerPosition.iY;
			                       
			        // check if threshold value has been exceeded and that
			        // x directional change is bigger than y directional
			        if (  xChange > KFrameChangeThreshold  &&
			              Abs( xChange ) > Abs ( yChange  ) )
    			        {
    			        NaviLeftL();
    			        }
			        else if( xChange < ( -KFrameChangeThreshold ) &&
			                 Abs( xChange ) > Abs( yChange ) )
    			        {
    			        NaviRightL();
    			        }
			        }
			        			    			        
			    break;
			    }
			}
		}
    }
// End of File
