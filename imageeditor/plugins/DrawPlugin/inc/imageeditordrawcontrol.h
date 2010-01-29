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
* Draw plugin control class header.
*
*/

#ifndef IMAGEEDITORDRAWCONTROL_H
#define IMAGEEDITORDRAWCONTROL_H

//  INCLUDES
#include "PreviewControlBase.h"
#include "MTimerCallBack.h"

#include <coecntrl.h>
#include <ConeResLoader.h>

//  FORWARD DECLARATIONS
class CAknView;
class CFbsBitmap;
class CPluginInfo;
class CSystemParameters;
class CJPTimer;
class CAknInfoPopupNoteController; 
class CDrawPath;

/*	CLASS: CImageEditorDrawControl
*
*   CImageEditorDrawControl is a control for text insertion plug-in.
*
*/
class CImageEditorDrawControl :	public CPreviewControlBase,
                                public MTimerCallBack
{
public:

/** @name Methods:*/
//@{

	/** NewL factory method
	*
	*	@param aRect - control rectangle
	*	@param aParent - pointer to window owning control
	*	@return pointer to created CImageEditorDrawControl object
	*/
	static CImageEditorDrawControl* NewL(
		const TRect& aRect,
		CCoeControl* aParent
		);

	/** Destructor */
	~CImageEditorDrawControl();

protected: // From CImageEditorControlBase
	/** Second phase constructor
	*
	*	@see CImageEditorControlBase
    */
	void ConstructL(
		const TRect& aRect,
		CCoeControl* aParent
		);

	/*	Setter for view reference
	*
	*	@see CImageEditorControlBase
    */
    void SetView(CAknView* aView);

    /*	SetSelectedUiItemL
	*
	*	@see CImageEditorControlBase
    */
    void SetSelectedUiItemL(CPluginInfo* aItem);

	/** PrepareL
	*
	*	@see CImageEditorControlBase
    */
	void PrepareL();

    /*	OfferKeyEventL
	*
	*	@see CImageEditorControlBase
    */
    TKeyResponse OfferKeyEventL(
        const TKeyEvent& aKeyEvent,
        TEventCode aType
        );   
    
    /*	HandlePointerEventL
    *
    *	@see CImageEditorControlBase
    */
	void HandlePointerEventL( const TPointerEvent &aPointerEvent );
	
	/*	HandlePluginCommandL
	*
	*	@see CImageEditorControlBase
    */
    void HandlePluginCommandL(const TInt aCommand);

	/*	GetSoftkeyIndexL
	*
	*	@see CImageEditorControlBase
    */
    TInt GetSoftkeyIndexL();

    /*	GetContextMenuResourceId
	*
	*	@see CImageEditorControlBase
	*/
    TInt GetContextMenuResourceId();
    
	/*	GetDimmedMenuItems
	*
	*	@see CImageEditorControlBase
	*/
	TBitField GetDimmedMenuItems();

	/*	GetNaviPaneTextL
	*
	*	@see CImageEditorControlBase
	*/
	TPtrC GetNaviPaneTextL(
		TBool& aLeftNaviPaneScrollButtonVisibile, 
		TBool& aRightNaviPaneScrollButtonVisible);
public: 
	/*	GetParam
	*
	*	Gets pointer to the parameter struct.
	*	@return - pointer to MIA parameter struct
	*/
    TDesC& GetParam();

	/*	SetSysteParameters
	*
	*	Sets reference to system parameters to plug-in.
	*	@param aSysPars - System parameters instance
	*/
    void SetSystemParameters(const CSystemParameters* aSysPars);
    
    /*  IsReadyToRender
    *
    *   @return - ETrue when control is ready render
    */
    TBool IsReadyToRender() const;    

protected:

    /*	GetHelpContext
	*
	*	@see CCoeControl
    */
	virtual void GetHelpContext(TCoeHelpContext& aContext) const;

    /*	TimerCallBack
	*
	*	@see MTimerCallBack
    */
	void TimerCallBack();

//@}


protected:

/** @name Methods:*/
//@{
	/*	SizeChanged
	*
	*	@see CImageEditorControlBase
    */
	void SizeChanged();

//@}

/** @name Members:*/
//@{

//@}

private:

	CImageEditorDrawControl ();

	/**	Draw
	*
	*	@see CImageEditorControlBase
	*
    */
	virtual void Draw (const TRect & aRect) const;

private: // Implementation	
    void NaviDown();
    void NaviUp();
    void NaviRight();
    void NaviLeft();
    void SelectSizeL();
    void StoreTempParams();
    void RestoreTempParams();
    void RenderTextImageL();
    void LoadIndicatorL ( TInt aBitmapInd, TInt aMaskInd ); 
    TPoint ComputeIndicatorPosition() const;
	void StoreParameters(TBool aLastItem, TBool aDone);
	void StorePosAndScaleRelScreen();
	void RestorePosAndScaleRelScreen();
	void StorePosAndScaleRelImage();
	void RestorePosAndScaleRelImage();
	void ClipPosition();
    void SetPositionOnImage( TPoint aPointedPosition );
    TPoint GetPositionOnView();
    void ShowTooltip();
    TSize ScaledLineSize( TSize aOriginalSize ) const;

//@}

/** @name Members:*/
//@{

    /// Ref: to Editor view
    CAknView* iEditorView;

    /// Source image size
    const CSystemParameters* iSysPars;

    /// Ref: to Plug-in info
    CPluginInfo* iItem;

    /// Own: Navigation pane text
    HBufC* iNaviPaneText;

    /// X position
    TInt iX;
    /// Y position
    TInt iY;
    /// Color
    TRgb iRgb;

    /// Timer for fast key events
	CJPTimer* iTimer;
	TUint32 iKeyCode;

	/// Pressed key event code
	TInt iPressedKeyScanCode;

    /// Number of ticks since timer start
    TInt iTickCount;

    /// Multiplier to control the navigation movement speed
    TInt iNaviStepMultiplier;

    /// Showing Ok options menu, where some items are dimmed
    TBool iDisplayingOkOptionsMenu;

    /// Own: State indicator glyph
    CFbsBitmap* iIndicator;
    CFbsBitmap* iIndicatorMask;

    /// Ready to render
    TBool iReadyToRender;
    
    /// Parameter
    TBuf<256> iParam;

	// Popup controller
	CAknInfoPopupNoteController* iPopupController;
	// Tooltip texts
	HBufC* iTooltipResize;
	
	// Previous pen position. Make touch rotating and resizing possible
	TPoint iPointerPosition;
	// Multiplier to control the rotating speed by touch
	TInt iTouchAngleMultiplier;
    
    /// Resource reader
	RConeResourceLoader iResLoader;
	/// Line size
	TSize iSize;
	/// Own: drawn paths
	RPointerArray<CDrawPath> iPaths;
       
//@}

};


#endif
