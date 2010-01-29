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
* Image Editor AppUI class.
*
*/


//	INCLUDE FILES
#include <fbs.h>
#include <f32file.h>
#include <badesca.h> 
#include <bautils.h> 
#include <e32math.h>
 
#include <aknnotifystd.h> 
#include <aknwaitdialog.h> 
#include <AknGlobalNote.h>
#include <eikprogi.h>
#include <sendui.h>
#include <hlplch.h>
#include <aknnotewrappers.h> 
#include <CMessageData.h>
#include <e32property.h>

#include <AknDlgShut.h>
#include <CAknFileNamePromptDialog.h> 
#include <CAknMemorySelectionDialog.h>
#include <PathInfo.h> 
#include <csxhelp/sie.hlp.hrh>

#ifdef VERBOSE
#include <eikenv.h>
#endif

#include <AknCommonDialogsDynMem.h> 
#include <CAknMemorySelectionDialogMultiDrive.h> 

#include <ImageEditor.rsg>
#include "ImageEditorUI.hrh"
#include "ImageEditorPanics.h"

#include "imageeditoruids.hrh"
#include "commondefs.h"
//#include "platform_security_literals.hrh"

#include "ImageEditorApp.h"
#include "ImageEditorAppUi.h"
#include "ImageEditorUIView.h"
#include "ImageEditorUI.hrh"
#include "ImageEditorError.h"
#include "PluginInfo.h"

#include "ImageEditorPluginLocator.h"
#include "ImageEditorPluginManager.h"
#include "ImageEditorImageController.h"

#include "ImageEditorUtils.h"
#include "ResolutionUtil.h"
#include "Callback.h"


//	CONSTANTS

//  Panic category
_LIT(KComponentName, "ImageEditorAppUi");

const TInt	KObConstructCallBackID          = 1;
const TInt	KObInitPluginID				    = 1000;
const TInt	KObReturnFromPluginID           = 1001;
const TInt  KObCancelCallbackID				= 1002;
const TInt  KObUndoDCallbackID				= 1003;
const TInt  KObSendCallbackID               = 1004;
const TInt  KObPrintCallbackID              = 1005;
const TInt  KObSaveCallbackID				= 1006;

//=============================================================================
CImageEditorAppUi::CImageEditorAppUi() :
    //  Initialize application flags and variables
	iDocumentName(),
	iSourceFileName(),
    iImageSaved             (EFalse),
    iForeground             (ETrue),
    iExitFromApp            (EFalse),
    iSaveOnExit             (ETrue),
    iSysExit                (EFalse),
    iBusy                   (ETrue),
    iOrientationChanged     (EFalse),
    iSaving                 (EFalse),
    iSending                (EFalse),
    iPrinting               (EFalse),
    iCancelling				(EFalse),
    iFullScreen             (EFalse),
    iPluginsScanned         (EFalse),
    iImageLoaded            (EFalse),
    iEditorReady            (EFalse),
    iCancelTextInputActive  (EFalse),
    iProcessPriorityAltered (EFalse)
{

}

//=============================================================================
void CImageEditorAppUi::ConstructL()
{
	LOG(KImageEditorLogFile, "CImageEditorAppUi::ConstructL");

	//	Initialize UI with standard values, read application resource file,
	//	read default document

#ifdef LANDSCAPE_ONLY
    BaseConstructL( EAppOrientationLandscape | EAknEnableSkin | EAknEnableMSK  );
#else 
#ifdef LANDSCAPE_SUPPORT
    BaseConstructL( EAppOrientationAutomatic|EAknEnableSkin | EAknEnableMSK );
#else
    BaseConstructL( EAknEnableSkin | EAknEnableMSK );
#endif
#endif // LANDSCAPE_SUPPORT

    //  Set busy
    SetBusy();

    //	Create main view  
    CImageEditorUIView * view = new (ELeave) CImageEditorUIView;
    CleanupStack::PushL (view);
    view->ConstructL();
    view->ConstructMenuAndCbaEarlyL();
	iEditorView = view;
    AddViewL (view); // transfers ownership
    CleanupStack::Pop();	// view
	LOG (KImageEditorLogFile, "CImageEditorAppUi: Main view created");

    //  Activate view
	ActivateLocalViewL  ( iEditorView->Id() );
    LOG(KImageEditorLogFile, "CImageEditorAppUi: View activated");

    //  Set default navi pane text
    ClearTitlePaneTextL();
    ClearNaviPaneTextL();

    //	Create CObCallback instance
	iConstructionState = EConstructionStateAlloc;
	iCallback = CObCallback::NewL ( (MObCallbackMethod *)this );
    iCallback->DoCallback (KObConstructCallBackID);
   
    // Volume key observer instance
    iVolumeKeyObserver = CImageEditorRemConObserver::NewL( *this );
    
#ifdef S60_31_VOLUME_KEYS
	iMGXRemConTarget = CMGXRemConTarget::NewL( this );
#endif
}

//=============================================================================
CImageEditorAppUi::~CImageEditorAppUi()
{
    LOG(KImageEditorLogFile, "CImageEditorAppUi::~CImageEditorAppUi() starting...");

	//KillWaitNote();
	if (iWaitNote != NULL)
		{
        delete iWaitNote;
        iWaitNote = NULL;
		}
    
    delete iLocator;
    delete iImageController;
    delete iEditorManager;
	delete iSendAppUi;
	delete iCallback;
	iSourceImageMgAlbumIdList.Close();

    iEditorView = NULL;
    
	iFile.Close();
    
#ifdef S60_31_VOLUME_KEYS
	delete iMGXRemConTarget;
#endif
	
	// deleted by CCoeEnv
	iResolutionUtil = NULL;
	delete iVolumeKeyObserver;

    LOG(KImageEditorLogFile, "CImageEditorAppUi: AppUi deleted");
}

//=============================================================================
void CImageEditorAppUi::OperationReadyL ( 
	TOperationCode		aOpId,
	TInt				aError 
	)
{
    LOGFMT2(KImageEditorLogFile, "CImageEditorAppUi:OperationReadyL (aOpId:%d, aError:%d)", (TInt)aOpId, aError);

    //	Kill wait note
    KillWaitNote();

    //	Handle error
	if ( aError != KErrNone )
	{
		switch ( aOpId ) 
		{
			case EOperationCodeLoad:
			case EOperationCodeBlockLoad:
			case EOperationCodeSearch:
            {

                if (KErrCancel == aError)
                {
                    // Loading image was cancelled. 
                    User::Leave (KSIEEErrorOkToExit);
                }
                else
                {
                    User::Leave (KSIEEOpenFile);
                }
                break;
            }
            case EOperationCodeSave:
            case EOperationCodeBlockSave:
            {
                LOG(KImageEditorLogFile, "CImageEditorAppUi: Save cancelled");
#ifdef VERBOSE
                LOGFMT3(KImageEditorLogFile, "\tiSourceFileName: %S, iDocumentName: %S, iTempSaveFileName: %S", &iSourceFileName, &iDocumentName, &iTempSaveFileName);
#endif

                iSaving = EFalse;

                // delete the temporary files
                DeleteTempFiles();

                // Reset bitmap sink and screen size
                UpdateScreenRectL();
                iOrientationChanged = EFalse;


                if (!iExitFromApp)
                {
    
                    if (iSending)
                    {
                        // Restore document name
                        iEditorManager->SetImageName (iDocumentName);
                        iSending = EFalse;
                    }
                    if (KErrCancel == aError)
                    {
                        // KErrCancel means user cancel, not an error.
                        ResetBusy();
                    }
                    else
                    {
                        // Leave in case of error.
                        User::Leave (KSIEESaveFile);
                    }
                }
                else
                {
                	TInt exit = 0;
                    if (iSaveOnExit)
                    {
	                    exit = ((CImageEditorUIView *)iEditorView)->LaunchExitWithoutSavingQueryL();
                    }

                    if (exit == EImageEditorSoftkeyCmdYes || !iSaveOnExit)
                    {
                        // If the image has not been saved before, the reserved target
                        // file has zero size and can be deleted.
                        TEntry entry;
                        TInt err = iEikonEnv->FsSession().Entry( iDocumentName, entry );
                        if ( err == KErrNone && entry.iSize == 0 )
                        {
                            BaflUtils::DeleteFile (iEikonEnv->FsSession(), iDocumentName);
                        }

                        // Exit the application
                        User::Leave (KSIEEErrorOkToExit);
                    }
                    else
                    {
                        iExitFromApp = EFalse;
                        ResetBusy();
                    }
                }
                SetOrientationL(EAppUiOrientationUnspecified);
                break;
            }
            default:
            {
                User::Leave (aError);
                break;
            }
		}
	}
	else
	{
		switch ( aOpId ) 
		{

			case EOperationCodeLoad:
			case EOperationCodeBlockLoad:
            {
                LOG(KImageEditorLogFile, "CImageEditorAppUi: Image loaded");

                // Find out whether the source file belongs to any albums
                ImageEditorUtils::FindAlbumsForImageFileL (
                    iSourceImageMgAlbumIdList,
                    iSourceFileName );

                // Generate the name for the saved file
                TInt err = ImageEditorUtils::GenerateNewDocumentNameL (
                    iEikonEnv->FsSession(), 
                    iSourceFileName, 
                    iDocumentName,
                    &iSourceImageMgAlbumIdList );

				//	Delete old temp files, which could exist if the exit was not clean
				DeleteTempFiles();
	
                // If there is not enough disk space to save the edited image, show
                // error note but continue anyway (user can free up memory before saving)
                if (KSIEENotEnoughDiskSpace == err)
                {
                    ShowErrorNoteL (err);
                }
                else if (KErrNone != err)
                {
                    // Fatal error 
                    User::Leave (err);
                }                
                iEditorManager->SetImageName (iDocumentName);
                iImageLoaded = ETrue;
                InitializeUiItemsL();
                
                if (!iOrientationChanged)
                {
                    //  Immediately display the loaded image
                    ((CImageEditorUIView *)iEditorView)->
                        SetImageL ( iEditorManager->GetPreviewImage() );
                }
                
                ApplicationReadyL();
				
				// Do not have to keep file open anymore. 
				// Fixes problem with Online Printing
				iFile.Close();

                break;
            }

			case EOperationCodeSave:
			case EOperationCodeBlockSave:
            {
                LOG(KImageEditorLogFile, "CImageEditorAppUi: Image saved");
#ifdef VERBOSE
                LOGFMT3(KImageEditorLogFile, "\tiSourceFileName: %S, iDocumentName: %S, iTempSaveFileName: %S", &iSourceFileName, &iDocumentName, &iTempSaveFileName);
#endif
                ResetBusy();
                iSaving = EFalse;

		        RFs & fs = iEikonEnv->FsSession();
		        if ( BaflUtils::FileExists (fs, iTempSaveFileName ))
                {
                    // Image has been successfully saved
                    iImageSaved = ETrue;                                         
                
            		CFileMan* fileMan = CFileMan::NewL( fs );	
            		CleanupStack::PushL( fileMan );
            		
            		// the temporary image file and the target file are located in the same drive
            	    if ( iTempSaveFileName.Left(1) == iDocumentName.Left(1) )
            			{
            			User::LeaveIfError( fileMan->Rename( iTempSaveFileName, iDocumentName ));	
            			}
           			// the temporary image file and the target file are not in the same drive
            		else
            			{
            			User::LeaveIfError( fileMan->Move( iTempSaveFileName, iDocumentName ));	
            			}
            		CleanupStack::PopAndDestroy(); //fileMan
                                       
                    // Source file changes (at least if saved with a new name)
                    iSourceFileName = iDocumentName;
                    
                    // Update title
                    SetFileNameTitlePaneL(iDocumentName); 
                    iEditorManager->SetImageName(iDocumentName);
                    
                    LOGFMT(KImageEditorLogFile, "CImageEditorAppUi: Temp file renamed to %S", &iDocumentName);

                    // Purge undo stack if on the way to exit.
                    // Don't purge if printing or sending the image.
                    if( iExitFromApp )
                    {
                        iEditorManager->PurgeUndoRedoHistory();
                    }

                    FinalizeSaveL();
                }
			
                if (iExitFromApp)
                {
                    DeleteTempFiles();
                    User::Leave (KSIEEErrorOkToExit);
                }
                else 
                {
                    if (iSending)
                    {
                        // Restore document name
                        SetBusy();
                        iEditorManager->SetImageName (iDocumentName);
                        iCallback->DoCallback (KObSendCallbackID);
                    }
                    
                    if (iPrinting)
                    {
                        // Restore document name
                        SetBusy();
                        iEditorManager->SetImageName (iDocumentName);
                        iCallback->DoCallback (KObPrintCallbackID);
                    }

                    // Reset bitmap sink and screen size
                    UpdateScreenRectL();

                    // Set saved image name for printing
                    ((CImageEditorUIView *)iEditorView)->SetImageFileName(iDocumentName);

                }

                // Update menu
                SetUndoFlag ();
                SetOrientationL(EAppUiOrientationUnspecified);
                LOG(KImageEditorLogFile, "CImageEditorAppUi: Saving procedure completed");
                break;
            }

            default:
            {
                break;
            }
        }
    }
}

