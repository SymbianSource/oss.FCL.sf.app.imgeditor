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
* Text plugin control class header.
*
*/



#ifndef IMAGEEDITORTEXTCONTROL_H
#define IMAGEEDITORTEXTCONTROL_H

//  INCLUDES
#include <coecntrl.h>

#include "PreviewControlBase.h"
#include "MTimerCallBack.h"
#include <ConeResLoader.h>

//  FORWARD DECLARATIONS
class CAknView;
class CFbsBitmap;
class CPluginInfo;
class CSystemParameters;
class CJPTimer;
class CAknInfoPopupNoteController; 

#ifdef RD_TACTILE_FEEDBACK 
class MTouchFeedback;
#endif /* RD_TACTILE_FEEDBACK  */

/*	CLASS: CImageEditorTextControl
*
*   CImageEditorTextControl is a control for text insertion plug-in.
*
*/
class CImageEditorTextControl :		public CPreviewControlBase,
                                   	public MTimerCallBack

{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param aRect - control rectangle
	*	@param aParent - pointer to window owning control
	*	@return pointer to created CImageEditorTextControl object
	*/
	static CImageEditorTextControl * NewL (
		const TRect &		aRect,
		CCoeControl	*		aParent
		);

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	virtual ~CImageEditorTextControl ();

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
    
    /*	HandlePointerEventL
    *
    *	@see CImageEditorControlBase
    *
    */
	virtual void HandlePointerEventL( const TPointerEvent &aPointerEvent );
	
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

    /*	GetContextMenuResourceId
	*
	*	@see CImageEditorControlBase
	*/
    virtual TInt GetContextMenuResourceId();
    
	/*	GetDimmedMenuItems
	*
	*	@see CImageEditorControlBase
	*/
	virtual TBitField GetDimmedMenuItems();

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

	/*	SetSysteParameters
	*
	*	Sets reference to system parameters to plug-in.
	*
	*	@param aSysPars - System parameters instance
	*	@return - 
	*/
    void SetSystemParameters (const CSystemParameters * aSysPars);

	/*	SetTextL 
	*
	*	Sets user inputted text to text plug-in.
	*
	*	@param aText - text data
	*	@return - 
	*/
    void SetTextL (const TDesC & aText);

    /*	GetHelpContext
	*
	*	@see CCoeControl
	*
    */
	virtual void GetHelpContext(TCoeHelpContext& aContext) const;

    /*	IsReadyToRender
	*
	*	@param -
	*	@return -
	*/
	TBool IsReadyToRender () const;

    /*	TimerCallBack
	*
	*	@see MTimerCallBack
	*
    */
	virtual void TimerCallBack();

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
	CImageEditorTextControl ();

	/**	Draw
	*
	*	@see CImageEditorControlBase
	*
    */
	virtual void Draw (const TRect & aRect) const;

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
	*	Handles nave key left event.
	*
	*	@param -
	*	@return -
	*/
    void NaviLeft();

	/**	ToMoveStateL
	*
    *   Handles transition to Move state.
	*
	*	@param -
	*	@return -
	*/
    void ToMoveStateL();

	/**	ToResizeStateL
	*
    *   Handles transition to Resize state.
	*
	*	@param -
	*	@return -
	*/
    void ToResizeStateL();

	/**	ToRotateStateL
	*
    *   Handles transition to Rotate state.
	*
	*	@param -
	*	@return -
	*/
    void ToRotateStateL();

	/**	ToMainStateL
	*
    *   Handles transition to Main state.
	*
	*	@param -
	*	@return -
	*/
    void ToMainStateL();

    /**	StoreTempParams
	*
    *   Store all parameters to temp variables.
	*
	*	@param -
	*	@return -
	*/
    void StoreTempParams();

    /**	RestoreTempParams
	*
    *   Restore parameters from temp variables.
	*
	*	@param -
	*	@return -
	*/
    void RestoreTempParams();

	/**	RenderTextImageL
	*
    *   Renders the text into a EGray2 binary bitmap and copies the contents
    *   of the bitmap to a binary buffer.
	*
	*	@param -
	*	@return -
	*/
    void RenderTextImageL();

	/**	LoadIndicatorL
	*
	*	Loads indicator.
	*
	*	@param aBitmapInd - indicator bitmap MBM index
	*	@param aMaskInd - indicator mask MBM index
	*	@return -
	*/
    void LoadIndicatorL (
        TInt    aBitmapInd,
        TInt    aMaskInd
        ); 

	/**	ComputeIndicatorPosition
	*
	*	Computes indicator position.
	*
	*	@param -
	*	@return -
	*/
    TPoint ComputeIndicatorPosition() const;

	/**	StoreParameters
	*
	*	Stores parameters to command line.
	*
	*	@param -
	*	@return -
	*/
	void StoreParameters();

	/**	StorePosAndScaleRelScreen
	*
	*	Stores text position and scale relative to
	*	screen.
	*
	*	@param -
	*	@return -
	*/
	void StorePosAndScaleRelScreen();

	/**	RestorePosAndScaleRelScreen
	*
	*	Restores text position and scale relative to
	*	screen.
	*
	*	@param -
	*	@return -
	*/
	void RestorePosAndScaleRelScreen();

	/**	StorePosAndScaleRelImage
	*
	*	Stores position and scale relative to image.
	*
	*	@param -
	*	@return -
	*/
	void StorePosAndScaleRelImage();

	/**	StoreParameters
	*
	*	Restores position and scale relative to image.
	*
	*	@param -
	*	@return -
	*/
	void RestorePosAndScaleRelImage();

	/**	ClipPosition
	*
	*	Clips clipart position after screen mode change.
	*
	*	@param -
	*	@return -
	*/
	void ClipPosition();
    
    /**	SetTextPosition
	*
	*	Sets current Text to a new position
	*
	*	@param - aPointedPosition Middle point position on the screen where 
	*                             text should should be moved
	*	@return -
	*/
    void SetTextPosition( TPoint aPointedPosition );
    
    /** ShowTooltip
    *
    *   Show tooltip text on the screen. Tooltip text is set based 
    *   on the current state.
    *
    *   @param -
    *   @return -
    */
    void ShowTooltip();
    
    /** CalculateRotation
    *
    *   Function is used to calculate rotation when text is rotated
    *   with touch. Needed rotation is calculated based on dragging start
    *   and end points. This function also changes rotation variables after
    *   calculation.
    *
    *   @param - aStartPoint - A position where dragging started
    *   @param - aEndPoint - A position where dragging ended
    *   @return - ETrue if rotate value is changed in the function
    */
    TBool CalculateRotation( TPoint aStartPoint, TPoint aEndPoint );
    
    /** CalculateResize
    *
    *   Function is used to calculate new scale value when text is resized
    *   with touch. The value is calculated based on dragging start
    *   and end points. This function also changes scale  member variable 
    *   after calculation.
    *
    *   @param - aStartPoint - A position where dragging started
    *   @param - aEndPoint - A position where dragging ended
    *   @return - TBool - ETrue if scale value is changed in the function
    */
    TBool CalculateResize( TPoint aStartPoint, TPoint aEndPoint );
    
//@}

/** @name Typedefs:*/
//@{
    enum TInsertTextState
    {
        //  Invalid
        EInsertTextStateMin = 0,
        //  Plugin main view first time
        EInsertTextStateFirst,
        //  Plugin main view
        EInsertTextStateMain,
        //  Move
        EInsertTextStateMove,
        //  Rotate
        EInsertTextStateRotate,
        //  Resize
        EInsertTextStateResize,
        //  Invalid
        ECropStateMax
    } iState;
//@}


/** @name Members:*/
//@{
    // Parent control
    CCoeControl*                iParent;

    /// Editor view
    CAknView *					iEditorView;

    /// Source image size
    const CSystemParameters *   iSysPars;

    /// Plug-in info
    CPluginInfo *               iItem;

    /// Navigation pane text
    TBuf<64>                    iNaviPaneText;

    /// User inputted text
    TBuf<256>                   iText;

    /// Text x position
    TInt						iX;
    /// Text y position
    TInt						iY;
	/// Text scale	
    TInt                        iScale;
    /// Text scale minimum
    TInt						iScaleMin;
    /// Text scale maximum
    TInt						iScaleMax;
    /// Text angle 
    TInt						iAngle;
    /// Text color
    TRgb						iRgb;

    /// Temp parameters
    TInt                        iTempX;
    TInt                        iTempY;
    TInt                        iTempScale;
    TInt                        iTempAngle;

    TReal						iScaleOld;
    TReal						iPosXOld;
    TReal 						iPosYOld;

    /// Timer for fast key events
	CJPTimer *					iTimer;
	TUint32						iKeyCode;

	/// Pressed key event code
	TInt 						iPressedKeyScanCode;

    /// Number of ticks since timer start
    TInt						iTickCount;

    /// Multiplier to control the navigation movement speed
    TInt						iNaviStepMultiplier;

    /// Showing Ok options menu, where some items are dimmed
    TBool                       iDisplayingOkOptionsMenu;

    /// State indicator glyph
    CFbsBitmap *                iIndicator;
    CFbsBitmap *                iIndicatorMask;

    /// Ready to render
    TBool						iReadyToRender;
    
    /// Parameter
    TBuf<256>					iParam;

	// Popup controller
	CAknInfoPopupNoteController* iPopupController;
	// Tooltip texts
	HBufC * 					iTooltipResize;
	HBufC * 					iTooltipMove;	
	HBufC * 					iTooltipRotate;	
	
	// Previous pen position. Make touch rotating and resizing possible
	TPoint                      iPointerPosition;
    
    /// Resource reader
	RConeResourceLoader         iResLoader;

	// Feedback for screen touch:
#ifdef RD_TACTILE_FEEDBACK 
	MTouchFeedback* iTouchFeedBack;
#endif /* RD_TACTILE_FEEDBACK  */	
//@}

};


#endif
