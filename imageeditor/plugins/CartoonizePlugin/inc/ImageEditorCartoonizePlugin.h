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



#ifndef IMAGEEDITORCARTOONIZEPLUGIN_H
#define IMAGEEDITORCARTOONIZEPLUGIN_H

//	INCLUDES
#include "ImageEditorPluginBaseDefs.h"
#include "iepb.h"

//  FORWARD DECLARATIONS
class CEditorImage;
class CPluginParameters;


/*	CLASS:	CImageEditorCartoonizePlugin
*
*	CImageEditorCartoonizePlugin represents cartoonize plug-in
*	for Image Editor application.
*/
class CImageEditorCartoonizePlugin :	public CImageEditorPluginBase
{

public:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageEditorCartoonizePlugin ();

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
	virtual ~CImageEditorCartoonizePlugin ();

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
	CImageEditorCartoonizePlugin (const CImageEditorCartoonizePlugin & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorCartoonizePlugin & operator= (const CImageEditorCartoonizePlugin & rhs);

//@}

/** @name Members:*/
//@{

//@}

};


#endif

// End of File
