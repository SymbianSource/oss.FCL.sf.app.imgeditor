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



#ifndef IMAGEEDITORCROPPLUGIN_H
#define IMAGEEDITORCROPPLUGIN_H

//	INCLUDES
#include "iepb.h"

//  FORWARD DECLARATIONS
class CEditorImage;
class CCoeControl;
class CSystemParameters;


/*	CLASS:	CImageEditorCropPlugin
*
*	CImageEditorCropPlugin represents crop plug-in for Image Editor 
*   application.
*
*/
class CImageEditorCropPlugin :	public CImageEditorPluginBase
{

public:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageEditorCropPlugin ();

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
	virtual ~CImageEditorCropPlugin ();

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
	CImageEditorCropPlugin (const CImageEditorCropPlugin & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorCropPlugin & operator= (const CImageEditorCropPlugin & rhs);
    
    /** ReadWallPaperCropDefinitionsL
	*
    *   Adds the crop ratio names to aCropTexts and aspect ratio description 
    *   to aCropRatios. Aspect ratio is current screen resolution either in 
    *   portrait or in landscape.
    *
    *   The aspect ratio is defined by two integers W for width and H for 
    *   height. On return, aCropRatios have the aspect ratio definitions 
    *   inserted as follows: W1 H1 W2 H2... 
    
	*	@param aCropTexts - on return includes crop ratio names
	*	@param aCropRatios - on return includes crop ratio definitions
	*	@return -
	*/
    void ReadWallPaperCropDefinitionsL (
        CDesCArray &  aCropTexts,
        RArray<TInt> &  aCropRatios
        ) const;
        
	/** ReadCropDefinitionsL
	*
    *   Reads crop definition resource. Adds the crop ratio names to
    *   aCropTexts and aspect ratio description to aCropRatios.
    *
    *   The aspect ratio is defined by two integers W for width and H for 
    *   height. On return, aCropRatios have the aspect ratio definitions 
    *   inserted as follows: W1 H1 W2 H2... This means that after this method 
    *   is called, count of aCropRatios is three times the count of aCropTexts.
    *
	*	@param aCropTexts - on return includes crop ratio names
	*	@param aCropRatios - on return includes crop ratio definitions
	*	@return -
	*/
    void ReadCropDefinitionsL (
        CDesCArray &  aCropTexts,
        RArray<TInt> &  aCropRatios
        ) const;

//@}

/** @name Members:*/
//@{
    /// Plug-in UI control
    CCoeControl *             iControl;
    /// Image height
    const CSystemParameters * iSysPars;

//@}

};


#endif

// End of File
