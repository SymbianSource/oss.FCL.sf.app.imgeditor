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



//  INCLUDES
#include <eikenv.h>
#include <bautils.h>
#include <e32math.h>

#include "ImageEditorError.h"
#include "ImageEditorImageController.h"
#include "ImageEditorPluginManager.h"
#include "ImageEditorImageHandler.h"
#include "ResolutionUtil.h"
#include "ImageEditorPanics.h"
#include "SystemParameters.h"
#include "imageeditordebugutils.h"
#include "editorversion.h"
#include "commondefs.h"


//  CONSTANTS

//  Panic category
_LIT(KComponentName, "CImageController");

//=============================================================================
CImageController * CImageController::NewL (
	MImageControllerObserver *		aObserver,
	CImageEditorPluginManager *		aManager
	)
{
	CImageController * self = new (ELeave) CImageController;
	CleanupStack::PushL (self);
	self->ConstructL (aObserver, aManager);
	CleanupStack::Pop(); // self
	return self;
}

//=============================================================================
CImageController::~CImageController ()
{
    LOG(KImageEditorLogFile, "CImageController::~CImageController() starting...");

	Cancel();
	DeletePreviewBitmap();
	delete iImageHandler;
    iManager = NULL;
    iObserver = NULL;

    LOG(KImageEditorLogFile, "CImageController::~CImageController() finished.");
}

//=============================================================================
void CImageController::ConstructL (
	MImageControllerObserver *		aObserver,
	CImageEditorPluginManager *		aManager
	)
    {
	CActiveScheduler::Add (this);
	iObserver = aObserver;
	iManager = aManager;
	iImageHandler = CImageEditorImageHandler::NewL ();	
    iQuality = KDefaultSavedJpegQuality;
    iJpegImage = EFalse;
    iIsCancelled = EFalse;
    LOG(KImageEditorLogFile, "CImageController: Image controller created");
    }	


//=============================================================================
void CImageController::LoadImageL (const TDesC & aFileName)
    {
	LOGFMT(KImageEditorLogFile, "CImageController: Loading image %S", &aFileName);

    iOperation = MImageControllerObserver::EOperationCodeLoad;
    iIsCancelled = EFalse;

    //	If active, cancel
    if ( IsActive() )
        {
        Cancel();
        }

    //	Create new CFbsBitmap
    DeletePreviewBitmap();
    CreatePreviewBitmapL();
    
    TBool useIPECodec = EFalse;
    // try open first with IPE ICL codec
	LOG (KImageEditorLogFile, "CImageController::LoadImageL() -- try open image with ICL");

    //	Create new TFileName and store it
    iManager->SetImageName (aFileName);

    //	Load RAJPEG image
	TRAPD ( err, iManager->LoadImageL( useIPECodec ) );
    if ( KErrNone == err )
        {
        LOG (KImageEditorLogFile, "CImageController::LoadImageL() -- image opened with ICL");
        //  Force RunL()
        CompleteRequest();
        }
    else
        {
        LOG (KImageEditorLogFile, "CImageController::LoadImageL() -- failed open image with ICL");
        // Try with IPE codec
        useIPECodec = ETrue;        
        }
    
    if ( useIPECodec )
        {
        //	If JPEG source image, try opening with IPE JPEG filter
        iJpegImage = iImageHandler->IsRajpegImageL (aFileName);
        
        if ( !iJpegImage )
            {
            iObserver->OperationReadyL ( iOperation, KSIEEIncompatibleImage );
            }
        else
            {            
    		LOG(KImageEditorLogFile, "CImageController::LoadImageL() -- open JPEG image with IPE");
        
            //	Create new TFileName and store it
            iManager->SetImageName (aFileName);

            //	Load RAJPEG image
    		TRAPD (err, iManager->LoadImageL( useIPECodec ));
            if (KErrNone == err)
                {
                //  Force RunL()
                CompleteRequest();
                }
            else if ((KSIEEIncompatibleImage == err) || (KErrNotSupported == err))
                {
    			LOG (KImageEditorLogFile, "CImageController::LoadImageL() -- JPEG not supported");
    			iObserver->OperationReadyL ( iOperation, err ); 
                }
            // Added because system runs out of memory if jpeg size is too big 
            // (around 5 Mb)
            else if ( KErrNoMemory == err ) 
                {
    			LOG (KImageEditorLogFile, "CImageController::LoadImageL() -- Not enough memory to use IPE");
    			iObserver->OperationReadyL ( iOperation, err );
                }
            else
                {
                User::Leave (err);
                }
            }
        }
    
}

//=============================================================================
void CImageController::SaveImageL (const TDesC & /*aFileName*/)
{

    LOG(KImageEditorLogFile, "CImageController: Saving image");

    // Start saving operation
	iOperation = MImageControllerObserver::EOperationCodeBlockSave;
    iSavedPercentage = 1;
    iSavedPercentageOld = 1;
    iIsCancelled = EFalse;
	iManager->PrepareAsyncSaveL (iQuality);

    //  Force RunL()
    CompleteRequest();
}

