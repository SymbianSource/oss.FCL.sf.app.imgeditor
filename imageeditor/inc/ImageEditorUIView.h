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
* Image Editor View class.
*
*/



#ifndef IMAGEEDITORUIVIEW_H
#define IMAGEEDITORUIVIEW_H


// INCLUDES
#include <coneresloader.h>

#include <aknview.h>
#include <aknprogressdialog.h> 

#include "bitfield.h"
#include "commondefs.h"

// CONSTANTS

// UID of view
const TUid KViewId = {0x00000001};

// FORWARD DECLARATIONS
class CImageEditorUIContainer;
class CPluginInfo;
class CAknWaitDialog;
class CAknNavigationControlContainer;
class CAknNavigationDecorator;
class CAknTitlePane;
class CSendUi;
class TSendingCapabilities;
class CAknInfoPopupNoteController;

#ifdef AIW_SUPPORT
class CAiwServiceHandler;
#endif



/*	CLASS: CImageEditorUIView
*
*	View part of Image Editor UI. 
*
*/
class CImageEditorUIView :	public CAknView
{

public:

/** @name Methods:*/
//@{

	/*	Default constructor
	*
	*   @param
	*   @return
	*/
    IMPORT_C CImageEditorUIView();

	/*	Second phase constructor
	*
	*   @param
	*   @return
	*/
	IMPORT_C void ConstructL();

	/*	Destructor	
	*
	*   @param
	*   @return
	*/
    IMPORT_C ~CImageEditorUIView();

	/*	Id
	*
	*   Returns UID of the view.
	*
	*   @param
	*   @return
	*	@see CAknView
	*/
    IMPORT_C TUid Id() const;

	/*	HandleCommandL
	*
	*	Handle view commands.   
	*
	*   @param aCommand - command ID
	*   @return -
	*	@see CAknView
	*/
    IMPORT_C void HandleCommandL (TInt aCommand);

	/*	HandleClientRectChange
	*
	*	Handles client rectangle changes.
	*
	*   @param -
	*   @return -
	*/
    IMPORT_C void HandleClientRectChange();

	/*	SetImage
	*
	*	Set bitmap for preview pane.
	*
	*   @param aBitmap - preview bitmap
	*   @return -
	*/
    IMPORT_C void SetImageL (CFbsBitmap * aBitmap);

	/*	SetImageFileName
	*
	*	Set image file name
	*
	*   @param aFileName - image filename
	*   @return -
	*/
    IMPORT_C void SetImageFileName (const TDesC& aFileName);

    /*	GetContainer
	*
	*	Set plug-in control to UI.
	*
	*   @param -
	*   @return - pointer to container control
	*/
	IMPORT_C CCoeControl * GetContainer () const;

    /*	ActivatePluginL
	*
	*	Activates plug-in UI.
	*
	*   @param aControl - plug-in control
	*   @return -
	*/
    IMPORT_C void ActivatePluginL (CCoeControl * aControl);

    /*	ActivateMainViewL
	*
	*	Activates main view.
	*
	*   @param -
	*   @return -
	*/
    IMPORT_C void ActivateMainViewL();

    /*	GetSelectedPluginInfoL
	*
	*	Gets plug-in information of the selected plug-in.
	*
	*   @param -
	*   @return plug-in info
	*/
    IMPORT_C CPluginInfo * GetSelectedPluginInfoL ();

	/*	AddPluginUiItemL
	*
	*	Adds a new plug-in UI item.
	*
	*   @param aPluginInfo - plug-in info class
	*   @return - 
	*/
    IMPORT_C void AddPluginUiItemL (const CPluginInfo * aPluginInfo);

	/*	SetCustomButtonsL
	*
	*	Sets custom buttons for the view.
	*
	*   @param aResourceID - resource ID for CBA
	*   @return - 
	*/
    IMPORT_C void SetCustomButtonsL (TInt aResourceID) const;

	/*	LaunchSaveChangesQueryL
	*
	*	Launches a query dialog "Save changes?" query.
	*
	*   @param - 
	*   @return - !0 if exit, 0 if not exit
	*/
    IMPORT_C TInt LaunchSaveChangesQueryL () const;

	/*	LaunchExitWithoutSavingQueryL
	*
	*	Launches a query dialog "Exit anyway" if disk is full
    *   when trying to exit and save - confirmation query.
	*
	*   @param - 
	*   @return - !0 if exit, 0 if not exit
	*/
    IMPORT_C TInt LaunchExitWithoutSavingQueryL () const;

