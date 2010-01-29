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
* Crop plugin control class header.
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
class CPeriodic;
class CAknInfoPopupNoteController;

#ifdef RD_TACTILE_FEEDBACK 
class MTouchFeedback;
#endif /* RD_TACTILE_FEEDBACK  */

/*	CLASS: CImageEditorCropControl
*
*   CImageEditorCropControl is a control for crop plug-in.
*
*/
class CImageEditorCropControl :		public CPreviewControlBase
{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param aRect - control rectangle
	*	@param aParent - pointer to window owning control
	*	@return pointer to created CImageEditorCropControl object
	*/
	static CImageEditorCropControl * NewL (
		const TRect &		aRect,
		CCoeControl	*		aParent
		);

	/** DancingAntsCallback
	*
	*	Static callback for rotating dancing ants.
	*
	*	@param aPtr - parameter given for TCallBack
	*	@return TInt
	*/
	static TInt DancingAntsCallback (TAny * aPtr);

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
	virtual ~CImageEditorCropControl ();

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

#ifdef DOUBLE_BUFFERED_CROP

    /*	SetImage
	*
	*	@see CImageEditorControlBase
	*
    */
    virtual void SetImageL (CFbsBitmap *	aBitmap);

#endif

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
	*	@param aWidth - image width
	*	@return - 
	*/
    void SetSystemParameters (const CSystemParameters * aSysPars);

	/*	SetCropModeL
	*
	*	Sets cropping mode and aspect ratio.
	*
	*	@param aRatio - crop aspect ratio
	*	@return - 
	*/
    void SetCropModeL (
        const TInt      aMode,
        const float     aRatio
        );

	/*	IsReadyToRender
	*
	*	Returns boolean value indicating when the parameters
    *   are set and filter is ready to be rendered
	*
	*	@param  - 
	*	@return TBool - is ready to render?
	*/
    TBool IsReadyToRender () const;


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
	CImageEditorCropControl ();

	/**	Draw
	*
	*	@see CImageEditorControlBase
	*
    */
	virtual void Draw (const TRect & aRect) const;

	/**	DrawDancingAnts
	*
	*	Drawing function for dancing ants crop rectangle.
	*
	*	@param aRect - control rectangle in need of refreshing 
	*	@return -
    */
	void DrawDancingAnts (const TRect & aRect) const;
    
    /**	DrawCursors
	*
	*	Drawing function for both of the cropping cursors.
	*
	*	@param aCropRect - cropping rectangle which indicates
	*                      top left and bottom right corners (i.e.
	*                      places for the cursors.)
	*	@return -
    */
    void DrawCursors ( const TRect& aCropRect ) const;
    
	/**	ComputeCropParams
	*
	*	Computes cropping parameters.
	*
	*	@param -
	*	@return -
	*/
	void ComputeCropParams ();

	/**	NaviDownL
	*
	*	Handles navi key down event.
	*
	*	@param -
	*	@return -
	*/
    void NaviDownL();

	/**	NaviUpL
	*
	*	Handles nave key up event.
	*
	*	@param -
	*	@return -
	*/
    void NaviUpL();

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

	/**	SetInitialPointsL
	*
	*	Sets initial crop points based on wanted cropping aspect 
    *   ratio (crop mode).
	*
	*	@param -
	*	@return -
	*/
    void SetInitialPointsL ();

	/**	StartDancingAntsTimer
	*
	*	Starts dancing ants timer. 
	*
	*	@param -
	*	@return -
	*/
	void StartDancingAntsTimer();

	/**	StartDancingAntsTimer
	*
	*	Starts dancing ants timer. 
	*
	*	@param -
	*	@return -
	*/
	void StartFastKeyTimer();

	/**	OnDancingAntsCallBack
	*
	*	Timer event handler for dancing ants.
	*
	*	@param -
	*	@return -
	*/
	void OnDancingAntsCallBack();

