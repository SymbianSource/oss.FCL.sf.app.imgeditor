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


#ifndef JPEGROTATOR_H
#define JPEGROTATOR_H


// INCLUDES
#include <e32base.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CJpegRotatorImpl;
class RFs;


/*  CLASS:  MJpegRotatorObserver
*
*   Observer interface, which can be optionally implemented
*   to receive progress information during the image rotation
*   (in block mode only).
*
*/
class MJpegRotatorObserver
{
public:

    /**
    *   Informs the caller that one set of image data blocks has been 
    *   processed, so the progress bar should be incremented. 
    *   The size of the progress increment is returned by RotateImageL 
    *
    *   @param - 
    *   @param - 
    */
    virtual void Increment() = 0;
};



/*  CLASS:  CJpegRotator
*
*    Rotates an image in 90 degrees steps.
*    Available rotation modes:
*    
*    - ERotModeCounterClockwise,
*    - ERotModeClockwise,
*    - ERotMode90,
*    - ERotMode180,
*    - ERotMode270
*
*
*   USAGE
*
*   Example 1: asynchronous rotate
*
*   iJpegRotator = CJpegRotator::NewL( iEikonEnv->FsSession() ); 
*   
*   // Launch wait dialog
*   iWaitDialog = new (ELeave) CAknWaitDialog (
*       reinterpret_cast<CEikDialog**>(&iWaitDialog),
*       ETrue );
*       iWaitDialog->SetTone( CAknNoteDialog::EConfirmationTone );
*       iWaitDialog->ExecuteLD( R_WAIT_DIALOG );
*
*   // Start waiting active object. Inthis example, we have an dedicated AO
*   // to wait for the completion, but of course the caller could itself be the AO.
*   delete iWaiter;
*   iWaiter = new (ELeave) CJpegRotatorTestWaiter (*this);
*   iWaiter->StartWaitL();
*
*   // Start rotate
*   iJpegRotator->RotateImageL( (iWaiter->iStatus), iSource, iTarget, CJpegRotator::ERotModeClockwise );
*   ...
*   ...
*   // When the rotate is finished, (iWaiter->iStatus) will be completed with KErrNone,
*   // or an error code, if the rotate failed or was cancelled.
*
*
*/
class CJpegRotator : public CBase
{
    
public:

/** @name Typedefs:*/
//@{
    enum TRotationMode
    {
        ERotModeCounterClockwise  = 0,
        ERotModeClockwise         = 1,
        ERotMode90                = ERotModeCounterClockwise,
        ERotMode180               = 2,
        ERotMode270               = ERotModeClockwise,
        ERotMode0                 = 3
    };
//@}

/** @name Methods:*/
//@{

    /** NewLC factory method, does not pop cleanup stack
    *
    *   @param - aFsSession
    *   @param - aObserver 
    *   @return pointer to created CJpegRotator object
    */
    IMPORT_C static CJpegRotator * NewLC ( RFs & aFsSession );
    
    /** NewL factory method, pops cleanup stack
    *
    *   @param - aFsSession
    *   @param - aObserver 
    *   @return pointer to created CJpegRotator object
    */
    IMPORT_C static CJpegRotator * NewL ( RFs & aFsSession );

    /** Destructor
    *
    *   @param -
    *   @return -
    */
    IMPORT_C ~CJpegRotator ();

    /** RotateImageL
    *
    *   Rotates the JPEG image, asynchronous one-block version.
    *
    *   Lossless rotate mode is used by default
    *
    *   Starts the rotate. After the rotation is completed, the
    *   supplied TRequestStatus is completed. 
    *
    *   If the rotation fails, the request is completed with the error code, 
    *   KErrNone otherwise. The function may also leave, if the error happens
    *   during the pre-rotate check phase.
    *
    *   Possible error codes include:
    *     - KErrArgument if the rotation argument is out of range.
    *     - KErrNotFound if the source file does not exist.
    *     - KErrCorrupt if the source image could not be read.
    *     - KErrNotSupported if the image type can't be handled (not baseline JPEG)
    *     - KErrPathNotFound if the target path does not exist.
    *     - KErrAlreadyExists if the target file already exists.
    *     - Other system-wide error codes.
    *   
    *   @param aStatus - 
    *       Reference to the TRequestStatus of an active object,
    *       which will be completed when the rotation finishes.
    *   @param aRotationMode - The rotation direction
    *   @param aSourceFileName - The image to be rotated
    *   @param aTargetFileName - The target file where the rotated image will be saved
    *   @param aHandleExifData - Do we update/create the EXIF data in the JPEG image
    *   @param aForceLossyMode - Use lossy mode instead of lossless mode
    *   @return -
    */
    IMPORT_C void RotateImageL (
        TRequestStatus &			aStatus,
        const TDesC &				aSourceFileName,
        const TDesC &				aTargetFileName,
        CJpegRotator::TRotationMode aRotationMode,
        TBool						aHandleExifData = ETrue,
        TBool					 	aForceLossyMode = EFalse
        );

    /** Cancel
    *
    *   @param -
    *   @return -
    */
    IMPORT_C void Cancel ();

//@}

private:

/** @name Methods:*/
//@{

    /** Default constructor
    *   @param - 
    *   @return -
    */
    CJpegRotator ();

    /** Second phase constructor, may leave
    *
    *   @param - aFsSession
    *   @return -
    */
    void ConstructL ( RFs& aFsSession );

//@}

/** @name Members:*/
//@{

    /// Internal representation
    CJpegRotatorImpl * iImplementation;

//@}

};

#endif


// End of File