//=============================================================================
void CImageEditorAppUi::RenderL ()
{
	LOG(KImageEditorLogFile, "CImageEditorAppUi: Rendering...");
	SetBusy();
	iEditorManager->ProcessImageL();
	((CImageEditorUIView *)iEditorView)->GetContainer()->DrawNow();
	ResetBusy();
}

//=============================================================================
TKeyResponse CImageEditorAppUi::HandleKeyEventL (
    const TKeyEvent &	aKeyEvent,
	TEventCode			aType
	)
{
    LOGFMT2(KImageEditorLogFile, "CImageEditorAppUi:HandleKeyEventL (aKeyEvent: %d, aType:\t %d)", aKeyEvent,aType);

    TKeyResponse response = EKeyWasNotConsumed;

    // if we are busy doing something
	// then we consume the key
    if ( iBusy || (iConstructionState != EConstructionStateReady) )
    {
        response = EKeyWasConsumed;
    }

	// we are only interested in real key events
	// rather than ups or downs
	else if (aType != EEventKey)
    {
        // These are handled separately here because pressing shift key doesn't
        // give EEventKey event at all
        if ( aType == EEventKeyDown )
        {
            switch (aKeyEvent.iScanCode)
	        {
		        case EStdKeyLeftShift:
		        {
		            iEditorView->HandleCommandL (EImageEditorPreGlobalZoomChange);
                    ZoomL( EZoomIn );
			    	iEditorView->HandleCommandL (EImageEditorGlobalZoomChanged);
			        response = EKeyWasConsumed;
			        break;
		        }
                case EStdKeyRightShift:
		        {
		            iEditorView->HandleCommandL (EImageEditorPreGlobalZoomChange);
                    ZoomL( EZoomOut );
			    	iEditorView->HandleCommandL (EImageEditorGlobalZoomChanged);
			        response = EKeyWasConsumed;
		            break;
		        }
            }
        }
            
	    response = EKeyWasNotConsumed;
    }

	// scan codes for numbers are their ASCII codes
    else
    {
        TBool rotated = LandscapeScreenOrientation();
        
        switch (aKeyEvent.iScanCode)
	    {

		    case 0x30: // 0
		    {
#ifdef FULLSCREEN_AVAILABLE 
		    	if (iFullScreen)
			    {
				    ResetFullScreenL();
			    }
			    else
			    {
				    SetFullScreenL();
			    }
			    response = EKeyWasConsumed;
#endif // FULLSCREEN_AVAILABLE		    
                break;
		    }
#ifndef LANDSCAPE_ROTATE_HOTKEYS
	        case 0x31: // 1
#else
            case 0x33: // 3
#endif
		    {
		        RotateL (ERotationCounterClockwise);
			    response = EKeyWasConsumed;
                break;
		    }

		    case 0x32: // 2
			{
				if (!rotated)
				{
					PanL (EDirectionUp);
	                response = EKeyWasConsumed;			
				}
				else
				{
					PanL (EDirectionLeft);
				    response = EKeyWasConsumed;
				}
                break;				
			}

#ifndef LANDSCAPE_ROTATE_HOTKEYS
		    case 0x33: // 3
#else		    
		    case 0x39: // 9
#endif		    		    
		    {
                RotateL (ERotationClockwise);
			    response = EKeyWasConsumed;
                break;
		    }

		    case 0x34: // 4
		    {
				if (!rotated)
				{
		            PanL (EDirectionLeft);
				    response = EKeyWasConsumed;
				}
				else
				{
					PanL (EDirectionDown);
				    response = EKeyWasConsumed;	
				}
                break;
		    }
		    case 0x35: // 5
		    {
		        // No function		    	
                break;
		    }
		    
            case 0x36: // 6 
		    {	
		    	if (!rotated)
					{
	                PanL (EDirectionRight);
				    response = EKeyWasConsumed;
					}
				else
					{
					PanL (EDirectionUp);
	                response = EKeyWasConsumed;		
					}	
                break;
		    }
		    
		    case 0x38: // 8      		    
		    {
		    	if (!rotated)
					{
	                PanL (EDirectionDown);
				    response = EKeyWasConsumed;
					}
				else
					{
					PanL (EDirectionRight);
				    response = EKeyWasConsumed;
					}
                break;
		    }

		    // following cases handle four directions and select key
            case EStdKeyRightArrow: 
		    {
                PanL (EDirectionRight);
			    response = EKeyWasConsumed;
                break;
		    }
		    case EStdKeyLeftArrow: 
		    {
                PanL (EDirectionLeft);
			    response = EKeyWasConsumed;
                break;
		    }
		    case EStdKeyUpArrow: 
		    {
                PanL (EDirectionUp);
			    response = EKeyWasConsumed;
                break;
		    }
		    case EStdKeyDownArrow: 
		    {
                PanL (EDirectionDown);
			    response = EKeyWasConsumed;
                break;
		    }
            case EStdKeyEnter: 
		    {			    			                    
                TBool zoomedIn = ( ( CImageEditorUIView *)iEditorView )->InZoomingState();                
                if ( zoomedIn )
                    {
                    iEditorView->HandleCommandL (EImageEditorPreGlobalZoomChange);
                    ZoomL( EZoomIn );
    		    	iEditorView->HandleCommandL (EImageEditorGlobalZoomChanged);    		            
                    }
                else
                    {
                    // display the plugin selection grid if not in zoomed state
                    HandleCommandL( EImageEditorMenuCmdApplyEffect );
                    }    
                response = EKeyWasConsumed;                		        
                break;
		    }
	        case 0x2a: // *
		    case EStdKeyNkpAsterisk: // *
		    case EStdKeyIncVolume:
		    {
		    	if ( iEditorManager->IsPluginLoaded() )
		    	{
		    		if ( iEditorManager->GetZoomMode()== EZoomIn3 )
		    		{
		    			  response = EKeyWasConsumed;
		    			  break;
		    		}
		    	}
			    iEditorView->HandleCommandL (EImageEditorPreGlobalZoomChange);
                ZoomL (EZoomIn);
				iEditorView->HandleCommandL (EImageEditorGlobalZoomChanged);
			    response = EKeyWasConsumed;
                break;
		    }
		    case 0x23: // #
	        case EStdKeyHash: // #
	        case EStdKeyDecVolume:
	        {
	        	  if ( iEditorManager->IsPluginLoaded() )
	        	  {
	        		   if ( iEditorManager->GetZoomMode()== EZoomNormal )
	        		   {
	        		       response = EKeyWasConsumed;
	        		       break;
	        		   }
	        	  }
	            iEditorView->HandleCommandL (EImageEditorPreGlobalZoomChange);
                ZoomL (EZoomOut);
				iEditorView->HandleCommandL (EImageEditorGlobalZoomChanged);
				response = EKeyWasConsumed;
	            break;
	        }
	        case EStdKeyYes: // green answer call key
		    {
                if (!iEditorManager->IsPluginLoaded())
                {   
                    // when touch is enabled, dialer application is launched 
                    // by the framework and application should not consume
                    // this key. Otherwise image is saved and send menu opened.
			        if ( !AknLayoutUtils::PenEnabled() )
			            {
			            if ( !iSending )
			            	{
			            	SaveChangesAndSendL();
			            	} 
			            response = EKeyWasConsumed;    
			            }
			        else
			            {
			            response = EKeyWasNotConsumed;
			            }    				    
                }
                break;
		    }		    		  
            case EStdKeyDevice3: // Select key (OK key)
            {
                // display the plugin selection grid
                HandleCommandL(EImageEditorMenuCmdApplyEffect);
                response = EKeyWasConsumed;
                break;
            }
	        case EStdKeyNo: // end key
		    {
		    	iBusy = ETrue;
			    response = EKeyWasNotConsumed;
                break;
		    }		    		  

		    default:
		    {
			    response = EKeyWasNotConsumed;
                break;
		    }
        }
    }
	return response;
}

