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



#ifndef PREVIEWCONTROLBASE_H
#define PREVIEWCONTROLBASE_H

//  INCLUDES
#include "ImageEditorControlBase.h"
#include "DrawUtils.h"

//  FORWARD DECLARATIONS
class CFbsBitmap;
class CPluginInfo;

// CONSTANTS
const TInt KDancingAntzTimerDelayInMicroseconds     = 100000;
const TInt KDancingAntzTimerIntervalInMicroseconds  = 250000;

// These can be replaced in the plug-ins to adjust the movement
const TInt KDefaultFastKeyTimerDelayInMicroseconds      = 200000;
const TInt KDefaultFastKeyTimerIntervalInMicroseconds   = 50000;
const TInt KDefaultFastKeyTimerMultiplyThresholdInTicks = 3;
const TInt KDefaultSmallNavigationStepMultiplier        = 1;
const TInt KDefaultBigNavigationStepMultiplier          = 6;


/*	CLASS: CPreviewControlBase
*
*   CPreviewControlBase acts as a base class for preview pane Image Editor UI.
*
*/
class CPreviewControlBase :		public CImageEditorControlBase
{

public:

/** @name Methods:*/
//@{

	/*	NewL factory method, pops cleanupstack
	*
	*	@param aRect - control rectangle
	*	@param aParent - pointer to window owning control
	*   @return - pointer to newly created CPreviewControlBase
	*/
    IMPORT_C static CPreviewControlBase * NewL (
		const TRect &		aRect,
		CCoeControl	*		aParent
		);

	/*	Second phase constructor
	*
	*	@see CImageEditorControlBase
	*/
    IMPORT_C virtual void ConstructL (
		const TRect &		aRect,
		CCoeControl	*		aParent
		);

	/*	Destructor
	*
	*   @param -
	*   @return -
	*/
    IMPORT_C virtual ~CPreviewControlBase();

	/*	SetImage
	*
	*	@see CImageEditorControlBase
	*/
    IMPORT_C virtual void SetImageL (CFbsBitmap * aBitmap);
    
    /*	SetImageL
	*
	*	@see CImageEditorControlBase
	*/
    IMPORT_C virtual void SetImageL (const CFbsBitmap * aBitmap);

    /*	DrawPreviewImage
    *
    *   Draws preview image
	*
	*   @param aRect - region of control in need of redrawing
	*   @return -
	*/
    IMPORT_C virtual void DrawPreviewImage (const TRect & aRect) const;
    

//@}

protected:

/** @name Methods:*/
//@{    
//@}

/** @name Members:*/
//@{
    /// Preview image
	const CFbsBitmap *		        iPreview;
//@}

private:

/** @name Methods:*/
//@{

	/*	Draw
	*
	*	@see CImageEditorControlBase
	*/
    IMPORT_C virtual void Draw (const TRect & aRect) const;

//@}

/** @name Members:*/
//@{

//@}

       
};


#endif

// End of File
