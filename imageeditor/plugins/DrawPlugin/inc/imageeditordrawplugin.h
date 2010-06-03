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
* Draw plugin UI.
*
*/


#ifndef IMAGEEDITORDRAWPLUGIN_H
#define IMAGEEDITORDRAWPLUGIN_H

//	INCLUDES
#include "iepb.h"

//  FORWARD DECLARATIONS
class CEditorImage;
class CCoeControl;
class CSystemParameters;
class CDrawPath;

/*	CLASS:	CImageEditorDrawPlugin
*
*	CImageEditorDrawPlugin represents draw plug-in for Image Editor 
*   application.
*/
class CImageEditorDrawPlugin : public CImageEditorPluginBase
{

public:
/** @name Methods:*/
//@{

	/** Default constructor */
	CImageEditorDrawPlugin ();

	/** Second phase constructor */
	void ConstructL ();

	/** Destructor */
	~CImageEditorDrawPlugin ();

	/**	SetProperty
	*
	*   @see CPluginBase
	*/
	virtual TInt SetProperty (
		TInt		aPropertyId, 
		TDesC&		aPropertyValue
		);

	/**	GetProperty
	*
	*   @see CPluginBase
	*/
	virtual TInt GetProperty (
		TInt		aPropertyId, 
		TDes&		aPropertyValue
		);

    /**	InitPluginL
	*
	*	@see CImageEditorPluginBase
    */
    virtual TInt InitPluginL (
		const TRect&		aRect,
		CCoeControl*		aParent,
		CCoeControl*&		aPluginControl
		);

    /**	ProcessImageL
	*
	*	@see CImageEditorPluginBase
    */
    virtual void ProcessImageL (CEditorImage* aImage);    

    /**	ReleasePlugin 
	*
	*	@see CImageEditorPluginBase
    */
	virtual void ReleasePlugin ();    
	
	/**CalculatePositionOnView
	 * 
	 *  calculate the point on view when changing the screen mode 
	 */
	void CalculatePositionOnView();
//@}

private: // Data
/** @name Members:*/
//@{
    /// Plug-in UI control
    CCoeControl* iControl;
    TBool iDiplayToolPluginParam;
  /// System parameters
    const CSystemParameters* iSysPars;
    /// Is landscape enabled, Must be EFalse until text is set
    TBool iLandscapeEnabled;
//@}

private: // Data 
	//for drawcontrol
	//drawed lines in drawcontrol
	RPointerArray<CDrawPath> iPaths;
	//selected pencolor 
	TRgb       iRgb;
	//selected pensize
	TSize      iSize;
	//first image rect on preview
	TRect      iVisibleImageRectPrevFirst;
  
};

#endif // IMAGEEDITORDRAWPLUGIN_H

// End of File