//=============================================================================
void CImageEditorAppUi::HandleCommandL (TInt aCommand)
{
    LOGFMT(KImageEditorLogFile, "CImageEditorAppUi:HandleCommandL (%d)", aCommand);

    switch ( aCommand )
    {

        /** 
        *
        *   MENU COMMANDS
        *   
        */

        //  Launch plug-in selection grid
        case EImageEditorMenuCmdApplyEffect:
		{
			SetBusy();
            iCallback->DoCallback (KObInitPluginID);
            break;
		}
	
        //  Undo effect
		case EImageEditorMenuCmdUndo:
        {
			SetBusy();
            iCallback->DoCallback (KObUndoDCallbackID);
            break;
        }

        //  Switch to full screen
		case EImageEditorMenuCmdFullScreen:
        {
#ifdef FULLSCREEN_AVAILABLE         
            SetFullScreenL();
#endif // FULLSCREEN_AVAILABLE         
            break;
        }

        //  Switch to normal screen
		case EImageEditorMenuCmdNormalScreen:
        {
#ifdef FULLSCREEN_AVAILABLE         
            ResetFullScreenL();
#endif // FULLSCREEN_AVAILABLE            
            break;
        }
        
        // Increase Zoom
        case EImageEditorMenuCmdZoomIn:
        {
            ZoomL (EZoomIn);
            break;
        }

        // Decrease Zoom
        case EImageEditorMenuCmdZoomOut:
        {
            ZoomL (EZoomOut);
            break;
        }
        
        // Set zoom to minimum (=normal image)
        case EImageEditorMenuCmdFitToScreen:
        {
           ZoomL( EZoomMin );
           break;
        }
        
        //  Launch CSHelp
        case EAknCmdHelp:
        case EImageEditorMenuCmdHelp:
        {
            CArrayFix<TCoeHelpContext>* context = CCoeAppUi::AppHelpContextL();
            if (context)
            {
                HlpLauncher::LaunchHelpApplicationL(iEikonEnv->WsSession(), context);
            }
            break;
        }
        case EImageEditorMenuCmdSave:
        {
        	if ( ! ((CImageEditorUIView*) iEditorView)->IsMemoryInCriticalLevel() )
        		{
        		QueryAndSaveL(); 
        		}
            break;
        }
        
        //  Close editor
        case EImageEditorMenuCmdExit:
        {
            TBool exitNow = ETrue;
            if ( iEditorManager->IsImageChanged() )
            {
            	if ( ! ((CImageEditorUIView*) iEditorView)->IsMemoryInCriticalLevel() )
            		{
            		// launch "Save changes?" query
            		TInt result = ((CImageEditorUIView *)iEditorView)->LaunchSaveChangesQueryL();            	
            		if( result == EImageEditorSoftkeyCmdYes )
            		{	
            			if (QueryAndSaveL())
            			{
            				SetBusy();
            				iExitFromApp = ETrue;
            				exitNow = EFalse;
            				LOG(KImageEditorLogFile, "CImageEditorAppUi:HandleCommandL: Hide application from Fast-swap window.");
            				CAknAppUi::HideApplicationFromFSW(ETrue);			     		
            			}
            			else
            			{
			     			iExitFromApp = EFalse;
			     			exitNow = EFalse;
            			}

            		}
            		else if( result == EImageEditorSoftkeyCmdNo )
            		{	
		            	// do nothing, exitNow == ETrue
            		}
            		else
            		{
	                	// Automatic cancel may have occurred, don't exit
	                	iExitFromApp = EFalse;
			     		exitNow = EFalse;
            		}
	            }
            }
                
            if (exitNow)
            {
                DeleteTempFiles();
                RunAppShutter();
               
                
                //Exit();
            }
            break;
        } 

        /** 
        *
        *   SOFTKEY COMMANDS
        *   
        */

        //  Back softkey pressed => Close editor
		case EImageEditorSoftkeyCmdBack:
        {
        	TBool exitNow = ETrue;
        	if (iSaving || iCancelling)
        	{
        		// ignore	
        	}
        	if ( ! ((CImageEditorUIView*) iEditorView)->IsMemoryInCriticalLevel() )
        	{
            	if ( iEditorManager->IsImageChanged() )
            	{   
            		// launch "Save changes?" query    
                	TInt result = ((CImageEditorUIView *)iEditorView)->LaunchSaveChangesQueryL();            	
                	if( result == EImageEditorSoftkeyCmdYes )
                	{
			        	if (QueryAndSaveL())
			        	{
				        	SetBusy();
				        	iExitFromApp = ETrue;
				        	exitNow = EFalse;
				        	LOG(KImageEditorLogFile, "CImageEditorAppUi:HandleCommandL: Hide application from Fast-swap window.");
				        	CAknAppUi::HideApplicationFromFSW(ETrue);			     		
			        	}
			        	else
			        	{
			     			iExitFromApp = EFalse;
			     			exitNow = EFalse;
			        	}

                	}
                	else if( result == EImageEditorSoftkeyCmdNo )
                	{
                		// do nothing, exitNow == ETrue
                	}
                	else
                	{
                		// Automatic cancel may have occurred, don't exit
                		iExitFromApp = EFalse;
                		exitNow = EFalse;
                	}
	            }
            }
            
        	if (exitNow)
        	{
	            DeleteTempFiles();
    	        RunAppShutter();
	            //ProcessCommandL(EAknCmdExit);
        	}
           	break;
        }

        /** 
        *
        *   SYSTEM COMMANDS
        *   
        */

        case EEikCmdExit:
        {
			LOG(KImageEditorLogFile, "CImageEditorAppUi: EEikCmdExit caught");
			DeleteTempFiles();
            Exit();
            break;
        }

        /** 
        *
        *   MISC COMMANDS
        *   
        */

        //  Render image
        case EImageEditorCmdRender:
        {
            RenderL();
            break;
        }

        case EImageEditorCmdViewReady:
        {

            LOG(KImageEditorLogFile, "CImageEditorAppUi: View ready");
			iEditorReady = ETrue;

            //  Initialize UI items
            InitializeUiItemsL();
			ApplicationReadyL();
            break;
        }

		case EImageEditorApplyPlugin:
        {
			//	Return from plug-in
            SetBusy();
            iCallback->DoCallback (KObReturnFromPluginID);
            break;
        }

        case EImageEditorCancelPlugin:
        {
			SetBusy();
			iCancelling = ETrue;
            iCallback->DoCallback (KObCancelCallbackID);
            break;
        }

        case EImageEditorAddFilterToEngine:
        {
            // Add the current plugin parameters to engine
            // and continue with the same plugin
			iEditorManager->AddFilterToEngineL();
            break;
        }

        case EImageEditorStoreParameters:
        {
            break;
		}

        case EImageEditorUndoFilter:
        {
            // Undo one filter (can be called if one plugin
            // has put more than one filter to the engine)
            SetBusy();
            iEditorManager->UndoL();
            SetUndoFlag();
            ((CImageEditorUIView *)iEditorView)->GetContainer()->DrawNow();
			ResetBusy();
            break;
        }

        case EImageEditorSaveAndPrint:
        {
            if ( iEditorManager->IsImageChanged() )
            {
                iPrinting = ETrue;
                SaveImageL();
            }
            else
            {
                SetBusy();
                iCallback->DoCallback (KObPrintCallbackID);
            }
            break;
        }
        case EImageEditorResetZoom:
        {
            // If this is called rendering must be done separately
            iEditorManager->ZoomL( EZoomMin );
            ((CImageEditorUIView *)iEditorView)->SetZoomModeL( iEditorManager->GetZoomMode() );
            break;
        }
        case EImageEditorCmdTouchPan:
            {   
            TouchPanL();            
            break;
            }
        case EImageEditorOpenContextMenu:
            {   
            CAknViewAppUi::ProcessCommandL(EAknSoftkeyContextOptions);
            break;
            }
        default:
        {
            // CSendUi commands  
            if (aCommand >= EImageEditorMenuCmdSend &&
                aCommand < EImageEditorMenuCmdSendLast)
    		{
		    	SetBusy();
                SaveChangesAndSendL();
                break;
            }

            /** 
            *
            *   BEVERLY HILLS 
            *   
            */
            else
            {
                // Do not handle unknown commands
                ResetBusy();
                break;
            }
        }
	}
}

//=============================================================================
void CImageEditorAppUi::HandleWsEventL ( 
	const TWsEvent &		aEvent,
	CCoeControl *			aDestination 
	)
{
#ifdef VERBOSE
    LOGFMT3 (KImageEditorLogFile, "CImageEditorAppUi::HandleWsEventL: aEvent.Type() == %d, aEvent.Key().iCode == %d, aEvent.Key().iScanCode == %d", aEvent.Type(), (TInt)aEvent.Key()->iCode, (TInt)aEvent.Key()->iScanCode);
#endif

    switch ( aEvent.Type() )
    {

        case EEventFocusLost:
        {
			iForeground = EFalse;
			break;
        }
		
        case EEventFocusGained:
        {

    	    // Update the screen rect when we gain focus.
		    // This isn't really needed every time, it should only do this if something has changed
            if (iImageController)
            {
                UpdateScreenRectL();
            }

            if (iFullScreen)
            {
                StatusPane()->MakeVisible ( EFalse );
            }
			iForeground = ETrue;
			break;
        }

		default:
        {
			break;
        }

    }

	if ( ( aEvent.Type() == KAknUidValueEndKeyCloseEvent) && iEditorManager->IsImageChanged() )
		{
		if (iCancelling)
			{
			return;
			}
		iExitFromApp = ETrue;            
		if (iSaving)
			{
			iImageController->CancelOperation(ETrue);
			iSaveOnExit = EFalse;

			return;
			}

		TInt err = ImageEditorUtils::GenerateNewDocumentNameL (
											iEikonEnv->FsSession(), 
											iSourceFileName, 
											iDocumentName,
											&iSourceImageMgAlbumIdList 
											);
		SaveImageOverWriteL( EFalse );
		}
	else
		{
		TInt err( 0 );    
		TRAP( err, CAknViewAppUi::HandleWsEventL( aEvent, aDestination ) );
		}
}


