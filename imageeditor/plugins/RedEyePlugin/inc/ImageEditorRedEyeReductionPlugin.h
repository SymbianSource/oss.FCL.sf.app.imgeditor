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



#ifndef IMAGEEDITORREDEYEREDUCTIONPLUGIN_H
#define IMAGEEDITORREDEYEREDUCTIONPLUGIN_H

//	INCLUDES
#include "iepb.h"

//  FORWARD DECLARATIONS
class CEditorImage;
class CCoeControl;
class CSystemParameters;

/*	CLASS:	CImageEditorRedEyeReductionPlugin
*
*	CImageEditorRedEyeReductionPlugin represents red eye reduction
*	plug-in for Image Editor application.
*
*/
class CImageEditorRedEyeReductionPlugin :	public CImageEditorPluginBase
{

public:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageEditorRedEyeReductionPlugin ();

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
	virtual ~CImageEditorRedEyeReductionPlugin ();

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

	/** Copy constructor, disabled
	*/
	CImageEditorRedEyeReductionPlugin (const CImageEditorRedEyeReductionPlugin & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorRedEyeReductionPlugin & operator= (const CImageEditorRedEyeReductionPlugin & rhs);

//@}

/** @name Members:*/
//@{
    /// Plug-in UI control
    CCoeControl *             iControl;
    /// System parameters (width and height used)
    const CSystemParameters * iSysPars;

//@}

};


#endif

// End of File
