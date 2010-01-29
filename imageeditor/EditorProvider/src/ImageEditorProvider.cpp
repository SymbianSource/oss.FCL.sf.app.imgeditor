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
#include <eikmenup.h>
#include <eikenv.h>
#include <eikproc.h>
#include <bautils.h>
#include <e32property.h>

#include <ImageEditorProviderInternal.rsg>
#include <aiwmenu.h>
#include <ImplementationProxy.h>
#include <aknutils.h>
#include <ImageConversion.h>
#include <AknOpenFileService.h>

#include "ImageEditorProvider.h"
#include "ImageEditorProvider.hrh"

#include "ImageEditorUids.hrh"
#include "ImageEditorUtils.h"
#include "commondefs.h"

#include <AiwCommon.hrh>
#include <AiwGenericParam.hrh>
#include <AiwCommon.h>
#include <eikstart.h>
#include <PathInfo.h>
#include <apgcli.h>

// LOCAL CONSTANTS AND MACROS
//#ifdef DRM_SUPPORT
#include <caf.h>
#define DRM_FILE_ARG(x) x
//#else
//#define DRM_FILE_ARG(x)
//#endif


// debug log
#include "imageeditordebugutils.h"

using namespace ContentAccess;

_LIT(KEditorServiceProviderLogFile,"EditorServiceProvider.log");

// resource file
_LIT (KResourceFile, "\\resource\\imageeditorproviderinternal.rsc");

// application
const TUid KImageEditorUID = {UID_IMAGE_EDITOR};


//=============================================================================
CEditorProvider * CEditorProvider::NewL()
{
    LOG(KEditorServiceProviderLogFile, "CEditorProvider::NewL");

	return new (ELeave) CEditorProvider;
}

//=============================================================================
CEditorProvider::CEditorProvider()
: iResLoader(*CEikonEnv::Static()),
  iResLoaderOpen(EFalse),
  iFileServerConnected(EFalse)
{

}

//=============================================================================
CEditorProvider::~CEditorProvider()
{
	LOG(KEditorServiceProviderLogFile, "CEditorProvider::~CEditorProvider");

	iCallback = NULL;

	if (iInParamList)
		{
		iInParamList->Reset();
		delete iInParamList;
		}
	if (iOutParamList)
		{
		iOutParamList->Reset();
		delete iOutParamList;
		}
	delete iOpenFileService;
	iOpenFileService = 0;

#ifdef VERBOSE  	
    TInt count = iSharableFS.ResourceCount();   
    LOGFMT(KEditorServiceProviderLogFile, "Resource count: %d", count);
	iSharableFS.ResourceCountMarkEnd();
#endif

	delete iInputFileList;
	delete iTargetFileList;


    iSharableFS.Close();
    iResLoader.Close();
}

//=============================================================================
void CEditorProvider::InitialiseL (
	MAiwNotifyCallback &			/*aFrameworkCallback*/,
	const RCriteriaArray &			/*aInterest*/
    )
{
    LOG(KEditorServiceProviderLogFile, "CEditorProvider::InitialiseL");

    if (!iInParamList)
    {
        iInParamList = CAiwGenericParamList::NewL();    
    }

    if (!iOutParamList)
    {
        iOutParamList = CAiwGenericParamList::NewL();    
    }
    
    if (!iResLoaderOpen)
        {
        //	Load AIW edit provider internal resource file
        TFileName fileName;
        TParse p;    

        Dll::FileName(fileName);
        p.Set(KResourceFile, &fileName, NULL);
        iResourceFile = p.FullName();
        BaflUtils::NearestLanguageFile( CEikonEnv::Static()->FsSession(), iResourceFile );

        LOGFMT(KEditorServiceProviderLogFile, "\tLoading resource file: %S", &iResourceFile);

		iResLoader.OpenL ( iResourceFile );
        iResLoaderOpen = ETrue;

        LOG(KEditorServiceProviderLogFile, "\tResources loaded succesfully");
        }

    // Connect to file server     
    if (!iFileServerConnected)
    {
        User::LeaveIfError(iSharableFS.Connect());    
        iFileServerConnected = ETrue;

#ifdef VERBOSE        
        iSharableFS.ResourceCountMarkStart();
#endif

        // Share file server sessions with other processes. Needed for Open File service.
        User::LeaveIfError(iSharableFS.ShareProtected());
    }   
    
    // Publish & Subscribe API used for delivering document name to AIW provider
    TInt err = RProperty::Define(KImageEditorProperties, EPropertyFilename, RProperty::EText);
    
    if (err != KErrAlreadyExists)
    {
        User::LeaveIfError(err);   
    }
    
    LOG(KEditorServiceProviderLogFile, "\tInitialized.");
}