//=============================================================================
void CImageEditorAppUi::HandleScreenDeviceChangedL()
    {

	LOG(KImageEditorLogFile, "CImageEditorAppUi::HandleScreenDeviceChangedL()");

    // Work-a-round to avoid softkeys to drawn incorrectly
    // when a dialog is open when changing to view mode
    if (iConstructionState == EConstructionStateReady &&
        iSaving == EFalse &&
        iSending == EFalse )
    {   
        StopDisplayingMenuBar();
    }
    
    CAknViewAppUi::HandleScreenDeviceChangedL();

    TBool visibleFlag = EFalse;
    if (((CImageEditorUIView *)iEditorView)->GetContainer())
    {
        visibleFlag = ((CImageEditorUIView *)iEditorView)->GetContainer()->IsVisible();
    }

    // If orientation changes during loading - screen bitmap need to be recreated
    // In the start-up there can be event from system without screen device change
    if (!iImageLoaded && iConstructionState != EConstructionStateAlloc )
	    {
        iOrientationChanged = ETrue;    
        // Set landscape parameter
        if (LandscapeScreenOrientation())
            {
            iResolutionUtil->SetLandscape(ETrue);
            }
        else
            {
            iResolutionUtil->SetLandscape(EFalse);
            }
        	
     	iResolutionUtil->UpdateScreenMode();
    	TRect rect;
    	iResolutionUtil->GetClientRect(rect);
    	CCoeControl * control = ((CImageEditorUIView *)iEditorView)->GetContainer();
		control->SetRect(rect);
	    }
    
    // Update screen rects if we are currently in the foreground.
    else if ((iForeground && iImageController) || visibleFlag)
    {
        UpdateScreenRectL();
    }

}

//=============================================================================
TBool CImageEditorAppUi::LandscapeScreenOrientation()
{

	LOG(KImageEditorLogFile, "CImageEditorAppUi::LandscapeScreenOrientation()");

    TBool orientation = EFalse;

#ifdef LANDSCAPE_SUPPORT

    TRect rect;

    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, rect);

	if ( rect.iBr.iX > rect.iBr.iY )
        {
        orientation = ETrue;
        }
	else
        {
        orientation = EFalse;
        }

#endif

	LOGFMT(KImageEditorLogFile, "CImageEditorAppUi: Orientation (%d)", (TInt)orientation);

    return orientation;
}

//=============================================================================
void CImageEditorAppUi::HandleForegroundEventL (TBool aForeground)
{
	LOG(KImageEditorLogFile, "CImageEditorAppUi::HandleForegroundEventL()");

	if (aForeground)
	{
		if ( !BaflUtils::FileExists( iEikonEnv->Static()->FsSession() , iSourceFileName  ) )
			{
			DeleteTempFiles();
			RunAppShutter();
			}

		if ( iProcessPriorityAltered )
		{
		// Return to normal priority.
		RProcess myProcess;
		TProcessPriority priority = myProcess.Priority();
		if ( priority < iOriginalProcessPriority )
			{
			myProcess.SetPriority( iOriginalProcessPriority );
			}
		iProcessPriorityAltered = EFalse;
		}

        // Check that application is in sync with the system 
        if (!iImageLoaded && iConstructionState != EConstructionStateAlloc )
 	    	{
        	iOrientationChanged = ETrue;    
    		}
        else if (iResolutionUtil && LandscapeScreenOrientation() != iResolutionUtil->GetLandscape())
            {
            UpdateScreenRectL();
            }
        
		iEditorView->HandleCommandL (EImageEditorFocusGained);
	}
	else
	{
		iEditorView->HandleCommandL (EImageEditorFocusLost);
		// Set the priority to low. This is needed to handle the situations 
		// where the engine is performing heavy processing while the application 
		// is in background.
		RProcess myProcess;
		iOriginalProcessPriority = myProcess.Priority();
		myProcess.SetPriority( EPriorityLow );
		iProcessPriorityAltered = ETrue;


	}
    CAknViewAppUi::HandleForegroundEventL (aForeground);
}


//=============================================================================
TBool CImageEditorAppUi::ProcessCommandParametersL (
	TApaCommand		aCommand,
	TFileName &		aDocumentName,
	const TDesC8 &	aTail 
	)
{
	LOG(KImageEditorLogFile, "CImageEditorAppUi::ProcessCommandParametersL");

    /* ------------- : test only ----------
    if ( this->ContainerAppUi() == NULL ) // launched as standalone
	{
	    aDocumentName = _L("C:\\Data\\Images\\test.jpg");
	};
    // ---------------- test only ------------- */
    
	//	Store command parameter document name 
    iDocumentName.Copy (aDocumentName);

    //  Save original document name
    iSourceFileName.Copy(aDocumentName);

    //  Set document name for printing
    ((CImageEditorUIView *)iEditorView)->SetImageFileName(iDocumentName);

    //	Return default action
	return CAknViewAppUi::ProcessCommandParametersL (aCommand, aDocumentName, aTail);
	}

//=============================================================================
void CImageEditorAppUi::OpenFileL(RFile &aFile)
    {
    LOG(KImageEditorLogFile, "CImageEditorAppUi::OpenFileL");

	// This function should be called only when editor is started
	// as an embedded application.

    //	Store command parameter document name 
    aFile.FullName(iDocumentName);

	//  Open file for editor 
	//  File must be opened until editor is closed, otherwise
	//  shared session is lost -> Error code -14.
    RFs& fs = iEikonEnv->FsSession();
    fs.ShareProtected();
	TInt err = iFile.Open(fs, iDocumentName, EFileRead | EFileShareReadersOnly);
	if (KErrNone != err)
		{
		User::LeaveIfError(iFile.Open(fs, iDocumentName, EFileRead | EFileShareAny));
		}

    //  Save original document name
    aFile.FullName(iSourceFileName);

    //  Set document name for printing
    ((CImageEditorUIView *)iEditorView)->SetImageFileName(iDocumentName);

	// If OpenFileL was not called when the startup sequence reached 
	// the point where it needs the document name, the construction was
	// suspended to wait for this. 
	if (EConstructionStateWaitForStartupParameters == iConstructionState)
		{
		// Resume construction
		iConstructionState = EConstructionStateLoadImage;
		iCallback->DoCallback (KObConstructCallBackID);
		}
    }

//=============================================================================
CArrayFix<TCoeHelpContext>* CImageEditorAppUi::HelpContextL() const
{
    CArrayFixFlat<TCoeHelpContext>* array = 
        new(ELeave)CArrayFixFlat<TCoeHelpContext>(1);
    CleanupStack::PushL(array);
    
    TBool zoomedIn = ( ( CImageEditorUIView *)iEditorView )->InZoomingState();                
    if ( zoomedIn )
        {
        array->AppendL(TCoeHelpContext(TUid::Uid(UID_IMAGE_EDITOR), KSIE_HLP_ZOOMING));    		            
        }
    else
        {
        array->AppendL(TCoeHelpContext(TUid::Uid(UID_IMAGE_EDITOR), KSIE_HLP_MAIN));
        }
                    
    CleanupStack::Pop(array);
    return array;
}

//=============================================================================
void CImageEditorAppUi::InitializePluginL ()
{
    LOG(KImageEditorLogFile, "CImageEditorAppUi: Initialising plugin");

    //	Get pointer to the view
	CImageEditorUIView * view = (CImageEditorUIView *)iEditorView;
	
	//	Close the previous plug-in 
	iEditorManager->ClosePlugin();
	
    //	Open a new plug-in 
    CPluginInfo * pgn_info = view->GetSelectedPluginInfoL();

    if (pgn_info)
    {
	    iEditorManager->OpenPluginL ( pgn_info->PluginDll()->Des() );

        LOG(KImageEditorLogFile, "CImageEditorAppUi: Plugin opened");

        // current rect
        TRect rect;
        iResolutionUtil->GetClientRect(rect);

	    //	Create a plug-in control
	    //  Ownership is moved to the plug-in
	    CCoeControl * control = NULL;
	    TInt err = iEditorManager->CreatePluginControlL (
            rect,
		    view->GetContainer(),
		    control
    	    );

        LOG(KImageEditorLogFile, "CImageEditorAppUi: Plugin control created");

        //  The plug-in activation was cancelled
	    if (err != KErrNone)
	    {
            //	Cancel plug-in
            iCancelling = ETrue;
		    iCallback->DoCallback (KObCancelCallbackID);
	    }

        //  Activate plug-in
	    else
        {

            //  Register control to UI
            view->ActivatePluginL (control);
    
            LOG(KImageEditorLogFile, "CImageEditorAppUi: Plugin activated");


            //  If the filter did not have a control, close plug-in
            if ( !control )
            {
                iEditorManager->ClosePlugin();

            }
                    
            //  Set the last opened image to editor view
            ((CImageEditorUIView *)iEditorView)->SetImageL (
                iEditorManager->GetPreviewImage()
                );
            
            ResetBusy();
        }
    }
	else
	{
		ResetBusy();
	}
}

//=============================================================================
void CImageEditorAppUi::CreatePluginLocatorL ()
{
    iLocator = CPluginLocator::NewL();
}   

//=============================================================================
void CImageEditorAppUi::CreateEditorManagerL ()
{
    iEditorManager = CImageEditorPluginManager::NewL();
}

//=============================================================================
void CImageEditorAppUi::CreateImageControllerL ()
{
	iImageController = CImageController::NewL (this, iEditorManager);
}

//=============================================================================
void CImageEditorAppUi::LoadPluginsL ()
{
    //  Scan plug-ins
    iLocator->ScanPluginsL ();
}

//=============================================================================
void CImageEditorAppUi::InitializeUiItemsL ()
{

    LOG (KImageEditorLogFile, "CImageEditorAppUi: InitializeUiItemsL");

    //  Set UI items
    if (iEditorReady && iPluginsScanned && iImageLoaded)
    {
	    for (TInt i = 0, c = iLocator->CountPlugins(); i < c; ++i)
        {	
	        ((CImageEditorUIView *)iEditorView)->AddPluginUiItemL (iLocator->GetPluginInfo(i));
	    }

		//  Set the last opened image to editor view
		((CImageEditorUIView *)iEditorView)->SetImageL (
            iEditorManager->GetPreviewImage()
            );

        SetFileNameTitlePaneL(iSourceFileName);

       ((CImageEditorUIView *)iEditorView)->SetZoomModeL( iEditorManager->GetZoomMode() );

		LOG (KImageEditorLogFile, "CImageEditorAppUi: Plugins initialized");

    }
}

