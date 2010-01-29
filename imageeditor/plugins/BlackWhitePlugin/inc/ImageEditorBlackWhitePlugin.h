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



#ifndef IMAGEEDITORBLACKWHITEPLUGIN_H
#define IMAGEEDITORBLACKWHITEPLUGIN_H

//	INCLUDES
#include "ImageEditorPluginBaseDefs.h"
#include "iepb.h"

//  FORWARD DECLARATIONS
class CEditorImage;
class CPluginParameters;

/*	CLASS:	CImageEditorBlackWhitePlugin
*
*	CImageEditorBlackWhitePlugin represents Black & White color conversion plug-in
*	for Image Editor application.
*/
class CImageEditorBlackWhitePlugin :	public CImageEditorPluginBase
{

public:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageEditorBlackWhitePlugin ();

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
	virtual ~CImageEditorBlackWhitePlugin ();

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
	CImageEditorBlackWhitePlugin (const CImageEditorBlackWhitePlugin & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorBlackWhitePlugin & operator= (const CImageEditorBlackWhitePlugin & rhs);

//@}

/** @name Members:*/
//@{

//@}

};


#endif

// End of File
