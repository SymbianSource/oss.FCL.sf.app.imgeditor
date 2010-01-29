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
* Header file for RedEyeReduction plugin control class.
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
class CSystemParameters;
class CAknInfoPopupNoteController; 

#ifdef RD_TACTILE_FEEDBACK 
class MTouchFeedback;
#endif /* RD_TACTILE_FEEDBACK  */

/*	CLASS: CImageEditorRedEyeReductionControl
*
*   CImageEditorRedEyeReductionControl is a control for red eye reduction plug-in.
*
*/
class CImageEditorRedEyeReductionControl :		public CPreviewControlBase
{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param aRect - control rectangle
	*	@param aParent - pointer to window owning control
	*	@return pointer to created CImageEditorRedEyeReductionControl object
	*/
	static CImageEditorRedEyeReductionControl * NewL (
		const TRect &		aRect,
		CCoeControl	*		aParent
		);

	/** DancingAntzCallback
	*
	*	Static callback for rotating dancing ants.
	*
	*	@param aPtr - parameter given for TCallBack
	*	@return TInt
	*/
	static TInt DancingAntzCallback (TAny * aPtr);

	/** FastKeyCallback
	*
	*	Static callback for triggering keys faster.
	*
	*	@param aPtr - parameter given for TCallBack
	*	@return TInt
	*/
	static TInt FastKeyCallback (TAny * aPtr);
    
	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	virtual ~CImageEditorRedEyeReductionControl ();

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

	/** PrepareL
	*
	*	@see CImageEditorControlBase
	*
    */
	virtual void PrepareL ();

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

	/*	GetDimmedMenuItems
	*
	*	@see CImageEditorControlBase
	*/
	virtual TBitField GetDimmedMenuItems();

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
    TRect GetParam ();

	/*	SetSysteParameters
	*
	*	Sets reference to system parameters to plug-in.
	*
	*	@param aSysPars - System parameters instance
	*	@return - 
	*/
    void SetSystemParameters (const CSystemParameters * aSysPars);

	/**	IsReadyToRender
	*
    *   Return ETrue if all parameters have valid value and filter
    *   can be rendered.
	*
	*	@param -
	*	@return - ETrue if ready to render, otherwise EFalse
	*/
    TBool IsReadyToRender() const;

    /*	GetHelpContext
	*
	*	@see CCoeControl
	*
    */
	virtual void GetHelpContext(TCoeHelpContext& aContext) const;


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
	CImageEditorRedEyeReductionControl ();

	/** Second phase constructor
	*
	*	@see CImageEditorControlBase
	*
    */
	virtual void ConstructL (
		const TRect &		aRect,
		CCoeControl	*		aParent
		);

	/**	Draw
	*
	*	@see CImageEditorControlBase
	*
    */
	virtual void Draw (const TRect & aRect) const;

	/**	DrawDancingAntz
	*
	*	Drawing function for dancing ants selection border.
	*
	*	@param aRect - control rectangle in need of refreshing 
	*	@return -
    */
	void DrawDancingAntz (const TRect & aRect) const;

	/**	NaviDown
	*
	*	Handles navi key down event.
	*
	*	@param -
	*	@return -
	*/
    void NaviDown();

	/**	NaviUp
	*
	*	Handles nave key up event.
	*
	*	@param -
	*	@return -
	*/
    void NaviUp();

	/**	NaviRight
	*
	*	Handles nave key right event.
	*
	*	@param -
	*	@return -
	*/
    void NaviRight();

	/**	NaviLeft
	*
	*	Handles navi key left event.
	*
	*	@param -
	*	@return -
	*/
    void NaviLeft();

	/**	KeyOkL
	*
	*	Handles OK key event.
	*
	*	@param -
	*	@return -
	*/
	void KeyOkL();
	
	/**	CancelPluginL
	*
	*	Handles the cancel command; Undo all the filters
	*	except for the first; the plugin manager takes care of that.
	*
	*	@param -
	*	@return -
	*/
	void CancelPluginL();

    /**	ToMoveState
	*
    *   Handles transition to Move state.
	*
	*	@param -
	*	@return -
	*/
    void ToMoveStateL();

	/**	ToResizeState
	*
    *   Handles transition to Resize state.
	*
	*	@param -
	*	@return -
	*/
    void ToResizeStateL();

	/**	CursorLocation
	*
 	*	Calculates the selection point in real coordinates 
	*	relative to the preview image.
	*
	*	@param -
	*	@return TPoint
	*/
    TPoint CursorLocation() const;

	/**	SelectionRect
	*
	*	Calculates a rectangle from the current coordinates and selection radius,
	*	in real coordinates relative to the preview image.
	*
	*	@param aCenter centerpoint in real coordinates
	*	@return TRect
	*/
    TRect SelectionRect( const TPoint& aCenter ) const;

	/**	ComputeParams
	*
	*	Calculates the filter parameters and
	*	updates them to the engine parameter struct.
	*
	*	@param -
	*	@return -
	*/
    void ComputeParams();

	/**	AddFilterToEngineL
	*
	*	
	*	
	*
	*	@param -
	*	@return -
	*/
	void AddFilterToEngineL();

	/**	UndoL
	*
	*	Undo one step of the red-eye reduction process.
	*	(The scope is different from the global undo.)
	*	
	*	@param -
	*	@return -
	*/
	void UndoL();

