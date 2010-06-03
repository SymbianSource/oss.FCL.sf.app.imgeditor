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
* A base class for image editor controls.
*
*/


#ifndef IMAGEEDITORCONTROLBASE_H
#define IMAGEEDITORCONTROLBASE_H

//  INCLUDES
#include <coecntrl.h>
#include "bitfield.h"

//  FORWARD DECLARATIONS
class CImageEditorUIView;
class CUiItem;
class CAknView;
class CPluginInfo;
class CEikButtonGroupContainer;

/*	CLASS: CImageEditorControlBase
*
*	CImageEditorControlBase represents a base class for all Image Editor
*	controls: main UI controls and plug-in controls.
*/
class CImageEditorControlBase   : public CCoeControl
{

public:

/** @name Methods:*/
//@{

	/*	Default constructor
	*
	*   @param -
	*   @return - 
	*/
    IMPORT_C CImageEditorControlBase ();

	/*	Destructor
	*
	*   @param -
	*   @return - 
	*/
    IMPORT_C virtual ~CImageEditorControlBase ();

	/** Second phase constructor
	*
	*	@param aRect - control rectangle
	*	@param aParent - pointer to window owning control
	*	@return -
	*/
	IMPORT_C virtual void ConstructL (
		const TRect &		aRect,
		CCoeControl	*		aParent
		);

	/*	SetImageL
	*
	*   Setter for bitmap pointer 
	*
	*   @param aBitmap - pointer to bitmap image
	*   @return - 
	*/
    IMPORT_C virtual void SetImageL (CFbsBitmap * aBitmap);
    
    /*	SetImageL
	*
	*   Setter for constant bitmap pointer 
	*
	*   @param aBitmap - const pointer to bitmap image
	*   @return - 
	*/
    IMPORT_C virtual void SetImageL (const CFbsBitmap * aBitmap);
    
	/*	SetView
	*
	*   Sets reference to the parent view
	*
	*   @param aView - pointer to view
	*   @return - 
	*/
    IMPORT_C virtual void SetView (CAknView * aView);

	/*	SetSelectedUiItemL
	*
	*   Sets selected UI item
	*
	*   @param aItem - pointer to selected UI Item
	*   @return - 
	*/
    IMPORT_C virtual void SetSelectedUiItemL (CPluginInfo * aItem);

	/*	PrepareL
	*
	*	This function is called to finalize the plugin control construction.
	*	It is called after ConstructL, SetView and SetSelectedUiItemL
	*	have been done.
	*	
	*	@param -
	*	@return -
	*/
    IMPORT_C virtual void PrepareL ();
    

    /*	OfferKeyEventL
    *
    *   Handles key events. When a key event occurs, the control framework 
    *   calls this function for each control on the control stack, until one 
    *   of them can process the key event (and returns EKeyWasConsumed).
    *
    *   @param aKeyEvent - the key event
    *   @param aType - type of key event
    *   @return indicator for event to be handled
    *   @see CCoeControl
    */
    IMPORT_C virtual TKeyResponse OfferKeyEventL (
        const TKeyEvent &   aKeyEvent,
        TEventCode          aType
        );   

	/*	CountComponentControls
	*
	*   Gets the number of controls in compound control.
	*
	*   @param -
	*   @return number of controls
	*	@see CoeControl
	*/
    IMPORT_C virtual TInt CountComponentControls() const; 

	/*	ComponentControl
	*
	*   Gets special component control indexed with aIndex.
	*
	*   @param aIndex - component control index
	*   @return pointer to component control 
	*	@see CoeControl
	*/
    IMPORT_C virtual CCoeControl * ComponentControl (TInt aIndex) const;

	/*	SetBusy
	*
	*	Sets busy, when busy UI does not handle key events.
	*
	*   @param -
	*   @return - 
	*/
    IMPORT_C virtual void SetBusy();

	/*	ResetBusy
	*
	*	Resets busy, when busy UI does not handle key events.
	*
	*   @param -
	*   @return - 
	*/
    IMPORT_C virtual void ResetBusy();

	/*	Busy
	*
	*	Returns the busy value, when busy UI does not handle key events
	*
	*   @param -
	*   @return - 
	*/
    IMPORT_C virtual TBool Busy() const;

	/*	HandlePluginCommandL
	*
	*	Handles plug-in command for a plug-in.
	*
	*   @param aCommand - command id
	*   @return UI step index >= 0, -1 if no UI update needed
	*/
    IMPORT_C virtual void HandlePluginCommandL (const TInt aCommand);

	/*	GetSoftkeyIndexL
	*
	*	Gets soft key index of a plug-in.
	*
	*   @param -
	*   @return -1 if no change, index otherwise
	*/
    IMPORT_C virtual TInt GetSoftkeyIndexL ();
    
    /*	GetContextMenuResourceId
	*
	*	Gets context specific menu resource id of a plug-in.
	*
	*   @param -
	*   @return 0 if no id, resource id otherwise
	*/
    IMPORT_C virtual TInt GetContextMenuResourceId ();
    
	/*	GetNaviPaneTextL
	*
	*	Gets navi pane text.
	*	
	*	By default a plain navi pane label is created. Scroll
	*	indicators can be enabled by setting the boolean parateters.
	*
	*   @param - aLeftNaviPaneScrollButtonVisibile
	*   @param - aRightNaviPaneScrollButtonVisible
	*   @return - 
	*/
	IMPORT_C virtual TPtrC GetNaviPaneTextL (
		TBool& aLeftNaviPaneScrollButtonVisibile, 
		TBool& aRightNaviPaneScrollButtonVisible);

	/*	GetDimmedMenuItems
	*
	*	Get the plugin menu item visibility information to dynamically
	*	initialise the menu pane.
	*
	*   @param - 
	*   @return - the visibility information bitfield
	*/
	IMPORT_C virtual TBitField GetDimmedMenuItems ();

	
//@}

protected:

/** @name Methods:*/
//@{

	/*	SizeChanged
	*
	*   Responds to size and position changes of the control. Called every
	*	time if one of the control size-changing methods is called.
	*
	*   @param -
	*   @return -
	*	@see CoeControl
	*/
	IMPORT_C virtual void SizeChanged();

    /*	HandlePointerEventL
	*
	*   @param aPointerEvent event to handle in the function
	*   @return -
	*	@see CoeControl
	*/
	IMPORT_C virtual void HandlePointerEventL( const TPointerEvent &aPointerEvent );
	
//@}

/** @name Members:*/
//@{

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
	IMPORT_C virtual void Draw (const TRect & aRect) const;

//@}

/** @name Members:*/
//@{
    /// Busy flag
    TBool                       iBusy;
//@}

};


#endif

// End of File