	/**	OnFastKeyCallBackL
	*
	*	Timer event handler for fast keys.
	*
	*	@param -
	*	@return -
	*/
	void OnFastKeyCallBackL();

	/**	CalculateMinCrop
	*
	*	Calculates minimum crop area
	*
	*	@param -
	*	@return -
	*/
    void CalculateMinCrop();

	/**	ComputePreservedULC
	*
	*	Computes aspect ratio preserver ULC coordinate.
	*
	*	@param -
	*	@return -
	*/
    void ComputePreservedULC();

	/**	ComputePreservedULC
	*
	*	Computes aspect ratio preserver LRC coordinate.
	*
	*	@param -
	*	@return -
	*/
    void ComputePreservedLRC();

	/**	ComputePreservedULR
	*
	*	Computes aspect ratio preserver ULR coordinate.
	*
	*	@param -
	*	@return -
	*/
    void ComputePreservedULR();

	/**	ComputePreservedLRR
	*
	*	Computes aspect ratio preserver LRR coordinate.
	*
	*	@param -
	*	@return -
	*/
    void ComputePreservedLRR();

#ifdef DOUBLE_BUFFERED_CROP

	/**	ClonePreviewBitmapL
	*
	*	Copy preview image data to buffer.
	*
	*	@param -
	*	@return -
	*/
	void ClonePreviewBitmapL();

	/**	DarkenUnselectedAreaL
	*
	*	Darkens the area outside crop region in the buffer.
	*
	*	@param -
	*	@return -
	*/
	void DarkenUnselectedAreaL();

#endif

	/**	UpdateCropRectangle
	*
	*	Update crop rectangle after cropping.
	*
	*	@param -
	*	@return -
	*/
	void UpdateCropRectangle();

	void StoreCropRelScreen();
	void RestoreCropRelScreen();
	void StoreCropRelImage();
	void RestoreCropRelImage();
	
	/**	SetTLPosition
	*
	*	Touch related function.
	*   Calculates the stylus movement according to the given parameters 
	*   and moves top-left corner respectively. 
	*
	*	@param  aOldPosition Previous pointer position.
	*           aNewPosition Current pointer position.
	*	@return -
	*/
	void SetTLPosition( TPoint aOldPosition, TPoint aNewPosition );
	
	/**	SetBRPosition
	*
	*	Touch related function.
	*   Calculates the stylus movement according to the given parameters 
	*   and moves bottom-right corner respectively. 
	*
	*	@param  aOldPosition Previous pointer position.
	*           aNewPosition Current pointer position.
	*	@return -
	*/
	void SetBRPosition( TPoint aOldPosition, TPoint aNewPosition );
    
    /**	IsCursorTapped
	*
	*   Touch related function.
	*   Checks if user has tapped inside the cursor area 
	*
	*	@param aTappedPosition A position value to check
	*	@return ETrue if cursor area contains tapped position, EFalse otherwise
	*/
    TBool IsCursorTapped( TPoint aTappedPosition ) const;
    
    /**	IsOppositeCornerTapped
	*
	*	Touch related function.
	*   Checks if user has tapped opposite corner to the current 
	*   cursor corner. 
	*    	
	*	@param aTappedPosition A position value to check
	*	@return ETrue if cursor area in opposite corner contains tapped position, 
	*           EFalse otherwise
	*/
    TBool IsOppositeCornerTapped( TPoint aTappedPosition ) const;
    
    /**	IsCropAreaTapped
	*
	*	Touch related function.
	*   Checks if user has tapped inside the crop rectangle
	*
	*	@param aTappedPosition A position value to check
	*	@return ETrue if current crop rectangle contains tapped position, 
	*           EFalse otherwise
	*/
    TBool IsCropAreaTapped( TPoint aTappedPosition ) const;
    
