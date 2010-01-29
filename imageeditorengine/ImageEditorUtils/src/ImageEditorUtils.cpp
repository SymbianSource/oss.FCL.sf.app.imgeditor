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


// INCLUDES
#include <f32file.h>
#include <bautils.h>
#include <eikenv.h>
#include <badesca.h>
#include <PathInfo.h>
#include <sysutil.h> 
#include <s32file.h>

#include "ImageEditorUtils.h"
//#include "ImageEditorUids.hrh"
#include "ImageEditorError.h"
#include "commondefs.h"
#include "imageeditordebugutils.h"

#include <utf.h>
#include <Etel3rdParty.h>


// CONSTANTS
_LIT (KEditedSuffix, "-");
const TInt KCopyBufferSize = 10000;



//=============================================================================
EXPORT_C void ImageEditorUtils::NotifyNewMediaDocumentL (
    RFs& , 
    const TDesC& aFileName )
{
    LOGFMT(KImageEditorLogFile, "ImageEditorUtils::NotifyNewMediaDocumentL( %S )", &aFileName );
}

//=============================================================================
EXPORT_C void ImageEditorUtils::NotifyNewMediaDocumentL (
    RFs& , 
    const MDesCArray& aFileNameArray )
{
    LOGFMT(KImageEditorLogFile, "ImageEditorUtils::NotifyNewMediaDocumentL: %d files", aFileNameArray.MdcaCount() );
}

//=============================================================================

EXPORT_C TInt ImageEditorUtils::GenerateNewDocumentNameL (
    RFs& aFsSession, 
    const TDesC& aSourceFileName, 
    TDes& aTargetFileName,
    RArray<TInt>* /*aMgAlbumIdList*/,
    const CDesCArray* aReservedFileNames,
    ImageEditorUtils::TMemorySelection aMemoryInUse )
{
    TInt err = KErrNone;
    TPtrC srcFileName = aSourceFileName.Left( 155 );
    LOG(KImageEditorLogFile, "ImageEditorUtils::GenerateNewDocumentNameL" );

    //	Set file name to parser
	TParsePtrC fileParse ( srcFileName );

    //  Test filename is already too long
    if (fileParse.NameAndExt().Length() > KMaxFileName - 5)
    {
        err = KSIEEOpenFile;
    }

    // Otherwise proceed to generate the filename
    else
    {
        //  Find file suffix that is not yet used 
        TInt val = 1;
        TFileName temp;
        TMemoryLocation memoryToSaveImage = EMixed;
        
        switch (aMemoryInUse)
        {
            case ESelectPhone:
            {
                memoryToSaveImage = EPhone;
                break;
            }
            case ESelectMmc:
            {
                memoryToSaveImage = EMmc;
                break;
            }
            case ESelectAutomatic:
            default:
            {
                memoryToSaveImage = EMixed;
                break;   
            }
        }

        TFileName driveAndPath;

        if (memoryToSaveImage == EPhone)
       	{
	        driveAndPath.Copy( PathInfo::PhoneMemoryRootPath() );
	        driveAndPath.Append( PathInfo::ImagesPath() );
       	}
       	else if (memoryToSaveImage == EMmc)
       	{
	        driveAndPath.Copy( PathInfo::MemoryCardRootPath() );
	        driveAndPath.Append( PathInfo::ImagesPath() );
       	}
       	else // ESelectAutomatic
       	{
	        //  By default save images to memory card's images folder.
		    //  If memory card is not present, or is full save to phone memory.
	        driveAndPath.Copy( PathInfo::MemoryCardRootPath() );
	        driveAndPath.Append( PathInfo::ImagesPath() );

	        if ( memoryToSaveImage != EMmc 
	            && ( !BaflUtils::FolderExists (aFsSession, driveAndPath)
	            || !ImageFitsToDriveL (aFsSession, aSourceFileName, driveAndPath) ) )
	        {
	            driveAndPath.Copy (PathInfo::PhoneMemoryRootPath() );
	            driveAndPath.Append ( PathInfo::ImagesPath() );
		    }
       	}

        // Check that the file fits to the selected drive
        if ( !ImageFitsToDriveL( aFsSession, aSourceFileName, driveAndPath ) )
        {
            err = KSIEENotEnoughDiskSpace;
        }
        else
        {
            LOGFMT(KImageEditorLogFile, "ImageEditorUtils: Image will be saved to path: %S", &driveAndPath );

            //  Copy drive and path to temporary file name
            temp.Copy( driveAndPath );

            //  Add file name without suffix 
            TPtrC name = fileParse.Name();
            TInt offset = FindSuffix ( name );
            if (offset == KErrNotFound)
	        {
                temp.Append ( fileParse.Name() );
            }
            else
            {
                temp.Append ( name.Left (offset) );
            }
    
            temp.Append ( KEditedSuffix );
            temp.AppendNumFixedWidth (val, EDecimal, 3);
            temp.Append ( KJpegExtension ); // .jpg

            //  Increase edit number until we find a file name that is not used
            while ( FileAlreadyExistsL(aFsSession, temp, aReservedFileNames) )
            {
                ++val;
                temp.Zero();
                temp.Copy ( driveAndPath );
                if (offset == KErrNotFound)
                {
                    temp.Append ( fileParse.Name() );
                }
		        else
                {
                    temp.Append ( name.Left (offset) );
                }

                temp.Append ( KEditedSuffix );
                if (val < 1000)
                {
                    temp.AppendNumFixedWidth ( val, EDecimal, 3);
                }
                else
                {
                    temp.AppendNumFixedWidth ( val, EDecimal, 4);
                }

                temp.Append ( KJpegExtension );
            }

            //  Set document name 
            aTargetFileName.Copy ( temp );
        }
    }
    
    return err;
}

