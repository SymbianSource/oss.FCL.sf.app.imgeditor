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
#include "AnimationAO.h"
#include "CImageLabel.h"
#include "CustomerComponet.h"
#include "RollAO.h"

//  FORWARD DECLARATIONS
class CAknView;
class CFbsBitmap;
class CPluginInfo;
class CSystemParameters;
class CJPTimer;
class CAknInfoPopupNoteController;
class CDrawPath;
class CImageLabel;
class CAnimationAO;

/*	CLASS: CImageEditorDrawControl
 *
 *   CImageEditorDrawControl is a control for text insertion plug-in.
 *
 */
class CImageEditorDrawControl : public CPreviewControlBase,
		public MTimerCallBack,
		public MAnimationObserver,
		public MHandlePointerObserver,
		public MRollObserver
	{
	static const int DRAG_LENGTH = 5;
	enum TMainMenuLayout
		{
		EMainMenuHorizontal, EMainMenuVertical
		};
public:

	/** @name Methods:*/
	//@{

	/** NewL factory method
	 *
	 *	@param aRect - control rectangle
	 *	@param aParent - pointer to window owning control
	 *	@return pointer to created CImageEditorDrawControl object
	 */
	static CImageEditorDrawControl* NewL(const TRect& aRect,
			CCoeControl* aParent, RPointerArray<CDrawPath>& aPaths,TRgb& aRgb, TSize& aSize,TBool &aDisplayTool);

	/** Destructor */
	~CImageEditorDrawControl();

protected:
	// From CImageEditorControlBase
	/** Second phase constructor
	 *
	 *	@see CImageEditorControlBase
	 */
	void ConstructL(const TRect& aRect, CCoeControl* aParent);

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
	TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

	/*	HandlePointerEventL
	 *
	 *	@see CImageEditorControlBase
	 */
	void HandlePointerEventL(const TPointerEvent &aPointerEvent);

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
	TPtrC GetNaviPaneTextL(TBool& aLeftNaviPaneScrollButtonVisibile,
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

	/*MakeVisibleLable
	 * 
	 * make the controller visible
	 * @param aFlag - ETrue,make the tool bar visible,otherwise.
	 */
	void MakeVisibleLable(TBool aFlag);
    
	/**StartTheAnimation
	 * 
	 * start the auto-moving active object for tool bar. 
	 */
	void StartTheAnimation();

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
	/** Default constructor */
	CImageEditorDrawControl(RPointerArray<CDrawPath>& aPaths,TRgb& aRgb, TSize& aSize,TBool &aDisplayTool);

	/**	Draw
	 *
	 *	@see CImageEditorControlBase
	 *
	 */
	virtual void Draw(const TRect & aRect) const;

protected:
	/**CountComponentControls
	 * 
	 * @see CImageEditorControlBase
	 */
	TInt CountComponentControls() const;
	
	/**ComponentControl
	 * 
	 * 
	 * @see CImageEditorControlBase
	 */
	CCoeControl* ComponentControl(TInt aIndex) const;

private:
	// Implementation	
	void NaviDown();
	void NaviUp();
	void NaviRight();
	void NaviLeft();
	void SelectSizeL();
	void StoreTempParams();
	void RestoreTempParams();
	void RenderTextImageL();
	void LoadIndicatorL(TInt aBitmapInd, TInt aMaskInd);
	TPoint ComputeIndicatorPosition() const;
	void StoreParameters(TBool aLastItem, TBool aDone);
	void StorePosAndScaleRelScreen();
	void RestorePosAndScaleRelScreen();
	void StorePosAndScaleRelImage();
	void RestorePosAndScaleRelImage();
	void ClipPosition();
	void SetPositionOnImage(TPoint aPointedPosition);
	TPoint GetPositionOnView();
	void ShowTooltip();
	TSize ScaledLineSize(TSize aOriginalSize) const;
	
public:
	/**Notify
	 * 
	 * handle the event by Active object 
	 */
	virtual void Notify();
	
	/**HandlePointerNotify
	 * 
	 * handle the press icon event 
	 * @param aComponent - the icon control of tool bar
	 */
	virtual void HandlePointerNotify(CCoeControl* aComponent);
	
	/**Roll
	 * 
	 * the fuction for AO to move the tool bar
	 */
	virtual void Roll();

private:
	/**InitializeMainMenu
	 * 
	 * initialize the tool bar 
	 * @param aMainMenuLayout - horizon or veritical layout of the tool bar 
	 */
	void InitializeMainMenu(TMainMenuLayout aMainMenuLayout);
	
	/**CalculateEffectiveOffset
	 * 
	 * calculate vaild offset for tool bar
	 */
	void CalculateEffectiveOffset();

	/**HandleMainMenuButtonUp
	 * 
	 * handle the up event on tool bar 
	 * @param aPoint - the position on screen 
	 */
	void HandleMainMenuButtonUp(TPoint aPoint);
	
	/**HandleMainMenuButtonDrag
	 * 
	 * handle the drag event on tool bar
	 * @param aPoint - the position on screen 
	 */
	void HandleMainMenuButtonDrag(TPoint aPoint);
	
private:
	/**UndoDrawPath
	 * 
	 * undo the drawed lines on screen 
	 */
	void UndoDrawPath();
	
	/**RedoDrawPath
	 * 
	 * redo the undo lines on screen 
	 */
	void RedoDrawPath();
	
private:
	
	/** @name Members:*/


	// Ref: to Editor view
	CAknView* iEditorView;

	//Source image size
	const CSystemParameters* iSysPars;

	// Ref: to Plug-in info
	CPluginInfo* iItem;

	//Own: Navigation pane text
	HBufC* iNaviPaneText;

	// X position
	TInt iX;
	// Y position
	TInt iY;

	// Timer for fast key events
	CJPTimer* iTimer;
	TUint32 iKeyCode;

	// Pressed key event code
	TInt iPressedKeyScanCode;

	// Number of ticks since timer start
	TInt iTickCount;

	// Multiplier to control the navigation movement speed
	TInt iNaviStepMultiplier;

	// Showing Ok options menu, where some items are dimmed
	TBool iDisplayingOkOptionsMenu;

	// Own: State indicator glyph
	CFbsBitmap* iIndicator;
	CFbsBitmap* iIndicatorMask;

	// Ready to render
	TBool iReadyToRender;

	// Parameter
	TBuf<256> iParam;

	// Popup controller
	CAknInfoPopupNoteController* iPopupController;
	// Tooltip texts
	HBufC* iTooltipResize;

	// Previous pen position. Make touch rotating and resizing possible
	TPoint iPointerPosition;
	// Multiplier to control the rotating speed by touch
	TInt iTouchAngleMultiplier;

	// Resource reader
	RConeResourceLoader iResLoader;
	// Line size
	TSize& iSize;
	// Color
	TRgb& iRgb;
	// Own: drawn paths
	RPointerArray<CDrawPath> &iPaths;
	//bitmap device
	CFbsBitmapDevice* iBitmapDev;
	CGraphicsContext* iGc;


private:
	//Get and set tool bar status display or hidden
    void SetToolBarStatus();
    //The self define component array for tool bar.
	RPointerArray<CImageLabel> iLabels;
    //The tool bar area.
	TRect iLoopMenuRect;
	//Mark the button down event
	TPoint iButtonDownPoint;
	//Mark the tool bar start drag point.
	TPoint iDragStartPoint;
	//Prepare drag point.
	TPoint iDragPrePoint;
	//Mark Button up point for stop drag event.
	TPoint iDragUpPoint;
    //The total of tool bar compoent 
	TInt iTotalComponent;
	//The param for drag tool bar event.
	TInt iDragOffset;
	//The param for drag tool bar event.
	TInt iStartSpeed;
    //The Active Object for  drag tool bar.
	CAnimationAO* iAnimationAO;
	//The Active object for tool bar Automatic moving.
	CRollAO* iRollAo;
    //whether the button down point in the tool bar area.
	TBool iPointStartInLoopRect;
	//Mark for iAnimationAO;
	TBool iDragIsStart;
	//Current screen layout.
	TMainMenuLayout iMainMenuLayout;


private:
	
	RPointerArray<CDrawPath> iUndoPaths;
	TBool iUndo;
	TBool iRedo;
	TBool iCanRedo;
    //The tool bar Automatic moving direction.
	TBool iRollDirection;
private:
	//The compoent use for display and hidden tool bar.
	CCustomerComponet *iCustomComponet;
	//whether the customer compoent was clicked. 
	TBool iCustomComponetClick;
	//Mark the customr compoent last pointer event.
	TBool iLastPointEventType;
	//Mark the tool bar Automatic moving.
	TBool iRollflag;
	//Mark the tool bar status display or hidden.
	TBool &iDisplayTool;
	//Mark in the sizechanged function.
	TBool iSizeChangeFlag;
	//Mark last tool bar event.
	TBool iFlagDragToolBar;
	};

#endif
