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
* ImageEditor container class header file.
*
*/


#ifndef IMAGEEDITORUICONTAINER_H
#define IMAGEEDITORUICONTAINER_H


// INCLUDES
#include <coecntrl.h>
   
#include "ImageEditorUIView.h"
#include <AknNaviDecoratorObserver.h>

#ifdef RD_TACTILE_FEEDBACK 
class MTouchFeedback; 
#endif /* RD_TACTILE_FEEDBACK  */

// FORWARD DECLARATIONS
class CImageEditorControlBase;
class CFbsBitmap;
class CPluginInfo; 
class CAknsBasicBackgroundControlContext;
class CWaitIndicator;

/*	CLASS: CImageEditorUIContainer
*
*   Container control for Image Editor UI.
*
*/
class CImageEditorUIContainer :		public CCoeControl,
									public MCoeControlObserver,
									public MAknNaviDecoratorObserver
{

public:

/** @name Methods:*/
//@{

	/*	IsDrawPlugin
	*
	*	Judge if it is draw plug-in.
	*   @return - 
	*/
	IMPORT_C void IsDrawPlugin(TBool aFlag);
	/*	Second phase constructor
	*
	*   @param aRect - frame rectangle for container
	*   @return
	*/		
    void ConstructL (const TRect & aRect);

	/*	Destructor
	*
	*   @param -
	*   @return -
	*/
    ~CImageEditorUIContainer();

	/*	SetImage
	*
	*	Set bitmap for preview pane.
	*
	*   @param aBitmap - pointer to bitmap image
	*   @return - 
	*/
    void SetImageL (CFbsBitmap *	aBitmap);

    /*	SetControl
	*
	*	Set plug-in control to UI.
	*
	*   @param aControl - plug-in control
	*   @return -
	*/
    void SetControl (CCoeControl * aControl);

    /*	Setter for view reference, non-const
	*
	*   @param aView - CImageEditorUIView pointer
	*   @return -
	*/
    void SetView (CImageEditorUIView * aView);

    /*	OfferKeyEventL
    *
    *   Handles key events. When a key event occurs, the control framework 
    *   calls this function for each control on the control stack, until one 
    *   of them can process the key event (and returns EKeyWasConsumed).
    *
    *   @param aKeyEvent - the key event
    *   @param aType - type of key event
    *   @return indicator for event to be handled
    */
    virtual TKeyResponse OfferKeyEventL (
        const TKeyEvent &   aKeyEvent,
        TEventCode          aType
        );   
    
    /*	HandlePointerEventL
    *
    *   Handles pointer events gotten from the framework. This function gets 
    *   called whenever a pointer event occurs in the control.
    *   
    *   @param aKeyEvent - the key event
    *   @return -
    */
    virtual void CImageEditorUIContainer::HandlePointerEventL(const TPointerEvent &aPointerEvent);
    
	/*	CountComponentControls
	*
	*   Gets the number of controls in compound control.
	*
	*   @param -
	*   @return number of controls
	*	@see CoeControl
	*/
    TInt CountComponentControls() const;

	/*	ComponentControl
	*
	*   Gets special component control indexed with aIndex.
	*
	*   @param aIndex - component control index
	*   @return pointer to component control 
	*	@see CoeControl
	*/
    CCoeControl * ComponentControl (TInt aIndex) const;

	/*	SetBusy
	*
	*	Sets busy, when busy UI does not handle key events.
	*
	*   @param -
	*   @return - 
	*/
    void SetBusy();

	/*	ResetBusy
	*
	*	Resets busy, when busy UI does not handle key events.
	*
	*   @param -
	*   @return - 
	*/
    void ResetBusy();

	/*	Busy
	*
	*	Returns the busy value, when busy UI does not handle key events
	*
	*   @param -
	*   @return - 
	*/
    TBool Busy() const;

	/*	SetFullScreen
	*
	*	Sets full screen flag. When full screen flag is set, the view is in
    *   full screen mode. Normal screen item is shown in menu.
	*
	*   @param -
	*   @return - 
	*/
    void SetFullScreen();

	/*	ResetFullScreen
	*
	*	Resets full screen flag. When full screen flag is reset, the view is 
    *   in normal screen mode. Full screen item is shown in menu.
	*
	*   @param -
	*   @return - 
	*/
    void ResetFullScreen();

    /*	GetHelpContext
	*
	*	@see CCoeControl
	*
    */
	virtual void GetHelpContext(TCoeHelpContext& aContext) const;
    
    /* GetDragDirections
    *
    *   Resets full screen flag. When full screen flag is reset, the view is 
    *   in normal screen mode. Full screen item is shown in menu.
    *
    *   @since S60 v5.0
    *   @param - xMovement x-directional change on screen
    *   @param - yMovement y-directional change on screen
    *   @return - 
    */    
    void GetDragDirections( TInt& xMovement, TInt& yMovement );
    
//@}

public: //From MAknNaviDecoratorObserver

    /* HandleNaviDecoratorEventL
    * 
    * Method for handling touch events on navigator (navi pane)
    * 
    * @since S60 v5.0
    * @param aEventID Id of the event.
    *
    */
    void HandleNaviDecoratorEventL( TInt aEventID );

    void ShowWaitNote();

    void HideWaitNote();

protected:

/** @name Methods:*/
//@{

	/*	SizeChanged
	*
	*   Sets the size and position of the contents of the container according
	*	to container size changes. This methods is called whenever SetExtent, 
	*	SetSize, SetRect, SetCornerAndSize or SetExtentToWholeScreen is called.
	*
	*   @param -
	*   @return -
	*	@see CoeControl
	*/
    virtual void SizeChanged();

	/*	MopSupplyObject
	*
	*	@see CoeControl
	*/
	virtual TTypeUid::Ptr MopSupplyObject( TTypeUid aId );

//@}

private:

/** @name Methods:*/
//@{

	/*	Draw
	*
	*   Draw controls, called by window server.
	*
	*   @param aRect - region of control in need of redrawing
	*   @return -
	*	@see CoeControl
	*/
    void Draw (const TRect & aRect) const;

	/*	HandleControlEventL
	*
	*   Handles an event sent from the observed (this control being observer).
	*
	*   @param aControl - pointer to control that sent the event
	*   @param aEventType - control event type
	*   @return - 
	*	@see MCoeControlObserver
	*/
    void HandleControlEventL (
		CCoeControl *		aControl,
		TCoeEvent			aEventType
		);

//@}

/** @name Members:*/
//@{
	/// Reference to the view class
	CImageEditorUIView *		iEditorView;
	/// Preview image
	CFbsBitmap *		        iPreview;
	/// Plug-in control
	CCoeControl *               iControl;
    /// Editor rectangle
    TRect                       iEditorRect;
	//	Busy flag
	TBool						iBusy;
	//	Full screen flag
    TBool                       iFullScreen;
            
	/// Control context that provides a layout background with a 
	/// background bitmap and its layout rectangle.
	CAknsBasicBackgroundControlContext*	iBgContext;

    //TouchPan enablers
    TPoint                      iTappedPosition;
    TInt                        iXDirChange;
    TInt                        iYDirChange;
    
    //for draw plguin
    TBool iIsDrawPlugin;
    // Wait indicator (own)
    CWaitIndicator* iWaitIndicator;
    
    // Feedback for screen touch:    
#ifdef RD_TACTILE_FEEDBACK 
    MTouchFeedback* iTouchFeedBack;
#endif /* RD_TACTILE_FEEDBACK  */

//@}
};

#endif

// End of File