//=============================================================================
EXPORT_C TInt ImageEditorUtils::GenerateNewFileNameL (
                                        RFs& aFsSession, 
                                        const TDesC& aSourceFileName, 
                                        TDes& aTargetFileName,
                                        TFileName aDrive,
                                        const CDesCArray* aReservedFileNames )
    {
    TInt err = KErrNone;
    TPtrC srcFileName = aSourceFileName.Left( 155 );
    LOG( KImageEditorLogFile, "ImageEditorUtils::GenerateNewFileNameL" );

    //	Set file name to parser
	TParsePtrC fileParse ( srcFileName );

    //  Test filename is already too long
    if ( fileParse.NameAndExt().Length() > KMaxFileName - 5 )
        {
        err = KSIEEOpenFile;
        }   

    TFileName driveAndPath ( aDrive );
    driveAndPath.Append( PathInfo::ImagesPath() );
    TBool fitsToDrive = ETrue;
    TRAP_IGNORE( fitsToDrive = ImageFitsToDriveL( aFsSession, aSourceFileName, driveAndPath ) );
    // Check that the file fits to the selected drive
    if ( KErrNone == err && !fitsToDrive )
        {
        err = KSIEENotEnoughDiskSpace;
        }
    else
        {
        LOGFMT( KImageEditorLogFile, 
          "ImageEditorUtils: Image will be saved to path: %S", &driveAndPath );

        //  Copy drive and path to temporary file name
        TFileName temp;
        TInt val = 1;
        temp.Copy( driveAndPath );

        //  Add file name without suffix 
        TPtrC name = fileParse.Name();
        TInt offset = FindSuffix ( name );
        if ( offset == KErrNotFound )
	        {
            temp.Append ( fileParse.Name() );
            }
        else
            {
            temp.Append ( name.Left ( offset ) );
            }

        temp.Append ( KEditedSuffix );
        temp.AppendNumFixedWidth ( val, EDecimal, 3 );
        temp.Append ( KJpegExtension ); // .jpg

        //  Increase edit number until we find a file name that is not used
        while ( FileAlreadyExistsL( aFsSession, temp, aReservedFileNames ) )
            {
            ++val;
            temp.Zero();
            temp.Copy( driveAndPath );
            if ( offset == KErrNotFound )
                {
                temp.Append ( fileParse.Name() );
                }
		    else
                {
                temp.Append( name.Left ( offset ) );
                }

            temp.Append( KEditedSuffix );

            if ( val < 1000 )
                {
                temp.AppendNumFixedWidth( val, EDecimal, 3 );
                }
            else
                {
                temp.AppendNumFixedWidth( val, EDecimal, 4 );
                }

            temp.Append ( KJpegExtension );
            }

            //  Set document name 
        aTargetFileName.Copy ( temp );
        }

    return err;

    }
        
//=============================================================================
EXPORT_C TInt ImageEditorUtils::GenerateFilePathL (
    RFs& aFsSession, 
    const TDesC& aSourceFileName, 
    TDes& aTargetFileName,
    ImageEditorUtils::TMemorySelection aMemoryInUse )
{
    TInt err = KErrNone;

    LOG(KImageEditorLogFile, "ImageEditorUtils::GenerateFilePath" );

    //	Set file name to parser
	TParsePtrC fileParse (aSourceFileName);

    TFileName driveAndPath;

    if (aMemoryInUse == ESelectMmc)
    {
	    driveAndPath.Copy( PathInfo::MemoryCardRootPath() );
	    driveAndPath.Append( PathInfo::ImagesPath() );
    }
   	else
    {
	    driveAndPath.Copy( PathInfo::PhoneMemoryRootPath() );
	    driveAndPath.Append( PathInfo::ImagesPath() );
    }

    // Check that the file fits to the selected drive
    if ( KErrNone == err && !ImageEditorUtils::ImageFitsToDriveL( aFsSession, aSourceFileName, driveAndPath ) )
    {
        err = KSIEENotEnoughDiskSpace;
    }
    else
    {
        LOGFMT(KImageEditorLogFile, "ImageEditorUtils: Image will be saved to path: %S", &driveAndPath );

        //  Copy drive and path to temporary file name
        TFileName temp;
        temp.Copy( driveAndPath );
		temp.Append( aTargetFileName);
			
        //  Set document name 
        aTargetFileName.Copy ( temp );
        
        if (FileAlreadyExistsL(aFsSession, aTargetFileName, NULL))
    	{
     		err = KSIEEFileExists;
    	}
    }
    

    return err;
}

