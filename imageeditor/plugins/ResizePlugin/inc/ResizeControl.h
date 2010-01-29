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


/*
*   File:       ResizeControl.h
*   Created:    22-10-2004
*   Author:     Wellu Mäkinen
*/

#ifndef RESIZECONTROL_H
#define RESIZECONTROL_H

//  INCLUDES
#include "PreviewControlBase.h"
#include "ImageEditorPluginBaseDefs.h"
#include "SingleParamControlObserver.h"

//  FORWARD DECLARATIONS
class CCoeControl;
class CAknView;
class CPluginInfo;
class CSystemParameters;
class CImageEditorResizePlugin;

/*	CLASS: CResizeControl
*
*   CResizeControl represents a single parameter adjustment UI control,
*   that can be used by any of the plug-ins just by defining a UI type to
*   resource file.
*/
class CResizeControl : public CPreviewControlBase,
                       public MSingleParControlObserver
{

public:

/** @name Methods:*/
//@{
	/** NewL factory method, pops cleanupstack
	*
	*   @param aRect - control rectangle
	*	@param aParent - pointer to window owning control
	*   @return - pointer to the newly created CResizeControl instance
	*/
	static CResizeControl * NewL (
		const TRect &		aRect,
		CCoeControl	*		aParent
		);

	/*	ConstructL
	*
	*   @see CImageEditorControlBase
	*/
    virtual void ConstructL (
        const TRect &   aRect,	
        CCoeControl *   aParent
        );

    /** Destructor
	*
	*	@param -
	*	@return -
	*/
	virtual ~CResizeControl ();

	/*	SetView
	*
	*	@see CImageEditorControlBase
	*/
    virtual void SetView (CAknView * aView);

	/*	SetSelectedUiItemL
	*
	*	@see CImageEditorControlBase
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
	*/
    virtual TKeyResponse OfferKeyEventL (
        const TKeyEvent &   aKeyEvent, 
        TEventCode          aType
        );

    /**	HandlePointerEventL	
	*
	*	@see CCoeControl
	*/
    virtual void HandlePointerEventL( const TPointerEvent &aPointerEvent );

	/*	CountComponentControls
	*
	*	@see CImageEditorControlBase
	*/
    virtual TInt CountComponentControls() const;

	/*	ComponentControl
	*
	*	@see CImageEditorControlBase
	*/
    virtual CCoeControl * ComponentControl (TInt aIndex) const;

	/*	HandlePluginCommandL
	*
	*	@see CImageEditorControlBase
	*/
	virtual void HandlePluginCommandL (const TInt aCommand);

	/*	GetSoftkeyIndexL
	*
	*	@see CImageEditorControlBase
	*/
    virtual TInt GetSoftkeyIndexL ();

	/*	GetNaviPaneTextL
	*
	*	@see CImageEditorControlBase
	*/
//	virtual TPtrC GetNaviPaneTextL (
//		TBool& aLeftNaviPaneScrollButtonVisibile, 
//		TBool& aRightNaviPaneScrollButtonVisible);

	/*	GetDimmedMenuItems
	*
	*	@see CImageEditorControlBase
	*/
	virtual TBitField GetDimmedMenuItems ();

	/*	SetSysteParameters
	*
	*	Sets reference to system parameters to plug-in.
	*
	*	@param aWidth - image width
	*	@return - 
	*/
    void SetSystemParameters (CSystemParameters * aSysPars, CImageEditorResizePlugin* aParent);

	/** ParamOperation 
    *
    *   @see MSingleParControlObserver
	*/
	virtual void ParamOperation (const TParamOperation aOperation);

	/*	ShowPopupNote
	*
	*	@see CImageEditorPluginBase
	*/
	virtual void ShowPopupNote ();

//@}


protected:

/** @name Methods:*/
//@{

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
	CResizeControl ();

    /*	SizeChanged
	*
	*   @see CImageEditorControlBase
	*/
	virtual void SizeChanged();

	/*	Draw
	*
	*   @see CImageEditorControlBase
	*/
	virtual void Draw (const TRect & aRect) const;

	/*	ComputeAndSetSliderParameters 
	*
	*   Computers and sets parameters for slider.
	*
	*	@param -
	*	@return -
	*/
	void ComputeAndSetSliderParameters (CImageEditorResizePlugin* aParent);

	/*	ComputeSizeAndScale 
	*
	*   Computers current image size and scale.
	*
	*	@param -
	*	@return -
	*/
	void ComputeSizeAndScale();

	/*	ComputeImageSizes 
	*
	*   Computers available target sizes.
	*
	*	@param -
	*	@return -
	*/
	void ComputeImageSizes();

	/*	PopulateParamArray 
	*
	*   Add target size to param array if appropriate.
	*
	*	@param -
	*	@return -
	*/
	void PopulateParamArray(const TSize& aTargetSize);

	/** GetParam
    *
    *   @see MSingleParControlObserver
	*/
	virtual TReal GetParam() const;

//@}

/** @name Members:*/
//@{

    /// Plug-in UI control
    CCoeControl *       iControl;
    /// Reference to view
//    CAknView *          iEditorView;
	/// Plug-in info
	CPluginInfo *       iPluginInfo;
    /// Reference to system parameters
	CSystemParameters * iSysPars;
	/// Navi pane text
//	TBuf<64>            iNaviPaneText;
	/// Array of allowed heights / widths
	RArray<TInt>		iParamArray;
	/// Original input image size
	TSize				iOrigSize;
	/// Current image size
	TSize				iSize;
	//	Initial position
	TInt				iInitPosition;
	// Array for storing parameter strings (from rss file)
	CDesCArray *        iParameters;
	//	Preview buffer
	CFbsBitmap *        iBufBitmap;
	
//@}

};



#endif
