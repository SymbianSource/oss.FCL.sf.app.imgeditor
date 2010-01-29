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


#ifndef IMAGEEDITORSHARPNESSPLUGIN_H
#define IMAGEEDITORSHARPNESSPLUGIN_H

//	INCLUDES
#include "ImageEditorPluginBaseDefs.h"
#include "iepb.h"
#include "SingleParamControlObserver.h"

//  FORWARD DECLARATIONS
class CEditorImage;

/*	CLASS:	CImageEditorSharpnessPlugin
*
*	CImageEditorSharpnessPlugin represents sharpness adjustment plug-in for
*	Image Editor application.
*
*/
class CImageEditorSharpnessPlugin :	public CImageEditorPluginBase,
                                    public MSingleParControlObserver
{

public:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageEditorSharpnessPlugin ();

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
	virtual ~CImageEditorSharpnessPlugin ();

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

	/** ParamOperation 
    *
    *   @see MSingleParControlObserver
	*/
	virtual void ParamOperation (const TParamOperation aOperation);

	/** GetParam
    *
    *   @see MSingleParControlObserver
	*/
	virtual TReal GetParam() const;

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
	CImageEditorSharpnessPlugin (const CImageEditorSharpnessPlugin & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorSharpnessPlugin & operator= (const CImageEditorSharpnessPlugin & rhs);

//@}

/** @name Members:*/
//@{
    /// Plug-in UI control
    CCoeControl *           iControl;
    //  Sharpness parameter
    TInt					iSharpness;
//@}

};


#endif

// End of File