	/*	LaunchCancelTextInputQueryL
	*
	*	Launches a query dialog "Cancel text input" if screen
    *   is turned to landscape mode and text input is not available
	*
	*   @param - 
	*   @return - !0 if cancel, 0 otherwise
	*/
    IMPORT_C TInt LaunchCancelTextInputQueryL () const;

	/*	LaunchSendQueryL
	*
	*	Launches a send list query.
	*
	*   @param aCaps - sending capabilities
    *   @param aFileName - aAttachment
	*   @return - 
	*/
	IMPORT_C void LaunchSendQueryL (
        const TSendingCapabilities& aCaps,
        TFileName& aFileName
        ) const;

	/*	LaunchSendQueryL
	*
	*	Launches a send list query.
	*
	*   @param aCaps - sending capabilities
    *   @param aFileHandle - aAttachment
	*   @return - 
	*/
	IMPORT_C void LaunchSendQueryL (
        const TSendingCapabilities& aCaps,
        RFile& aFileHandle
        ) const;

	/*	LaunchSaveWaitDialogL
	*
	*	Launches a save wait dialog.
	*
	*   @param -	aSelfPtr
    *				The wait dialog is allocated at this pointer.
	*				The pointer needs to be valid when the dialog 
	*				is dismissed and must be a class member.
	*	@param 		aFinalValue Final value for the process
	*   @param -	aCallback   The wait dialog callback.
	*				Pass NULL if no callback is needed.
	*   @return - 
	*/
	IMPORT_C void LaunchSaveWaitDialogL(
        CAknProgressDialog** aSelfPtr,
        TInt aFinalValue,
        MProgressDialogCallback* aCallback
        ) const;

	/*	LaunchLoadWaitDialogL
	*
	*	Launches a load wait dialog.
	*
	*   @param -	aSelfPtr
    *				The wait dialog is allocated at this pointer.
	*				The pointer needs to be valid when the dialog 
	*				is dismissed and must be a class member.
	*   @param -	aCallback
    *				The wait dialog callback.
	*				Pass NULL if no callback is needed.
	*   @return - 
	*/
	IMPORT_C void LaunchLoadWaitDialogL(
        CAknWaitDialog** aSelfPtr,
        MProgressDialogCallback* aCallback
        ) const;

	IMPORT_C void LaunchWaitDialog() const;

	IMPORT_C void HideWaitDialog() const;

	/*	ShowInformationNoteL
	*
	*	Launches a information note.
	*
	*   @param aNoteText - text to be shown
	*   @return - 
	*/
    IMPORT_C void ShowInformationNoteL (const TDesC& aNoteText) const;

	/*	ShowConfirmationNoteL
	*
	*	Launches a confirmation note.
	*
	*   @param aNoteText - text to be shown
	*   @return - 
	*/
    IMPORT_C void ShowConfirmationNoteL (const TDesC& aNoteText) const;

	/*	SetNaviPaneTextL 
	*
	*	Sets navigation pane text.
	*
	*   @param aText - navigation pane text 
	*   @param aLeftNaviPaneScrollButtonVisibile
	*   @param aRightNaviPaneScrollButtonVisible
	*   @return - 
	*/
	IMPORT_C void SetNaviPaneTextL (
		const TDesC & aText,
		TBool aLeftNaviPaneScrollButtonVisibile, 
		TBool aRightNaviPaneScrollButtonVisible );

	/*	ClearNaviPaneTextL 
	*
	*	Clears navigation pane text.
	*
	*   @param -
	*   @return - 
	*/
	IMPORT_C void ClearNaviPaneTextL();

    /*	SetTitlePaneTextL 
	*
	*	Sets title pane text.
	*
	*   @param aText - title pane text 
	*   @return - 
	*/
	IMPORT_C void SetTitlePaneTextL (const TDesC & aText);

    /*	ClearTitlePaneTextL  
	*
	*	Clears title pane text.
	*
	*   @param -
	*   @return - 
	*/
	IMPORT_C void ClearTitlePaneTextL ();

    /*	SetSoftKey1L
	*
	*	Sets soft key 1 command id and text.
	*
	*   @param aText - soft key text
	*   @param aCommand - command id
	*   @return - 
	*/
	IMPORT_C void SetSoftKey1L (
		const TDesC &	aText,
		const TInt		aCommand
		);

    /*	SetSoftKey2L
	*
	*	Sets soft key 2 command id and text.
	*
	*   @param aText - soft key text
	*   @param aCommand - command id
	*   @return - 
	*/
	IMPORT_C void SetSoftKey2L (
		const TDesC &	aText,
		const TInt		aCommand
		);
    
