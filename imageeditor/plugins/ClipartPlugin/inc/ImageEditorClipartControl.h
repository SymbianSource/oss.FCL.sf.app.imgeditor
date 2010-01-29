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
* Control class for Clipart plugin.
*
*/


#ifndef IMAGEEDITORCROPCONTROL_H
#define IMAGEEDITORCROPCONTROL_H

//  INCLUDES
#include <coecntrl.h>
#include <eiklbo.h>
#include <coecobs.h>
#include <ConeResLoader.h>

#include "PreviewControlBase.h"
#include "MTimerCallBack.h"
#include "ClipartSelectionDialog.h"

//  FORWARD DECLARATIONS
class CAknView;
class CFbsBitmap;
class CPluginInfo;
class CSystemParameters;
class CClipartSelectionGrid;
class CJPTimer;
class CAknInfoPopupNoteController; 

#ifdef RD_TACTILE_FEEDBACK 
class MTouchFeedback;
#endif /* RD_TACTILE_FEEDBACK  */

/*	CLASS: CImageEditorClipartControl
*
*   CImageEditorClipartControl is a control for clipart insertion plug-in.
*
*/
class CImageEditorClipartControl : public CPreviewControlBase,
                                   public MCoeControlObserver,  
                                   public MEikListBoxObserver,
                                   public MTimerCallBack
                                   
{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param aRect - control rectangle
	*	@param aParent - pointer to window owning control
	*	@return pointer to created CImageEditorClipartControl object
	*/
	static CImageEditorClipartControl * NewL (
		const TRect &		aRect,
		CCoeControl	*		aParent
		);

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	virtual ~CImageEditorClipartControl ();

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

    /*	SetSelectedUiItem
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

	/*	HandleListBoxEventL
	*
	*	@see CImageEditorControlBase
	*
    */
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

	/*	HandleControlEventL
	*
	*	@see CImageEditorControlBase
	*
    */
    void HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType);

	/*	SetBitmapL
	*
	*	Sets clip-art bitmap to filter paramerters object
	*
    */
    void SetBitmapL();

	/*	SetBusy
	*
	*	@see CImageEditorControlBase
	*
    */
    void SetBusy();

    /*	ResetBusy
	*
	*	@see CImageEditorControlBase
	*
    */
    void ResetBusy();
    
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
	CImageEditorClipartControl ();

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

	/**	ToRotateState
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

    /**	SelectClipartL
	*
	*	Select clipart and set to the parameter object
	*
	*	@param -
	*	@return -
	*/
    void SelectClipartL();

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
	*	Stores clipart position and scale relative to
	*	screen.
	*
	*	@param -
	*	@return -
	*/
	void StorePosAndScaleRelScreen();

	/**	RestorePosAndScaleRelScreen
	*
	*	Restores clipart position and scale relative to
	*	screen.
	*
	*	@param -
	*	@return -
	*/
	void RestorePosAndScaleRelScreen();

	/**	StorePosAndScaleRelImage
	*
	*	Stores scale and pan
	*
	*	@param -
	*	@return -
	*/
	void StorePosAndScaleRelImage();

	/**	RestorePosAndScaleRelImage
	*
	*	Stores scale and pan
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
    
    /**	SetClipPosition
	*
	*	Sets current clipart to a new position
	*
	*	@param - aPointedPosition Middle point position on the screen where 
	*                             clip art should should be moved
	*	@return -
	*/
    void SetClipPosition( TPoint aPointedPosition );
    
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
    *   Function is used to calculate a new scale value when clipart is resized
    *   with touch. The value is calculated based on dragging start
    *   and end points. This function also changes scale variable member after
    *   calculation.
    *
    *   @param - aStartPoint - A position where dragging started
    *   @param - aEndPoint - A position where dragging ended
    *   @return - TBool - ETrue if scale value is changed in the function
    */
    TBool CalculateResize( TPoint aStartPoint, TPoint aEndPoint );
    
    /** CalculateRotation
    *
    *   Function is used to calculate rotation when clipart is rotated
    *   with touch. Needed rotation is calculated based on dragging start
    *   and end points. This function also changes rotation variables after
    *   calculation.
    *
    *   @param - aStartPoint - A position where dragging started
    *   @param - aEndPoint - A position where dragging ended
    *   @return - TBool - ETrue if rotate value is changed in the function
    */
    TBool CalculateRotation( TPoint aStartPoint, TPoint aEndPoint );
    
//@}

/** @name Typedefs:*/
//@{
    enum TInsertClipartState
    {
        //  Invalid
        EInsertClipartStateMin      = 0,
        //  Select clipart
        EInsertClipartStateSelect   = 1<<1,
        //  Convert
        EInsertClipartStateConvert  = 1<<2,
        //  First time in main
        EInsertClipartStateFirst    = 1<<3,
        //  Main view
        EInsertClipartStateMain     = 1<<4,
        //  Move
        EInsertClipartStateMove     = 1<<5,
        //  Rotate
        EInsertClipartStateRotate   = 1<<6,
        //  Resize
        EInsertClipartStateResize   = 1<<7,
        //  Invalid
        EInsertClipartStateMax      = 1<<8
    } iState;

//@}


/** @name Members:*/
//@{

    /// Editor view
    CAknView *					iEditorView;

    /// System parameters
    const CSystemParameters*    iSysPars;

    /// Plug-in info
    CPluginInfo *               iItem;

    /// Navigation pane text
    TBuf<64>                    iNaviPaneText;
    
    /// Clipart x position
    TInt						iX;
    /// Clipart y position
    TInt						iY;
	/// Clipart scale	
    TInt                        iScale;
    /// Clipart scale minimum
    TInt						iScaleMin;
    /// Clipart scale maximum
    TInt						iScaleMax;
    /// Clipart angle 
    TInt						iAngle;
	///	Do we scale clipart to maximum height
	TBool						iScaleToHeight;
    
    TInt						iScaleOld;
    TInt						iPosXOld;
    TInt						iPosYOld;
    
	/// Temp params
    TInt						iTempX;
    TInt						iTempY;
    TInt                        iTempScale;
    TInt                        iTempAngle;

    /// Popup list
    CClipartSelectionDialog*    iPopupList;
	
	/// Clipart filename
	TFileName					iClipartFileName;
	
	/// Clipart filename index
	TInt						iClipartFileNameIndex;
	
	/// Resource reader
	RConeResourceLoader         iResLoader;

    /// Timer for fast key events
	CJPTimer *					iTimer;
	TUint32						iKeyCode;
    
    /// Number of ticks since timer start
    TInt						iTickCount;

    /// Multiplier to control the navigation movement speed
    TInt						iNaviStepMultiplier;

    /// Showing Ok options menu, where some items are dimmed
    TBool                       iDisplayingOkOptionsMenu;

    /// Plug-in is ready to render the image
    TBool                       iReadyToRender;

    /// State indicator glyph
    CFbsBitmap *                iIndicator;
    CFbsBitmap *                iIndicatorMask;

	/// Clipart parameter
	TBuf<256>					iParam;
	

	// Popup controller
	CAknInfoPopupNoteController* iPopupController;
	// Tooltip texts
	HBufC * 					iTooltipResize;
	HBufC * 					iTooltipMove;	
	HBufC * 					iTooltipRotate;	
	
	// Previous pen position. Make touch rotating and resizing possible
	TPoint                      iPointerPosition;

	// Feedback for screen touch:
#ifdef RD_TACTILE_FEEDBACK 
	MTouchFeedback* iTouchFeedBack;
#endif /* RD_TACTILE_FEEDBACK  */
		
//@}

};


#endif
