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


#ifndef IMAGEEDITORRESIZEPLUGIN_H
#define IMAGEEDITORRESIZEPLUGIN_H

//	INCLUDES
#include "iepb.h"


//  FORWARD DECLARATIONS
class CEditorImage;
class CSystemParameters;


/*	CLASS:	CImageEditorResizePlugin
*
*	CImageEditorResizePlugin represents resize adjustment plug-in for
*	Image Editor application.
*
*/
class CImageEditorResizePlugin : public CImageEditorPluginBase
{

public:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageEditorResizePlugin ();

	/** Second phase constructor, may leave
	*
	*	@param -
	*	@return -
	*/
	virtual void ConstructL ();

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	virtual ~CImageEditorResizePlugin ();

	/*	SetProperty
	*
	*   @see CPluginBase
	*/
	virtual TInt SetProperty (
		TInt		aPropertyId,
		TDesC &		aPropertyValue
		);

	/*	GetProperty
	*
	*   @see CPluginBase
	*/
	virtual TInt GetProperty (
		TInt		aPropertyId,
		TDes &		aPropertyValue
		);

    /**	InitPluginL
	*
	*	@see CImageEditorPluginBase
    */
    virtual TInt InitPluginL (
		const TRect &		aRect,
		CCoeControl *		aParent,
		CCoeControl *&		aPluginControl
		);

    /**	ProcessImageL
	*
	*	@see CImageEditorPluginBase
    */
    virtual void ProcessImageL (CEditorImage * aImage);

    /**	ReleasePlugin
	*
	*	@see CImageEditorPluginBase
    */
	virtual void ReleasePlugin ();

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

//@}

/** @name Members:*/
//@{

    /// Plug-in UI control
    CCoeControl *           iControl;
    /// System parameter reference
    CSystemParameters *     iSysPars;

//@}

};


#endif

// End of File