//=============================================================================
void CImageEditorAppUi::ApplicationReadyL()
{
    LOG (KImageEditorLogFile, "CImageEditorAppUi: ApplicationReadyLL");

    //  Set UI items
    if (iEditorReady && iPluginsScanned && iImageLoaded)
    {     
        if ( iOrientationChanged )
        {
        	LOG(KImageEditorLogFile, 
        	    "CImageEditorAppUi: Orientation changed, updating screen rect");
            // Check that application is in sync with the system 
            // This should fix the problem with screen update after image is loading when
            // screen is folded during loading
            UpdateScreenRectL();
            iOrientationChanged = EFalse;
        }
        else if ( ( LandscapeScreenOrientation() && 
                  ( StatusPane()->CurrentLayoutResId() != 
                  R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL_FLAT  ) ) )
            {
            LOG(KImageEditorLogFile, 
            "CImageEditorAppUi: Status pane needs to be changed->updating screen rect");
            // Updating screen rect updates also status pane to correct "flat version"
            UpdateScreenRectL();
            }
            
		TRect screenRect;
		CResolutionUtil::Self()->GetClientRect(screenRect);
		iOldScreenRect = screenRect;

		ResetBusy();
		
		// Open Plugin selection grid right away when application start is finished
		//HandleCommandL(EImageEditorMenuCmdApplyEffect);
		
	}
}


//=============================================================================
void CImageEditorAppUi::LoadImageL ()
{
    LOGFMT (KImageEditorLogFile, "CImageEditorAppUi: Loading image (%S)", &iSourceFileName);

	//	Load the image to be edited
	iImageController->LoadImageL (iSourceFileName);
}

//=============================================================================
void CImageEditorAppUi::SaveImageL ()
{
    LOG(KImageEditorLogFile, "CImageEditorAppUi: Saving image");

    // Check if source image still exists
    if (!BaflUtils::FileExists(iEikonEnv->FsSession(), iSourceFileName))
    {
        if (iExitFromApp)
        {
            TInt exit = 
                ((CImageEditorUIView *)iEditorView)->LaunchExitWithoutSavingQueryL();
            if (exit)
            {
                // Exit
                User::Leave (KSIEEErrorOkToExit);
            }
            else
            {
                iExitFromApp = EFalse;
                User::Leave(KSIEEInternal);
            }
        }
        else
        {
            User::Leave(KSIEEInternal);
        }
    }

    // Check document name if not saved yet or memory card removed
    if ( !iImageSaved ||
         !BaflUtils::PathExists(iEikonEnv->FsSession(), iTempSaveFileName.Left(3)))
    {
        //  If not saved yet, check that document name is valid 
        SetImageNameL(); 
    }

	//	Set temp save image name to engine
    iEditorManager->SetImageName (iTempSaveFileName);
    
    SetLayoutAwareApp(EFalse);
    //	Launch save wait note
	LaunchSaveWaitNoteL ();

    iCallback->DoCallback (KObSaveCallbackID);

}

//=============================================================================
void CImageEditorAppUi::SetImageNameL()
    {
    
    // Create new document name
    // (note that this also selects the drive where the image is saved)
    TInt err = ImageEditorUtils::GenerateNewDocumentNameL (
        iEikonEnv->FsSession(), 
        iSourceFileName, 
        iDocumentName,
        &iSourceImageMgAlbumIdList 
        );
    User::LeaveIfError (err); // cannot save the image
    iEditorManager->SetImageName (iDocumentName);

    // Change title pane text
    SetFileNameTitlePaneL(iSourceFileName);

	//	Check that everything is in order with the file
	CheckFileL (iDocumentName);

    // Generate temporary save file name
    TParse tempFileParser;
    tempFileParser.Set (KTempSaveFile, &KTempPath, &iDocumentName); 
    iTempSaveFileName.Copy (tempFileParser.FullName());
    BaflUtils::EnsurePathExistsL (iEikonEnv->FsSession(), iTempSaveFileName);

    }


//=============================================================================
void CImageEditorAppUi::SaveChangesAndSendL()
{

    LOG(KImageEditorLogFile, "CImageEditorAppUi::SaveChangesAndSendL()");

    iSending = ETrue;

    // Delete old temp files if exists
    DeleteTempFiles();

    if (iEditorManager->IsImageChanged())
    {
        // Save image
        SaveImageL();

    }
    //	Otherwise send last saved image
    else
    {
	    SendImageL ();
    }

}

//=============================================================================
void CImageEditorAppUi::SendImageL ()
{
    LOG(KImageEditorLogFile, "CImageEditorAppUi::SendImageL()");

    ResetBusy();

    RFs& fs = iEikonEnv->FsSession();
    TFileName fileToSend;        
    
    // Check if image has been modified and saved
    if (BaflUtils::FileExists(fs, iDocumentName))
    {
        fileToSend.Append(iDocumentName);        
    }
    // Use source file if no changes done
    else
    {
        fileToSend.Append(iSourceFileName);     
    }
    
    LOGFMT(KImageEditorLogFile, "CImageEditorAppUi: Sending image: %S", &fileToSend);

    iEikonEnv->SetBusy(ETrue);
        
	// Launch send query dialog
	TSendingCapabilities cap( 0, 0, TSendingCapabilities::ESupportsAttachments );
	((CImageEditorUIView *)iEditorView)->LaunchSendQueryL(cap, fileToSend);

    iSending = EFalse;
    iEikonEnv->SetBusy(EFalse);
}

//=============================================================================
void CImageEditorAppUi::CheckFileL (const TDesC & aFileName) const
{
	RFs & fs = iEikonEnv->FsSession();

	//	Is file name valid
    if ( !fs.IsValidName (aFileName) )
	{
        User::Leave ( KSIEESaveFile );
	}

}

//=============================================================================
TBool CImageEditorAppUi::CallbackMethodL (TInt aParam)
{
	LOGFMT(KImageEditorLogFile, "CImageEditorAppUi::CallbackMethodL (%d)", aParam);

    TBool ret = EFalse;

	switch (aParam) 
	{

		case KObConstructCallBackID:
		{

	        switch (iConstructionState) 
	        {

		        case EConstructionStateAlloc:
		        {
                    //  Launch wait note
                    LaunchLoadWaitNoteL();
                    
                    // Initialize resolution utility. CResolutionUtil::InitializeL()
                    // Returns the singleton instance of the class, creating it if
                    // it does not yet exist.
                    iResolutionUtil = CResolutionUtil::InitializeL();
					iResolutionUtil->SetFullScreen(EFalse);

#ifdef LANDSCAPE_SUPPORT
                    // Set landscape parameter
                    if (LandscapeScreenOrientation())
                    {
                        iResolutionUtil->SetLandscape(ETrue);
                    }
                    else
                    {
                        iResolutionUtil->SetLandscape(EFalse);
                    }
#else
#ifdef LANDSCAPE_ONLY
					iResolutionUtil->SetLandscape(ETrue);
#else
                    iResolutionUtil->SetLandscape(EFalse);
#endif 
#endif // LANDSCAPE_SUPPORT

                    iResolutionUtil->UpdateScreenMode();
                    LOG(KImageEditorLogFile, "CImageEditorAppUi: Resolution set");

                    //  Create the Send UI instance
   	                iSendAppUi = CSendUi::NewL();
                    ((CImageEditorUIView *)iEditorView)->SetSendAppUi(iSendAppUi);

                    //  Create plug-in locator
                    CreatePluginLocatorL();
					LOG(KImageEditorLogFile, "Locator created");
                    //  Create plug-in manager
                    CreateEditorManagerL();
					LOG(KImageEditorLogFile, "Manager created");
                    //  Create image controller
                    CreateImageControllerL();
					LOG(KImageEditorLogFile, "Image Controller created");
                    iConstructionState = EConstructionStateLoadImage;
                    ret = ETrue;
                    break;
		        }

		        case EConstructionStateLoadImage:
		        {
					LOG(KImageEditorLogFile, "Loading image");

					//	iSourceFileName is available (in embedded mode) only after 
					//	CImageEditorDocument::OpenFileL has been called by the system.
					//	If that has not been done 
					if (!iSourceFileName.Length())
					{
						iConstructionState = EConstructionStateWaitForStartupParameters;
						ret = EFalse;
					}
					else
					{
	                    //  Kill wait note before loading image. 
	                    //  Some corrupted images loading stacks if wait note is displayd
	                    //KillWaitNote();
	                   
	                    // Relaunch wait note
	                    //LaunchLoadWaitNoteL();

	                    //  Load image
				        LoadImageL();
	                    
	                    SetFileNameTitlePaneL(iSourceFileName);
	                    iConstructionState = EConstructionStateScanPlugins;
	                    ret = ETrue;
					}
                    break;
		        }

		        case EConstructionStateScanPlugins:
		        {
					LOG(KImageEditorLogFile, "Scanning plugins");
                    LoadPluginsL();
                    iConstructionState = EConstructionStateInitUi;
                    ret = ETrue;
                    break;
		        }

                case EConstructionStateInitUi:
		        {
					LOG(KImageEditorLogFile, "Init UI");
                    //  Initialize UI items
                    iPluginsScanned = ETrue;
                    InitializeUiItemsL();
			        iConstructionState = EConstructionStateReady;
                    ret = ETrue;
                    break;
		        }

                default:
		        {
					LOG(KImageEditorLogFile, "Ready");
					KillWaitNote();
                    ApplicationReadyL();
                    break;
		        }
	        }
            break;
        }

		case KObReturnFromPluginID:
		{
            SetUndoFlag();
			iEditorManager->ShowPluginPopup();
			iEditorManager->ClosePlugin();

            __ASSERT_ALWAYS( iEditorView, User::Panic(KComponentName, EImageEditorPanicNoView) );
            ((CImageEditorUIView *)iEditorView)->ActivateMainViewL();

            ((CImageEditorUIView *)iEditorView)->SetImageL (
                    iEditorManager->GetPreviewImage());
            
			ResetBusy();
			break;
		}

		case KObInitPluginID:
		{
			InitializePluginL ();
            SetUndoFlag();
			break;
		}

		case KObCancelCallbackID:
		{
			iEditorManager->ClosePlugin();
            iEditorManager->UndoL();


            __ASSERT_ALWAYS( iEditorView, User::Panic(KComponentName, EImageEditorPanicNoView) );
            ((CImageEditorUIView *)iEditorView)->ActivateMainViewL();

            ((CImageEditorUIView *)iEditorView)->SetImageL (
                iEditorManager->GetPreviewImage());

            SetUndoFlag();
			ResetBusy();
			iCancelling = EFalse;
			break;
		}

		case KObUndoDCallbackID:
		{
            iEditorManager->UndoL();
            SetUndoFlag();
            ((CImageEditorUIView *)iEditorView)->GetContainer()->DrawNow();
			ResetBusy();
			break;
		}

		case KObSendCallbackID:
		{
            SendImageL ();
            ResetBusy();
			break;
		}

        case KObPrintCallbackID:
        {
            // Reset first otherwise canceling print application
            // leaves busy state active
            iPrinting = EFalse;
            ResetBusy();
            ((CImageEditorUIView *)iEditorView)->
                HandleCommandL(EImageEditorDoPrint);
            break;
        }

        case KObSaveCallbackID:
        {
			//	Start saving 
		    iImageController->SaveImageL (iTempSaveFileName);

    		iSaving = ETrue;
            break;
        }

        default:
		{
			break;
		}
	}

    return ret;
}

