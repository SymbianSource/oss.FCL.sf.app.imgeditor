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

#include "ImageEditorProvider.hrh"
#include "JpegRotatorHelper.h"
#include "ImageRotaterProvider.h"
#include "ImageEditorUids.hrh"
#include "ImageEditorUtils.h"
#include "commondefs.h"

#include <AiwCommon.hrh>
#include <AiwCommon.h>
#include <eikstart.h>
#include <PathInfo.h>
#include <apgcli.h>


#include <ErrorUI.h>

// LOCAL CONSTANTS AND MACROS
#ifdef DRM_SUPPORT
#include <DRMCommon.h>
#define DRM_FILE_ARG(x) x
#else
#define DRM_FILE_ARG(x)
#endif

#ifdef JPEG_ROTATOR_AIW_PROVIDER_SUPPORT
#define JPEG_ROTATOR_AIW_PROVIDER_SUPPORT_ARG(x) x
#else
#define JPEG_ROTATOR_AIW_PROVIDER_SUPPORT_ARG(x)
#endif

#ifdef AIW_MULTIPLE_FILE_SUPPORT
const TBool KMultipleFileSupport = ETrue;
#else
const TBool KMultipleFileSupport = EFalse;
#endif

// debug log
#include "imageeditordebugutils.h"
_LIT(KEditorServiceProviderLogFile,"EditorServiceProvider.log");

// resource file
_LIT (KResourceFile, "\\resource\\imageeditorproviderinternal.rsc");

// application


//=============================================================================
CRotateProvider * CRotateProvider::NewL()
{
    LOG(KEditorServiceProviderLogFile, "CRotateProvider::NewL");

	return new (ELeave) CRotateProvider;
}

//=============================================================================
CRotateProvider::CRotateProvider()
: iResLoader(*CEikonEnv::Static()),
  iResLoaderOpen(EFalse),
  iFileServerConnected(EFalse)
{

}

//=============================================================================
CRotateProvider::~CRotateProvider()
{
	LOG(KEditorServiceProviderLogFile, "CRotateProvider::~CRotateProvider");

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

	iFileHandle.Close();

#ifdef VERBOSE  	
    TInt count = iSharableFS.ResourceCount();   
    LOGFMT(KEditorServiceProviderLogFile, "Resource count: %d", count);
	iSharableFS.ResourceCountMarkEnd();
#endif

	delete iInputFileList;
	delete iTargetFileList;

	delete iJpegRotatorHelper;	

    iSharableFS.Close();
    iResLoader.Close();
}

//=============================================================================
void CRotateProvider::InitialiseL (
	MAiwNotifyCallback &			/*aFrameworkCallback*/,
	const RCriteriaArray &			/*aInterest*/
    )
{
    LOG(KEditorServiceProviderLogFile, "CRotateProvider::InitialiseL");

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
    
    LOG(KEditorServiceProviderLogFile, "\tInitialized.");
}

//=============================================================================
void CRotateProvider::InitializeMenuPaneL (
    CAiwMenuPane &                  aMenuPane,
    TInt                            aIndex,
    TInt                            /*aCascadeId*/,
    const CAiwGenericParamList &    aInParamList
    )
    {
	LOG(KEditorServiceProviderLogFile, "CRotateProvider::InitializeMenuPaneL: In");

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

			LOGFMT(KEditorServiceProviderLogFile, "CRotateProvider::InitializeMenuPaneL: inputFileCount: %d", inputFileCount);

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

			LOG(KEditorServiceProviderLogFile, "CRotateProvider::InitializeMenuPaneL: AIW parameter list content analyzed");
			TBool displayJpegRotatorMenu = EFalse;
			TInt count = fileList->MdcaCount();
			if( (KMultipleFileSupport && count > 0) || count == 1 )
				{
				LOGFMT(KEditorServiceProviderLogFile, "CRotateProvider::InitializeMenuPaneL: count %d ", count);
				for (TInt index = 0; index < count; ++index)
					{
					// Check if file is DRM protected
					displayJpegRotatorMenu = !CheckDRMProtectionL( fileList->MdcaPoint(index) );
					if (displayJpegRotatorMenu)
						{
						// Check that it is JPEG
						displayJpegRotatorMenu = IsJpeg( mimeTypeList[0] );
						}
					if (!displayJpegRotatorMenu)
						{
						break;
						}
					}
				}
			// Show the menu if all selected files were of supported type
			if (displayJpegRotatorMenu)
				{
			    	aMenuPane.AddMenuItemsL (iResourceFile,	R_JPEG_ROTATOR_MENU,KAiwCmdRotate,aIndex);
				}
			}
		CleanupStack::PopAndDestroy(2); // fileList, mimeTypeList
		}

	LOG(KEditorServiceProviderLogFile, "CRotateProvider::InitializeMenuPaneL: Out");
	}

//=============================================================================
void CRotateProvider::HandleServiceCmdL (
    const TInt &                    aCmdId,
    const CAiwGenericParamList &    aInParamList,
    CAiwGenericParamList &          aOutParamList,
    TUint                           aCmdOptions,
    const MAiwNotifyCallback *		aCallback
    )
{
    LOGFMT(KEditorServiceProviderLogFile, "CRotateProvider::HandleServiceCmdL (%d)", aCmdId);
    HandleCmdsL(aCmdId, aInParamList, aOutParamList, aCmdOptions, aCallback);
}

