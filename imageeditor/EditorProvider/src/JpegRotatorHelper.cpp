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
#include    "JpegRotatorHelper.h"
#include    "JpegRotator.h"
#include    "ImageEditorUtils.h"
#include    <e32panic.h>
#include    <bautils.h>
#include    <ImageEditorProviderInternal.rsg>

//  debug log
#include <imageeditordebugutils.h>
_LIT(KEditorServiceProviderLogFile,"EditorServiceProvider.log");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// Default constructor
// 
// ---------------------------------------------------------
//
CJpegRotatorHelper::CJpegRotatorHelper(
	RFs& aFsSession, 
	MJpegRotatorHelperObserver& aObserver) :
	CActive (EPriorityIdle), 
	iFsSession (aFsSession), 
	iObserver (aObserver)
	{
	CActiveScheduler::Add(this);
	}

// ---------------------------------------------------------
// Destructor
// 
// ---------------------------------------------------------
//
CJpegRotatorHelper::~CJpegRotatorHelper()
	{
	Cancel();
	delete iJpegRotator;
	}

// ---------------------------------------------------------
// Rotate
// 
// ---------------------------------------------------------
//
void CJpegRotatorHelper::StartRotateL (
	const CDesCArray* aSourceFileList,
	CDesCArray* aTargetFileList,
	CJpegRotator::TRotationMode aMode,
	TBool aPreserveOriginalFiles )
	{
	LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::StartRotateL: In");

	__ASSERT_DEBUG(!IsActive(), User::Panic(_L("CJpegRotatorHelper"),EReqAlreadyActive));

	// ownership not taken
	iTargetFileList = aTargetFileList;
	iSourceFileList = aSourceFileList;
	iMode = aMode;
	iPreserveOriginalFiles = aPreserveOriginalFiles;
	
	// instantiate JpegRotator
	delete iJpegRotator;
	iJpegRotator = NULL;
	iJpegRotator = CJpegRotator::NewL( iFsSession );

	// Generate names for the rotated images
	for( TInt i = 0; i < iSourceFileList->MdcaCount(); i++ )
		{
		TPtrC source( iSourceFileList->MdcaPoint(i) );
		
		TFileName target;
		if (!aPreserveOriginalFiles)
			{
			LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::StartRotateL: Not preserve original files");
			// If we intend to replace the original file, the generated
			// file must be on the same drive.
			TInt drive;
			User::LeaveIfError( RFs::CharToDrive(source[0], drive) );
			if (EDriveC == drive)
				{
				User::LeaveIfError( ImageEditorUtils::GenerateNewDocumentNameL( iFsSession, source, target, NULL, iTargetFileList, ImageEditorUtils::ESelectPhone) );
				}
			else if (EDriveE == drive)
				{
				User::LeaveIfError( ImageEditorUtils::GenerateNewDocumentNameL( iFsSession, source, target, NULL, iTargetFileList, ImageEditorUtils::ESelectMmc) );
				}
			else
				{
				User::Leave(KErrArgument);
				}
			}
		else
			{
			LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::StartRotateL: Preserve original files");			
			User::LeaveIfError( ImageEditorUtils::GenerateNewDocumentNameL( iFsSession, source, target, NULL, iTargetFileList) );
			}
		iTargetFileList->AppendL( target );
		}

	// Start waiting for rotator completion
	iStatus = KRequestPending;
	SetActive();

	// Start rotating the first file
	LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::StartRotateL: Starting rotate...");
	iCounter = 0;
	iJpegRotator->RotateImageL (iStatus, iSourceFileList->MdcaPoint(iCounter), iTargetFileList->MdcaPoint(iCounter), aMode );

	// Launch wait dialog
	LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::StartRotateL: Launching wait note");
	iWaitDialog = new (ELeave) CAknWaitDialog (reinterpret_cast<CEikDialog**>(&iWaitDialog), ETrue );
	iWaitDialog->PrepareLC( R_PROVIDER_WAIT_DIALOG );
	iWaitDialog->SetCallback( this );
	iWaitDialog->RunLD();

	LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::StartRotateL: Out");
	}