//=============================================================================
void CImageEditorAppUi::SetBusy ()
{
    LOG(KImageEditorLogFile, "CImageEditorAppUi: SetBusy()");
    if (iEditorView)
    {
	    ((CImageEditorUIView *)iEditorView)->SetBusy();
    }
    iBusy = ETrue;
}

//=============================================================================
void CImageEditorAppUi::ResetBusy ()
{
    LOG(KImageEditorLogFile, "CImageEditorAppUi: ResetBusy()");
    // Keep busy until application is fully constructed
    if (iConstructionState == EConstructionStateReady)
    {
        if (iEditorView)
        {
            ((CImageEditorUIView *)iEditorView)->ResetBusy();
        }
        iBusy = EFalse;
    }
}

//=============================================================================
void CImageEditorAppUi::SetFullScreenL ()
{
    __ASSERT_ALWAYS( iEditorView, User::Panic(KComponentName, EImageEditorPanicNoView) );

    LOG(KImageEditorLogFile, "CImageEditorAppUi: Setting full screen");

    iFullScreen = ETrue; 

    //  Set to full screen mode
   ((CImageEditorUIView *)iEditorView)->SetFullScreen();

    // In the startup image controller might not be created yet 
    if (iImageController)
    {
        // Update new screen resolution
        UpdateScreenRectL();
    }
    
 
}

//=============================================================================
void CImageEditorAppUi::ResetFullScreenL ()
{
    __ASSERT_ALWAYS( iEditorView, User::Panic(KComponentName, EImageEditorPanicNoView) );

    LOG(KImageEditorLogFile, "CImageEditorAppUi: Resetting full screen");

    iFullScreen = EFalse;

    //  Set to full screen mode
    ((CImageEditorUIView *)iEditorView)->ResetFullScreen();

    // In the startup image controller might not be created yet 
    if (iImageController)
    {
        UpdateScreenRectL();
    }
   
}

//=============================================================================
void CImageEditorAppUi::SetUndoFlag ()
{
    if (iEditorView)
    {
        ((CImageEditorUIView *)iEditorView)->SetUndoFlag( iEditorManager->CanUndo() );
        ((CImageEditorUIView *)iEditorView)->SetSaveFlag( iEditorManager->IsImageChanged() );
    }
}

//=============================================================================
void CImageEditorAppUi::SetTitlePaneTextL (const TDesC & aText)
{
    const TInt KMaximumTitleTextLength = 13;

    if (iEditorView)
    {
        HBufC * title_text = HBufC::NewLC ( aText.Length() );
        TPtr title_text_ptr = title_text->Des();

        //  No truncation
        if ( aText.Length() - 4 < KMaximumTitleTextLength )
        {
            //  Copy whole file name 
            title_text_ptr.Copy (aText);         
        }

        //  With truncation
        else
        {

            //  Copy maximum amount of the file
            title_text_ptr.Copy ( aText.Left (KMaximumTitleTextLength) );         

            //  Append horizontal ellipsis
            title_text_ptr.Append ( KBaflCharTruncation );         

            //  Append sequence number
            _LIT(KSearchWildcard, "*-???");
            TInt offset = aText.Match (KSearchWildcard);
            if ( offset != KErrNotFound )
            {
                title_text_ptr.Append ( aText.Mid (offset, 4) );         
            }
            
        }

        //  Set title pane text
	    ((CImageEditorUIView *)iEditorView)->SetTitlePaneTextL (title_text_ptr);
    
        CleanupStack::PopAndDestroy(); // title_text
    }
}

//=============================================================================
void CImageEditorAppUi::ClearTitlePaneTextL ()
{
    if (iEditorView)
    {
	    ((CImageEditorUIView *)iEditorView)->ClearTitlePaneTextL ();
    }
}


//=============================================================================
void CImageEditorAppUi::SetFileNameTitlePaneL (const TDesC& aFullFileName)
{
	TParsePtrC parser (aFullFileName); 
    SetTitlePaneTextL ( parser.Name() );
}

//=============================================================================
void CImageEditorAppUi::ClearNaviPaneTextL()
{
    if (iEditorView)
    {
	    ((CImageEditorUIView *)iEditorView)->ClearNaviPaneTextL();
    }
}


//=============================================================================
void CImageEditorAppUi::UpdateScreenRectL()
    {
    LOG(KImageEditorLogFile, "CImageEditorAppUi::UpdateScreenRectL()");
    
	if (iEditorView == NULL ||
		iEditorManager == NULL ||
		iImageController == NULL)
		{
		return;
		}
	
	iEditorView->HandleCommandL (EImageEditorPreScreenModeChange);

    // Set Full screen parameter
    iResolutionUtil->SetFullScreen(iFullScreen);

#ifdef LANDSCAPE_SUPPORT
    // Set landscape parameter
    if (LandscapeScreenOrientation())
        {
        iResolutionUtil->SetLandscape(ETrue);
        }
    else
        {
        iResolutionUtil->SetLandscape(EFalse);
        }
#else
#ifdef LANDSCAPE_ONLY
	iResolutionUtil->SetLandscape(ETrue);
#else
        iResolutionUtil->SetLandscape(EFalse);
#endif
#endif

    // Update view layout
    ((CImageEditorUIView *)iEditorView)->UpdateLayoutL(
        iEikonEnv->ScreenDevice()->CurrentScreenMode());

	LOGFMT(KImageEditorLogFile, "CImageEditorAppUi: CurrentScreenMode() = %d", iEikonEnv->ScreenDevice()->CurrentScreenMode() );

    // Set current screen size to resolution util
    iResolutionUtil->UpdateScreenMode();

	// Check if screen size has changed
	TRect newScreenRect;
	CResolutionUtil::Self()->GetClientRect(newScreenRect);
	if (iOldScreenRect == newScreenRect && !iOrientationChanged)
		{
		LOG(KImageEditorLogFile, "CImageEditorAppUi: Not setting new preview image");
		return;
		}
    iOldScreenRect = newScreenRect;
 
    LOG(KImageEditorLogFile, "CImageEditorAppUi: Setting new preview image");
    
    // Create screen bitmap for new screen size
    iImageController->CreatePreviewBitmapL();
    if ((iConstructionState == EConstructionStateReady ||
        iConstructionState == EConstructionStateInitUi) &&
        !iSaving && iImageLoaded )
        {
        iEditorManager->SetScreenSizeL();
        }
   
	//  Set the new image pointer to editor view
	if (!iSaving)
		{
		((CImageEditorUIView *)iEditorView)->SetImageL (
	        iEditorManager->GetPreviewImage()
	        );		
		}
	else // if saving cannot update new image until saving is completed.
		{
		iOrientationChanged = ETrue; 
		((CImageEditorUIView *)iEditorView)->SetImageL ( NULL );
		}

	//  Set container extent to whole screen
	CCoeControl * control = ((CImageEditorUIView *)iEditorView)->GetContainer();
	if (control)
		{
		if (iFullScreen)
			{
			control->SetExtentToWholeScreen();
			}
		else
			{
			TRect rect;
			iResolutionUtil->GetClientRect(rect);
			control->SetRect(rect);
			}

        if (control->IsFocused())
            {
            control->DrawNow();
            }
        }

	iEditorView->HandleCommandL (EImageEditorPostScreenModeChange);

    }


//=============================================================================
void CImageEditorAppUi::ZoomL (const TZoom aZoom)
    {
    __ASSERT_ALWAYS( iEditorView, User::Panic(KComponentName, EImageEditorPanicNoView) );
    __ASSERT_ALWAYS( iEditorManager, User::Panic(KComponentName, EImageEditorPanicNoEditorManager) );
    SetBusy();

    // Zoom
 	iEditorManager->ZoomL (aZoom);

    RenderL();
	iEditorManager->GetSystemParameters();

    // Inform the view about current zoom mode
    ((CImageEditorUIView *)iEditorView)->
                            SetZoomModeL( iEditorManager->GetZoomMode() );
    }

//=============================================================================
void CImageEditorAppUi::PanL (TDirection aDirection)
{
    __ASSERT_ALWAYS( iEditorView, User::Panic(KComponentName, EImageEditorPanicNoView) );
    __ASSERT_ALWAYS( iEditorManager, User::Panic(KComponentName, EImageEditorPanicNoEditorManager) );

	iEditorView->HandleCommandL (EImageEditorPreGlobalPanChange);

    SetBusy();

	// Pan
    iEditorManager->PanL (aDirection);

    RenderL();
    iEditorManager->GetSystemParameters();

    ResetBusy();

    iEditorView->HandleCommandL (EImageEditorGlobalPanChanged);
}

//=============================================================================
void CImageEditorAppUi::TouchPanL ()
{
    __ASSERT_ALWAYS( iEditorView, User::Panic(KComponentName, EImageEditorPanicNoView) );
    __ASSERT_ALWAYS( iEditorManager, User::Panic(KComponentName, EImageEditorPanicNoEditorManager) );

	iEditorView->HandleCommandL (EImageEditorPreGlobalPanChange);

    SetBusy();
    
    // Fetch directions
    TInt xMovement(0);
    TInt yMovement(0);
    
    ((CImageEditorUIView *)iEditorView)->GetTouchPanDirections( xMovement, yMovement );
    
	// Pan
    iEditorManager->PanL( xMovement, yMovement );

    RenderL();
    iEditorManager->GetSystemParameters();

    ResetBusy();

    iEditorView->HandleCommandL (EImageEditorGlobalPanChanged);
}

//=============================================================================
void CImageEditorAppUi::RotateL (const TRotation aRot)
{
	__ASSERT_ALWAYS( iEditorView, User::Panic(KComponentName, EImageEditorPanicNoView) );
	__ASSERT_ALWAYS( iEditorManager, User::Panic(KComponentName, EImageEditorPanicNoEditorManager) );

	if (!iEditorManager->IsPluginLoaded())
	{
		iEditorManager->RotateL (aRot);
		RenderL();
		SetUndoFlag();
		iEditorView->HandleCommandL (EImageEditorGlobalRotationChanged);
	}
}