    /*	SetMiddleSoftKeyL
	*
	*	Sets Middle soft key command id and text.
	*
	*   @param aText - soft key text
	*   @param aCommand - command id
	*   @return - 
	*/
	IMPORT_C void SetMiddleSoftKeyL (
		const TDesC &	aText,
		const TInt		aCommand
		);

	/*	SetBusy
	*
	*	Sets busy flag. When busy flag is set, no key events are handled.
	*
	*   @param -
	*   @return - 
	*/
    IMPORT_C void SetBusy();

	/*	ResetBusy
	*
	*	Resets busy flag. When busy flag is reset, key events are handled.
	*
	*   @param -
	*   @return - 
	*/
    IMPORT_C void ResetBusy();

	/*	SetUndoFlag
	*
	*	Sets undo flag. When undo flag is set to ETrue, Undo item is shown 
    *   in menu.
	*
	*   @param -
	*   @return - 
	*/
    IMPORT_C void SetUndoFlag (const TBool aUndo);

	/*	SetCanSave
	*
	*	Sets save flag. When save flag is set, Save item is shown in menu.
	*
	*   @param aSave - flag indicating, wheather saving is possible or not.
	*   @return - 
	*/
    IMPORT_C void SetSaveFlag (const TBool aSave);

	/*	SetFullScreen
	*
	*	Sets full screen flag. When full screen flag is set, the view is in
    *   full screen mode. Normal screen item is shown in menu.
	*
	*   @param -
	*   @return - 
	*/
    IMPORT_C void SetFullScreen();

	/*	ResetFullScreen
	*
	*	Resets full screen flag. When full screen flag is reset, the view is 
    *   in normal screen mode. Full screen item is shown in menu.
	*
	*   @param -
	*   @return - 
	*/
    IMPORT_C void ResetFullScreen();

	/*	SetZoomModeL
	*
	*	Sets zoom mode.
	*
	*   @param aMode - zoom mode
	*   @return - 
	*/
    IMPORT_C void SetZoomModeL( const TZoomMode& aMode );

    /*	DynInitMenuPaneL
	*
	*	Initialize menu items dynamically
	*
	*   @param aResourceId - resource ID of menu to be initialized
	*   @param aMenuPane - run-time presentation of the menu pane
	*   @return - 
	*	@see MEikMenuObserver
	*/
	IMPORT_C virtual void DynInitMenuPaneL (
		TInt			aResourceId,
		CEikMenuPane *	aMenuPane
		);

	/*	HandleStatusPaneSizeChange
	*
	*	Main pane and status pane are partly overlapping. This method 
    *   handles the status pane changes
	*
	*   @param -
	*   @return -
	*	@see CAknView
	*/
    IMPORT_C void HandleStatusPaneSizeChange();

	/*	UpdateLayoutL
	*
	*	Update layout of the view according to screenmode
	*
	*   @param aScreenMode - screen mode
	*   @return -
	*/
    IMPORT_C void UpdateLayoutL(TInt aScreenMode) const;

	/*	SetSendAppUi
	*
	*	Sets CSendUi pointer
	*
	*   @param aSendAppUi - pointer to CSendUi instance
	*   @return -
	*/
    IMPORT_C void SetSendAppUi(CSendUi* aSendAppUi);

	/*	LaunchSaveImageQueryL 
	*
	*	Launches a query dialog "Save image:".
	*
	*   @param - 
	*   @return - list query id or -1 if the user selects No
	*/
    IMPORT_C TInt LaunchSaveImageQueryL () const;
    
    /* GetTouchPanDirections 
    *
    *   Can be used to get the x- and y-directional movement changes
    *   when image is panned with touch.
    *   
    *   @since S60 v5.0
    *   @param - xMovement x-directional change
    *   @param - yMovement y-directional change
    *   @return - 
    */    
    IMPORT_C void GetTouchPanDirections( TInt& xMovement, TInt& yMovement );
    
    /*  InZoomingState()
    *
    *   Checks whether an image has been zoomed or not.
    *
    *   @param -
	*   @return ETrue if in zooming state, EFalse otherwise
    */
    IMPORT_C TBool InZoomingState();
    IMPORT_C TBool IsMemoryInCriticalLevel();
//@}

protected:

/** @name Methods:*/
//@{
//@}

	/*	ViewScreenModeCompatible
	*
	*	@see CAknView
	*
	*/
    IMPORT_C TBool ViewScreenModeCompatible(TInt aScreenMode);


/** @name Members:*/
//@{
//@}

private:

/** @name Methods:*/
//@{

	/*	DoActivateL
	*
	*	Called when view is activated.
	*
	*   @param aPrevViewId - 
	*   @param aCustomMessageId -
	*   @param aCustomMessage -
	*   @return -
	*	@see CAknView
	*/
    IMPORT_C void DoActivateL (
		const TVwsViewId &  aPrevViewId,
		TUid				aCustomMessageId,
        const TDesC8 &		aCustomMessage
		);