// ---------------------------------------------------------
// CJpegRotatorHelper::FinalizeRotatedFileL
// 
// ---------------------------------------------------------
//
void CJpegRotatorHelper::FinalizeRotatedFileL( TInt aCurrentFileIndex )
	{
	LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::FinalizeRotatedFileL");

#ifdef FILE_TIME_STAMP_UPDATE
	// Set the timestamp of the saved file to original file's timestamp + 1 second.
	// The idea is to make the original and edited images appear next to each other.
	TEntry original;
	TInt err = iFsSession.Entry( iSourceFileList->MdcaPoint(aCurrentFileIndex), original );
	if (KErrNone == err)
		{
		TTime newTime = original.iModified;

		CFileMan* fileMan = CFileMan::NewL( iFsSession );
		CleanupStack::PushL (fileMan);
		fileMan->Attribs( iTargetFileList->MdcaPoint(aCurrentFileIndex), 0, 0, newTime ); // do not set or clear anything, mofify time 
		CleanupStack::PopAndDestroy (fileMan);
		}
#endif

	if (!iPreserveOriginalFiles)
		{
		LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::FinalizeRotatedFileL 2");
		// Delete the source file and rename the temporary 
		// target file to the source file name
		TFileName sourceFile( iSourceFileList->MdcaPoint(aCurrentFileIndex) );
		TInt err = BaflUtils::DeleteFile( iFsSession, sourceFile );
		err = BaflUtils::RenameFile (
			iFsSession, 
			iTargetFileList->MdcaPoint(aCurrentFileIndex),
			sourceFile );
		User::LeaveIfError( err );

		// Replace the file name in the target file list
		iTargetFileList->Delete(aCurrentFileIndex);
		iTargetFileList->InsertL(aCurrentFileIndex, sourceFile);
		}
	}

// ---------------------------------------------------------
// CJpegRotatorHelper::RunL
// 
// ---------------------------------------------------------
//
void CJpegRotatorHelper::RunL()
	{
    LOGFMT( KEditorServiceProviderLogFile, "CJpegRotatorHelper::RunL (status: %d)", iStatus.Int());

	if( KErrNone == iStatus.Int() )
		{
		FinalizeRotatedFileL( iCounter );

		if( (iCounter + 1) < iSourceFileList->MdcaCount() )
			{
			// Start rotating the next file
			LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::RunL: Starting rotate next...");

			iStatus = KRequestPending;
			SetActive();

			iCounter++;
			iJpegRotator->RotateImageL (iStatus, iSourceFileList->MdcaPoint(iCounter), iTargetFileList->MdcaPoint(iCounter), (CJpegRotator::TRotationMode)iMode );
			}
		else
			{
			// All files rotated. Finish operation.
			delete iJpegRotator;
			iJpegRotator = NULL;
			iTargetFileList = NULL;

			// Dismiss wait dialog
		    if (iWaitDialog)
				{
				LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::RunL: deleting wait note" );
		        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
				LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::RunL: process finished" );
				delete iWaitDialog;
				LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::RunL: dialog deleted" );
				iWaitDialog = NULL;
				}

			// Notify completion to observer
			iObserver.RotateOperationReadyL ( iStatus.Int() );
			}
		}
	else
		{
		
		// Try delete failed target file
		BaflUtils::DeleteFile( iFsSession, iTargetFileList->MdcaPoint(iCounter));
		
		delete iJpegRotator;
		iJpegRotator = NULL;
		iTargetFileList = NULL; // not owned

        // Dismiss wait dialog
	    if (iWaitDialog)
			{
			LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::RunL: deleting wait note" );
	        TRAP_IGNORE( iWaitDialog->ProcessFinishedL() );
			LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::RunL: process finished" );
			delete iWaitDialog;
			LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::RunL: dialog deleted" );
			iWaitDialog = NULL;
			}

		iObserver.RotateOperationReadyL ( iStatus.Int() );

		}
	}

// ----------------------------------------------------
// MProgressDialogCallBack
// 
// ----------------------------------------------------
//
void CJpegRotatorHelper::DialogDismissedL( TInt aButtonId )
	{
	LOGFMT(KEditorServiceProviderLogFile, "CJpegRotatorHelper::DialogDismissedL( %d )", aButtonId);
	
	if( EAknSoftkeyCancel == aButtonId )
		{
		Cancel();
		}
	}

// ---------------------------------------------------------
// CJpegRotatorHelper::DoCancel()
// 
// ---------------------------------------------------------
//
void CJpegRotatorHelper::DoCancel()
	{
	LOG( KEditorServiceProviderLogFile, "CJpegRotatorHelper::DoCancel");

	iJpegRotator->Cancel();

    // Try delete failed target file
	BaflUtils::DeleteFile( iFsSession, iTargetFileList->MdcaPoint(iCounter));


	if( KRequestPending == iStatus.Int() )
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrCancel);
		}
	}

// End of File  