//=============================================================================
void CEditorProvider::InitializeMenuPaneL (
    CAiwMenuPane &                  aMenuPane,
    TInt                            aIndex,
    TInt                            /*aCascadeId*/,
    const CAiwGenericParamList &    aInParamList
    )
    {
	LOG(KEditorServiceProviderLogFile, "CEditorProvider::InitializeMenuPaneL: In");

	if (BaflUtils::FileExists(iSharableFS, iResourceFile))
		{
		// Check how many input files & examine the MIME types
		CDesCArray* fileList = CheckInputFilesLC( aInParamList );
		RArray<TDataType> mimeTypeList;
		CleanupClosePushL(mimeTypeList);

		if( fileList && fileList->Count())
			{
			// If we support multiple selection, all the files are opened here.
			// This migh be very slow.
			TInt inputFileCount = fileList->MdcaCount();

			LOGFMT(KEditorServiceProviderLogFile, "CEditorProvider::InitializeMenuPaneL: inputFileCount: %d", inputFileCount);

			// Get the MIME type(s) of the file(s)
			RApaLsSession lsSession;
			TInt err = lsSession.Connect();
			CleanupClosePushL (lsSession);
			for( TInt i = 0; i < inputFileCount; i++ )
				{
				TDataRecognitionResult dataType;
				RFile file;
				err = file.Open (iSharableFS, fileList->MdcaPoint(i), EFileShareReadersOnly);
				if (KErrNone != err)
					{
					// second try...
					err = file.Open (iSharableFS, fileList->MdcaPoint(i), EFileShareAny);
					}
				if (KErrNone == err)
					{
					CleanupClosePushL( file );
					User::LeaveIfError( lsSession.RecognizeData(file, dataType) );
					mimeTypeList.AppendL( dataType.iDataType );
					CleanupStack::PopAndDestroy(); // file
					}
				else
					{
					mimeTypeList.AppendL( TDataType() );
					}
				}
			CleanupStack::PopAndDestroy(); // lsSession

			LOG(KEditorServiceProviderLogFile, "CEditorProvider::InitializeMenuPaneL: AIW parameter list content analyzed");

			// Based on the MIME type, decice whether we support this file.
			// Show the menu only if the single file is selected
			if( (fileList->MdcaCount() == 1) && IsSupportedImageFile(mimeTypeList[0]) )
				{
				// Check that file is supported by ICL
				TPtrC filename = fileList->MdcaPoint(0);
				TRAPD(err, 
					CImageDecoder* decoder = CImageDecoder::FileNewL(iSharableFS, filename);
					delete decoder;
					);

				// Check if file is DRM protected
				if ( !CheckDRMProtectionL( filename ) &&
					err == KErrNone)
					{
					aMenuPane.AddMenuItemsL (
						iResourceFile, 
						R_EDITORPROVIDER_MENU,
						KAiwCmdEdit, 
						aIndex);
					}
				}
			}

		CleanupStack::PopAndDestroy(2); // fileList, mimeTypeList
		}

	LOG(KEditorServiceProviderLogFile, "CEditorProvider::InitializeMenuPaneL: Out");
	}