	/*	DoDeactivate
	*
	*   Called when view is deactivated.
	*
	*   @param -
	*   @return -
	*	@see CAknView
	*/
    IMPORT_C void DoDeactivate();

    /*	DimAllL
	*
	*	Dims all menu items
	*
	*   @param aResourceId - resource ID of menu to be initialized
	*   @param aMenuPane - run-time presentation of the menu pane
	*   @return - 
	*/
	void DimAllL (
		TInt			aResourceId,
		CEikMenuPane &	aMenuPane
		) const;

    /*	UpdateSoftkeysL 
	*
	*	Updates softkeys.
	*
	*   @param -
	*   @return - 
	*/
    void UpdateSoftkeysL();

    /*	UpdateNaviPaneL 
	*
	*	Updates navi pane text.
	*
	*   @param -
	*   @return - 
	*/
    void UpdateNaviPaneL();

    /*	InsertPluginItemsL
	*
	*	Inserts plug-in menu items to menu pane.
	*
	*   @param aMenuPane - run-time presentation of the menu pane
	*   @return - 
	*/
    void InsertPluginItemsL (CEikMenuPane &	aMenuPane);

    /*	MenuItemPressedL
	*
	*	Checks if a plug-in menu item has been pressed.
	*
	*   @param aCommand - command id
	*   @return ETrue if menu item pressed, EFalse otherwise
	*/
    TBool MenuItemPressedL (TInt aCommand);
    
    /*	AddContainerToStack()
	*
	*	Add container to control stack and ensure it is not added twice
	*
	*   @param -
	*   @return - 
	*/
	void AddContainerToStack();
    
    /*	RemoveContainerFromStack()
	*
	*	Remove container from control stack and ensure it is not removed twice
	*
	*   @param -
	*   @return - 
	*/
	void RemoveContainerFromStack();
    
    /*  HandleZoomModeChangeL()
    *
    *   Handle needed operations (CBA buttons, tooltip) 
    *   after zoom mode change.
    *
    *   @param -
	*   @return -
    */
    void HandleZoomModeChangeL();
    
    /*  ResolveCaptionNameL
    *
    *   Resolves the caption (Media gallery) that should be added to 
    *   saving note. Leaves if something goes wrong and caption can't be
    *   resolved.
    *
    *   @param aCaption - Caption to be added 
	*   @return -
    */
    void ResolveCaptionNameL( TApaAppCaption& aCaption ) const;

//@}

/** @name Members:*/
//@{
	/// UI container control
    CImageEditorUIContainer *	        iContainer;
    //  Navigation pane
    CAknNavigationControlContainer *    iNaviPane;
    /// Navigation decorator
    CAknNavigationDecorator *           iNaviDecorator;
    /// Previous navigation decorator
    CAknNavigationDecorator *           iPreviousNaviDecorator;
    /// Title pane
    CAknTitlePane *                     iTitlePane;
    /// SendAppUi
    CSendUi*                            iSendAppUi;
	/// Resource reader
	RConeResourceLoader                 iResLoader;
    /// Resource ID
	TInt                                iResID;
	/// Can undo
	TBool                               iCanUndo;
	/// Can save
	TBool                               iCanSave;
	/// In plug-in
	TBool                               iInPlugin;
	/// Are we in full screen mode
	TBool                               iFullScreen;
	/// Busy
    TBool                               iBusy;
    /// Zoom mode
	TZoomMode                           iZoomMode;
	/// Left soft key ID
	TInt                                iSoftkey1;
	/// Right soft key ID
	TInt                                iSoftkey2;
	/// Middle soft key ID
	TInt                                iMSK;
    /// Array of UI items
    RPointerArray<CPluginInfo>          iArray;
    /// Selected plug-in UI item index
    TInt                                iIndex;
    /// Currently loaded image name
    TFileName                           iImageFileName;
    /// Pointer to plug-in control
    CCoeControl *                       iControl;
    /// Preview bitmap
    CFbsBitmap*                         iPreview;
    /// Selected print menu command
    TInt                                iSelectedPrintMenuCmd;
    /// Is container added to stack
    TBool								iContainerInStack;
    // Array for zoom popup note texts
    CDesCArray*                         iZoomTexts;
    // Popup controller
	CAknInfoPopupNoteController*        iPopupController;
	
#ifdef AIW_SUPPORT
    /// AIW Service Handler
    CAiwServiceHandler*                 iServiceHandler;
#endif

//@}

};

#endif

// End of File
