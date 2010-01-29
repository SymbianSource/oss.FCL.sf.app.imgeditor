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
* Image Editor AppUI class header.
*
*/



#ifndef IMAGEEDITORAPPUI_H
#define IMAGEEDITORAPPUI_H


// INCLUDES
#include <coemain.h> 
#include <eikapp.h>
#include <eikdoc.h>
#include <e32std.h>
#include <coeccntx.h>
#include <aknviewappui.h>
#include <akntabgrp.h>
#include <aknnavide.h>
#include <aknwaitdialog.h>
#include <CAknMemorySelectionDialog.h> 
#include "ImageControllerObserver.h"
#include "CallbackMethod.h"
#include "commondefs.h"
#include "CImageEditorVolumeKeyObserver.h"

#ifdef S60_31_VOLUME_KEYS
#include "CMGXRemConTarget.h"
#endif

// debug log writer
#include "imageeditordebugutils.h"

// FORWARD DECLARATIONS
class CPluginLocator;
class CImageController;
class CImageEditorPluginManager;
class CAknView;
class CPluginLocator;
class CImageEditorPluginManager;
class CImageController;
class CSendUi;
class CObCallback;
class CResolutionUtil;


// CONSTANTS


/**	CLASS: CImageEditorAppUi
*
*	Image Editor application UI class. Encapsulates and provides support
*	for EIKON control architecture, view architecture and status pane. 
*
*/
class CImageEditorAppUi :	public CAknViewAppUi,
							public MImageControllerObserver,
							public MObCallbackMethod,
							public MProgressDialogCallback,
							public MImageEditorVolumeKeyObserver
							
							#ifdef S60_31_VOLUME_KEYS
							, public MMGXMediakeyObserver
							#endif
{

public:

/** @name Methods:*/
//@{

	/**	ConstructL, second phase constructor
	*
	*   @param -
	*   @return -
	*/
    void ConstructL();

	/**	C++ constructor
	*
	*   @param -
	*   @return -
	*/
    CImageEditorAppUi();

	/**	Destructor
	*
	*   @param -
	*   @return -
	*/
    ~CImageEditorAppUi();

	/**	ProcessCommandParametersL
	*
	*   Processes shell commands sent to the application.
	*
	*   @param aCommand - sent shell command
	*   @param aDocumentName - document name
	*   @param aTail - command line tail
	*   @return ETrue if aDocumentName exists, EFalse otherwise
	*	@see CEikAppUi
	*/
	virtual TBool ProcessCommandParametersL (
		TApaCommand		aCommand,
		TFileName &		aDocumentName,
		const TDesC8 &	aTail 
		);

    /**	OperationReadyL
	*
	*	@see MImageControllerObserver
	*/
	virtual void OperationReadyL (
		TOperationCode		aOpId,
		TInt				aError 
		);

	/**	IncrementProgressBarL 
	*
	*	@from MImageControllerObserver
	*   Increments the iProgressNote one step
	*
	*   @param aIncrement - increment
	*   @return - 
	*/
    void IncrementProgressBarL(const TInt aIncrement);

	/**	SendImageL 
	*
	*	Sends image by using CSendAppUi class.
	*
	*	@param -
	*	@return -
	*/
	void SendImageL ();

	/**	RenderL 
	*
	*	Calls manager to render image
	*
	*	@param -
	*	@return -
	*/
	void RenderL ();

	/**	OpenFileL 
	*
	*	Set document filename. Called when started as an embedded
	*
	*	@param aFile - image file
	*	@return -
	*/
    void OpenFileL (RFile& aFile);

    /** CallbackMethodL
	*
	*	@see MObCallbackMethod
	*/
	virtual TBool CallbackMethodL (TInt aParam);

    /** DialogDismissedL
	*
	*	@see MProgressDialogCallback
	*/
    void DialogDismissedL( TInt aButtonId );
    
    /** HandleVolumeKeyEvent
	*
	*	@From MIVVolumeKeyObserver
	*/
	void HandleVolumeKeyEvent( TRemConCoreApiOperationId aOperationId,
	                           TRemConCoreApiButtonAction aButtonAct );
//@}
        
protected:

/** @name Methods:*/
//@{
//@}

/** @name Members:*/
//@{
//@}

private:

/** @name Typedefs:*/
//@{
	enum TConstructionState
	{
		//	Invalid
		EConstructionStateMin = 0,
		//  Allocate new locator, manager and controller objects
		EConstructionStateAlloc,
		//  Scan plug-ins
		EConstructionStateScanPlugins,
		//  Load image plug-ins
		EConstructionStateLoadImage,
		//	Wait for the application framework to provide parameters (calling OpenFileL)
		EConstructionStateWaitForStartupParameters,
        //  Initialize UI
        EConstructionStateInitUi,
		//  Invalid
		EConstructionStateReady
	} iConstructionState;
//@}

/** @name Methods:*/
//@{

	/**	HandleCommandL
	*
	*	Handles user commands. This implementation overwrites the dummy
	*	implementation in CEikAppUi to provide application specific response
	*	to CEikAppUi::ProcessCommandL() called by application framework.
	*
	*   @param aCommand - 
	*   @return -
	*	@see CEikAppUi
	*/
	virtual void HandleCommandL (TInt aCommand);

    /**	HandleKeyEventL
	*
	*	This function is called whenever the window server sends an event 
    *   aEvent (standard or special) to the application. It calls one of a 
    *   number of functions, according to the type of the event receivedHandles 
    *   key events generated when keys are pressed. 
	*
	*   @param aEvent - generated window server event
	*   @param aDestination - target control (pointer events)
	*   @return -
	*	@see CCoeAppUi
	*/
    void HandleWsEventL ( 
	    const TWsEvent &		aEvent,
	    CCoeControl *			aDestination 
	    );

    /**	HandleForegroundEventL
	*
	*	Handles application transfer to foreground and background.
	*
	*   @param aForeground - foreground flag
	*   @return -
	*	@see CCoeAppUi
	*/
	void HandleForegroundEventL (TBool aForeground);

    /**	HandleKeyEventL
	*
	*	Handles key events generated when keys are pressed. This method is 
	*	called by HandleWsEventL() if the whole control stack returns 
	*	EKeyWasNotConsumed from OfferKeyEventL().
	*
	*   @param aKeyEvent - key event that occurred
	*   @param aType - window server event type that is being handled
	*   @return TKeyResponse - response enum incicating whether or not
	*	the event has been handled, defaults to EKeyWasNotConsumed
	*	@see CCoeAppUi
	*/
	virtual TKeyResponse HandleKeyEventL (
		const TKeyEvent &	aKeyEvent,
		TEventCode			aType
		);

    /**	HandleScreenDeviceChangedL
	*
	*	Handles screen device changes like orientation
    *
	*   @param -
	*   @return - 
    *
	*	@see CEikAppUi
	*/
    virtual void HandleScreenDeviceChangedL();

    /**	LandscapeScreenOrientation
	*
	*	Check the screen orientation. ote that is not necessarily the same as 
    *   the current device orientation.
    *
	*   @param -
	*   @return TBool - Returns true if the screen device is 
    *                   rotated by 90 degrees (landscape display)
    *
	*	@see CEikAppUi
	*/
    virtual TBool LandscapeScreenOrientation();

	/*	HelpContextL
	*
	*	Return the help context for this application
	*
	*	@PARAM A pointer to the help context
	*	@see CAknAppUi
	*/ 
    CArrayFix<TCoeHelpContext>* HelpContextL() const;

	/**	CreatePluginLocatorL
	*
    *   Creates plug-in locator.
    *
	*   @param -
	*   @return -
	*/
    void CreatePluginLocatorL();

	/**	CreateEditorManagerL
	*
    *   Creates editor manager.
    *
	*   @param -
	*   @return -
	*/
    void CreateEditorManagerL();

	/**	CreateImageControllerL
	*
    *   Creates image controller.
    *
	*   @param -
	*   @return -
	*/
    void CreateImageControllerL();

	/**	LoadPluginsL
	*
    *   Loads Image Editor plug-ins.
    *
	*   @param -
	*   @return -
	*/
    void LoadPluginsL();
    
	/**	InitializeUiItemsL
	*
	*	Initializes UI: sets preview image and UI items for plug-ins.
	*	Activates main view.
	*
	*   @param -
	*   @return -
	*/
    void InitializeUiItemsL ();

    /**	LoadImageL
	*
    *   Loads the editor image
    *
	*   @param -
	*   @return -
	*/
    void LoadImageL();

    /**	SaveImageL
	*
    *   Saves the edited image with name orig-012.ext
    *
	*   @param -
	*   @return -
	*/
	void SaveImageL ();

    /**	SaveChangesAndSendL
	*
    *   Shows a list dialog with two options:
	*	Save as new image
	*	Replace original
	*
	*	Called before sending image.
    *
	*   @param -
	*   @return -
	*/
	void SaveChangesAndSendL();

	/**	InitializePluginL
	*
	*   Loads plug-in and initializes it, activates plug-in control.
	*
	*   @param -
	*   @return -
	*/
	void InitializePluginL ();

	/**	SetBusy
	*
	*   Sets system busy.
	*
	*   @param -
	*   @return -
	*/
	void SetBusy();

	/**	ResetBusy
	*
	*   Resets system busy.
	*
	*   @param -
	*   @return -
	*/
	void ResetBusy();

	/**	SetFullScreenL
	*
	*   Sets full screen.
	*
	*   @param -
	*   @return -
	*/
    void SetFullScreenL ();

	/**	ResetFullScreenL
	*
	*   Resets full screen.
	*
	*   @param -
	*   @return -
	*/
    void ResetFullScreenL ();

	/**	SetUndoFlag 
	*
	*   Sets the CanUndo flag to editor view.
	*
	*   @param -
	*   @return - 
	*/
    void SetUndoFlag();

    /**	SetTitlePaneTextL 
	*
	*	Sets title pane text.
	*
	*   @param aText - title pane text 
	*   @return - 
	*/
	void SetTitlePaneTextL (const TDesC & aText);

    /**	ClearTitlePaneTextL 
	*
	*	Clears title pane text.
	*
	*   @param -
	*   @return - 
	*/
    void ClearTitlePaneTextL();


    /**	ClearNaviPaneTextL 
	*
	*	Clears navigation pane text.
	*
	*   @param -
	*   @return - 
	*/
    void ClearNaviPaneTextL();

    /**	SetFileNameTitlePaneL 
	*
	*	Sets current file name to title pane.
	*
	*   @param aFullFileName - Full file name
	*   @return - 
	*/
    void SetFileNameTitlePaneL (const TDesC& aFullFileName);

	/**	HandleError
	*
	*   @CAknAppUi
	*/
    virtual TErrorHandlerResponse HandleError (
		TInt					aError,
        const SExtendedError &	aExtErr,
        TDes &					aErrorText,
        TDes &					aContextText
		);

	/**	HandleErrorL
	*
	*   Handles errors.
	*
	*   @param -
	*   @return -
	*/
	void HandleErrorL (const TInt aError);

	/**	ShowErrorNoteL
	*
	*   Shows error note based on error.
	*
	*   @param -
	*   @return -
	*/
	void ShowErrorNoteL (const TInt aError) const;

    /**	CheckFileL 
	*
	*   Checks the file whether saving is OK to do.
	*
	*   @param aFileName - file name
	*   @return - 
	*/
	void CheckFileL (const TDesC & aFileName) const;

    /**	UpdateScreenRectL 
	*
	*   Updates screen rect e.g. after switching full screen or landscape mode.
	*
	*   @param -
	*   @return - 
	*/
    void UpdateScreenRectL();

    /**	ZoomL
	*
	*   Zoom in or out by one step
	*
	*   @param aZoom - zoom direction
	*   @return - 
	*/
    void ZoomL (const TZoom aZoom);

    /**	PanL
	*
	*   Pan image.
	*
	*   @param aDirection - pan direction
	*   @return - 
	*/
	void PanL (TDirection aDirection);
    
    /** TouchPanL
    *
    *   Pan image. Panning has been done with touch. No parameters are
    *   delivered but function fetches direction information from view
    *   before forwarding pan command to engine.
    *
    *   @since S60 v5.0
    *   @param  - 
    *   @return - 
    */    
    void TouchPanL();

    /**	RotateL
	*
	*   Rotate image (add rotate filter to engine).
	*
	*   @param aRot - rotate direction
	*   @return - 
	*/
	void RotateL (const TRotation aRot);

    /**	SetImageNameL 
	*
	*   Create new document name
	*
	*   @param - 
	*   @return - 
	*/
    void SetImageNameL ();

    /**	LaunchLoadWaitNoteL 
	*
	*   Launches load wait note.
	*
	*   @param -
	*   @return - 
	*/
	void LaunchLoadWaitNoteL();

    /**	LaunchSaveWaitNoteL 
	*
	*   Launches load wait note.
	*
	*   @param -
	*   @param aNumberOfSteps - Number of blocks to save
	*   @return - 
	*/
	void LaunchSaveWaitNoteL ();

    /**	KillWaitNote 
	*
	*   Kills wait note.
	*
	*   @param -
	*   @return - 
	*/
	void KillWaitNote();

    /**	DeleteTempFiles 
	*
	*   Deletes all temp files.
	*
	*   @param -
	*   @return - 
	*/
	void DeleteTempFiles() const;

    /**	ApplicationReadyL 
	*
	*   Resets busy flag when application is ready. For synchronization.
	*
	*   @param -
	*   @return - 
	*/
	void ApplicationReadyL();

    /**	FinalizeSaveL 
	*   
	*
	*   @param -
	*   @return - 
	*/
	void FinalizeSaveL();

    /**	HandleVolumeUpL 
	*
	*   @see MMGXMediakeyObserver
	*/
	virtual void HandleVolumeUpL();	

    /**	HandleVolumeDownL 
	*
	*   @see MMGXMediakeyObserver
	*/
	virtual void HandleVolumeDownL();	

    /**	SaveImageOverWriteL
	*   
	*	Saves the changed image
	*	
	*	
	*
	*   @param aOverWrite if ETrue, overwrites the edited image
						  if EFalse, the image is saved to a given file name
	*   @return - 
	*/
    void SaveImageOverWriteL(TBool aOverWrite);
    
    /**	QueryAndSaveL
	*   
	*	Displays Avkon file handling queries and calls
	*	SaveImageOverWriteL() 
	*	
	*
	*   @param -
	*   @return TInt 1 if the image has been saved, otherwise 0
	*/    
    TInt QueryAndSaveL();
    

//@}

/** @name Members:*/
//@{
	/// Main view
	CAknView *						iEditorView;
    /// Wait dialog reference
	CAknWaitDialog *				iWaitNote;
    /// Progress dialog reference
	CAknProgressDialog *			iProgressNote;
	/// File handle
	RFile							iFile;
	/// Command line file name
	TFileName						iDocumentName;
    /// Source file name
	TFileName						iSourceFileName;
    /// Temporary saved file name
	TFileName						iTempSaveFileName;
    /// Active factory instance
	CObCallback *					iCallback;
    /// Plug-in locator instance
    CPluginLocator *                iLocator;
    /// Editor manager instance
    CImageEditorPluginManager *     iEditorManager;
    /// Image operation controller
    CImageController *              iImageController;
    /// CSendUi instance
	CSendUi *						iSendAppUi;
    /// Has the image been saved at least once
	TBool							iImageSaved;
    /// Application focus indicator
    TBool                           iForeground;
    /// Are we exiting from UI
    TBool                           iExitFromApp;
    /// Do we need to save changes
    TBool                           iSaveOnExit;
    /// System exit
    TBool                           iSysExit;
    /// Are we busy
    TBool                           iBusy;
    /// Orientation changed during loading
    TBool                           iOrientationChanged;
    /// Are we saving
    TBool                           iSaving;
    /// Are we going to send
    TBool                           iSending;
    /// Are we going to print
    TBool                           iPrinting;
    /// Are we going to cancel a plugin
    TBool							iCancelling;
    /// Are we in full screen
    TBool                           iFullScreen;
    /// Plug-ins are scanned
    TBool                           iPluginsScanned;
    /// Image is loaded
    TBool                           iImageLoaded;
    /// Main view is ready
    TBool                           iEditorReady;
    /// Cancel text input query dialog shown
    TBool                           iCancelTextInputActive;
    /// Resolution util
    CResolutionUtil*                iResolutionUtil;
    /// Media Gallery Albums support. 
    /// List of albums where the source image belongs to.
    RArray<TInt>                    iSourceImageMgAlbumIdList;
    /// Screen rectangle
    TRect							iOldScreenRect;

	TProcessPriority 				iOriginalProcessPriority;
	TBool 							iProcessPriorityAltered;
	CImageEditorRemConObserver*		iVolumeKeyObserver;
	
	TBool							iSavingQuery;
      
#ifdef S60_31_VOLUME_KEYS
	/// Remote connection API used to handle the volume keys
	/// (implementation borrowed from Media gallery)
	CMGXRemConTarget* 				iMGXRemConTarget;
#endif

//@}

};

#endif
