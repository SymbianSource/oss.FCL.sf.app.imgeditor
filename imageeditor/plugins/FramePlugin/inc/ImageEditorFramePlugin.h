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



#ifndef IMAGEEDITORFRAMEPLUGIN_H
#define IMAGEEDITORFRAMEPLUGIN_H

//	INCLUDES
#include "iepb.h"

// debug log
#include "imageeditordebugutils.h"
_LIT(KFramePluginLogFile,"FramePlugin.log");

//  FORWARD DECLARATIONS
class CEditorImage;
class CCoeControl;


/*	CLASS:	CImageEditorFramePlugin
*
*	CImageEditorFramePlugin represents Frame insertion plug-in for Image Editor 
*   application.
*
*/
class CImageEditorFramePlugin :	public CImageEditorPluginBase
{

public:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageEditorFramePlugin ();

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
	virtual ~CImageEditorFramePlugin ();

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
	CImageEditorFramePlugin (const CImageEditorFramePlugin & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorFramePlugin & operator= (const CImageEditorFramePlugin & rhs);

//@}

/** @name Members:*/
//@{
    /// Plug-in UI control
    CCoeControl *           iControl;
//@}

};

#endif

// End of File
