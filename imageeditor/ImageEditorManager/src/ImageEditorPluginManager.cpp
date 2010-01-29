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



#include <fbs.h>
#include <utf.h>
#include <eikenv.h>

#include "ImageEditorPluginManager.h"
#include "ImageEditorManagerDef.h"

#include "PluginBaseDefs.h"
#include "ImageEditorPluginBaseDefs.h"
#include "ImageEditorPluginBase.hrh"
#include "iepb.h"
#include "ImageEditorError.h"

#include "ImageEditorEngineWrapper.h"
#include "PluginLoader.h"
#include "EditorImage.h"
#include "SystemParameters.h"
#include "ResolutionUtil.h"

#ifdef RD_CONTENTNOTIFICATION

// For notifying Media Gallery of new files
//#include <contentnotification.hrh> // notification event type definitions
#include <contentnotification.h>
#include <contentcreatedevent.h>

#endif //RD_CONTENTNOTIFICATION
// LOCAL CONSTANTS

_LIT( KComponentName, "ImageEditorPluginManager"); // Panic category
const TInt KImageEditorPluginManagerPanicNoPluginLoaded = 0; 


//=============================================================================
EXPORT_C CImageEditorPluginManager * CImageEditorPluginManager::NewL ()
{
	CImageEditorPluginManager * self = new (ELeave) CImageEditorPluginManager;
	CleanupStack::PushL (self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
}

//=============================================================================
EXPORT_C CImageEditorPluginManager::~CImageEditorPluginManager ()
{
	Cleanup();
    delete iPluginLoader;
 #ifdef RD_CONTENTNOTIFICATION
    delete iEventNotifier;
 #endif
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::OpenPluginL (const TDesC &	aFileName)
{
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Opening plugin");

	//	Load plug-in
    iPluginLoader = CPluginLoader::NewL (aFileName, &TUid2, NULL);

	// Get the plug-in filter type
	TBuf<256>	iobuf;  
	iPluginLoader->GetPlugin()->GetProperty (KCapPluginFilterType, iobuf);
	TLex lex (iobuf);
	iFilterType = 0;
	lex.Val (iFilterType);

    // Get the plug-in ui type
	iobuf.Zero();
	iPluginLoader->GetPlugin()->GetProperty (KCapPluginUiType, iobuf);
	lex.Assign (iobuf);
	iUiType = 0;
	lex.Val (iUiType);

    // Disable global zoom / pan if plug-in does not support it 
    iobuf.Zero();
    TInt temp = 0;
    iGlobalZoomDisabled = EFalse;
    iPluginLoader->GetPlugin()->GetProperty (KCapGlobalZoomDisabled, iobuf);
	lex.Assign (iobuf);
	lex.Val (temp);
    iGlobalZoomDisabled = (TBool)temp;
    if (iGlobalZoomDisabled)
    {
        // Store old zoom value for restore
        iEngineWrapper->StoreZoomL();
        RenderL();
    }

	//	Add filter to the engine.
	AddFilterToEngineL();

    //  Add system parameters to plug-in
    CSystemParameters * syspars = iEngineWrapper->GetSystemPars();
    iobuf.Zero();
	iobuf.AppendNum ((TInt)((TAny *)(syspars)));
    iPluginLoader->GetPlugin()->SetProperty (KCapSystemParameters, iobuf);


}

//=============================================================================
EXPORT_C TInt CImageEditorPluginManager::CreatePluginControlL (
	const TRect &	aRect,
	CCoeControl *	aParent,
	CCoeControl *&	aPluginControl
	)
{
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Creating plugin control");
    
    //	Initialize plug-in control
    if (iPluginLoader)
    {
        TInt ret = KErrGeneral;
        CPluginType* plugin = iPluginLoader->GetPlugin();
        if ( plugin )
        {
            ret = plugin->InitPluginL (aRect, aParent, aPluginControl);
        }
        return ret;
    }
    else
    {
        aPluginControl = 0;
        return KErrGeneral;
    }
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::ClosePlugin()
{
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Closing plugin");

    if (iPluginLoader && !iOnTheWayToDestruction)
    {
        //  Enable zoom if disabled
        if (iGlobalZoomDisabled)
        {   
            TRAP_IGNORE(iEngineWrapper->RestoreZoomL());
        }
		TRAP_IGNORE(ProcessImageL());
    }

    //  Delete and unload current plug-in
    delete iPluginLoader;
    iPluginLoader = NULL;
    iFilterType = NULL;
    iUiType = NULL;
    iGlobalZoomDisabled = EFalse;
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::ShowPluginPopup()
{
    if (iPluginLoader)
    {
        CPluginType* plugin = iPluginLoader->GetPlugin();
        if ( plugin )
        {
            plugin->ShowPopupNote ();
        }
    }
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::UndoL()
{
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Undoing");
    
    //	Undo step
	iEngineWrapper->UndoL();

	//	Render
	RenderL();

}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::RedoL()
{
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Redoing");
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::LoadImageL(const TBool aUseIPECodec)
{

    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Loading image");

	if ( aUseIPECodec )
	{
	    //	Create new jpeg source
		iEngineWrapper->CreateJpegSourceL ( iImage->GetImageName() );
	}
	else
	{
	    //	Create new icl source
		iEngineWrapper->CreateIclSourceL ( iImage->GetImageName() );
	}

	//	Render
	RenderL();
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::SaveImageL (
    const TInt      aQuality,
    const TSize *   aSize
    )
{
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Saving image");

    //	Create RAJPEG sink
	iEngineWrapper->CreateJpegTargetL (iImage->GetImageName(), aQuality, aSize);

	//	Render
	iEngineWrapper->RenderL ();
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::PrepareAsyncSaveL (
    const TInt      aQuality,
    const TSize *   aSize
    )
    {
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Preparing for async save");

	//  Disable and store zoom
	iEngineWrapper->StoreZoomL();

    //	Create new EXIF thumbnail
	iEngineWrapper->CreateExifThumbNailL ();

    //	Create RAJPEG sink
	iEngineWrapper->CreateJpegTargetL  (iImage->GetImageName(), aQuality, aSize);

    //  Start rendering
    iEngineWrapper->RenderL ();
    }

//=============================================================================
EXPORT_C TInt CImageEditorPluginManager::SaveBlockL ()
    {
   	//	Render
    return iEngineWrapper->RenderBlockL();
    }

//=============================================================================
EXPORT_C void CImageEditorPluginManager::SaveAbortL ()
    {
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Saving aborted");

    iEngineWrapper->RenderAbortL();

	iEngineWrapper->RestoreZoomL();

    RenderL();
    }

//=============================================================================
EXPORT_C void CImageEditorPluginManager::FinishAsyncSaveL ()
    {
    LOG(KImageEditorLogFile, "CImageEditorPluginManager::FinishAsyncSaveL");

#ifdef RD_CONTENTNOTIFICATION    
    TArray<CContentNotificationEvent*> eventArray;
    
    // Nofication about new content
	CContentNotificationEvent* event = CContentCreatedEvent::NewLC( iImage->GetImageName() );
	eventArray->AppendL( event );
	CleanupStack::Pop( event );

    User::LeaveIfError(iEventNotifier->SendNotification( eventArray ));
    delete event;
    delete eventArray;
#endif
    
    iEngineWrapper->RestoreZoomL();
     
    RenderL();
    }


//=============================================================================
EXPORT_C void CImageEditorPluginManager::ProcessImageL()
{
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Rendering image");

	//	Set filter parameters
	SetParametersL();

    //	Render
 	RenderL ();
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::PrepareAsyncProcessL (TInt& aBlockCount)
{
	//	Set filter parameters
	SetParametersL();

    // Start rendering
    RenderL (&aBlockCount);
}

//=============================================================================
EXPORT_C TInt CImageEditorPluginManager::ProcessBlockL()
{
    //	Render
 	return iEngineWrapper->RenderBlockL ();
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::FinishAsyncProcess()
{

}

//=============================================================================
EXPORT_C CFbsBitmap * CImageEditorPluginManager::GetPreviewImage()
{
	return iImage->GetPreviewData();
}

//=============================================================================
EXPORT_C CFbsBitmap * CImageEditorPluginManager::GetFullImage()
{
	return iImage->GetFullData();
}

//=============================================================================
EXPORT_C const TDesC & CImageEditorPluginManager::GetImageName()
{
	return iImage->GetImageName();
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::SetPreviewImage (CFbsBitmap * aBitmap)
{
	iImage->GetPreviewData() = aBitmap;
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::SetFullImage (CFbsBitmap * aBitmap)
{
	iImage->GetFullData() = aBitmap;
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::SetImageName (const TDesC & aFileName)
{
	iImage->GetImageName().Copy ( aFileName );
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::ResetEngineL()
{
	//	Clean up
	Cleanup();
    
    //  Create a new MIA engine
    iEngineWrapper = CEngineWrapper::NewL();
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::PurgeUndoRedoHistory()
{
    iEngineWrapper->InitUndoRedo();
}

//=============================================================================
EXPORT_C TBool CImageEditorPluginManager::CanUndo()
{
    return iEngineWrapper->CanUndo();
}

//=============================================================================
EXPORT_C TBool CImageEditorPluginManager::IsImageChanged()
{
    return iEngineWrapper->IsImageChanged();
}

//=============================================================================
EXPORT_C TBool CImageEditorPluginManager::IsLandscapeEnabledL() const
{
    TBool ret = ETrue;
    
    if (iPluginLoader)
        {
        TBuf<256>	iobuf; 
        TInt err = iPluginLoader->GetPlugin()->GetProperty(KCapIsLandscapeEnabled, iobuf);
        if (err != KErrNone)
            {
            User::Leave(KSIEEInternal);
            }
        else
            {
            TLex parser(iobuf);
            User::LeaveIfError(parser.Val(ret));
            }
        }

    return ret;
}

//=============================================================================
CImageEditorPluginManager::CImageEditorPluginManager () 
: iGlobalZoomDisabled(EFalse), iOnTheWayToDestruction (EFalse)
{

}

//=============================================================================
void CImageEditorPluginManager::ConstructL ()
{
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Creating manager");
    //  Create new image
	iImage = CEditorImage::NewL ();

    //  Create a new MIA engine
    iEngineWrapper = CEngineWrapper::NewL();

#ifdef RD_CONTENTNOTIFICATION
    iEventNotifier = CContentNotification::NewL ();
#endif 

	//  Get the screen size from the Resolution Util
    TRect rect;
    CResolutionUtil::Self()->GetClientRect(rect);
    TSize size = rect.Size();

    //  Set screen size to the engine
   iEngineWrapper->SetScreenSizeL (size);
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::SetScreenSizeL () 
{
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Setting new screen size");

	iEngineWrapper->CreateRGB888TargetL ();
	
    //  Get the screen size from the Resolution Util
    TRect rect;
    CResolutionUtil::Self()->GetClientRect(rect);
    TSize size = rect.Size();

    //  Set screen size to the engine
    iEngineWrapper->SetScreenSizeL (size);

    //  Render to new screen size
    RenderL();

	//	Compute system parameters
	iEngineWrapper->GetSystemPars();
}


//=============================================================================
void CImageEditorPluginManager::SetParametersL()
{
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Setting parameters");

	if (EPluginFilterTypeABITO == iFilterType)
	{
		//	used as a buffer
		TBuf<256> param; 
			
		TInt err = iPluginLoader->GetPlugin()->GetProperty(KCapParamStruct, param);

		//	If cancelling, there might not be a control, KErrNotReady returned
		//	Ignore KErrNotReady
        if ( (err != KErrNone) && (err != KErrNotReady) )
		{
            User::Leave(KSIEEInternal);
        }

		iEngineWrapper->SetParamsL (param);
	}
}

//=============================================================================
void CImageEditorPluginManager::RenderL (TInt * aMultiSessionBlockCount)
{
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Rendering");

	TBuf<256>	iobuf;  
	TBool readyToRender = ETrue;

    // Check that filter can be rendered
    if ( iPluginLoader && iFilterType == EPluginFilterTypeABITO )
	    {
        TInt err = iPluginLoader->GetPlugin()->GetProperty(KCapReadyToRender, iobuf);
        if (err != KErrNone)
            {
            User::Leave(KSIEEInternal);
            }
        else
            {
            TLex parser(iobuf);
            User::LeaveIfError(parser.Val(readyToRender));
            }
        }

	if (readyToRender)
		{
		iEngineWrapper->SetBitmap ( iImage->GetPreviewData() );
		iEngineWrapper->CreateRGB888TargetL ();
	    iEngineWrapper->RenderL (aMultiSessionBlockCount);
		}    

        
}

//=============================================================================
EXPORT_C void CImageEditorPluginManager::AddFilterToEngineL()
{
    LOG(KImageEditorLogFile, "CImageEditorPluginManager: Adding filter to engine");

	//	Add undo-redo step
    iEngineWrapper->AddUndoRedoStepL();

    if (EPluginFilterTypeABITO == iFilterType)
    {

		TBuf<256>	iobuf;  
		TLex 		parser;

        //	Add filter to the engine
	    TFileName	filterFile;
        CPluginType* plugin = iPluginLoader->GetPlugin(); // must not return null
        __ASSERT_ALWAYS( plugin, User::Panic(KComponentName, KImageEditorPluginManagerPanicNoPluginLoaded) );

        User::LeaveIfError( plugin->GetProperty (KCapFilterName, iobuf) );
        filterFile.Copy ( iobuf );

        // Construct full dll path
        TFileName filterNameAndPath( KPluginDriveAndPath );
        filterNameAndPath.Append( filterFile );

        LOGFMT( KImageEditorLogFile,"\tPlug-in file: %S", &filterNameAndPath );

        iobuf.Zero();
        User::LeaveIfError( iPluginLoader->GetPlugin()->GetProperty (KCapPluginScope, iobuf) );
        parser = iobuf;
        parser.Val (iPluginScope);
    
        iEngineWrapper->AddFilterL ( filterNameAndPath );
    }
}

//=============================================================================
void CImageEditorPluginManager::Cleanup()
{
    iOnTheWayToDestruction = ETrue;

	//	Close plug-in
	ClosePlugin();

	//	Delete engine
    delete iEngineWrapper;
	iEngineWrapper = NULL;

	//	Delete editor image
    delete iImage;
	iImage = NULL;
}


//=============================================================================
EXPORT_C void CImageEditorPluginManager::ZoomL (const TZoom aZoom)
    {
    if( !iGlobalZoomDisabled )
    	{
    	iEngineWrapper->ZoomL (aZoom);
    	}
    }
    
//=============================================================================
EXPORT_C TZoomMode CImageEditorPluginManager::GetZoomMode()
    {
    return iEngineWrapper->GetZoomMode();
    }
    
//=============================================================================
EXPORT_C void CImageEditorPluginManager::PanL (const TDirection aDir)
    {
    iEngineWrapper->PanL (aDir);
    }

//=============================================================================    
EXPORT_C void CImageEditorPluginManager::PanL( TInt aXChange, TInt aYChange )
    {
    iEngineWrapper->PanL( aXChange, aYChange );
    }

//=============================================================================
EXPORT_C void CImageEditorPluginManager::RotateL (const TRotation aRot)
    {
   	//	Rotate image
    iEngineWrapper->RotateL (aRot);
    }

//=============================================================================
EXPORT_C const CSystemParameters & CImageEditorPluginManager::GetSystemParameters() const
{
    return *iEngineWrapper->GetSystemPars();
}

//=============================================================================
EXPORT_C TBool CImageEditorPluginManager::IsPluginLoaded () const
{
    if (iPluginLoader)
    {
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}


//=============================================================================
EXPORT_C void CImageEditorPluginManager::RestoreFilterL() const
{

}

// End of file
