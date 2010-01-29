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



#ifndef IMAGEEDITORPLUGINBASE_H
#define IMAGEEDITORPLUGINBASE_H

///	INCLUDES
#include <e32base.h>
#include <e32std.h>
#include "pluginbase.h"

///	FORWARD DECLARATIONS
class CCoeControl;
class CEditorImage;


/// CONSTANTS
_LIT (KPgnResourcePath, "\\private\\101ffa91\\plugins\\");


/*  CLASS: CImageEditorPluginBase
*
*	CImageEditorPluginBase is a plugin base class for all Image Editor
*	plug-ins.
*/ 
class CImageEditorPluginBase :	public CPluginBase
{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
    *
	*	@param aResourcePath - plug-in resource path
	*	@param aResourceFile - plug-in resource file
	*	@return CImageEditorPluginBase *, pointer to created CImageEditorPluginBase object
	*/
	IMPORT_C static CImageEditorPluginBase * NewL (
        const TDesC & aResourcePath,
        const TDesC & aResourceFile
        );

	/** Default constructor, cannot leave.
    *
	*	@param -
	*	@return -
	*/
	IMPORT_C CImageEditorPluginBase ();

	/** Second phase constructor, may leave
    *
	*	@param aResourcePath - plug-in resource path
	*	@param aResourceFile - plug-in resource file
    *   @return -
	*/
	IMPORT_C void ConstructL (
        const TDesC & aResourcePath,
        const TDesC & aResourceFile
        );

    /** Destructor
	*	@param -
	*	@return -
	*/
	IMPORT_C virtual ~CImageEditorPluginBase ();

	/*	GetProperty
	*
	*	@see CPluginBase
	*/
	IMPORT_C virtual TInt SetProperty (
		TInt		aPropertyId, 
		TDesC &		aPropertyValue
		);

	/*	GetProperty
	*
	*	@see CPluginBase
	*/
	IMPORT_C virtual TInt GetProperty (
		TInt		aPropertyId, 
		TDes &		aPropertyValue
		);

	/*	ShowPopupNote
	*
	*   Shows plug-in related popup note.
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C virtual void ShowPopupNote ();

    /**	InitPluginL
	*
	*	Initializes image processing plug-in and returns CCoeControl.
	*
	*	@param aRect - control rectangle
	*	@param aParent - parent control
	*	@param aPluginControl - reference to the plug-in control
    *	@return CCoeControl - plugin drawing area
    */
	IMPORT_C virtual TInt InitPluginL (
		const TRect &		aRect,
		CCoeControl *		aParent,
		CCoeControl *&		aPluginControl
		);

    /**	ProcessImageL
	*
	*	Sets the image to be processed.
	*
	*	@param CFbsBitmap - pointer to CMiaImage instance
    *	@return -
    */
    IMPORT_C virtual void ProcessImageL (CEditorImage * aImage);    

    /**	ReleasePlugin 
	*
	*	Releases plugin.
	*
	*	@param -
    *	@return -
    */
	IMPORT_C virtual void ReleasePlugin ();    

	/*	ReservedImageEditorBaseMethod1
	*
	*	@see CPluginBase
	*/
	IMPORT_C virtual void ReservedImageEditorBaseMethod1 ();

	/*	ReservedImageEditorBaseMethod2
	*
	*	@see CPluginBase
	*/
	IMPORT_C virtual void ReservedImageEditorBaseMethod2 ();

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

//@}

/** @name Members:*/
//@{
	/*	
	*	Internal representation pointer.
	*
	*	DO NOT add any other private members here, so that the size 
	*	of the class stays constant.
	*/
    void * iEditorPluginBaseInternalRepresentation;

//@}

};


#endif

// End of File