//=============================================================================
void CEditorProvider::HandleServiceCmdL (
    const TInt &                    aCmdId,
    const CAiwGenericParamList &    aInParamList,
    CAiwGenericParamList &          aOutParamList,
    TUint                           aCmdOptions,
    const MAiwNotifyCallback *		aCallback
    )
{
    LOGFMT(KEditorServiceProviderLogFile, "CEditorProvider::HandleServiceCmdL (%d)", aCmdId);
    HandleCmdsL(aCmdId, aInParamList, aOutParamList, aCmdOptions, aCallback);
}

//=============================================================================
void CEditorProvider::HandleMenuCmdL (
    TInt                            aMenuCmdId,
    const CAiwGenericParamList &	aInParamList,
    CAiwGenericParamList &          aOutParamList,
    TUint                           aCmdOptions,
    const MAiwNotifyCallback *      aCallback
    )
{
    LOGFMT(KEditorServiceProviderLogFile, "CEditorProvider::HandleMenuCmdL (%d)", aMenuCmdId);

    HandleCmdsL(aMenuCmdId, aInParamList, aOutParamList, aCmdOptions, aCallback);
}

//=============================================================================
void CEditorProvider::HandleCmdsL (
    TInt                            aCmdId,
    const CAiwGenericParamList &	aInParamList,
    CAiwGenericParamList &          aOutParamList,
    TUint                           /*aCmdOptions*/,
    const MAiwNotifyCallback *      aCallback
    )
{
	switch (aCmdId)
	{
	    case KAiwCmdEdit:
		case EEditorProviderCmdIdEdit:
		{
			// Check how many input files
			delete iInputFileList;
			iInputFileList = NULL;
			iInputFileList = CheckInputFilesLC( aInParamList );
			CleanupStack::Pop(iInputFileList);
			CheckAiwCallBackL( aOutParamList, aCallback );

			// "Edit" command available only if a single file is selected
			if( iInputFileList->MdcaCount() == 1 )
			{
				TPtrC filename = iInputFileList->MdcaPoint(0);
//                if( CheckDiskSpaceL(iSharableFS, filename) )
//                {
                    LaunchImageEditorL( filename, aInParamList );
//                }
//                else
//                {
//                    LOGFMT(KEditorServiceProviderLogFile, "CEditorProvider: not enough disk space to start editing %S", &filename);
//
//                    delete iInputFileList;
//                    iInputFileList = NULL;
//                    User::Leave (KErrDiskFull);
//                }
			}
			break; 
		}
		default:
		{
			break;
		}
	}
}

//=============================================================================
void CEditorProvider::CheckAiwCallBackL ( 
    CAiwGenericParamList &          aOutParamList,
    const MAiwNotifyCallback *      aCallback
	)
{
    LOG(KEditorServiceProviderLogFile, "CEditorProvider::CheckAiwCallBackL: In");

    if (aCallback)
    {
        LOG(KEditorServiceProviderLogFile, "CEditorProvider::CheckAiwCallBackL: Callback used");
        iCallback = aCallback;
		iOutParamList->Reset();
		iOutParamList->AppendL(aOutParamList);
		LOG(KEditorServiceProviderLogFile, "CEditorProvider::CheckAiwCallBackL: Using AIW call back");
		iUseCallBack = ETrue;
	}
	else
	{
		iCallback = NULL;
		iUseCallBack = EFalse;
	}
    LOG(KEditorServiceProviderLogFile, "CEditorProvider::CheckAiwCallBackL: Out");
}

