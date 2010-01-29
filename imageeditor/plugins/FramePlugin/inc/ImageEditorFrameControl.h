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
* Frame plugin control's header.
*
*/


#ifndef IMAGEEDITORCROPCONTROL_H
#define IMAGEEDITORCROPCONTROL_H

//  INCLUDES
#include <coecntrl.h>

#include "PreviewControlBase.h"

//  FORWARD DECLARATIONS
class CAknView;
class CFbsBitmap;
class CPluginInfo;

#ifdef RD_TACTILE_FEEDBACK 
class MTouchFeedback;
#endif /* RD_TACTILE_FEEDBACK  */


/*	CLASS: CImageEditorFrameControl
*
*   CImageEditorFrameControl is a control for red eye reduction plug-in.
*
*/
class CImageEditorFrameControl : public CPreviewControlBase
{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param aRect - control rectangle
	*	@param aParent - pointer to window owning control
	*	@return pointer to created CImageEditorFrameControl object
	*/
	static CImageEditorFrameControl * NewL (
		const TRect &		aRect,
		CCoeControl	*		aParent
		);

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	virtual ~CImageEditorFrameControl ();

	/** Second phase constructor
	*
	*	@see CImageEditorControlBase
	*
    */
	virtual void ConstructL (
		const TRect &		aRect,
		CCoeControl	*		aParent
		);

	/*	Setter for view reference
	*
	*	@see CImageEditorControlBase
	*
    */
    virtual void SetView (CAknView * aView);

    /*	SetSelectedUiItemL
	*
	*	@see CImageEditorControlBase
	*
    */
    virtual void SetSelectedUiItemL (CPluginInfo * aItem);

    /*	OfferKeyEventL
	*
	*	@see CImageEditorControlBase
	*
    */
    virtual TKeyResponse OfferKeyEventL (
        const TKeyEvent &   aKeyEvent,
        TEventCode          aType
        );   

	/*	HandlePluginCommandL
	*
	*	@see CImageEditorControlBase
	*
    */
    virtual void HandlePluginCommandL (const TInt aCommand);

	/*	GetSoftkeyIndexL
	*
	*	@see CImageEditorControlBase
    */
    virtual TInt GetSoftkeyIndexL();

	/*	GetNaviPaneTextL
	*
	*	@see CImageEditorControlBase
	*/
	virtual TPtrC GetNaviPaneTextL (
		TBool& aLeftNaviPaneScrollButtonVisibile, 
		TBool& aRightNaviPaneScrollButtonVisible);

	/*	GetParam
	*
	*	Gets pointer to the parameter struct.
	*
	*	@param -
	*	@return - pointer to MIA parameter struct
	*/
    TDesC & GetParam ();

//@}

protected:

/** @name Methods:*/
//@{
	/*	SizeChanged
	*
	*	@see CImageEditorControlBase
	*
    */
	virtual void SizeChanged();

	/*	PrepareL
	*
	*	@see CImageEditorControlBase
	*
    */
    virtual void PrepareL ();

    /*	HandlePointerEventL
	*
	*	@see CImageEditorControlBase
	*
    */
	virtual void HandlePointerEventL( const TPointerEvent &aPointerEvent );
//@}

/** @name Members:*/
//@{

//@}

private:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageEditorFrameControl ();

	/**	Draw
	*
	*	@see CImageEditorControlBase
	*
    */
	virtual void Draw (const TRect & aRect) const;

	/**	NaviRightL
	*
	*	Handles nave key right event.
	*
	*	@param -
	*	@return -
	*/
    void NaviRightL();

	/**	NaviLeftL
	*
	*	Handles nave key left event.
	*
	*	@param -
	*	@return -
	*/
    void NaviLeftL();

    /** FindFramesL
	*
    *   Search all the frames available in the system
	*/
    void FindFramesL();

    /** 
	*	Select and decode frame bitmap.
    *   Done asyncronously.
	*/
	void SelectFrameL();

    /** SetNaviPaneTextL
	*
    *   Update the navi pane.
	*/
    void SetNaviPaneTextL();

    /** Copy constructor, disabled
	*/
	CImageEditorFrameControl (const CImageEditorFrameControl & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorFrameControl & operator= (const CImageEditorFrameControl & rhs);

//@}

/** @name Typedefs:*/
//@{
    enum TFrameControlState
    {
        //  Invalid
        EFrameControlStateMin = 0,
        //  Loading initial frame
        EFrameControlStateInitializing,
        //  Loading frame
        EFrameControlStateLoadingFrame,
        //  Ready
        EFrameControlStateIdle,
        //  Invalid
        EFrameControlStateMax
    } iState;
    
    enum TFrameChangeDirection
        {
        // No direction
        ENoDirection = 0,
        // Direciton from right to left (x-axis)
        EPreviousFrame,
        // Direciton from left to right (x-axis)
        ENextFrame
	    };
//@}
    
/** @name Members:*/
//@{
    /// Editor view
    CAknView *					iEditorView;
    /// Plug-in info
    CPluginInfo *               iItem;
    /// Navigation pane text
    TBuf<64>                    iNaviPaneText;
    /// Frame file array
    CDesCArray*                 iFrameFileArray;
    /// Array of frames in each file
    RArray<TInt>                iMultiplicities;
    /// Current frame bitmap
    TInt                        iCurrentFrameIndex;
    /// Number of frame bitmaps
    TInt                        iFrameCount;
    /// Parameter
    TBuf<256>					iParameter;
    
    // Pointer event enablers
    // Pointer position in last pointer event
    TPoint                      iPointerPosition;
    // Position on button down event
    TPoint                      iInitialPointerPosition;
    // Position on button up event
    TPoint                      iFinalPointerPosition;
    // Whether stylus has been moved to one direction only or not 
    // (in x direction) 
    TBool                       iOneDirectionalChange;
    // Change direction on previous drag event
    TFrameChangeDirection       iPreviousChange;
    
	// Feedback for screen touch:
#ifdef RD_TACTILE_FEEDBACK 
	MTouchFeedback* iTouchFeedBack;
#endif /* RD_TACTILE_FEEDBACK  */
//@}

};


#endif
