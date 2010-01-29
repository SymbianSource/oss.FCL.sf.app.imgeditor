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



#ifndef IMAGEEDITORBUBBLEPLUGIN_H
#define IMAGEEDITORBUBBLEPLUGIN_H

//	INCLUDES
#include "iepb.h"

//  FORWARD DECLARATIONS
class CEditorImage;
class CCoeControl;
class CSystemParameters;


/*	CLASS:	CImageEditorBubblePlugin
*
*	CImageEditorBubblePlugin represents Bubble insertion plug-in for Image Editor 
*   application.
*
*/
class CImageEditorBubblePlugin :	public CImageEditorPluginBase
{

public:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageEditorBubblePlugin ();

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
	virtual ~CImageEditorBubblePlugin ();

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
	CImageEditorBubblePlugin (const CImageEditorBubblePlugin & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorBubblePlugin & operator= (const CImageEditorBubblePlugin & rhs);

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
