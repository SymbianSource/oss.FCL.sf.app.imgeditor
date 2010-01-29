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


#ifndef IMAGEEDITORCLIPARTPLUGIN_H
#define IMAGEEDITORCLIPARTPLUGIN_H

//	INCLUDES
#include "iepb.h"

//  FORWARD DECLARATIONS
class CEditorImage;
class CCoeControl;
class CSystemParameters;


/*	CLASS:	CImageEditorClipartPlugin
*
*	CImageEditorClipartPlugin represents clipart insertion plug-in for Image Editor 
*   application.
*
*/
class CImageEditorClipartPlugin :	public CImageEditorPluginBase
{

public:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageEditorClipartPlugin ();

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
	virtual ~CImageEditorClipartPlugin ();

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
	CImageEditorClipartPlugin (const CImageEditorClipartPlugin & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorClipartPlugin & operator= (const CImageEditorClipartPlugin & rhs);

//@}

/** @name Members:*/
//@{
    /// Plug-in UI control
    CCoeControl *             iControl;
    /// System Parameters
    const CSystemParameters * iSysPars;
//@}

};


#endif

// End of File