//=============================================================================
TInt ImageEditorUtils::FindSuffix ( 
    const TDesC &   aName
    )
{
    TInt offset = KErrNotFound;
    TInt l = aName.Length();

    while (l)
    {
        l--;
                
        if ( l <= (aName.Length() - 3) && aName[l] == TChar('-') )
        {
            offset = l;
            break;    
        }
        else if ( aName[l] < 0x30 || aName[l] > 0x39 )
        {
            break;                
        }
        
    }

    return offset;
}

//=============================================================================
TBool ImageEditorUtils::FileAlreadyExistsL ( 
	RFs& aFsSession, 
	const TDesC& aFileName,
	const CDesCArray* aReservedFileNames )
{
    TBool fileExists = BaflUtils::FileExists( aFsSession, aFileName );
    if (!fileExists)
    {
    	// If the file does not exist on the disk, check that it
    	// is not included in the list of explicitly reserved files
        if (aReservedFileNames)
        {
            TInt pos;
            fileExists = !aReservedFileNames->Find( aFileName, pos, ECmpFolded );
        }
    }
    return fileExists;
}

//=============================================================================
EXPORT_C TBool ImageEditorUtils::ImageFitsToDriveL ( 
    RFs& aFsSession, 
    const TDesC& aSourceFile,
    const TDesC& aTargetPath )
{
    LOG( KImageEditorLogFile, "ImageEditorUtils::ImageFitsToDriveL" );

    TBool response = EFalse;

    // Get the current file size and compute an estimate for the new image size
    TEntry entry;
    User::LeaveIfError( aFsSession.Entry (aSourceFile, entry) );
    TInt size = entry.iSize;
    size = (int)(1.25 * size + 0.5);

    // Check whether flash space is below critical
    TInt drive;
    User::LeaveIfError( RFs::CharToDrive(aTargetPath[0], drive) );
    TRAPD(err,
        response = ! SysUtil::DiskSpaceBelowCriticalLevelL (&aFsSession, size, drive);
         );

    // if MMC is not available, return EFalse
    if (err)
    {
        response = EFalse;
    }

    LOGFMT2( KImageEditorLogFile, "ImageEditorUtils::ImageFitsToDriveL: drive: %d response: %d", drive, response );

    return response;
}

//=============================================================================
EXPORT_C TBool ImageEditorUtils::ImagesFitToDriveL ( 
    RFs& aFsSession, 
    const CDesCArray& aSourceFileList,
    const CDesCArray& aTargetFileList )
{
    LOG( KImageEditorLogFile, "ImageEditorUtils::ImagesFitToDriveL" );

    TBool spacePhone = ETrue;
    TBool spaceMmc = ETrue;

    // Get size of the current files and compute an estimate
    TInt sizePhone = 0;
    TInt sizeMmc = 0;
    for( TInt i = 0; i < aSourceFileList.MdcaCount(); i++ )
    {
	    TEntry entry;
	    User::LeaveIfError( aFsSession.Entry (aSourceFileList.MdcaPoint(i), entry) );

        TInt drive;
        User::LeaveIfError( RFs::CharToDrive(aTargetFileList.MdcaPoint(i)[0], drive) );
		if (EDriveC == drive)
		{
			sizePhone += entry.iSize;
		}
		else if (EDriveE == drive)
		{
			sizeMmc += entry.iSize;
		}
		else
		{
			User::Leave(KErrArgument);
		}
    }
    sizePhone = (int)(1.25 * sizePhone + 0.5);
    sizeMmc = (int)(1.25 * sizeMmc + 0.5);

    // First check the space on phone memory
    TInt err = KErrNone;
    if (sizePhone)
    {
        TRAP(err,
            spacePhone = ! SysUtil::DiskSpaceBelowCriticalLevelL (&aFsSession, sizePhone, EDriveC);
            );
    }

    // Check space on memory card
    if (sizeMmc)
    {
        TRAP(err,
            spaceMmc = ! SysUtil::DiskSpaceBelowCriticalLevelL (&aFsSession, sizeMmc, EDriveE);
            );
    }

    // if MMC is not available, return EFalse
    if (err)
    {
        spaceMmc = EFalse;
    }

    LOGFMT( KImageEditorLogFile, "ImageEditorUtils::ImagesFitToDriveL: response: %d", (spacePhone && spaceMmc) );

    return (spacePhone && spaceMmc);
}

