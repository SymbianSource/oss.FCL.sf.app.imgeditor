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



#ifndef IMAGEEDITORIMAGECONTROLLER_H
#define IMAGEEDITORIMAGECONTROLLER_H

//  INCLUDES
#include <e32base.h>
#include <fbs.h>

#include "ImageControllerObserver.h"


//  FORWARD DECLARATIONS
class CImageEditorPluginManager;
class CImageEditorImageHandler;

/*	CLASS: CImageController
*
*	CImageController is an active object controlling asynchronous saving
*	and loading of images. Also, synchronous methods for renaming and deleting
*	images can be found.
*/
class CImageController : public CActive
{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param aObserver - CImageController observer instance, ownership not changed
	*	@param aManager - CImageEditorPluginManager instance
	*	@return pointer to created CImageController object
	*/
	static CImageController * NewL (
		MImageControllerObserver *		aObserver,
		CImageEditorPluginManager *		aManager
		);

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	virtual ~CImageController ();

	/** LoadImage
	*
	*	Loads image.
	*
	*	@param aFileName - name of the image file to be loaded
	*	@return -
	*/
	void LoadImageL (const TDesC & aFileName);

	/** SaveImage
	*
	*	Saves image 
	*
	*	@param aFileName - saved image name
	*	@param aProgressCount - On return, contains the number of blocks to progress
	*	@return -
	*/
	void SaveImageL (const TDesC & aFileName);

    /** RenameImage
	*
	*	Renames image. 
	*
	*	@param aFileNameSrc - source image name
	*	@param aFileNameDst - destination image name
	*	@return KErrNone if no errors, system wide error otherwise
	*/
	TInt  RenameImage (
		const TDesC	&		aFileNameSrc,
		const TDesC	&		aFileNameDst
		) const;

	/** DeleteImage
	*
	*	Deletes image. 
	*
	*	@param aFileName - image name to be deleted
	*	@return KErrNone if no errors, system wide error otherwise
	*/
	TInt DeleteImage (const TDesC & aFileName) const;

	/** CreatePreviewBitmapL 
	*
	*	Creates the preview bitmap.
	*
	*	@param -
	*	@return -
	*/
	void CreatePreviewBitmapL ();

    /** DeletePreviewBitmap 
	*
	*	Deletes the preview bitmap.
	*
	*	@param -
	*	@return -
	*/
	void DeletePreviewBitmap ();

    /** CancelOperation
	*
	*	Cancels loading, saving or processing operation.
	*	This does not call the AO's Cancel(), but rather waits for the nexts RunL()
	*	to finish, and finishes after that.
	*	
	*	@param aOperation - the on-going operation
	*	@return -
	*/
	void CancelOperation(TBool aForce = EFalse);


//@}

protected:

/** @name Methods:*/
//@{

	/** DoCancel
	*
	*	Implements cancellation of outstanding request. This method is 
	*	called from Cancel() of active object. Cancels asynchronous request
	*	and returns. DOES NOT WAIT THE REQUEST TO COMPLETE.
	*
	*	@param -
	*	@return -
	*	@see CActive
	*/
	virtual void DoCancel();

	/** RunL
	*
	*	Active object's request completion handler. 
	*
	*	Active scheduler selects this class based on class priority and
	*	marks the status completed before calling this method. This method
	*	should handle the completion as fast as possible and return or issue
	*	another request.
	*
	*	@param -
	*	@return -
	*	@see CActive
	*/
	virtual void RunL ();


//@}

/** @name Members:*/
//@{

//@}

private:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageController ();

	/** Second phase constructor, may leave
	*
	*	@param aObserver - CImageController observer instance, ownership not changed
	*	@param aManager - CImageEditorPluginManager instance
	*	@return -
	*/
	void ConstructL (
		MImageControllerObserver *		aObserver,
		CImageEditorPluginManager *		aManager
		);

	/** CompleteRequest
	*
	*	Force RunL
	*
	*	@param -
	*	@return -
	*/
    void CompleteRequest();

	/** CalculateProgressInterval
	*
	*	The saving operation typically consists of thousands of blocks. 
	*	It makes no sense to update the progress bar that often.
	*
	*	@param -
	*	@return -
	*/
    void CalculateProgressInterval (TInt aBlockCount);
//@}

/** @name Members:*/
//@{
	/// Image Handler
	CImageEditorImageHandler *	iImageHandler;
	
    /// Plug-in Manager
	CImageEditorPluginManager *	iManager;

	/// CImageController observer instance
	MImageControllerObserver *	iObserver;

    /// Preview bitmap
	CFbsBitmap *				iPreview;

    /// Jpeg Image
    TBool                       iJpegImage;

    /// JPEG quality
    TInt                        iQuality;

    /// Last operation
	MImageControllerObserver::TOperationCode	iOperation;

    /// Progress bar update interval (number of blocks)
    TInt                        iProgressBarInterval;

	//	Current save progress in percentage
    TInt                        iSavedPercentage;

	//	Previous save progress in percentage
	TInt						iSavedPercentageOld;

    /// Is operation cancelled
    TBool                       iIsCancelled;

//@}

};


#endif