//=============================================================================
CDesCArray* CEditorProvider::CheckInputFilesLC ( 
    const CAiwGenericParamList &    aInParamList
	)
{
	LOG(KEditorServiceProviderLogFile, "CEditorProvider::CheckInputFilesLC: In");

	iInParamList->Reset();
	iInParamList->AppendL(aInParamList);

	// Create array for input files.
	CDesCArray* inputFileList = new (ELeave) CDesCArraySeg(4);
	CleanupStack::PushL( inputFileList );

	// Count number of files in param list and store the file names.
	// Assuming that the files were already checked in
	// InitializeMenuPaneL; no need to check them here.
	TInt count = aInParamList.Count();
	TInt index;
	TInt fileCount = 0;
	for (index = 0; index < count; ++index)
	{
		const TAiwGenericParam& param = aInParamList[index];
		if (param.SemanticId() == EGenericParamFile)
		{
#ifndef AIW_MULTIPLE_FILE_SUPPORT		
			if (fileCount)
			{
				inputFileList->Reset();
				break;	
			}
#endif		
			// Get file name
			TPtrC fileName = param.Value().AsDes();
			inputFileList->AppendL( fileName );
			fileCount++;
			LOGFMT(KEditorServiceProviderLogFile, "\tfile: %S", &fileName);
		}
	}

	// Check that the files exist
	TBool filesExist = ETrue;
	for( TInt i = 0; i < inputFileList->MdcaCount(); i++ )
		{
		filesExist = BaflUtils::FileExists( iSharableFS, inputFileList->MdcaPoint(i) );
		if( !filesExist )
		{
			inputFileList->Reset();
			LOG(KEditorServiceProviderLogFile, "CEditorProvider::CheckInputFilesLC: file list deleted");
			break;
		}
	}

	LOG(KEditorServiceProviderLogFile, "CEditorProvider::CheckInputFilesLC: Out");
	return inputFileList;
}


//=============================================================================
void CEditorProvider::LaunchImageEditorL ( 
	const TDesC & 					aFileName,
    const CAiwGenericParamList &    /*aInParamList*/
	)
{
    LOGFMT(KEditorServiceProviderLogFile, "CEditorProvider::LaunchEditorL: file: %S", &aFileName);

#ifdef VERBOSE        
    TInt count = iSharableFS.ResourceCount();   
    LOGFMT(KEditorServiceProviderLogFile, "Resource count: %d", count);
#endif
    
    RFile fileHandle;

    TInt err = 
		fileHandle.Open (
		iSharableFS, 
		aFileName, 
		EFileWrite | EFileShareReadersOrWriters
		);
	if (KErrNone != err)
	{
		User::LeaveIfError (
			fileHandle.Open (
			iSharableFS, 
			aFileName, 
			EFileRead | EFileShareReadersOrWriters
			));
	}

    LOG(KEditorServiceProviderLogFile, "Filehandle opened");

    // fileHandle has to be closed to enable overwriting the original image
   	CleanupClosePushL (fileHandle); 

#ifdef VERBOSE        
    count = iSharableFS.ResourceCount();   
    LOGFMT(KEditorServiceProviderLogFile, "Resource count: %d", count);
#endif        

	iOpenFileService = CAknOpenFileService::NewL (
		KImageEditorUID, 
		fileHandle,
		(MAknServerAppExitObserver *)this,
		iInParamList
		);

	if (iCallback && iUseCallBack)
	{	    
	    LOG(KEditorServiceProviderLogFile, "CEditorProvider: Calling HandleNotifyL");
	    ((MAiwNotifyCallback*)iCallback)->HandleNotifyL(KAiwCmdEdit, KAiwEventStarted, *iOutParamList, *iInParamList);    
	}

#ifdef VERBOSE        
    count = iSharableFS.ResourceCount();   
    LOGFMT(KEditorServiceProviderLogFile, "Resource count: %d", count);
#endif

    CleanupStack::PopAndDestroy(); // close fileHandle 

}

