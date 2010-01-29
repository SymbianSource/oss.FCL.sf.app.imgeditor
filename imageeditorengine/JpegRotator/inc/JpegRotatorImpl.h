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


#ifndef JPEGROTATORIMPL_H
#define JPEGROTATORIMPL_H


// INCLUDES
#include "JpegRotator.h"
#include "MJpRotateCallBack.h"

// debug log writer
//#include "imageeditordebugutils.h"

// CONSTANTS

// FORWARD DECLARATIONS

class CJpRotate;

/*	CLASS: 	CJpegRotatorImpl
*
*	CJpegRotatorImpl
*	Internal representation of CJpegRotator
*
*/
class CJpegRotatorImpl : public CBase, public MJpRotateCallBack
{
    
public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanup stack
	*
	*	@param - aFsSession
	*	@return pointer to created CJpegRotatorImpl object
	*/
	static CJpegRotatorImpl * NewL (RFs& aFsSession);

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	~CJpegRotatorImpl ();

	/** PrepareRotateFileL
	*
	*	@param aRotationMode - 
	*	@param aSourceFileName - 
	*	@param aTargetFileName -
	*	@param aHandleExifData -
	*	@return -
	*/
    void PrepareRotateFileL (
        const TDesC& aSourceFileName,
        const TDesC& aTargetFileName,
        CJpegRotator::TRotationMode aRotationMode,
        TBool aHandleExifData
        );

	/** Start asynchronous rotate
	*
	*	@param - aStatus
    *   @param - aStatus
	*/
    void StartAsyncRotate (TRequestStatus& aStatus, TBool aForceLossyMode);

	/** Start asynchronous rotate
	*
	*	@param - aStatus
	*/
    void StartAsyncLosslessRotate (TRequestStatus& aStatus);

	/** Finish asynchronous rotate
	*
	*	@param - aError
	*	@return -
	*/
    void FinishAsyncRotate (TInt aError);

	/** IsLosslessRotatePossible
    *
    *   Check if the image dimensions are suitable for the use of
    *   rajpeg lossless rotate (divisible by 8)
	*/
    TBool IsLosslessRotatePossible() const;


	/** Cancel
    *   
    *   Set cancel flag.
	*/
    void Cancel ();

    /** SetExifData
	*
	*	@see MImageEditorExifHandlerObserver
	*/
	void SetExifData (TUint8* aExifData, TUint aDataSize);

    /** GetThumbnailImageL
	*
	*	@see MImageEditorExifHandlerObserver
	*/
	void GetThumbnailImageL (
        TUint8*& aThumbnailData, 
        TUint& aDataSize, 
        const TSize& aThumbnailResolution
        );

    /** SetJpegCommentL
	*
	*	@see MImageEditorExifHandlerObserver
	*/
    void SetJpegCommentL (const TDesC8& aComment);

    /** GetOutputImageSize
	*
	*	@see MImageEditorExifHandlerObserver
	*/
	void GetOutputImageSize ( TInt& aWidth, TInt& aHeight ) const;

    /** JpRotateStatus
	*
	*	@see MJpRotateCallBack
	*/
    void JpRotateStatus( TInt aCount, TInt aTotal );

//@}

protected:

/** @name Methods:*/
//@{

//@}

private:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param - aFsSession
	*	@return -
	*/
	CJpegRotatorImpl (RFs& aFsSession);

	/** Second phase constructor, may leave
	*
	*	@param -
	*	@return -
	*/
	void ConstructL ();

	/** Cleanup
	*
	*	@param -
	*	@return -
	*/
	void Cleanup ();
	
    /** AsyncRotate
	*
	*	@param - 
	*	@return -
	*/
    static TInt AsyncRotate (TAny* aThis);

    /** AsyncLosslessRotate
	*
	*	@param - 
	*	@return -
	*/
    static TInt AsyncLosslessRotate (TAny* aThis);

    /** DoRotateL
	*
	*	@param - 
	*	@return -
	*/
    void DoRotateL ();

    /** DoLosslessRotateL
	*
	*	@param - 
	*	@return -
	*/
    void DoLosslessRotateL ();

	/** GetSourceImagePropertiesL
	*
	*	@param - 
	*	@return -
	*/
    void GetSourceImagePropertiesL ( const TDesC& aFileName );

	/** PostRotate
	*
	*/
    void PostRotate ();

//@}

/** @name Members:*/
//@{

    /// File Server session handle
    RFs& 						iFsSession;

    /// Source and target files
    TFileName					iSourceFileName;
    TFileName					iTargetFileName;

    /// Current rotate mode 
    TInt        				iRotationMode;

    /// Rotator
    CJpRotate*                  iRotator;

    /// Do we update EXIF data or not?
    TBool 						iHandleExifData;

    /// Quality factor of the original image
    TUint						iOriginalJpegQualityFactor;

    /// Size of the source image
    TSize 						iSourceImageSize;

    /// 
    TRequestStatus *			iCallerStatus;

    ///
    CAsyncCallBack *			iCallBack;

    ///
    TCallBack                   iCb;

    /// Lossless mode
    TBool                       iLosslessMode;

    /// Cancel flag
    TBool                       iCancelled;
    
#if defined (LOG_TIMING)
    TTime                       iStartTime;
    TTime                       iSavingStartTime;
    TTime                       iFinishTime;
#endif


//@}

};

#endif


// End of File