//=============================================================================
void CRotateProvider::HandleMenuCmdL (
    TInt                            aMenuCmdId,
    const CAiwGenericParamList &	aInParamList,
    CAiwGenericParamList &          aOutParamList,
    TUint                           aCmdOptions,
    const MAiwNotifyCallback *      aCallback
    )
{
    LOGFMT(KEditorServiceProviderLogFile, "CRotateProvider::HandleMenuCmdL (%d)", aMenuCmdId);

    HandleCmdsL(aMenuCmdId, aInParamList, aOutParamList, aCmdOptions, aCallback);
}

//=============================================================================
void CRotateProvider::HandleCmdsL (
    TInt                            aCmdId,
    const CAiwGenericParamList &	aInParamList,
    CAiwGenericParamList &          aOutParamList,
    TUint                           /*aCmdOptions*/,
    const MAiwNotifyCallback *      aCallback
    )
{
	switch (aCmdId)
			{
	 	  case EEditorProviderCmdIdRotateLeft:
			case EEditorProviderCmdIdRotateRight:
				{
				delete iInputFileList;
				iInputFileList = NULL;
				iInputFileList = CheckInputFilesLC( aInParamList );
				CleanupStack::Pop(iInputFileList);
				CheckAiwCallBackL( aOutParamList, aCallback );

				// Check that there is enough disk space and launch the rotator
				if( iInputFileList )
					{
             if( CheckDiskSpaceL(iSharableFS, iInputFileList) )
                {
                   LaunchJpegRotatorL( iInputFileList, aCmdId );
                }
                else
                {
                  LOG(KEditorServiceProviderLogFile, "CRotateProvider::HandleCmdsL: not enough disk space to start editing");
                  delete iInputFileList;
                  iInputFileList = NULL;
                  User::Leave (KErrDiskFull);
                }
					}

				break; 
				}
			case EEditorProviderCmdIdScale:
				{
				User::Leave(KErrNotSupported);
				break;
				}
			case EEditorProviderCmdIdNoiseReduction:
				{
				User::Leave(KErrNotSupported);
				break;
				}
			case EEditorProviderCmdIdAutoFix:
				{
				User::Leave(KErrNotSupported);
				break;
				}
				default:
				{
					break;
				}
		}
}

//=============================================================================
void CRotateProvider::CheckAiwCallBackL ( 
    CAiwGenericParamList &          aOutParamList,
    const MAiwNotifyCallback *      aCallback
	)
{
    LOG(KEditorServiceProviderLogFile, "CRotateProvider::CheckAiwCallBackL: In");

    if (aCallback)
    {
        LOG(KEditorServiceProviderLogFile, "CRotateProvider::CheckAiwCallBackL: Callback used");
        iCallback = aCallback;
		iOutParamList->Reset();
		iOutParamList->AppendL(aOutParamList);
		LOG(KEditorServiceProviderLogFile, "CRotateProvider::CheckAiwCallBackL: Using AIW call back");
		iUseCallBack = ETrue;
	}
	else
	{
		iCallback = NULL;
		iUseCallBack = EFalse;
	}
    LOG(KEditorServiceProviderLogFile, "CRotateProvider::CheckAiwCallBackL: Out");
}

//=============================================================================
CDesCArray* CRotateProvider::CheckInputFilesLC ( 
    const CAiwGenericParamList &    aInParamList
	)
{
	LOG(KEditorServiceProviderLogFile, "CRotateProvider::CheckInputFilesLC: in");

	iInParamList->Reset();
	iInParamList->AppendL(aInParamList);

	// Create array for input files.
	CDesCArray* inputFileList = new (ELeave) CDesCArraySeg(4);
	CleanupStack::PushL( inputFileList );

	// Count number of files in param list and store the file names.
	// Assuming that the files were already checked in
	// InitializeMenuPaneL; no need to check them here.
	TInt count = aInParamList.Count();
	TInt index = 0;
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
			LOG(KEditorServiceProviderLogFile, "CRotateProvider::CheckInputFilesLC: file list deleted");
			break;
		}
	}

	LOG(KEditorServiceProviderLogFile, "CRotateProvider::CheckInputFilesLC: out");
	return inputFileList;
}