//=============================================================================

EXPORT_C void ImageEditorUtils::FindAlbumsForImageFileL ( 
    RArray<TInt>& aAlbumIdList,
    const TDesC&  )
{
    LOG(KImageEditorLogFile, "ImageEditorUtils::FindAlbumsForImageFileL" );
    aAlbumIdList.Reset();
    LOG(KImageEditorLogFile, "\tAlbums not supported" );
}


//=============================================================================
EXPORT_C void ImageEditorUtils::AddImageFileToAlbumL( 
    const TDesC& , 
    TInt  )
{
    LOG(KImageEditorLogFile, "ImageEditorUtils::AddImageFileToAlbumL");
    LOG(KImageEditorLogFile, "\tAlbums not supported" );
}

//=============================================================================
EXPORT_C void ImageEditorUtils::GetMakeAndModelL( TDes8& aMake, TDes8& aModel )
{
    LOG( KImageEditorLogFile, "ImageEditorUtils::GetMakeAndModelL");

    CTelephony::TPhoneIdV1 phoneId;

   LOGFMT3( KImageEditorLogFile, "ImageEditorUtils::GetMakeAndModelL: make: \"%S\", model: \"%S\", serial number: \"%S\"", &phoneId.iManufacturer, &phoneId.iModel, &phoneId.iSerialNumber);

    // Convert to 8-bit descriptors
    // In the emulator SharedData returns empty values.
    // In that case substitute with default values.
    if( phoneId.iManufacturer.Length() )
    {
        HBufC8* make8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( phoneId.iManufacturer );
        aMake = make8->Des();
        delete make8;
    }
    else
    {
        _LIT8( KDefaultMake,  "Nokia\0" );
        aMake = KDefaultMake();
    }

    if( phoneId.iModel.Length() )
    {
        HBufC8* model8 = CnvUtfConverter::ConvertFromUnicodeToUtf8L( phoneId.iModel );
        aModel = model8->Des();
        delete model8;
    }
    else
    {
        // Empty by default.
        aModel = KNullDesC8();
    }

}

//=============================================================================
EXPORT_C TInt ImageEditorUtils::CopyFile (
	RFs& aFsSession,
	const TDesC& aSourceFileName,
  	const TDesC& aDestinationFileName,
  	TBool aOverwriteTarget )
{
	LOG( KImageEditorLogFile, "ImageEditorUtils::CopyFile");

	// Check the source file
	TEntry fileEntry;
	TInt ret = aFsSession.Entry (aSourceFileName, fileEntry);

	// Check the target file
	if (KErrNone == ret)
	{
		if( BaflUtils::FileExists(aFsSession, aDestinationFileName) && !aOverwriteTarget )
		{
			ret = KErrAlreadyExists;
		}
	}

	TRAP( ret, ImageEditorUtils::DoCopyL(aFsSession, aSourceFileName, aDestinationFileName, fileEntry.iSize) );

	return ret;
}

//=============================================================================
void ImageEditorUtils::DoCopyL(
	RFs& aFsSession,
	const TDesC& aSourceFileName,
  	const TDesC& aDestinationFileName,
  	TUint aSourceFileSize )
{
	LOG( KImageEditorLogFile, "ImageEditorUtils::DoCopyL");

	// Create target file and do copying
	RFileWriteStream targetFile;
	User::LeaveIfError( targetFile.Replace (aFsSession, aDestinationFileName, EFileShareExclusive) );
	targetFile.PushL();

	HBufC8* fileBuf = HBufC8::NewLC (KCopyBufferSize);
	TPtr8 fileBufPtr( fileBuf->Des() );

	// Loop thorough the source file in blocks
	for (TInt i = 0; i < aSourceFileSize; i += KCopyBufferSize)
	{
		// read a block
		if (aSourceFileSize > (TUint)(i + KCopyBufferSize))
		{
			fileBufPtr.Set( fileBuf->Des() );
        	User::LeaveIfError( aFsSession.ReadFileSection(aSourceFileName, i, fileBufPtr, KCopyBufferSize) );
		}
		else
		{
			fileBufPtr.Set ( fileBuf->Des() );
			User::LeaveIfError( aFsSession.ReadFileSection(aSourceFileName, i, fileBufPtr, ((TInt)aSourceFileSize - i)) );
		}

		// Write the block to target file
		if( fileBufPtr.Length() )
		{
			targetFile.WriteL (fileBufPtr, fileBufPtr.Length());
		}
	}

	targetFile.CommitL();
	CleanupStack::PopAndDestroy(2); // targetFile, fileBuf
}

// End of File