	/**	StartDancingAntzTimer
	*
	*	Starts dancing ants timer. 
	*
	*	@param -
	*	@return -
	*/
	void StartDancingAntzTimer();

	/**	StartFastKeyTimer
	*
	*	Starts fast key timer. 
	*
	*	@param -
	*	@return -
	*/
	void StartFastKeyTimer();

	/**	OnDancingAntzCallBack
	*
	*	Timer event handler for dancing ants.
	*
	*	@param -
	*	@return -
	*/
	void OnDancingAntzCallBack();

	/**	OnFastKeyCallBackL
	*
	*	Timer event handler for fast keys.
	*
	*	@param -
	*	@return -
	*/
	void OnFastKeyCallBackL();

    /** Copy constructor, disabled
	*/
	CImageEditorRedEyeReductionControl (const CImageEditorRedEyeReductionControl & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorRedEyeReductionControl & operator= (const CImageEditorRedEyeReductionControl & rhs);

	/**	ClipPosition
	*
	*	Clips clipart position after screen mode change.
	*
	*	@param -
	*	@return -
	*/
	void ClipPosition();
	
	/**	SetCursorPosition
	*
	*   Sets the cursor to a position given as a parameter
	*
	*	@param aNewPosition a point where to set the cursor
	*	@return -
	*/
	void SetCursorPosition( TPoint aNewPosition );
	
	/**	CrossHairCursorRect
	*
	*   Returns the rect of Cross hair cursor
	*
	*	@param -
	*	@return TRect cursor rect
	*/
	TRect CrossHairCursorRect() const;
	
	/**	CursorTapped
	*
	*   Gets a point as an parameter and returns whether 
	*   current cursor rect includes that point
	*
	*	@param aTappedPosition a point that was tapped by pen
	*	@return TBool ETrue if cursor includes tapped point
	*                 EFalse otherwise
	*/
	TBool CursorTapped( TPoint aTappedPosition ) const;
	
    /** ShowTooltip
    *
    *   Show tooltip text on the screen. Tooltip text is set based 
    *   on the current state.
    *
    *   @param -
    *   @return -
    */
    void ShowTooltip();
    
    /** CalculateResize
    *
    *   Function is used to calculate a new redius value for outline circle 
    *   when circle is resized with touch. The value is calculated based on 
    *   dragging start and end points.
    *
    *   @param - aStartPoint - A position where dragging started
    *   @param - aEndPoint - A position where dragging ended
    *   @return - TBool - ETrue if radius value is changed in the function
    */
    TBool CalculateResize( TPoint aStartPoint, TPoint aEndPoint );
    
//@}

/** @name Typedefs:*/
//@{
    /** UI state
	*/
    enum TRedEyeReductionStateState
    {
        //  Invalid
        ERedEyeReductionStateMin = 0,
        //  Move
        ERedEyeReductionStateMoveCrosshair,
        //  Resize
        ERedEyeReductionStateResizeSelection,
        //  Done - preparing to close
        ERedEyeReductionStateDone,
        //  Invalid
        ERedEyeReductionStateMax
    } iState;
//@}


/** @name Members:*/
//@{
    /// Editor view
    CAknView *					iEditorView;
    /// Cross hair icon
    CFbsBitmap *				iCrossHair;
    CFbsBitmap *				iCrossHairMask;
    /// Visible image rect
    const TRect*                iVisibleImageRectPrev;
    /// Viewport rect (the original image)
    const TRect*                iVisibleImageRect;
    /// Plug-in info
    CPluginInfo *               iItem;
    /// Navigation pane text
    TBuf<64>                    iNaviPaneText;
    /// Relative X position of the crosshair
    TReal                       iX;
    /// Relative Y position of the crosshair
    TReal                       iY;
    /// Relative selection radius
    TReal                       iR;
    /// Array of already selected red eye locations
    RArray<TRect>               iUndoArray;
	/// Periodic timer for dancing ants and fast key events
	CPeriodic *					iTimer;
    /// Dancing ants flag
	TBool						iAntFlag;
	/// Pressed key event code
	TInt 						iPressedKeyScanCode;
	/// Are event keys handled
	TBool						iHandleEventKeys;
    /// Number of ticks since timer start
    TInt						iTickCount;
    /// Multiplier to control the navigation movement speed
    TInt						iNaviStepMultiplier;
    /// Ready to render
    TBool                       iReadyToRender;
    ///	Red-eye removal region
    TRect						iRedEyeRect;
    /// If the flag is set, return empty paraeters
    TBool 						iDoNotUpdateParameters;
    /// Selection point on image, used when changing modes
    TPoint						iLockPoint;
    /// System parameters
    const CSystemParameters	* 	iSysPars;
	// Popup controller
	CAknInfoPopupNoteController* 	iPopupController;
	// Tooltip texts
	HBufC * 						iTooltipLocate;
	HBufC * 						iTooltipOutline;
	
    // Cursor can be dragged or not with pen
	TBool                       iCursorDragEnabled; 
	
    // Tapped position 
	TPoint                      iPointerPosition;

	// Feedback for screen touch:
#ifdef RD_TACTILE_FEEDBACK 
	MTouchFeedback* iTouchFeedBack;
#endif /* RD_TACTILE_FEEDBACK  */
//@}

};


#endif
