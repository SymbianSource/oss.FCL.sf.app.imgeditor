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



#include <imageconversion.h>
#include <eikenv.h>
#include <bautils.h>

#include "imageeditordebugutils.h"
#include "commondefs.h"
#include "ImageEditorError.h"
#include "ImageEditorImageHandler.h"

//=============================================================================
EXPORT_C CImageEditorImageHandler * CImageEditorImageHandler::NewL ()
{
	CImageEditorImageHandler * self = new (ELeave) CImageEditorImageHandler;
	CleanupStack::PushL(self);
	self->ConstructL ();
	CleanupStack::Pop(); // self
	return self;
}

//=============================================================================
EXPORT_C CImageEditorImageHandler::~CImageEditorImageHandler ()
{
	CleanupConverters();
}	

//=============================================================================
EXPORT_C void CImageEditorImageHandler::LoadImageAsyncL (
	const TDesC &		aFileName,
	CFbsBitmap *&		aBitmap,
	TRequestStatus *	aStatus
	)
{
    LOG(KImageEditorLogFile, "CImageEditorImageHandler::LoadImageAsyncL()");

	//	Clean up old converters
	CleanupConverters();

	//	Create a new image decoder
	iImageDecoder = CImageDecoder::FileNewL (
		CEikonEnv::Static()->FsSession(), 
		aFileName
		);

	//	Create the bitmap for containing the decoded image
    TFrameInfo frame_info = iImageDecoder->FrameInfo();
    CFbsBitmap * bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL (bitmap);
    TInt err = bitmap->Create (frame_info.iOverallSizeInPixels, EColor16M) ;
    if (err != KErrNone)
        {
        LOG(KImageEditorLogFile, "CImageEditorImageHandler: Failed to create CFbsBitmap");
        User::Leave(KSIEEInternalNonRecoverable);
        }
    CleanupStack::Pop(); // bitmap
    aBitmap = bitmap;

	//	Start decoding
	iImageDecoder->Convert (aStatus, *aBitmap);
}

//=============================================================================
EXPORT_C void CImageEditorImageHandler::ContinueLoading (TRequestStatus * aStatus)
{
    iImageDecoder->ContinueConvert (aStatus);
}

//=============================================================================
EXPORT_C void CImageEditorImageHandler::SaveImageAsyncL (
	const TDesC	&		aFileName,
	const CFbsBitmap &		aBitmap,
	const TInt			aImageQuality,
	TRequestStatus *	aStatus
	)
{
    LOG(KImageEditorLogFile, "CImageEditorImageHandler::SaveImageAsyncL()");
	//	Clean up old converters
	CleanupConverters();

	//	Get MIME type based in file extension
	CFileExtensionMIMEType * mime = GetMIMETypeL (aFileName);

	//	Create a new image decoder for MIME type mime
	iImageEncoder = CImageEncoder::FileNewL (
		CEikonEnv::Static()->FsSession(), 
		aFileName,
		mime->MIMEType()
		);
	
	CFrameImageData * frame = GetFrameImageDataL (
		mime->FileExtension(),
		aImageQuality
		);

	//	Start encoding
	iImageEncoder->Convert (aStatus, aBitmap, frame);

}

//=============================================================================
EXPORT_C TInt CImageEditorImageHandler::GetJpegQualityFactorL (
    const TDesC & aFileName
	)
{
    LOG(KImageEditorLogFile, "CImageEditorImageHandler::GetJpegQualityFactorL()");

	//	Clean up old converters
	CleanupConverters();

	//	Create a new image decoder
	iImageDecoder = CImageDecoder::FileNewL (
		CEikonEnv::Static()->FsSession(), 
		aFileName
		);

    //  Get reference to frame image data
    const CFrameImageData & image_data = iImageDecoder->FrameData();

    //  Get JPEG quality factor
    TInt qualityfactor = 100;
    if ( image_data.ImageDataCount() > 0 )
    {
        qualityfactor = ((const TJpegImageData*)image_data.GetImageData(0))->iQualityFactor;
    }

	//	Clean up old converters
	CleanupConverters();

    return qualityfactor;
}

//=============================================================================
EXPORT_C TInt CImageEditorImageHandler::RenameImage (
	const TDesC	&		aFileNameSrc,
	const TDesC	&		aFileNameDst
	) const
{
	return BaflUtils::RenameFile (
		CEikonEnv::Static()->FsSession(),
		aFileNameSrc, 
		aFileNameDst, 
		0
		);
}