//=============================================================================
void CEditorProvider::HandleServerAppExit (TInt aReason)
{
    LOGFMT(KEditorServiceProviderLogFile, "CEditorProvider::HandleServerAppExit: %d", aReason);

	delete iOpenFileService;
	iOpenFileService = 0;

	if (iCallback && iUseCallBack)
	{
	    LOG(KEditorServiceProviderLogFile, "CEditorProvider: Calling HandleNotifyL");

		// Copy the name of the created image file to the output parameter list
	    TFileName newFileName;
	    User::LeaveIfError(RProperty::Get(KImageEditorProperties, EPropertyFilename, newFileName));

		iOutParamList->Reset();
		TAiwVariant variant(newFileName);
		TAiwGenericParam param(EGenericParamFile, variant);
		iOutParamList->AppendL(param);
		   
	    // Non-leaving function shall use TRAP
	    TRAP_IGNORE ( 
		LOG(KEditorServiceProviderLogFile, "CEditorProvider: Call HandleNotifyL 1");
	        ((MAiwNotifyCallback*)iCallback)->HandleNotifyL(KAiwCmdEdit, KAiwEventCompleted, *iOutParamList, *iInParamList);    	    	    
		LOG(KEditorServiceProviderLogFile, "CEditorProvider: Call HandleNotifyL 2");
	        ((MAiwNotifyCallback*)iCallback)->HandleNotifyL(KAiwCmdEdit, KAiwEventStopped, *iOutParamList, *iInParamList);    
		LOG(KEditorServiceProviderLogFile, "CEditorProvider: Call HandleNotifyL 3");
	    );

		// Reset new filename property and out paramlist
        User::LeaveIfError(RProperty::Set(KImageEditorProperties, EPropertyFilename, KNullDesC));
        iOutParamList->Reset();
	}
    
    MAknServerAppExitObserver::HandleServerAppExit(aReason);
        
#ifdef VERBOSE  
    TInt count = iSharableFS.ResourceCount();   
    LOGFMT(KEditorServiceProviderLogFile, "Resource count: %d", count);
#endif

}

//=============================================================================
TBool CEditorProvider::CheckDRMProtectionL(const TDesC& DRM_FILE_ARG(aFileName)) const
{
    LOG(KEditorServiceProviderLogFile, "CEditorProvider::CheckDRMProtectionL: In");

    //: Replace DRM checking with DRM utility for better performance once it's available
    
    TBool isDRMProtected = EFalse;

    // Create CContent-object
    CContent* pContent = CContent::NewLC(aFileName); 
    
    // See if the content object is protected
    User::LeaveIfError( pContent->GetAttribute( EIsProtected, isDRMProtected ) );
        	
	CleanupStack::PopAndDestroy (pContent);

	LOGFMT(KEditorServiceProviderLogFile, "CEditorProvider::CheckDRMProtectionL: Out (result: %d)", isDRMProtected);

    return isDRMProtected;
}

//=============================================================================
TBool CEditorProvider::IsSupportedImageFile (const TDataType& aDataType) const
{
	_LIT(KMimeAllImages, "image/");
	
	TBool supported = EFalse;
	
	// CompareC return 0 if equal
	if ( aDataType.Des().Left(6).CompareF( KMimeAllImages ) )
    {
        supported = EFalse;
    }
    else
    {
        supported = ETrue;
    }
	
	return supported;
}

//=============================================================================
TBool CEditorProvider::IsJpeg (const TDataType& aDataType) const
{
	_LIT(KMimeJpeg, "image/jpeg");

	// CompareC return 0 if equal
	return 0 == ( aDataType.Des().Left(10).CompareF( KMimeJpeg ) );
}

//=============================================================================
TBool CEditorProvider::CheckDiskSpaceL(RFs& aFsSession, const TDesC& aFileName) const
{
    LOG(KEditorServiceProviderLogFile, "CEditorProvider::CheckDiskSpaceL");

    TBool enoughFreeSpace = ETrue;

    TFileName targetDrive = PathInfo::MemoryCardRootPath();

    if (!ImageEditorUtils::ImageFitsToDriveL (aFsSession, aFileName, targetDrive))
    {
        targetDrive = PathInfo::PhoneMemoryRootPath();

        if (!ImageEditorUtils::ImageFitsToDriveL (aFsSession, aFileName, targetDrive))
        {
            enoughFreeSpace = EFalse;
        }
    }

    return enoughFreeSpace;
}

//=============================================================================
TBool CEditorProvider::CheckDiskSpaceL(RFs& aFsSession, const CDesCArray* aFileList) const
{
    LOG(KEditorServiceProviderLogFile, "CEditorProvider::CheckDiskSpaceL");

    // Check if the files fit onto the drive. Do this initial check assuming
    // that each rotated file is saved to the same drive than the original file
	return ImageEditorUtils::ImagesFitToDriveL (aFsSession, *aFileList, *aFileList);
}









// End of File
