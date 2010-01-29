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



#ifndef IMAGEEDITORIMAGEHANDLER_HPP
#define IMAGEEDITORIMAGEHANDLER_HPP

//	INCLUDES
#include <e32base.h>

//	FORWARD DECLARATIONS
class CImageDecoder;
class CImageEncoder;
class CFbsBitmap;
class MImageHandlerObserver;
class TRequestStatus;
class CFileExtensionMIMEType;
class CFrameImageData;


/*	CLASS: CImageEditorImageHandler
*
*   CImageEditorImageHandler represents image handling unit in Image Editor.
*	It includes functionality to asynchronously load and save images. Also,
*	it provides other image managing functionality.
*
*/
class CImageEditorImageHandler : public CBase
{

public:

/** @name Methods:*/
//@{

	enum TImageFormat
	{
		EImageFormatInvalid,
		EImageFormatBmp,
		EImageFormatJpg,
		EImageFormatMbm,
		EImageFormatGif,
		EImageFormatPng,
		EImageFormatTiff,
		EImageFormatMax
	};

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param -
	*	@return pointer to created CImageEditorImageHandler object
	*/
	IMPORT_C static CImageEditorImageHandler * NewL ();

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C virtual ~CImageEditorImageHandler ();

	/** LoadImageAsync
	*
	*	Loads image asyncronically. Calls RunL method of an active object
	*	when loading complete.
	*
	*	@param aFileName - name of the image file to be loaded
	*	@param aBitmap - bitmap to which image is loaded, must be created
	*	@param aStatus - status indicator
	*	and set to wanted size and displaymode
	*	@return -
	*/
	IMPORT_C void LoadImageAsyncL (
		const TDesC &		aFileName,
		CFbsBitmap *&		aBitmap,
		TRequestStatus *	aStatus
		);

	/** ContinueLoading
	*
	*	Continues loading of image data, if KErrUnderflow occurs.
	*
	*	@param aStatus - request status
	*	@return -
	*/
	IMPORT_C void ContinueLoading (TRequestStatus *	aStatus);

    /** SaveImageAsync
	*
	*	Saves image asyncronically. Calls RunL method of an active object
	*	when saving complete.
	*
	*	@param aFileName - saved image name
	*	@param aBitmap - bitmap to be saved
	*	@param aImageQuality - JPEG image quality (0,100)
	*	@param aStatus - status indicator
	*	@return -
	*/
	IMPORT_C void SaveImageAsyncL (
		const TDesC	&		aFileName,
		const CFbsBitmap &	aBitmap,
		const TInt			aImageQuality,
		TRequestStatus *	aStatus
		);

	/** GetJpegQualityFactorL
	*
	*	Gets the JPEG quality factor.
	*
	*	@param aFileName - saved image name
	*	@return - the quality factor
	*/
    IMPORT_C  TInt GetJpegQualityFactorL (const TDesC & aFileName);

	/** RenameImage
	*
	*	Renames image. 
	*
	*	@param aFileNameSrc - source image name
	*	@param aFileNameDst - destination image name
	*	@return KErrNone if no errors, system wide error code otherwise
	*/
	IMPORT_C TInt RenameImage (
		const TDesC	&		aFileNameSrc,
		const TDesC	&		aFileNameDst
		) const;

	/** DeleteImage
	*
	*	Deletes image. 
	*
	*	@param aFileName - image name to be deleted
	*	@return KErrNone if no errors, system wide error code otherwise
	*/
	IMPORT_C TInt DeleteImage (const TDesC & aFileName) const;

	/** CleanupConverters 
	*
	*	Cancels the asynchronous requests and cleans up image converters.
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C void CleanupConverters ();

	/** IsRajpegImageL
	*
	*	Checks whether an image is a RAJPEG image or not.
	*
	*	@param aFileName - file name
	*	@return ETrue if RAJPEG image, EFalse otherwise
	*/
	IMPORT_C TBool IsRajpegImageL (const TDesC & aFileName) const;

	/** IsJpegIntact
	*
	*	Verifies JPEG image content.
	*
	*	@param aFileName - file name
	*	@return ETrue if intact, EFalse if broken
	*/
	IMPORT_C TBool IsJpegIntact (const TDesC & aFileName);

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

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageEditorImageHandler ();

	/** Second phase constructor, may leave
	*
	*	@param -
	*	@return -
	*/
	void ConstructL ();

	/** GetMIMETypeLC
	*
	*	Finds and create CFileExtensionMIMEType type based on file extension.
	*
	*	@param aFileName - image file name
	*	@return MIME type instance
	*/
	CFileExtensionMIMEType * GetMIMETypeL (const TDesC & aFileName);

	/*	GetFrameImageDataL
	*
	*   Gets CFrameImageData instance.
	*
	*   @param aExt - image name extension
	*   @param aQuality - JPEG quality
	*   @return CFrameImageData instance
	*/
	CFrameImageData * GetFrameImageDataL (
		const TDesC &		aExt,
		const TInt			aQuality
		) const;

	/** Copy constructor, disabled
	*/
	CImageEditorImageHandler (const CImageEditorImageHandler & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorImageHandler & operator= (const CImageEditorImageHandler & rhs);

//@}

/** @name Members:*/
//@{
	/// Image decoder instance (used in loading)
	CImageDecoder * iImageDecoder;

	/// Image encoder instance (used in saving)
	CImageEncoder * iImageEncoder;
//@}

};


#endif

// End of File
