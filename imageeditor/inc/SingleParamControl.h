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
*
*/


#ifndef SINGLEPARAMCONTROL_H
#define SINGLEPARAMCONTROL_H

//  INCLUDES
#include <eiklabel.h>
#include "PreviewControlBase.h"

//  FORWARD DECLARATIONS
class CCoeControl;
class CAknView;
class MSingleParControlObserver;
class CPluginInfo;
class CHorizontalSlider;

#ifdef RD_TACTILE_FEEDBACK 
class MTouchFeedback;
#endif /* RD_TACTILE_FEEDBACK  */

/*	CLASS: CSingleParamControl
 *
 *   CSingleParamControl represents a single parameter adjustment UI control,
 *   that can be used by any of the plug-ins just by defining a UI type to
 *   resource file.
 */
class CSingleParamControl : public CPreviewControlBase
	{
public:

	/** @name Methods:*/
	//@{

	/** NewL factory method, pops cleanupstack
	 *
	 *   @param aRect - control rectangle
	 *	 @param aParent - pointer to window owning control
	 *   @param aActionOnButtonRelease - To indicate if rendering should happen only when
	 *   the button1 is released, i.e. when dragging has stopped. Default value is false.
	 *   @return - pointer to the newly created CSingleParamControl instance
	 */
	IMPORT_C static CSingleParamControl * NewL (
			const TRect & aRect,
			CCoeControl * aParent,
			TBool aActionOnButtonRelease = EFalse
	);

	/** Default constructor
	 *
	 *	@param -
	 *	@return -
	 */
	IMPORT_C CSingleParamControl ();

	/** Second phase constructor
	 *
	 *   @see CImageEditorControlBase
	 */
	IMPORT_C virtual void ConstructL (
			const TRect & aRect,
			CCoeControl * aParent,
			TBool aActionOnButtonRelease
	);

	/** Destructor
	 *
	 *	@param -
	 *	@return -
	 */
	IMPORT_C virtual ~CSingleParamControl ();

	/*	SetView
	 *
	 *   @see CImageEditorControlBase
	 */
	IMPORT_C virtual void SetView (CAknView * aView);

	/*	SetSelectedUiItemL
	 *
	 *   @see CImageEditorControlBase
	 */
	IMPORT_C virtual void SetSelectedUiItemL (CPluginInfo * aItem);

	/*	OfferKeyEventL
	 *
	 *   @see CImageEditorControlBase
	 */
	IMPORT_C virtual TKeyResponse OfferKeyEventL (
			const TKeyEvent & aKeyEvent,
			TEventCode aType
	);

	/**	HandlePointerEventL	
	 *
	 *	@see CCoeControl
	 */
	IMPORT_C virtual void HandlePointerEventL( const TPointerEvent &aPointerEvent );

	/*	SetParObserver 
	 *
	 *   Sets the MSingleParControlObserver for the control.
	 *
	 *	@param aObserver - parameter observer
	 *	@return -
	 */
	IMPORT_C void SetParObserver (MSingleParControlObserver * aObserver);

	/*	HandlePluginCommandL
	 *
	 *   @see CImageEditorControlBase
	 */
	IMPORT_C virtual void HandlePluginCommandL (const TInt aCommand);

	/*	GetSoftkeyIndexL
	 *
	 *   @see CImageEditorControlBase
	 */
	IMPORT_C virtual TInt GetSoftkeyIndexL();

	/*	GetNaviPaneTextL
	 *
	 *	@see CImageEditorControlBase
	 */
	IMPORT_C virtual TPtrC GetNaviPaneTextL (
			TBool& aLeftNaviPaneScrollButtonVisibile,
			TBool& aRightNaviPaneScrollButtonVisible);

	// these functions are for adjusting the slider
	IMPORT_C void SetSliderMinimumAndMaximum(TInt aMin, TInt aMax);
	IMPORT_C void SetSliderPosition(TInt aPosition);
	IMPORT_C void SetSliderStep(TUint aStep);
	IMPORT_C void SetSliderStepAmount(TUint8 aAmount);
	IMPORT_C void SetIcon(CEikImage* aIcon);
	IMPORT_C void SetCaption(const TDesC& aText);
	IMPORT_C TInt Position() const;

	// from CCoeControl
	IMPORT_C TInt CountComponentControls() const;
	IMPORT_C CCoeControl* ComponentControl(TInt aIndex) const;
	IMPORT_C void SizeChanged();

	//@}

protected:

private:

	/** @name Methods:*/
	//@{

	/*	Draw
	 *
	 *   @see CImageEditorControlBase
	 */
	IMPORT_C virtual void Draw (const TRect & aRect) const;

	//@}
	/// Parameter adjustment observer
	MSingleParControlObserver * iParObserver;

	IMPORT_C void CountImageSizesAndPositions();
	IMPORT_C void MoveSlider(TInt aSteps);

protected:

	/// Plug-in info
	CPluginInfo* iItem;

	/// View reference
	CAknView* iEditorView;

	/// drag flag
	TBool iDragging;
	
	/// should rendering happen only when the button is released
	TBool iActionOnButtonRelease;

	/// touch area
	TRect iTouchRect;

	/// minimum value of the slider
	TInt iMinimumValue;

	/// maximum value of the slider
	TInt iMaximumValue;

	/// step value
	TUint iStep;
	TUint iStepInPixels;

	/// number of steps
	TUint8 iNumberOfSteps;

	/// current position
	TInt iPosition;

	/// popup graphics
	RPointerArray<CEikImage> iBorders;

	/// scrollbar graphics
	RPointerArray<CEikImage> iScrollBar;

	/// caption text
	CEikLabel* iText;

	/// icon graphic, owned
	CEikImage* iIcon;

	/// use selection graphic
	TBool iMarkerPressed;

	// Feedback for screen touch:
#ifdef RD_TACTILE_FEEDBACK 
	MTouchFeedback* iTouchFeedBack;
#endif /* RD_TACTILE_FEEDBACK  */

	};

#endif

// End of File