//=============================================================================
void CImageEditorAppUi::LaunchLoadWaitNoteL ()
{
    __ASSERT_ALWAYS( iEditorView, User::Panic(KComponentName, EImageEditorPanicNoView) );
    
    SetBusy();
	((CImageEditorUIView *)iEditorView)->LaunchWaitDialog( );

//    if (!iWaitNote)
//    {
//        ((CImageEditorUIView *)iEditorView)->LaunchLoadWaitDialogL( &iWaitNote, this );
//    }
}

//=============================================================================
void CImageEditorAppUi::LaunchSaveWaitNoteL ()
{
    __ASSERT_ALWAYS( iEditorView, User::Panic(KComponentName, EImageEditorPanicNoView) );
    TAppUiOrientation orientation(EAppUiOrientationPortrait);
    if(LandscapeScreenOrientation())
    			{
    			orientation = EAppUiOrientationLandscape;
    			}
    
    SetOrientationL(orientation);
    if (!iProgressNote)
    {
        ((CImageEditorUIView *)iEditorView)->LaunchSaveWaitDialogL( &iProgressNote, 100, this );
		iSaving = ETrue;
    }
}

//=============================================================================
void CImageEditorAppUi::KillWaitNote ()
{
    

/*    if (iWaitNote)
    {
        // If the application is exiting, iWaitNote->ProcessFinishedL()
        // apparently does not finish in time and does something after the 
        // application has been deleted, causing USER 0 panic on debug builds.
        delete iWaitNote;
        iWaitNote = NULL;
    } */
    
      if (iProgressNote)
          {
          delete iProgressNote;
          iProgressNote = NULL;
          }
      else
          {
          // This can be hidden in all cases. If dialog is not active, nothing 
          // special happens
          ((CImageEditorUIView *)iEditorView)->HideWaitDialog( );
          // Commented due to multiple ResetBusy() call during initialization:
          //ResetBusy();
          }
}


//=============================================================================
void CImageEditorAppUi::IncrementProgressBarL(const TInt aIncrement)
{
    __ASSERT_ALWAYS( iProgressNote, User::Panic(KComponentName, EImageEditorPanicNoProgressDialog) );

    CEikProgressInfo * progi = iProgressNote->GetProgressInfoL();
    if (progi)
    {
        progi->IncrementAndDraw (aIncrement);
    }
}

//=============================================================================
void CImageEditorAppUi::DialogDismissedL( TInt aButtonId )
{
    LOGFMT(KImageEditorLogFile, "CImageEditorAppUi::DialogDismissedL( %d )", aButtonId);

    if( aButtonId == EAknSoftkeyCancel )
    {
    
    	if ( !iSaving )
    	{
			iCallback->Cancel();
        	iImageController->CancelOperation(ETrue);
    	}
    	else
    	{
        	iImageController->CancelOperation();
    	}

		iWaitNote = NULL;
		
		ResetBusy();
    }
}

//=============================================================================
TErrorHandlerResponse CImageEditorAppUi::HandleError (
	TInt					aError,
    const SExtendedError &	aExtErr,
	TDes &					aErrorText,
	TDes &					aContextText
	)
{
    LOGFMT(KImageEditorLogFile, "CImageEditorAppUi::HandleError: %d", aError);

	//	Reset busy flag
	KillWaitNote ();
	ResetBusy();
	iCancelling = EFalse;
	iSaving = EFalse;

    TErrorHandlerResponse ret = ENoDisplay;

    // Handle Image Editor errors
    if ( (aError < KSIEEErrorBase) && (aError > KSIEEErrorMax) )
	{
        TRAPD ( err, HandleErrorL (aError) );
		if ( err == KErrNoMemory )
		{
            Exit();
		}
	}

    // Let the framework handle system errors
    else if ( aError == KErrNoMemory )
    {
		iImageController->Cancel();
        ret = CAknViewAppUi::HandleError (aError, aExtErr, aErrorText, aContextText);
        Exit ();
    }
    // KErrCancel is returned when Bluetooth send is cancelled
    // No need to show error dialog
    else if ( aError == KErrCancel )
    {
        // Nothing to do
    } 
    else if ((aError == KErrArgument) || (aError == KErrInUse))
    {
    	//	Show error note
		HandleErrorL (aError);
    }
    //end
    else if ( (aError <= KErrExtended) && (aError >= KLeaveExit) ) 
    {
        ret = CAknViewAppUi::HandleError (aError, aExtErr, aErrorText, aContextText);
        // Close plugin if error occurs, otherwise plugin may crash
        iEditorManager->ClosePlugin();

        __ASSERT_ALWAYS( iEditorView, User::Panic(KComponentName, EImageEditorPanicNoView) );
        TRAP_IGNORE( ((CImageEditorUIView *)iEditorView)->ActivateMainViewL() );

        TRAP_IGNORE( ((CImageEditorUIView *)iEditorView)->SetImageL (
            iEditorManager->GetPreviewImage()) );
    }
    else 
    {
        ret = CAknViewAppUi::HandleError (aError, aExtErr, aErrorText, aContextText);
        // Close plugin if error occurs, otherwise plugin may crash
        iEditorManager->ClosePlugin();

        __ASSERT_ALWAYS( iEditorView, User::Panic(KComponentName, EImageEditorPanicNoView) );

        TRAPD( err, ((CImageEditorUIView *)iEditorView)->ActivateMainViewL() );
                    
        TRAP_IGNORE( ((CImageEditorUIView *)iEditorView)->SetImageL (
            iEditorManager->GetPreviewImage()) );
        
		if ( err == KErrNoMemory )
		{
            Exit();
		}
    }
    return ret;
}

//=============================================================================
void CImageEditorAppUi::HandleErrorL (const TInt aError)
{

	//	Show error note
	ShowErrorNoteL (aError);

	//	Handle error
	switch ( aError ) 
	{

        case KSIEEErrorOkToExit:
        case KSIEEOpenFile:
        case KSIEEExifRead:
        case KSIEEInternalNonRecoverable:
        case KSIEEIncompatibleImage:
		{
			// For some weird reason, in some cases when trying to 
			// exit from CImageEditorAppUi::OperationReadyL, calling 
			// Exit() here won't exit the application.
			RunAppShutter();
			break;
		}

		default:
		{
            // Close plugin if error occurs, otherwise plugin may crash
            if (iEditorManager)
            {
                iEditorManager->ClosePlugin();

            }
            if (iEditorView)
            {
                ((CImageEditorUIView *)iEditorView)->ActivateMainViewL();
                ((CImageEditorUIView *)iEditorView)->SetImageL (
                iEditorManager->GetPreviewImage());
            }

			break;
		}
	}
}

//=============================================================================
void CImageEditorAppUi::ShowErrorNoteL (const TInt aError) const
{
	TInt res_id = -1;

	switch (aError)
	{
        case KErrNotFound:
		case KSIEEOpenFile:
        case KSIEEExifRead:
        case KSIEEIncompatibleImage:
		{
			res_id = R_ERROR_LOADING;
			break;
		}
		case KSIEESaveFile:
        case KSIEEExifUpdate:
		case KErrArgument:
		case KErrInUse:
    	
		{
			res_id = R_ERROR_SAVING;
			break;
		}
		case KSIEENotEnoughDiskSpace:
		{
			res_id = R_ERROR_NOT_ENOUGH_DISK_SPACE;
			break;
		}
		case KSIEEProcessFile:
		{
			res_id = R_ERROR_PROCESSING;
			break;
		}
		case KSIEEEngine:
		{
			res_id = R_ERROR_ENGINE;
			break;
		}
		case KSIEEInternal:
		{
			res_id = R_ERROR_INTERNAL;
			break;
		}
		case KSIEEInternalNonRecoverable:
		{
			res_id = R_ERROR_INTERNAL_NON_RECOVERABLE;
			break;
		}
        case KSIEEProtectedFile:
        {
            res_id = R_ERROR_PROTECTED_FILE;
            break;
        }
    	default:
			break;
	}

    if (res_id != - 1)
    {
		HBufC * textdata = iEikonEnv->AllocReadResourceLC (res_id);
	    TPtrC ptr = textdata->Des();

        LOGFMT(KImageEditorLogFile, "CImageEditorAppUi::ShowErrorNoteL: \"%S\"", &ptr);
        
      
        CAknGlobalNote* globalNote = CAknGlobalNote::NewLC();
        TInt noteId = globalNote->ShowNoteL( EAknGlobalErrorNote, ptr );
        
        User::After(1500042);  // waiting for 1 second

        // discard note
        globalNote->CancelNoteL( noteId );

        
       CleanupStack::PopAndDestroy(); //textdata 
       CleanupStack::PopAndDestroy(); // GlobalNote
    }
}

//=============================================================================
void CImageEditorAppUi::DeleteTempFiles() const
{
    // Get the used temp directory path.
    // Take drive letter from iDocumentName
    TParse parse;
    parse.Set (KTempPath, &iDocumentName, NULL);
    TPtrC tempDir = parse.DriveAndPath();

    LOGFMT (KImageEditorLogFile, "CImageEditorAppUi::DeleteTempFiles(): %S", &tempDir);
    
    // Delete all files in the temp directory
    _LIT(KAllFiles, "*.*");
    TFindFile file_finder (iEikonEnv->FsSession());
    CDir* file_list; 
    TInt err = file_finder.FindWildByDir (KAllFiles, tempDir, file_list);
    while (err==KErrNone)
    {
        TInt i;
        for (i=0; i<file_list->Count(); i++)
        {
            const TDesC& name = (*file_list)[i].iName;
            const TDesC& related = file_finder.File();
            TParse fullentry;
            fullentry.Set (name, &related, NULL);
            BaflUtils::DeleteFile (iEikonEnv->FsSession(), fullentry.FullName());
        }
        delete file_list;
        err=file_finder.FindWild (file_list);
    }
    
}

