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



// INCLUDE FILES
#include    <eikenv.h>
#include    <bautils.h>
#include    <imageconversion.h>

#include    "JpegRotatorImpl.h"
#include    "JpegRotator.h"
#include    "CJpRotate.h"

//  debug log
#include "imageeditordebugutils.h"

// for timing log. It is separate, because other logging
// would affect performance
#include    <flogger.h>

// CONSTANTS
const TInt KDefaultSaveBuffer = 1048576;  // 16384

//=============================================================================
CJpegRotatorImpl * CJpegRotatorImpl::NewL (RFs& aFsSession)
{
	CJpegRotatorImpl * self = new (ELeave) CJpegRotatorImpl (aFsSession);
	CleanupStack::PushL (self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
}

//=============================================================================
CJpegRotatorImpl::~CJpegRotatorImpl ()
{

    if (iRotator)
    {
        delete iRotator;
    }

    if (iCallBack)
    {
        iCallBack->Cancel();
        delete iCallBack;
    }
    
}

//=============================================================================
void CJpegRotatorImpl::Cleanup ()
{
    LOG( KJpegRotatorLogFile, "CJpegRotatorImpl::Cleanup");

    iSourceFileName = KNullDesC();
    iTargetFileName = KNullDesC();
    iRotationMode = CJpegRotator::ERotModeCounterClockwise;
    iCallerStatus = NULL;
    iSourceImageSize.SetSize(0,0);
    iHandleExifData = ETrue;
    iCancelled = EFalse;

#if defined (LOG_TIMING)
    iLosslessMode = EFalse;
#endif
}

//=============================================================================
void CJpegRotatorImpl::Cancel ()
{
    if (iRotator)
    {
        iRotator->Cancel();    
    }
    
	iCancelled = ETrue;
}

//=============================================================================
void CJpegRotatorImpl::PrepareRotateFileL (
    const TDesC& aSourceFileName,
    const TDesC& aTargetFileName,
    CJpegRotator::TRotationMode aRotationMode,
    TBool aHandleExifData)
{
    LOG( KJpegRotatorLogFile, "CJpegRotatorImpl::PrepareRotateFileL");

#if defined (LOG_TIMING)
    iStartTime.UniversalTime();
#endif

    // Reset old values
    Cleanup();

    // Check that the source file exists
    if ( !BaflUtils::FileExists (iFsSession, aSourceFileName) )
    {
        User::Leave (KErrNotFound);
    }
    // Check that the target file does not exist
    if ( BaflUtils::FileExists (iFsSession, aTargetFileName) )
    {
        User::Leave (KErrAlreadyExists);
    }
    // Check that the target path exists
    TParsePtrC parse (aTargetFileName);
    if ( !BaflUtils::FolderExists (iFsSession, parse.DriveAndPath()) )
    {
        User::Leave (KErrPathNotFound);
    }

    // Check & store the rotate parameter
    if ( CJpegRotator::ERotModeCounterClockwise == aRotationMode )
    {
        iRotationMode = CJpegRotator::ERotModeCounterClockwise;
    }
    else if ( CJpegRotator::ERotModeClockwise == aRotationMode )
    {
        iRotationMode = CJpegRotator::ERotModeClockwise;
    }
    else if ( CJpegRotator::ERotMode180 == aRotationMode )
    {
        iRotationMode = CJpegRotator::ERotMode180;
    }
    else
    {
        User::Leave (KErrArgument);
    }


    // Store the source file name and set target name
    iSourceFileName = aSourceFileName;
    iTargetFileName = aTargetFileName;
    iHandleExifData = aHandleExifData;

    // Store the JPEG quality factor etc.
    GetSourceImagePropertiesL ( iSourceFileName );
}

//=============================================================================
void CJpegRotatorImpl::StartAsyncRotate (TRequestStatus& aStatus, TBool aForceLossyMode)
{
    LOG( KJpegRotatorLogFile, "CJpegRotatorImpl::StartAsyncRotate");

    // Use lossless rotate if possible
    if( !aForceLossyMode && IsLosslessRotatePossible() )
    {
        StartAsyncLosslessRotate (aStatus);
    }

    else 
    {
        aStatus = KRequestPending;
        iCallerStatus = &aStatus;

        iCb = TCallBack(CJpegRotatorImpl::AsyncLosslessRotate, this);
          
        iCallBack = new CAsyncCallBack (iCb, CActive::EPriorityStandard);
                
        if (iCallBack)
        {
        	iCallBack->CallBack();        	
        }
        else
        {
        	FinishAsyncRotate (KErrNoMemory); 
        }

    }
    
}

//=============================================================================
void CJpegRotatorImpl::StartAsyncLosslessRotate (TRequestStatus& aStatus)
{
    LOG( KJpegRotatorLogFile, "CJpegRotatorImpl::StartAsyncLosslessRotate");

    aStatus = KRequestPending;
    iCallerStatus = &aStatus;
    
    iCb = TCallBack(CJpegRotatorImpl::AsyncLosslessRotate, this);
    // Create the callback utility used with the asynchronous version of rotate
	iCallBack = new CAsyncCallBack (iCb, CActive::EPriorityStandard);
	
    if (iCallBack)
    {
    	iCallBack->CallBack();        	
    }
    else
    {
    	FinishAsyncRotate (KErrNoMemory); 
    }

}

//=============================================================================
TInt CJpegRotatorImpl::AsyncRotate (TAny* /*aThis*/)
{
    LOG( KJpegRotatorLogFile, "CJpegRotatorImpl::AsyncRotate");
 /*
    // In the asynchronous version, trap the rest of the functions 
    // to make sure that the caller's TRequestStatus is always 
    // completed, also in case of failures.
    CJpegRotatorImpl* impl = static_cast<CJpegRotatorImpl*>(aThis);

    // Rotate-in-one-go, lossy mode
    TRAPD (err, impl->DoRotateL());
    if (err == KErrNone)
    {
        TRAP (err, impl->SaveImageL())
    }
    impl->FinishAsyncRotate (err);
*/
    return KErrNotSupported;
}

//=============================================================================
TInt CJpegRotatorImpl::AsyncLosslessRotate (TAny* aThis)
{
    LOG( KJpegRotatorLogFile, "CJpegRotatorImpl::AsyncLosslessRotate");

    // In the asynchronous version, trap the rest of the functions 
    // to make sure that the caller's TRequestStatus is always 
    // completed, also in case of failures.
    CJpegRotatorImpl* impl = static_cast<CJpegRotatorImpl*>(aThis);

    // Rotate-in-one-go
    TRAPD (err, impl->DoLosslessRotateL());

    impl->FinishAsyncRotate (err);    

    return KErrNone;
}

//=============================================================================
void CJpegRotatorImpl::FinishAsyncRotate (TInt aError)
{
    LOGFMT( KJpegRotatorLogFile, "CJpegRotatorImpl::FinishAsyncRotate (error: %d)", aError);

	if( !iCancelled )
	{
		User::RequestComplete (iCallerStatus, aError);
	}
	else
	{
		Cleanup();
	}

    PostRotate();
}

//=============================================================================
void CJpegRotatorImpl::DoRotateL ()
{
    User::Leave(KErrNotSupported);
}

//=============================================================================
void CJpegRotatorImpl::DoLosslessRotateL ()
{
    LOG( KJpegRotatorLogFile, "CJpegRotatorImpl::DoLosslessRotateL");

    iLosslessMode = ETrue;

#if defined (LOG_TIMING)
    iSavingStartTime.UniversalTime();
#endif

	//	Load JPEG file to source file buffer

	RFile srcfile;
	CleanupClosePushL (srcfile);
	User::LeaveIfError ( srcfile.Open (iFsSession, iSourceFileName, EFileRead) );

	TInt sourcebufsize;
	User::LeaveIfError ( srcfile.Size(sourcebufsize) );
	TInt targetbufsize = (TInt)(1.2 * sourcebufsize);

    TInt saveBufferSize = KDefaultSaveBuffer;
    if (targetbufsize < saveBufferSize)
    {
        saveBufferSize = targetbufsize;
    }
    
    CleanupStack::PopAndDestroy();

	RFile trgfile;
	User::LeaveIfError ( trgfile.Replace (iFsSession, iTargetFileName, EFileRead) );
    CleanupClosePushL (trgfile);
  	
    CJpRotate* rotator = CJpRotate::NewLC(iFsSession, &trgfile, saveBufferSize);

	rotator->SetCallBack( this );

    switch ( iRotationMode )
    {
        case CJpegRotator::ERotModeCounterClockwise:
        {
            rotator->RotateL( iSourceFileName, true, false, false );
            break;
        }
        case CJpegRotator::ERotMode180:
        {
            rotator->RotateL( iSourceFileName, false, true, true );
            break;
        }
        case CJpegRotator::ERotModeClockwise:
        default:
        {
            rotator->RotateL( iSourceFileName, true, true, true );
            break;
        }
       
    }

    CleanupStack::PopAndDestroy(2);

    LOG( KJpegRotatorLogFile, "\t...DoLosslessRotateL: Done.");

}

//=============================================================================
void CJpegRotatorImpl::JpRotateStatus( TInt /*aCount*/, TInt /*aTotal*/ )
{
//    LOG( KJpegRotatorLogFile, "CJpegRotatorImpl::JpRotateStatus" );
//    LOGFMT( KJpegRotatorLogFile, "   count: %d", aCount );
//    LOGFMT( KJpegRotatorLogFile, "   total: %d", aTotal );   
}


//=============================================================================
CJpegRotatorImpl::CJpegRotatorImpl (RFs& aFsSession) : 
iFsSession (aFsSession),
iRotator (NULL), 
iHandleExifData (ETrue)
{
}

//=============================================================================
void CJpegRotatorImpl::ConstructL ()
{
    LOG( KJpegRotatorLogFile, "CJpegRotatorImpl::ConstructL");


}

//=============================================================================
void CJpegRotatorImpl::GetSourceImagePropertiesL ( const TDesC& /*aFileName*/ )
{
    /*
    //	Create a new image decoder
    CImageDecoder * decoder = CImageDecoder::FileNewL (iFsSession, aFileName);
    CleanupStack::PushL( decoder );

    //  Get reference to frame image data
    const CFrameImageData & imageData = decoder->FrameData();

    //  Get JPEG quality factor
    iOriginalJpegQualityFactor = KDefaultSavedJpegQuality;
    if ( imageData.ImageDataCount() > 0 )
    {
        iOriginalJpegQualityFactor = ((const TJpegImageData*)imageData.GetImageData(0))->iQualityFactor;
    }

    //  Image resolution
    const TFrameInfo& frameInfo = decoder->FrameInfo();
    iSourceImageSize = frameInfo.iOverallSizeInPixels;

    CleanupStack::PopAndDestroy( decoder );
    */
}

//=============================================================================
void CJpegRotatorImpl::SetJpegCommentL (const TDesC8& /*aComment*/)
{
    LOG( KJpegRotatorLogFile, "CJpegRotatorImpl::SetJpegCommentL");

}

//=============================================================================
void CJpegRotatorImpl::SetExifData (TUint8* /*aExifData*/, TUint /*aDataSize*/)
{
    LOG( KJpegRotatorLogFile, "CJpegRotatorImpl::SetExifData");

}

//=============================================================================
void CJpegRotatorImpl::PostRotate ()
{
#if defined (LOG_TIMING)
/*
    iFinishTime.UniversalTime();

    TInt64 prepareTime = iSavingStartTime.Int64() - iStartTime.Int64();
    TInt64 savingTime = iFinishTime.Int64() - iSavingStartTime.Int64();
    TInt64 totalTime = iFinishTime.Int64() - iStartTime.Int64();

    TBuf<128> text1;
    if (iLosslessMode)
    {
        text1 = _L("Rotating from %S to %S. Lossless rotate was used.");
    }
    else
    {
        text1 = _L("Rotating from %S to %S. Lossy rotate was used.");
    }

	RFileLogger::WriteFormat( 
        KImageEditorLogDir,
        KJpegRotatorTimingLogFile,
        EFileLoggingModeAppend,
        text1, &iSourceFileName, &iTargetFileName 
        );

    RFileLogger::WriteFormat( 
        KImageEditorLogDir,
        KJpegRotatorTimingLogFile,
        EFileLoggingModeAppend,
        _L("\tTime taken (microseconds): %d"),
        totalTime );

	RFileLogger::WriteFormat( 
        KImageEditorLogDir,
        KJpegRotatorTimingLogFile,
        EFileLoggingModeAppend,
        _L( "\tprepare phase: %d"),
        prepareTime );

	RFileLogger::WriteFormat( 
        KImageEditorLogDir,
        KJpegRotatorTimingLogFile,
        EFileLoggingModeAppend,
        _L( "\tsave phase: %d"),
        savingTime );
*/
#endif
}

//=============================================================================
TBool CJpegRotatorImpl::IsLosslessRotatePossible() const
{
    return ETrue;
}


// End of File