//=============================================================================
TInt CImageController::RenameImage (
	const TDesC	&		aFileNameSrc,
	const TDesC	&		aFileNameDst
	) const
{
	return iImageHandler->RenameImage (aFileNameSrc, aFileNameDst);
}

//=============================================================================
TInt CImageController::DeleteImage (const TDesC & aFileName) const
{
	return iImageHandler->DeleteImage (aFileName);
}

//=============================================================================
void CImageController::DoCancel()
{
    LOG(KImageEditorLogFile, "CImageController::DoCancel");

    if( IsActive() )
    {
        iImageHandler->CleanupConverters();
    }
}

//=============================================================================
void CImageController::CancelOperation (TBool aForce)
{
    LOGFMT(KImageEditorLogFile, "CImageController::CancelImageController: iOperation == %d", iOperation);

    iIsCancelled = ETrue;
    
	// Handle cancelling block save
	if (aForce)
	{
		CompleteRequest();
	}
}

//=============================================================================
void CImageController::RunL ()
{

    switch (iOperation)
    {
        /// Loading images
        case MImageControllerObserver::EOperationCodeLoad:
        {
            LOG(KImageEditorLogFile, "CImageController: Loading completed");
            iObserver->OperationReadyL ( iOperation, iStatus.Int() );
            break;
        }

        case MImageControllerObserver::EOperationCodeBlockLoad:
        {
            LOG(KImageEditorLogFile, "CImageController: Loading completed");
            iObserver->OperationReadyL ( iOperation, iStatus.Int() );
            break;
        }

        /// Block saving images
        case MImageControllerObserver::EOperationCodeBlockSave:
        {
            // Handle cancelling block save
            if (iIsCancelled)
            {
                LOG(KImageEditorLogFile, "CImageController: Saving cancelled");
				iManager->SaveAbortL();
                iObserver->OperationReadyL ( iOperation, KErrCancel );
            }

            else
            {
                // Increment progress bar
                if ( iSavedPercentage > iSavedPercentageOld )
                {
                    iObserver->IncrementProgressBarL (iSavedPercentage - iSavedPercentageOld);
                  	iSavedPercentageOld = iSavedPercentage;
                }
                
                // 	Save next block
                iSavedPercentage = iManager->SaveBlockL();
                
                //	Complete?
                if (iSavedPercentage == 100)
                {
                    // Save completed
                    LOG(KImageEditorLogFile, "CImageController: Saving completed");
                    iManager->FinishAsyncSaveL();
                    iObserver->OperationReadyL ( iOperation, iStatus.Int() );
                }
                
                //	Save next block
                else
                {
                    CompleteRequest();
                
                }
            }
            break;
        }

        /// Saving images
        case MImageControllerObserver::EOperationCodeSave:
        {
            iObserver->OperationReadyL ( iOperation, iStatus.Int() );
            break;
        }
        /// Deleting images
        case MImageControllerObserver::EOperationCodeDelete:
        /// Renaming images
        case MImageControllerObserver::EOperationCodeRename:
        /// Searching images
        case MImageControllerObserver::EOperationCodeSearch:
        default:
        {
            User::Panic(KComponentName, EImageEditorPanicUnexpectedCommand);
            break;
        }
    }
}

//=============================================================================
CImageController::CImageController () : 
CActive (CActive::EPriorityStandard)
{

}


//=============================================================================
void CImageController::CreatePreviewBitmapL () 
{
	LOG(KImageEditorLogFile, "CImageController: Creating preview bitmap");
    // Get current screen rect
    TRect rect;
    CResolutionUtil::Self()->GetClientRect(rect);
    TSize size = rect.Size();

	LOG(KImageEditorLogFile, "CImageController: Resolution read");
	LOGFMT(KImageEditorLogFile, "  Width: %d", size.iWidth);
    LOGFMT(KImageEditorLogFile, "  Height: %d", size.iHeight);

    //	Create bitmap
    if (iPreview)
    {
        delete iPreview;
        iPreview = NULL;
    }

    iPreview = new (ELeave) CFbsBitmap;
	User::LeaveIfError ( iPreview->Create ( size, EColor16M) );

    // Set bitmap
    iManager->SetPreviewImage (iPreview);
}

//=============================================================================
void CImageController::DeletePreviewBitmap() 
{
    LOG(KImageEditorLogFile, "CImageController: Deleting preview bitmap");
	delete iPreview;
    iPreview = NULL;
	iManager->SetPreviewImage (NULL);
}

//=============================================================================
void CImageController::CompleteRequest()
{
	if ( IsActive() )
	{
		Cancel();
	}
	TRequestStatus * p = &iStatus;
	SetActive();
	User::RequestComplete (p, KErrNone);
}

//=============================================================================
void CImageController::CalculateProgressInterval (TInt aBlockCount)
{
    const TInt KMaxProgressSteps = 25;

    iProgressBarInterval = 1;
    while( (aBlockCount / iProgressBarInterval) > KMaxProgressSteps )
    {
        iProgressBarInterval++;
    }

    LOGFMT(KImageEditorLogFile, "CImageController: Progress bar interval: %d", iProgressBarInterval);
}

// End of File