//=============================================================================
void CImageEditorAppUi::FinalizeSaveL()
{
    LOG(KImageEditorLogFile, "CImageEditorAppUi::FinalizeSaveL");

#ifdef FILE_TIME_STAMP_UPDATE    
    // Set the timestamp of the saved file to original file's timestamp + 2 seconds.
    // The idea is to make the original and edited images appear next to each other.
    TEntry original;
    TInt err = iEikonEnv->FsSession().Entry( iSourceFileName, original );
    if (KErrNone == err)
    {
        TTime newTime = original.iModified + TTimeIntervalSeconds (2);
        
        CFileMan* fileMan = CFileMan::NewL( iEikonEnv->FsSession() );
        CleanupStack::PushL (fileMan);
        fileMan->Attribs(iDocumentName, 0, 0, newTime); // do not set or clear anything, mofify time 
        CleanupStack::PopAndDestroy (fileMan);
    }
#endif

    LOG(KImageEditorLogFile, "CImageEditorAppUi: Check if source image belongs to an album");
   
    iSourceImageMgAlbumIdList.Reset();
    
    LOG(KImageEditorLogFile, "CImageEditorAppUi: publish the filename for provider");
    
    // Publish & Subscribe API used for delivering document name to AIW provider
    TInt err2 = RProperty::Define(KImageEditorProperties, EPropertyFilename, RProperty::EText);
    
    if (err2 != KErrAlreadyExists)
    {
        User::LeaveIfError(err2);   
    }
    
    User::LeaveIfError(RProperty::Set(KImageEditorProperties, EPropertyFilename, iDocumentName));
    
    LOG(KImageEditorLogFile, "CImageEditorAppUi::FinalizeSaveL out");
}

//=============================================================================
void CImageEditorAppUi::HandleVolumeUpL()
{
	LOG(KImageEditorLogFile, "CImageEditorAppUi::HandleVolumeUpL: in");

	TKeyEvent key;
	key.iScanCode = EStdKeyIncVolume;
	key.iCode = EKeyIncVolume;

	if( LandscapeScreenOrientation() )
	{
		key.iScanCode = EStdKeyDecVolume;
		key.iCode = EKeyDecVolume;
	}

	key.iRepeats = 0;
	iEikonEnv->SimulateKeyEventL( key, EEventKey );

	LOG(KImageEditorLogFile, "CImageEditorAppUi::HandleVolumeUpL: out");
}

//=============================================================================
void CImageEditorAppUi::HandleVolumeDownL()
{
	LOG(KImageEditorLogFile, "CImageEditorAppUi::HandleVolumeDownL: in");

	TKeyEvent key;
	key.iScanCode = EStdKeyDecVolume;
	key.iCode = EKeyDecVolume;

	if( LandscapeScreenOrientation() )
	{
		key.iScanCode = EStdKeyIncVolume;
		key.iCode = EKeyIncVolume;
	}

	key.iRepeats = 0;
	iEikonEnv->SimulateKeyEventL( key, EEventKey );

	LOG(KImageEditorLogFile, "CImageEditorAppUi::HandleVolumeDownL: out");
}

//=============================================================================
void CImageEditorAppUi::SaveImageOverWriteL(TBool aOverWrite)
{
    LOG(KImageEditorLogFile, "CImageEditorAppUi: SaveImageOverWriteL: in");

    // Generate temporary save file name
	TParse tempFileParser;
	tempFileParser.Set (KTempSaveFile, &KTempPath, &iDocumentName); 
	iTempSaveFileName.Copy (tempFileParser.FullName());
    BaflUtils::EnsurePathExistsL (iEikonEnv->FsSession(), iTempSaveFileName);

    if (iExitFromApp)
    {
        TInt exit = 
            ((CImageEditorUIView *)iEditorView)->LaunchExitWithoutSavingQueryL();
        if (exit)
        {
            // Exit
            User::Leave (KSIEEErrorOkToExit);
        }
        else
        {
            iExitFromApp = EFalse;
            User::Leave(KSIEEInternal);
        }
    }

	// overwrite the existing file
	if (aOverWrite)
	{
		iDocumentName = iSourceFileName; 
	}
	
	// save with a new file name
	else
	{	        		
   		// Check document name if not saved yet or memory card removed
    	if ( !iImageSaved ||
        	 !BaflUtils::PathExists(iEikonEnv->FsSession(), iTempSaveFileName.Left(3)))
    	{
        	//  If not saved yet, check that document name is valid 
			CheckFileL (iDocumentName);
    	}
	}
	
	//	Set temp save image name to engine
    iEditorManager->SetImageName (iTempSaveFileName);
    		
    //	Launch save wait note
	LaunchSaveWaitNoteL ();

    iCallback->DoCallback (KObSaveCallbackID);
    LOG(KImageEditorLogFile, "CImageEditorAppUi: SaveImageOverWriteL: out");    
}

//=============================================================================
TInt CImageEditorAppUi::QueryAndSaveL()
    {

    TInt isSaved = 0;
    TInt userSelection = 0;
    TBool isFileOpen = EFalse;
    RFs& fs = iEikonEnv->FsSession();
    TInt err = fs.IsFileOpen( iSourceFileName, isFileOpen );
            
    // Check if .jpg or .jpeg file
    TBool isJpgExtension = EFalse;
	TParsePtrC fileParse ( iSourceFileName );
	_LIT( KJpgExtension, ".jpg" );
	_LIT( KJpegExtension, ".jpeg" );
	TPtrC extension( fileParse.Ext() );
	if ( ( extension.MatchF( KJpgExtension ) == KErrNone ) || 
	     ( extension.MatchF( KJpegExtension ) == KErrNone ) )
	    {
	    isJpgExtension = ETrue;
	    }
		
    // If source file is open (e.g. in ImageViewer application) don't give
    // user an option to replace original but launch directly "Save with a
    // new name query. Don't give other that jpg originals.
    if ( !isFileOpen && isJpgExtension )
        {
        // the user selects to save with a new file name
        // launch query with choices "Replace original" and 
        // "Save with a new file name" 
        userSelection = ( ( CImageEditorUIView *)iEditorView )->LaunchSaveImageQueryL();
        } 
    SetOrientationL(Orientation()); 
    SetLayoutAwareApp(EFalse);
    // If user has selected "Save with a new file name"
    if( userSelection == 0 ) 
        {
        
        // Multiple drive support enabled
#ifdef RD_MULTIPLE_DRIVE
        
        TDriveNumber driveNumber;
        TFileName driveAndPath;
        // new multi drive dialog
        CAknMemorySelectionDialogMultiDrive* multiDriveDlg = 
            CAknMemorySelectionDialogMultiDrive::NewL( ECFDDialogTypeSave, 
                                                       EFalse );
        CleanupStack::PushL( multiDriveDlg );

        // launch "Select memory" query (mds)
        if ( multiDriveDlg->ExecuteL( driveNumber, &driveAndPath, NULL ) )
            {        
            // Generate a default name for the new file
            TInt err = ImageEditorUtils::GenerateNewFileNameL(
                                                iEikonEnv->FsSession(),
                                                iSourceFileName, 
                                                iDocumentName,
                                                driveAndPath,
                                                NULL );
                                    
            driveAndPath.Append( PathInfo::ImagesPath() );
                                   
            // launch file name prompt dialog
            // the generated name is shown as a default name in dialog
            TBuf<160> tmp( iDocumentName.Left(160) );
            iSavingQuery = ETrue;
            if ( CAknFileNamePromptDialog::RunDlgLD( tmp, /* iDocumentName, */
                                                     driveAndPath, 
                                                     KNullDesC ) )
                {
                iDocumentName.Copy( tmp );
                iSavingQuery = EFalse;
                // add just queried filename to the path
                driveAndPath.Append( iDocumentName );
    
                iDocumentName = driveAndPath;
                                               
                SaveImageOverWriteL( EFalse );   
                isSaved = 1;
                }
            iSavingQuery = EFalse;
            }
        CleanupStack::PopAndDestroy( multiDriveDlg );
        
#else // No multiple drive support
        CAknMemorySelectionDialog::TMemory selectedMemory(
                            CAknMemorySelectionDialog::EPhoneMemory );
    
        // launch "Select memory" query
        if ( CAknMemorySelectionDialog::RunDlgLD( selectedMemory ) )
            {    
            // create path for the image
            TFileName driveAndPath;         
            ImageEditorUtils::TMemorySelection memorySelection = 
                                            ImageEditorUtils::ESelectPhone;
            if( selectedMemory == CAknMemorySelectionDialog::EPhoneMemory )
                {
                memorySelection = ImageEditorUtils::ESelectPhone;
                driveAndPath.Copy( PathInfo::PhoneMemoryRootPath() );
                driveAndPath.Append( PathInfo::ImagesPath() );
                }
            else if ( selectedMemory == 
                      CAknMemorySelectionDialog::EMemoryCard )
                {
                memorySelection = ImageEditorUtils::ESelectMmc;
                driveAndPath.Copy( PathInfo::MemoryCardRootPath() );
                driveAndPath.Append( PathInfo::ImagesPath() );
                }         

            TInt err = ImageEditorUtils::GenerateNewDocumentNameL(
                                                iEikonEnv->FsSession(), 
                                                iSourceFileName, 
                                                iDocumentName,
                                                &iSourceImageMgAlbumIdList,
                                                NULL,
                                                memorySelection );     
            
            
            // launch file name prompt dialog
            // the generated name is shown as a default name
            iSavingQuery = ETrue;
            if ( CAknFileNamePromptDialog::RunDlgLD( iDocumentName,
                                                     driveAndPath, 
                                                     KNullDesC ) )
                {
                iSavingQuery = EFalse;
                TInt err = ImageEditorUtils::GenerateFilePathL(
                                                iEikonEnv->FsSession(),
                                                iSourceFileName, 
                                                iDocumentName,
                                                memorySelection
                                                );
                                                    
                SaveImageOverWriteL( EFalse );   
                isSaved = 1;
                }
            iSavingQuery = EFalse;
            }
#endif
        }
    // the user selects to overwrite the original image
    else if ( userSelection == 1 )
        {
        SaveImageOverWriteL( ETrue );
        isSaved = 1;
        }
    return isSaved;
    }

// ----------------------------------------------------------------------------
// CImageEditorAppUi::HandleVolumeKeyEvent
// ----------------------------------------------------------------------------
//
void CImageEditorAppUi::HandleVolumeKeyEvent( TRemConCoreApiOperationId aOperationId,
        							 		  TRemConCoreApiButtonAction /* aButtonAct */ )
    {
    if (((CImageEditorUIView *)iEditorView)->GetContainer())
    {
        if(! ((CImageEditorUIView *)iEditorView)->GetContainer()->IsFocused() )
     	   {
     	   return;
     	   }
    }
    
    if( iForeground )
    	{
    	TKeyEvent keyEvent;
    	if( aOperationId == ERemConCoreApiVolumeUp )
    		{
    		keyEvent.iScanCode = EStdKeyIncVolume;
    		TRAP_IGNORE( HandleKeyEventL( keyEvent, EEventKey ) );
    		}
    	else if( aOperationId == ERemConCoreApiVolumeDown )
    		{
    		keyEvent.iScanCode = EStdKeyDecVolume;
    		TRAP_IGNORE( HandleKeyEventL( keyEvent, EEventKey ) );
    		}
    	}
    }

// End of File