//=============================================================================
void CRotateProvider::LaunchJpegRotatorL ( 
	const CDesCArray*   aInputFileList,
    TInt                aCmdId
	)
{
    LOGFMT(KEditorServiceProviderLogFile, "CRotateProvider::LaunchJpegRotatorL: number of files: %d", aInputFileList->MdcaCount());
    
    ASSERT( EEditorProviderCmdIdRotateLeft == aCmdId || EEditorProviderCmdIdRotateRight == aCmdId);

    // Notify that the rotating has been started
	if (iCallback)
	{
	    LOG(KEditorServiceProviderLogFile, "CRotateProvider: Calling HandleNotifyL");
	    ((MAiwNotifyCallback*)iCallback)->HandleNotifyL(KAiwCmdEdit, KAiwEventStarted, *iOutParamList, *iInParamList);
	}

	// Set the rotation mode
	CJpegRotator::TRotationMode mode = CJpegRotator::ERotModeCounterClockwise;
	if (EEditorProviderCmdIdRotateRight == aCmdId)
	{
		mode = CJpegRotator::ERotModeClockwise;
	}

	// Start the asyncronous rotate operation
    delete iTargetFileList;
    iTargetFileList = NULL;
    iTargetFileList = new (ELeave) CDesCArraySeg(4);
	delete iJpegRotatorHelper;
	iJpegRotatorHelper = NULL;
	iJpegRotatorHelper = new (ELeave) CJpegRotatorHelper( iSharableFS, *this );

	// Start rotate, replacing original file(s)	
	TRAPD( err, iJpegRotatorHelper->StartRotateL( aInputFileList, iTargetFileList, mode, EFalse ) ); // replace original files
	if (KErrNone != err)
	{
		iJpegRotatorHelper->Cancel();
	}

#ifdef VERBOSE        
    count = iSharableFS.ResourceCount();   
    LOGFMT(KEditorServiceProviderLogFile, "Resource count: %d", count);
#endif

}

//=============================================================================
TBool CRotateProvider::CheckDRMProtectionL(const TDesC& DRM_FILE_ARG(aFileName)) const
{
    LOG(KEditorServiceProviderLogFile, "CRotateProvider::CheckDRMProtectionL: In");

    TBool isDRMProtected = EFalse;

#ifdef DRM_SUPPORT

	DRMCommon* drm = DRMCommon::NewL();
	CleanupStack::PushL (drm);
	drm->IsProtectedFile( aFileName, isDRMProtected );
	CleanupStack::PopAndDestroy (drm);

#endif

	LOGFMT(KEditorServiceProviderLogFile, "CRotateProvider::CheckDRMProtectionL: Out (result: %d)", isDRMProtected);

    return isDRMProtected;
}


//=============================================================================
TBool CRotateProvider::IsJpeg (const TDataType& aDataType) const
{
	_LIT(KMimeJpeg, "image/jpeg");

	// CompareC return 0 if equal
	return 0 == ( aDataType.Des().Left(10).CompareF( KMimeJpeg ) );
}

//=============================================================================
TBool CRotateProvider::CheckDiskSpaceL(RFs& aFsSession, const TDesC& aFileName) const
{
    LOG(KEditorServiceProviderLogFile, "CRotateProvider::CheckDiskSpaceL");

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
TBool CRotateProvider::CheckDiskSpaceL(RFs& aFsSession, const CDesCArray* aFileList) const
{
    LOG(KEditorServiceProviderLogFile, "CRotateProvider::CheckDiskSpaceL");

    // Check if the files fit onto the drive. Do this initial check assuming
    // that each rotated file is saved to the same drive than the original file
	return ImageEditorUtils::ImagesFitToDriveL (aFsSession, *aFileList, *aFileList);
}

//=============================================================================
void CRotateProvider::RotateOperationReadyL( TInt JPEG_ROTATOR_AIW_PROVIDER_SUPPORT_ARG(aError) )
{
  LOGFMT(KEditorServiceProviderLogFile, "CRotateProvider::OperationReadyL: %d", aError);

	if (iCallback)
	{
	    LOG(KEditorServiceProviderLogFile, "CRotateProvider: Calling MAiwNotifyCallback::HandleNotifyL");

		iOutParamList->Reset();
		if( KErrNone == aError )
		{
			// Pass the new file names to the AIW consumer.
			// In error cases pass empty list.
			for( TInt i = 0; i < iTargetFileList->MdcaCount(); i++ )
			{
				TAiwVariant variant( iTargetFileList->MdcaPoint(i) );
				TAiwGenericParam param( EGenericParamFile, variant );
				iOutParamList->AppendL( param );
			}
		    // Non-leaving function shall use TRAP
    	    TRAP_IGNORE ( 
    	        ((MAiwNotifyCallback*)iCallback)->HandleNotifyL(KAiwCmdRotate, KAiwEventCompleted, *iOutParamList, *iInParamList);    	    	    
    	    );

		}
        else
        {
        
            // Show error            
            CErrorUI* errorUi = CErrorUI::NewLC(*CEikonEnv::Static());
            TBool errorShown = errorUi->ShowGlobalErrorNoteL(aError);
            if (errorShown == EFalse)
            {
                User::Leave(aError);
            }
        
            CleanupStack::PopAndDestroy();
        
            TRAP_IGNORE (    
                ((MAiwNotifyCallback*)iCallback)->HandleNotifyL(KAiwCmdRotate, KAiwEventError, *iOutParamList, *iInParamList);    
            );
        }
        	
        TRAP_IGNORE ( 	   
	        ((MAiwNotifyCallback*)iCallback)->HandleNotifyL(KAiwCmdRotate, KAiwEventStopped, *iOutParamList, *iInParamList);    
	    );
	}
	
	delete iJpegRotatorHelper;
	iJpegRotatorHelper = NULL;


}



// End of File