//=============================================================================
EXPORT_C TInt CImageEditorImageHandler::DeleteImage (const TDesC & aFileName) const
{
	return BaflUtils::DeleteFile (
		CEikonEnv::Static()->FsSession(),
		aFileName,
		0
		);
}

//=============================================================================
EXPORT_C void CImageEditorImageHandler::CleanupConverters()
{
    LOG(KImageEditorLogFile, "CImageEditorImageHandler::CleanupConverters()");

	if (iImageDecoder)
	{
        LOG(KImageEditorLogFile, "CImageEditorImageHandler::CleanupConverters(): cancelling iImageDecoder");

		iImageDecoder->Cancel();
		delete iImageDecoder;
		iImageDecoder = NULL;
	}
	if (iImageEncoder)
	{
        LOG(KImageEditorLogFile, "CImageEditorImageHandler::CleanupConverters(): cancelling iImageEncoder");

		iImageEncoder->Cancel();
		delete iImageEncoder;
		iImageEncoder = NULL;
	}
}

//=============================================================================
EXPORT_C TBool CImageEditorImageHandler::IsRajpegImageL (const TDesC & aFileName) const
{
    _LIT8 (KMimeJpeg, "image/jpeg");

    TBuf8<256> mime;
    
    RFs fs;
    User::LeaveIfError(fs.Connect());
    CleanupClosePushL(fs);
    
    CImageDecoder::GetMimeTypeFileL (fs, aFileName, mime);

	CleanupStack::PopAndDestroy();
	
#ifdef ENABLE_DEBUGLOG
    TBuf16 <100> mime16;
    mime16.Copy(mime);
    LOGFMT(KImageEditorLogFile, "CImageEditorImageHandler::IsRajpegImageL: MIME type: %S", &mime16);
#endif

    // Images with the mime type image/jpeg can be rajpeg images.
    return ( mime.CompareF (KMimeJpeg) == 0 );
}

//=============================================================================
EXPORT_C TBool CImageEditorImageHandler::IsJpegIntact (const TDesC & aFileName)
{
    //	Clean up old converters
    CleanupConverters();

    TRAPD (err,
        //	Create a new image decoder
        iImageDecoder = CImageDecoder::FileNewL (
        CEikonEnv::Static()->FsSession(), 
        aFileName
        );
    );
    CleanupConverters();
    if (err != KErrNone)
    {
        return EFalse;
    }
    else
    {
        return ETrue;
    }
}

//=============================================================================
CImageEditorImageHandler::CImageEditorImageHandler ()
{

}

//=============================================================================
void CImageEditorImageHandler::ConstructL ()
{

}


//=============================================================================
CFileExtensionMIMEType * CImageEditorImageHandler::GetMIMETypeL (
	const TDesC &	aFileName
	)
{	

	//	Extract the file extension from aFullFileName
	TParsePtrC fileparser (aFileName);
	TBuf<12> ext;
	if (fileparser.ExtPresent())
	{
		ext.Append (fileparser.Ext());
	}

	//	Get all supported mime types with file extensions
	RFileExtensionMIMETypeArray mimes;
	iImageEncoder->GetFileTypesL (mimes);

	//	Get correct mime type based on file extension
	CFileExtensionMIMEType * ret = 0;
	for (TInt i = 0; i < mimes.Count(); ++i)
	{
		if (ext == mimes[i]->FileExtension())
		{
			ret = mimes[i];
			mimes.Remove(i);
			break;
		}
	}

	//	Delete pointer array
	mimes.ResetAndDestroy();
	
	return ret;
}

//=============================================================================
CFrameImageData * CImageEditorImageHandler::GetFrameImageDataL (
	const TDesC &		aExt,
	const TInt			aQuality
	) const
{
    LOG(KImageEditorLogFile, "CImageEditorImageHandler::GetFrameImageDataL()");
    
	CFrameImageData * frameData = CFrameImageData::NewL();
	CleanupStack::PushL (frameData);

	if (aExt.CompareF (KJpegExtension))
	{
		TJpegImageData * data = new (ELeave) TJpegImageData();
		data->iQualityFactor = (100 - aQuality);
		data->iSampleScheme = TJpegImageData::EColor422;
		frameData->AppendImageData (data);	// ownership transferred
	}
	else if (aExt.CompareF (KBmpExtension))
	{
		TBmpImageData * data = new (ELeave) TBmpImageData();
		data->iBitsPerPixel = 24;
		frameData->AppendImageData (data);	// ownership transferred
	}
	else
	{

	}

	CleanupStack::Pop(); //	frameData
	return frameData;
}

// End of File