    /**	IsMinCropAreaTapped
	*
	*	Touch related function.
	*   Checks if user has tapped inside the minimum crop rectangle.
	*   Minimum crop rectangle is inside crop regtacgle. A smallest
	*   possible rectangle to crop.
	*
	*	@param aTappedPosition A position value to check
	*	@return ETrue if current minimum crop rectangle contains 
	                  tapped position, 
	*           EFalse otherwise
	*/
    TBool IsMinCropAreaTapped( TPoint aTappedPosition ) const;
           
    /**	IsCornerTapped
	*
	*	Touch related function.
	*   Checks if either Top-Left or Bottom-Right corner is tapped. 
	*
	*	@param aTappedPosition Pointer position to check
	*          aTappedCorner Possible tapped corner index is returned here.
	*	@return ETrue if either one of the corners is tapped, EFalse otherwise
	*/
    TBool IsCornerTapped( TPoint aTappedPosition, TInt& aTappedCorner ) const;
    
    /**	MoveCropArea
	*
	*	Touch related function.
	*   Calculates the movement according to the given parameters and moves 
	*   current crop rect respectively.   
	*
	*	@param  aOldPosition Previous pointer position.
	*           aNewPosition Current pointer position.
	*	@return -
	*/
    void MoveCropArea( TPoint aOldPosition, TPoint aNewPosition );
            
    /**	ShowTooltip
    *
    *	Show tooltip text on the screen. Tooltip text is set based 
    *   on the current state.
    *
    *	@param -
    *	@return -
    */
    void ShowTooltip();
    
//@}

/** @name Typedefs:*/
//@{
    enum TCropState
    {
        //  Invalid
        ECropStateMin = 0,
        //  ULC
        ECropStateFirst,
        //  LRC
        ECropStateSecond,
        //  Move
        ECropStateMove,
        //  Minimum crop
        ECropStateMinCrop,
        //  Invalid
        ECropStateMax
    } iState;

	enum TFastKeyTimerState
    {
        // Inactive
        ETimerInactive,
        // Started but has not yet run
        ETimerStarted,
        // Started and running
        ETimerRunning
    } iFastKeyTimerState;
    
    enum TCropRectCorner
        {
        ETLCorner,
        EBRCorner,
        EInvalidCorner
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

    /// Cross hair icon
    CFbsBitmap *				iCrossHair;
    CFbsBitmap *				iCrossHairMask;
    
    /// Secondary cross hair icon
    CFbsBitmap *                iSecondaryCrossHair;
    CFbsBitmap *                iSecondaryCrossHairMask;
    
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
    
    const CSystemParameters *   iSysPars;

    /// Relative crop rect coordinates
    float                       iULC;
    float                       iULR;
    float                       iLRC;
    float                       iLRR;

    /// Minimum crop parameters
    float                       iMinX;
    float                       iMinY;

    /// Wanted aspect ratio
    float                       iCropRatio;

    /// Crop mode
	TBool                       iIsCropModeManual;

    /// Is input image too small
	TBool                       iIsCropDisabled;

#ifdef DOUBLE_BUFFERED_CROP

	///	Double buffer bitmap
	CFbsBitmap *				iPrevBitmap;
	CFbsBitmap *				iBufBitmap;

#endif

	TRect						iOldCropRectPrev;
	TBool						iBackground;
	
	TBuf<256>					iParam;
	TInt						iCropX;
	TInt						iCropY;
	TInt						iCropW;
	TInt						iCropH;
	// Popup controller
	CAknInfoPopupNoteController* iPopupController;
	// Tooltip texts
	HBufC * 					iTooltipResize;
	HBufC * 					iTooltipMove;
	
	// Indicates whether touch dragging is allowed or not
	// (cursor or the whole crop rect)
	TBool                       iTouchDragEnabled;
	// Stores the position for previously tapped screen point (touch enabler)
	TPoint                      iTappedPosition;
	
	// Feedback for screen touch:
#ifdef RD_TACTILE_FEEDBACK 
	MTouchFeedback* iTouchFeedBack;
#endif /* RD_TACTILE_FEEDBACK  */	
//@}

};


#endif
