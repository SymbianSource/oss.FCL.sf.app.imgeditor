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



//	INCLUDES
#include <bautils.h>
#include <eikenv.h>
#include <aknutils.h>
#include <e32math.h>

//#include "ImageEditorPluginBase.hrh"
#include "platform_security_literals.hrh"

#include "ImageEditorEngineWrapper.h"
#include "CFilterStack.h"
#include "MImageFilter.h"
#include "SystemParameters.h"
#include "ImageEditorError.h"
#include "ImageEditorUtils.h"
#include "EditorVersion.h"


#include "CExifParser.h"
#include "TBitmapHandle.h"
#include "MJpegSave.h"
#include "JpegSaveFactory.h"


//  Debug logger definitions
#include "imageeditordebugutils.h"
_LIT( KEngineWrapperLogFile,"EngineWrapper.log" );


// CONSTANTS

// 	Minimum source image x-y value that the engine is able to render
const TInt KObMinSourceDimension    		= 16; 

//	EXIF thumbnail quality
const TInt KThumbnailJpegQuality    		= 60; 		// [0,100]
const TInt KThumbnailMaxDimension			= 160;

//  Used fixed point resolution for scales
const TInt KScaleBits						= 12;

const TReal KZoomScaleFactor                 = 0.5; 

// Comment tag to be written to the EXIF data of JPEG file.
_LIT8( KImageEditorExifComment, "Edited with Nokia Image Editor %d.%d.%d" );


// 	Filter paths
_LIT(KFilterJpegSource, "FilterJpegSource.dll");
_LIT(KFilterIclSource, "FilterIclSource.dll");
_LIT(KFilterTarget, "FilterJpegTarget.dll");
_LIT(KFilterBuffer, "FilterBuffer.dll");
_LIT(KFilterScale, 	"FilterScale.dll");
_LIT(KFilterRotate, "FilterRotate.dll");

//=============================================================================
EXPORT_C CEngineWrapper * CEngineWrapper::NewL ()
{
    LOG_INIT ( KEngineWrapperLogFile );

	CEngineWrapper * self = new (ELeave) CEngineWrapper;
	CleanupStack::PushL (self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
}

//=============================================================================
EXPORT_C CEngineWrapper::~CEngineWrapper ()
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::~CEngineWrapper");
    Cleanup();

	delete iExifParser;
	iExifParser = NULL;
	
}

//=============================================================================
EXPORT_C void CEngineWrapper::CreateJpegSourceL (const TDesC & aFileName)
{
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL" );
#ifdef VERBOSE
    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL\taFileName: %S", &aFileName );
#endif // VERBOSE

    //  SANITY CHECKS
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

	//	Add JPEG source to engine
	LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL --- Add jpeg source" );
	if ( iMainEngine->NumFilters() == 0 )
	{
		iMainEngine->AddFilterL ( KFilterJpegSource );
	}
	else if  ( IsSameString ( (TUint8*)iMainEngine->Filter(0)->Type(), (TUint8*)"iclsource" ) )
	{
		iMainEngine->RemoveFilter (0);
		iMainEngine->AddFilterL (KFilterJpegSource, 0);
	}
	else if  ( !IsSameString ( (TUint8*)iMainEngine->Filter(0)->Type(), (TUint8*)"jpegsource" ) )
	{
		iMainEngine->AddFilterL (KFilterJpegSource, 0);
	}
	iCmd.Copy ( _L("file \""));
	iCmd.Append ( aFileName );
	iCmd.Append ( _L("\""));
	LOGFMT ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL: Cmd: %S", &aFileName );
	iMainEngine->FunctionL ( 0, iCmd );
	LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL --- Jpeg source added" );
	iMainEngine->FunctionL ( 0, _L("loadimage"));

	//	Get source image size
	TRect rect = iMainEngine->Filter (0)->Rect(); 
	iSourceSize = iMainEngine->Filter (0)->ViewPortSize(); 
	LOGFMT ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL: source width: %d", iSourceSize.iWidth);
	LOGFMT ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL: source height: %d", iSourceSize.iHeight);

    //  Check that the source image is of minimum size
    if (iSourceSize.iWidth < KObMinSourceDimension || iSourceSize.iHeight < KObMinSourceDimension)
    {
        User::Leave (KSIEEOpenFile);
    }

	//	Initialize crop parameters
	iBoundingRect.iTl.iX = 0;    
	iBoundingRect.iTl.iY = 0;    
	iBoundingRect.iBr.iX = iSourceSize.iWidth;    
	iBoundingRect.iBr.iY = iSourceSize.iHeight;    
    iScale = 1.0;
    iMaxScale = 1.0;
    iMinScale = (TReal)iScreenSize.iWidth / iSourceSize.iWidth; 
    iPanX = iSourceSize.iWidth * 0.5;
    iPanY = iSourceSize.iHeight * 0.5;
    iPanStep = 128.0;
    iZoomMode = EZoomNormal;
    
	//	Initialize system parameters
    iSysPars->SourceSize() = iSourceSize;
    iSysPars->Scale() = 1.0;


	LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL ---  Read EXIF data" );

	//	Read EXIF data pointer
	iCmd.Copy ( _L("exifdata"));
	TUint8 * exifptr = (TUint8 *)iMainEngine->FunctionL ( 0, iCmd );
	
	//	Read EXIF data length
	iCmd.Copy ( _L("exiflength"));
	TInt exiflen = (TInt)iMainEngine->FunctionL ( 0, iCmd );

	//	Create and initialize EXIF parser
	TPtrC8 exifdata (exifptr, exiflen);
	if (iExifParser)
	{
		delete iExifParser;
		iExifParser = NULL;		
	}
	
	iExifParser = CExifParser::NewL ();
	TRAPD( err, iExifParser->ParseL (exifdata) );
	if (KErrNone == err)
	{
		LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL --- EXIF data read." );
	}
	else
	{
		LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL --- FAILED to parse EXIF data. Ignored." );
	}


}

//=============================================================================
EXPORT_C void CEngineWrapper::CreateJpegTargetL (
	const TDesC & aFileName,
    const TInt      aQuality,
    const TSize *   aSize 
    )
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL" );
#ifdef VERBOSE
    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL\taFileName: %S", &aFileName );
    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL\taQuality: %d", aQuality );
    if (aSize)
    {
        LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL\taSize->iWidth: %d", aSize->iWidth);
        LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL\taSize->iHeight: %d", aSize->iHeight);
    }
    else
    {
		LOG( KEngineWrapperLogFile, "aSize == NULL" );
    }

#endif // VERBOSE

    //  SANITY CHECKS
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }
    if ( aQuality < 0 || aQuality > 100)
    {
        LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL: Invalid parameter aQuality!" );
        User::Panic (KEnginePanic, KEnginePanicParameter);
    }
    if ( aSize && ( (aSize->iWidth < 0 || aSize->iHeight < 0) ) )
    {
        LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL: Invalid parameter aSize!" );
        User::Panic (KEnginePanic, KEnginePanicParameter);
    }

	//	Remove source buffer
	iMainEngine->FunctionL (0, _L("fileoutput"));

	//	Remove screen buffer
	LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL --- Remove screen buffer" );
	if ( IsSameString ( (TUint8*)iMainEngine->Filter(iMainEngine->NumFilters() - 1)->Type(), (TUint8*)"buffer" ) )
	{
		iMainEngine->RemoveFilter(iMainEngine->NumFilters() - 1);	
	}
	LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL --- Screen buffer removed" );

	//	Remove jpeg target buffer
	if ( IsSameString ( (TUint8*)iMainEngine->Filter(iMainEngine->NumFilters() - 1)->Type(), (TUint8*)"jpegtarget" ) )
	{
		iMainEngine->RemoveFilter(iMainEngine->NumFilters() - 1);	
	}

	//	Remove scale buffer, if image is not scaled
	if ( iSysPars->Scale() == 1.0 )
	{
		LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL --- Remove screen scale filter." );
		if ( IsSameString ( (TUint8*)iMainEngine->Filter(iMainEngine->NumFilters() - 1)->Type(), (TUint8*)"scale" ) )
		{
			iMainEngine->RemoveFilter(iMainEngine->NumFilters() - 1);	
		}
		LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL --- Screen scale filter removed." );
	}

	LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL --- Add jpeg target filter." );
	iMainEngine->AddFilterL ( KFilterTarget );
	LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegTargetL --- Jpeg target filter added." );

	//	If external target size is given, set it to target.
	if ( aSize )
	{
		iCmd.Format( _L("width %d height %d"), aSize->iWidth, aSize->iHeight );
		iMainEngine->FunctionL ( iMainEngine->NumFilters() - 1, iCmd );
	}

	//	Set the scaled source size to target
	if ( iSysPars->Scale() != 1.0 )
	{
		TReal scale = iSysPars->Scale();
		TSize tgtsize = iMainEngine->Filter ( iMainEngine->NumFilters() - 3 )->ViewPortSize();
		tgtsize.iWidth = (TInt)(tgtsize.iWidth * scale + 0.5);
		tgtsize.iHeight = (TInt)(tgtsize.iHeight * scale + 0.5);
		iCmd.Format( _L("width %d height %d"), tgtsize.iWidth, tgtsize.iHeight);
		iMainEngine->FunctionL ( iMainEngine->NumFilters() - 1, iCmd );
		iMainEngine->FunctionL ( iMainEngine->NumFilters() - 2, _L("nop"));
	}
	
	//	Store save file name
	iTargetFile.Copy ( aFileName );
	
    LOG ( KEngineWrapperLogFile, "CreateJpegTargetL: Sink created" );
}

//=============================================================================
EXPORT_C void CEngineWrapper::CreateIclSourceL (const TDesC & aFileName)
{

    LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateIclSourceL" );
#ifdef VERBOSE
    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::CreateIclSourceL\taFileName: %S", &aFileName );
#endif // VERBOSE

    //  SANITY CHECKS
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateIclSourceL: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

	//	Add JPEG source to engine
	LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateIclSourceL --- Add icl source" );
	if ( iMainEngine->NumFilters() == 0 )
	{
		iMainEngine->AddFilterL ( KFilterIclSource );
	}
	else if  ( IsSameString ( (TUint8*)iMainEngine->Filter(0)->Type(), (TUint8*)"jpegsource" ) )
	{
		iMainEngine->RemoveFilter(0);
		iMainEngine->AddFilterL ( KFilterIclSource, 0);
	}
	else if  ( !IsSameString ( (TUint8*)iMainEngine->Filter(0)->Type(), (TUint8*)"iclsource" ) )
	{
		iMainEngine->AddFilterL ( KFilterIclSource, 0);
	}
	iCmd.Copy (_L("file \""));
	iCmd.Append ( aFileName );
	iCmd.Append (_L("\""));
	LOGFMT ( KEngineWrapperLogFile, "CEngineWrapper::CreateIclSourceL: Cmd: %S", &iCmd );
	iMainEngine->FunctionL ( 0, iCmd );
	LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateIclSourceL --- Icl source added" );
	iMainEngine->FunctionL ( 0, _L("loadimage"));

	//	Get source image size
	TRect rect = iMainEngine->Filter (0)->Rect(); 
	iSourceSize = iMainEngine->Filter (0)->ViewPortSize(); 
	LOGFMT ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL: source width: %d", iSourceSize.iWidth);
	LOGFMT ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL: source height: %d", iSourceSize.iHeight);

    //  Check that the source image is of minimum size
    if (iSourceSize.iWidth < KObMinSourceDimension || iSourceSize.iHeight < KObMinSourceDimension)
    {
        User::Leave (KSIEEOpenFile);
    }

	//	Initialize crop parameters
    iScale = 1.0;
    iMaxScale = 1.0;
    iMinScale = (TReal)iScreenSize.iWidth / iSourceSize.iWidth; 
    iPanX = iSourceSize.iWidth * 0.5;
    iPanY = iSourceSize.iHeight * 0.5;
    iPanStep = 128.0;
    iZoomMode = EZoomNormal;
    
	//	Initialize system parameters
    iSysPars->SourceSize() = iSourceSize;
    iSysPars->Scale() = 1.0;
    

	LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateIclSourceL ---  Create EXIF parser" );

	if (iExifParser)
	{
		delete iExifParser;
		iExifParser = NULL;		
	}

	iExifParser = CExifParser::NewL ();
	LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateJpegSourceL --- EXIF parser created." );

    
}

//=============================================================================
EXPORT_C void CEngineWrapper::CreateRGB888TargetL ()
{

    LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateRGB888TargetL " );

    //  SANITY CHECKS
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateRGB888TargetL: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

#ifdef VERBOSE
    LOGFMT( KEngineWrapperLogFile, "\tiScreenSize.w: %d", iScreenSize.iWidth);
    LOGFMT( KEngineWrapperLogFile, "\tiScreenSize.h: %d", iScreenSize.iHeight);
#endif
	
	//	Create source buffer
	iMainEngine->FunctionL (0, _L("bufferoutput"));
	
	//	Remove target filter and add screen buffer
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateRGB888TargetL --- Add buffer target." );
    TInt n = iMainEngine->NumFilters();
	if ( IsSameString ( (TUint8*)iMainEngine->Filter(n - 1)->Type(), (TUint8*)"jpegtarget") ) 
	{
		iMainEngine->RemoveFilter(n - 1);	
		iMainEngine->AddFilterL ( KFilterBuffer );
	}
	else if ( !IsSameString ( (TUint8*)iMainEngine->Filter(n - 1)->Type(), (TUint8*)"buffer" ) )
	{
		iMainEngine->AddFilterL ( KFilterBuffer );
	}
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateRGB888TargetL --- Buffer target added." );
	
	//	Set buffer size
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateRGB888TargetL --- Set buffer size ");
	iCmd.Format( _L("width %d height %d"), iScreenSize.iWidth, iScreenSize.iHeight );
	LOGDES (KEngineWrapperLogFile, iCmd);
	iMainEngine->FunctionL ( iMainEngine->NumFilters() - 1, iCmd );
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateRGB888TargetL --- Buffer size set." );

	//	Set scale buffer before screen buffer
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateRGB888TargetL --- Add screen buffer scale filter");
	if ( !IsSameString ( (TUint8*)iMainEngine->Filter(iMainEngine->NumFilters() - 2)->Type(), (TUint8*)"scale") ) 
	{
		iMainEngine->AddFilterL ( KFilterScale, iMainEngine->NumFilters() - 1);
	}
	iMainEngine->FunctionL ( iMainEngine->NumFilters() - 2, iCmd );
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateRGB888TargetL --- Screen buffer scale filter added.");

    LOG ( KEngineWrapperLogFile, "CEngineWrapper::CreateRGB888TargetL: Sink created" );
}

//=============================================================================
EXPORT_C void CEngineWrapper::SetScreenSizeL (const TSize & aSize)
{

    LOG ( KEngineWrapperLogFile, "CEngineWrapper::SetScreenSizeL" );
#ifdef VERBOSE
    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::SetScreenSizeL\taSize.iWidth: %d", aSize.iWidth );
    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::SetScreenSizeL\taSize.iHeight: %d", aSize.iHeight );
#endif // VERBOSE

    //  SANITY CHECKS
    if ( aSize.iWidth < 0 || aSize.iHeight < 0)
    {
        LOG ( KEngineWrapperLogFile, "CEngineWrapper::SetScreenSizeL: Invalid parameter aSize!" );
        User::Panic (KEnginePanic, KEnginePanicParameter);
    }

	if ( aSize != iScreenSize )
	{
		iScreenSize = aSize;

		//	Set buffer size
		TInt n = iMainEngine->NumFilters();
		if (n > 2)
		{
			if ( IsSameString ((TUint8*)iMainEngine->Filter(n - 1)->Type(), (TUint8*) "buffer" ) )
			{
				iCmd.Format( _L("width %d height %d"), iScreenSize.iWidth, iScreenSize.iHeight);
				iMainEngine->FunctionL ( n - 1, iCmd );
				iMainEngine->FunctionL ( n - 2, _L("nop"));
				UpdateCropRectL();
			}
		}
	}
}

//=============================================================================
EXPORT_C void CEngineWrapper::AddFilterL (const TDesC & aFilterName) 
{
    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::AddFilterL (TDes & aFilterName: %S)", &aFilterName );

    //  SANITY CHECKS
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "CEngineWrapper::AddFilterL: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

	//  Check whether the filter is found
	TFileName filtername;
	filtername.Copy (aFilterName);
    
    /*
    // no need for this as AddFilterL will leave if there is an error
    if (FileExists (filtername) )
    {
           TParse parse;
           User::LeaveIfError( parse.Set( aFilterName ,NULL, NULL ) );
           filtername = parse.NameAndExt();
    };
    */

	TInt n = iMainEngine->NumFilters();
	if (n >= 2)
	{
		iMainEngine->AddFilterL ( filtername, n - 2 );
	}
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::AddFilterL: Filter added" );
}

//=============================================================================
EXPORT_C void CEngineWrapper::SetParamsL (const TDesC & aParam)
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::SetParamsL " );

    //  SANITY CHECKS
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "CEngineWrapper::SetParamsL: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

    //  Initialize error value
	TInt n = iMainEngine->NumFilters();
	if ( n > 2 )
	{
    	LOGDES( KEngineWrapperLogFile, aParam );
		iMainEngine->FunctionL ( n - 3, aParam );
	}


	if ( IsSameString ( (TUint8*)iMainEngine->Filter(n - 3)->Type(), (TUint8*) "rotate" ) )
	{
		if (iScale == 1.0)
		{
			ComputeBoundingRectL();	
		}
		UpdateCropRectL();
	}
    
    
    LOG( KEngineWrapperLogFile, "CEngineWrapper::SetParamsL: Parameters set " );
}

//=============================================================================
EXPORT_C void CEngineWrapper::RenderL (TInt* /*aMultiSessionBlockCount*/)
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::RenderL" );

    //  SANITY CHECKS
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "CEngineWrapper::RenderL: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

	TInt n = iMainEngine->NumFilters();
	for ( TInt i = 0; i < iMainEngine->NumFilters(); ++i)
	{
		iMainEngine->FunctionL (i, _L("nop"));
		TBuf<100> buf;
		buf.Copy(TPtrC8 ( (TUint8 *)iMainEngine->Filter(i)->Type() ));
		LOGDES (KEngineWrapperLogFile,  buf) ;
		TRect rect = iMainEngine->Filter(i)->Rect();
		buf.Format( _L("%d %d %d %d"), rect.iTl.iX, rect.iTl.iY, rect.iBr.iX, rect.iBr.iY);
		LOGDES (KEngineWrapperLogFile,  buf) ;
	}

	//	Render to buffer target
	if ( IsSameString ( (TUint8*)iMainEngine->Filter(n - 1)->Type(), (TUint8*) "buffer" ) )
	{
		iCmd.Copy (_L("getbitmap"));
		if ( iRenderScaleBuffer )
		{
			iMainEngine->FunctionL ( 0, _L("loadimage"));
			iRenderScaleBuffer = EFalse;
			for ( TInt i = 0; i < iMainEngine->NumFilters(); ++i)
			{
				iMainEngine->FunctionL (i, _L("nop"));
			}
		}


		LOG ( KEngineWrapperLogFile, "CEngineWrapper::RenderL -- Render screen buffer." );
		TUint32 * buffer = (TUint32 *)iMainEngine->FunctionL ( n - 1, iCmd );
		CopyBufferL (buffer);
	}

	//	Render to JPEG target
	else if ( IsSameString ( (TUint8*)iMainEngine->Filter(n - 1)->Type(), (TUint8*)"jpegtarget" ) )
	{
		iCmd.Zero();
		

	LOG ( KEngineWrapperLogFile, "CEngineWrapper::RenderL: Set EXIF data" );
		
		//	Update EXIF tags
		UpdateExifTagsL();

		//	Update EXIF thumbnail 
		TPtrC8 savedexifdata = UpdateExifThumbnailL();

		//	Set EXIF data to target
		iCmd.Format (_L("exifdata %d exiflen %d"), (TInt)(savedexifdata.Ptr()), savedexifdata.Length());

	LOG ( KEngineWrapperLogFile, "CEngineWrapper::RenderL: EXIF data set." );

		
		iCmd.Append (_L(" file \""));
		iCmd.Append ( iTargetFile );
		iCmd.Append (_L("\""));
		iMainEngine->FunctionL ( n - 1, iCmd );
	}

    LOG( KEngineWrapperLogFile, "CEngineWrapper::RenderL: Rendered" );
}

//=============================================================================
EXPORT_C TInt CEngineWrapper::RenderBlockL()
{
#ifdef VERBOSE_2
    LOG( KEngineWrapperLogFile, "CEngineWrapper::RenderBlockL" );

    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::RenderBlockL\tiBlock: %d", iBlock);
    if (iTestBitmap)
    {
        LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::RenderBlock\tiTestBitmap->DataAddress(): %d", (TInt)iTestBitmap->DataAddress());
        LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::RenderBlock\tiTestBitmap->Handle(): %d", iTestBitmap->Handle());
    }
    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::RenderBlockL\tiBuffer: %d", (TInt)iBuffer);
#endif


    //  Check that the model is found
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "RenderBlockL: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

	iCmd.Copy ( _L("store") );
	TInt percentage = iMainEngine->FunctionL ( iMainEngine->NumFilters() - 1, iCmd );
	if (percentage == 100)
	{
		iChangeCount = 0;
	}
	return percentage;
}

//=============================================================================
EXPORT_C void CEngineWrapper::RenderAbortL()
{
	LOG( KEngineWrapperLogFile, "CEngineWrapper::RenderAbortL" );

    //  Check that the model is found
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "RenderAbort: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }
    
    iCmd.Copy ( _L("abort"));
	iMainEngine->FunctionL ( iMainEngine->NumFilters() - 1, iCmd );
}

//=============================================================================
EXPORT_C void CEngineWrapper::InitUndoRedo()
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::InitUndoRedo" );

    //  Check that the model is found
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "InitUndoRedo: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

	//	Purge undo stack
	iUndoPoints.Reset();

	//	Undo
	iPrevChangeCount = 0;
    iChangeCount = 0;
}

//=============================================================================
EXPORT_C void CEngineWrapper::AddUndoRedoStepL()
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::AddUndoRedoStepL" );

    //  Check that the model is found
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "AddUndoRedoStepL: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

	//	Store undo point
	TInt n = iMainEngine->NumFilters();
	if (n > 2)
	{
		//	Set undo point before target filter
		iUndoPoints.Append ( n - 3 );
	}
	
	iPrevChangeCount = iChangeCount;
    iChangeCount++;

    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::AddUndoRedoStepL: iChangeCount = %d", iChangeCount);
    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::AddUndoRedoStepL: iPrevChangeCount = %d", iPrevChangeCount);

    //  Store rotation and scale
    User::LeaveIfError( iScaleUndoBuf.Append ( iSysPars->Scale() ) );
}

//=============================================================================
EXPORT_C void CEngineWrapper::UndoL()
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::UndoL" );

    //  Check that the model is found
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "UndoL: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

	TBool computeBoundingRect = EFalse;
	TBool reloadImage = EFalse;

	if (iMainEngine->NumFilters() > 2)
	{
		TInt m = iUndoPoints.Count();
		TInt undoindex = iUndoPoints[m-1];
		while (iMainEngine->NumFilters() - 3 > undoindex)
		{
			TInt index = iMainEngine->NumFilters() - 3;
			if ( IsSameString ( (TUint8*)iMainEngine->Filter(index)->Type(), (TUint8*) "rotate" ) )
				{
				computeBoundingRect = ETrue;
				}
			else if ( IsSameString ( (TUint8*)iMainEngine->Filter(index)->Type(), (TUint8*) "crop" ) )
				{
				reloadImage = ETrue;
				}
		
			iMainEngine->RemoveFilter (index);
		}
		iUndoPoints.Remove (iUndoPoints.Count() - 1);
	}

    iPrevChangeCount = iChangeCount;
    iChangeCount--;

    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::UndoL: iChangeCount = %d", iChangeCount);
    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::UndoL: iPrevChangeCount = %d", iPrevChangeCount);

    TInt count = iScaleUndoBuf.Count();
    if ( count > 0 )
    {
        iSysPars->Scale() = iScaleUndoBuf[count - 1];
        iScaleUndoBuf.Remove (count - 1);
    }
	
	if (reloadImage)
	{
		iCmd.Format( _L("ulc %d ulr %d lrc %d lrr %d"), 0,0,0,0);
		iMainEngine->FunctionL ( 0, iCmd);
		iMainEngine->FunctionL ( 0, _L("loadimage"));
		for (TInt i = 0; i < iMainEngine->NumFilters(); ++i)
		{
			iMainEngine->FunctionL (i, _L("nop"));
		}
    	ComputeBoundingRectL();
	}

    if (  iMainEngine->NumFilters() > 3 && computeBoundingRect && iScale == 1.0 )
    {
    	ComputeBoundingRectL();
    }
    else if ( iMainEngine->NumFilters() <= 3 )
    {
    	iBoundingRect.iTl.iX = 0;    
    	iBoundingRect.iTl.iY = 0;    
    	iBoundingRect.iBr.iX = iSourceSize.iWidth;    
    	iBoundingRect.iBr.iY = iSourceSize.iHeight;   
    }
    UpdateCropRectL();
}

//=============================================================================
EXPORT_C TBool CEngineWrapper::CanUndo ()
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::CanUndo" );

    //  Check that the model is found
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "CanUndo: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

	if ( iUndoPoints.Count() > 0 )
	{
		return ETrue;
	}
	else
	{
		return EFalse;
	}
}

//=============================================================================
EXPORT_C TBool CEngineWrapper::IsImageChanged()
{
    LOGFMT2( KEngineWrapperLogFile, "CEngineWrapper::IsImageChanged: iChangeCount = %d, iPrevChangeCount = %d", iChangeCount, iPrevChangeCount );

    if ( iChangeCount || (iChangeCount == 0 && iPrevChangeCount < 0)) 
    {
        return ETrue;
    }
    else
    {
        return EFalse;
    }
}

//=============================================================================
CEngineWrapper::CEngineWrapper () : 
iScreenSize (),
iChangeCount(0),
iPrevChangeCount(0)
{

}

//=============================================================================
void CEngineWrapper::ConstructL ()
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::ConstructL" );

	//	Create new engine
	iMainEngine = CFilterStack::NewL();

    // Set screen size to default
    iScreenSize.iWidth = 1;
    iScreenSize.iHeight = 1;

    //  Create and initialize system parameters
	LOG( KEngineWrapperLogFile, "CEngineWrapper: Creating system parameters" );
    iSysPars = new (ELeave) CSystemParameters;
}

//=============================================================================
TInt CEngineWrapper::FileExists (TDes & aFileName) const
{
    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::FileExists (TDes & aFileName: %S)", &aFileName );

	TInt result = KErrNone;

	RFs &fs = CEikonEnv::Static()->FsSession();
	
	aFileName[0] = 'e';
	if ( !BaflUtils::FileExists (fs, aFileName) )
	{
		aFileName[0] = 'c';
		if ( !BaflUtils::FileExists (fs, aFileName) )
		{
			aFileName[0] = 'z';
			if ( !BaflUtils::FileExists (fs, aFileName) )
			{
				return KErrNotFound;
			}
		}
	}

	if ( !BaflUtils::FileExists (fs, aFileName) )
	{
		result = KErrNotFound;
	}

	LOGFMT( KEngineWrapperLogFile, "\tresult: %d", result );

	return result;
}

//=============================================================================
void CEngineWrapper::Cleanup()
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::Cleanup" );

	//	Purge undo / redo information
	iUndoPoints.Reset();
	
	//	Delete filters
	for ( TInt i = 0; i < iMainEngine->NumFilters(); ++i)
	{
		iMainEngine->RemoveFilter(i);	
	}
	
	//	Delete engine
	if (iMainEngine)
	{
		delete iMainEngine;	
		iMainEngine = NULL;
	}
	
    //  Clear rotation and scale undo buffers
    iScaleUndoBuf.Reset();

    //  Delete system parameters
    if (iSysPars)
    {
    	delete iSysPars;	
    	iSysPars = NULL;
    }
    
    if (iJpegComment)
    {
    	delete iJpegComment;	
    	iJpegComment = NULL;
    }
    
    //	Delete thumbnail buffer
    delete[] iThumb;
}
  
//=============================================================================
EXPORT_C void CEngineWrapper::StoreZoomL ()
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::StoreZoom" );

	iScaleSt = iScale;
	iPanXSt = iPanX;
	iPanYSt = iPanY;
	
	iScale = 1.0;
    iPanX = iSourceSize.iWidth * 0.5;
    iPanY = iSourceSize.iHeight * 0.5;
    UpdateCropRectL();
}

//=============================================================================
EXPORT_C void CEngineWrapper::RestoreZoomL()
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::RestoreZoom" );

	if (iScale != iScaleSt)
	{
		iScale = iScaleSt;
		iPanX = iPanXSt;
		iPanY = iPanYSt;
	    UpdateCropRectL();		
	}
}

//=============================================================================
EXPORT_C void CEngineWrapper::ZoomL (const TZoom aZoom)
    {
    LOGFMT( KEngineWrapperLogFile, "CEngineWrapper::ZoomL: %d", (TInt)aZoom);

    //  Check that the model is found
    if ( !iMainEngine )
        {
        LOG ( KEngineWrapperLogFile, "ZoomFactor: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
        }

	if ( aZoom == EZoomMin )
    	{
    	iScale = 1.0;
        iZoomMode = EZoomNormal;
	    
	    }
	else if ( aZoom == EZoomIn )
	    {

        if ( iScale == 1.0 )
            {
        	ComputeBoundingRectL();
            }
        
        if ( iZoomMode < ( ENumOfZooms - 1 ) )
            {
            // rescale
            iScale *= KZoomScaleFactor;
            // set next zoom mode
            iZoomMode = ( TZoomMode )( ( TInt )iZoomMode + 1 );
            }
	    }
	else
	    {	
	    if ( iZoomMode > EZoomNormal )
            {
            // rescale
            iScale *= 1.0 / KZoomScaleFactor;
            //set previous zoom mode	
            iZoomMode = ( TZoomMode )( ( TInt )iZoomMode - 1 );
	        }
	    }
	    
	UpdateCropRectL();
    }
    
//=============================================================================
EXPORT_C TZoomMode CEngineWrapper::GetZoomMode()
    {
    return iZoomMode;
    }
    
//=============================================================================
EXPORT_C void CEngineWrapper::PanL (const TDirection aDir)
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::PanL" );

#ifdef VERBOSE
    LOGFMT( KEngineWrapperLogFile, "\taDir: %d", (TInt)aDir);
#endif // VERBOSE

    if (iScale == 1.0)
    {
        return;
    }

    //  Check that the model is found
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "SetPanL: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

    TInt angle = ComputeRotationL();

	switch (aDir)
	{
		case EDirectionUp:
		{
			if (angle == 0)
			{
    			iPanY -= iPanStep  * iScale;
			}
			else if (angle == 90)
			{
    			iPanX -= iPanStep  * iScale;
			}
			else if (angle == 180)
			{
    			iPanY += iPanStep  * iScale;
			}
			else if (angle == 270)
			{
    			iPanX += iPanStep  * iScale;
			}
			break;
		}
		case EDirectionDown:
		{
			if (angle == 0)
			{
    			iPanY += iPanStep  * iScale;
			}
			else if (angle == 90)
			{
    			iPanX += iPanStep  * iScale;
			}
			else if (angle == 180)
			{
    			iPanY -= iPanStep  * iScale;
			}
			else if (angle == 270)
			{
    			iPanX -= iPanStep  * iScale;
			}
			break;
		}
		case EDirectionLeft:
		{
			if (angle == 0)
			{
    			iPanX -= iPanStep * iScale;
			}
			else if (angle == 90)
			{
    			iPanY += iPanStep * iScale;
			}
			else if (angle == 180)
			{
    			iPanX += iPanStep * iScale;
			}
			else if (angle == 270)
			{
    			iPanY -= iPanStep * iScale;
			}
			break;
		}
		case EDirectionRight:
		{
			if (angle == 0)
			{
    			iPanX += iPanStep  * iScale;
			}
			else if (angle == 90)
			{
    			iPanY -= iPanStep * iScale;
			}
			else if (angle == 180)
			{
    			iPanX -= iPanStep * iScale;
			}
			else if (angle == 270)
			{
    			iPanY += iPanStep * iScale;
			}
			break;
		}
		default:
			break;
	}
	UpdateCropRectL();

}

//=============================================================================
EXPORT_C void CEngineWrapper::PanL( TInt aXChange, TInt aYChange )
    {
    LOG( KEngineWrapperLogFile, "CEngineWrapper::Pan" );

#ifdef VERBOSE
    LOGFMT( KEngineWrapperLogFile, "\taXChange: %d, aYChange: %d ", aXChange, aYChange );
#endif // VERBOSE

    if (iScale == 1.0)
        {
        return;
        }

    //  Check that the model is found
    if ( !iMainEngine )
        {
        LOG ( KEngineWrapperLogFile, "SetPanL: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
        }

    TInt angle = ComputeRotationL();

    if (angle == 0)
		{
		// X change
        iPanX += aXChange*(iScale/iMinScale);

        // Y change
        iPanY += aYChange*(iScale/iMinScale);
   		}
	else if (angle == 90)
		{
		// X change
        iPanX += aYChange*(iScale/iMinScale);

        // Y change
        iPanY -= aXChange*(iScale/iMinScale);
		}
	else if (angle == 180)
		{
		// X change
        iPanX -= aXChange*(iScale/iMinScale);

        // Y change
        iPanY -= aYChange*(iScale/iMinScale);
		}
	else if (angle == 270)
		{
		// X change
        iPanX -= aYChange*(iScale/iMinScale);

        // Y change
        iPanY += aXChange*(iScale/iMinScale);
		}			

	UpdateCropRectL();

}

//=============================================================================
EXPORT_C void CEngineWrapper::RotateL (const TRotation aRot)
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::RotateL" );
#ifdef VERBOSE
    LOGFMT( KEngineWrapperLogFile, "\taRot: %d", (TInt)aRot);
#endif // VERBOSE

    //  Check that the model is found
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "Rotation: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

    //  Set undo / redo point
    AddUndoRedoStepL ();

    //  Add rotate filter
    AddRotateFilterL();

    //  Set rotation pars
    TBuf<128> cmd;
	cmd.Copy (_L("angle "));
    
    if ( ERotationCounterClockwise == aRot )
    {
		cmd.AppendNum (270);
    }
    else if ( ERotationClockwise == aRot )
    {
		cmd.AppendNum (90);
    }
    else if ( ERotation180 == aRot )
    {
		cmd.AppendNum (180);
    }
    else
    {
        User::Leave (KErrArgument);
    }
    SetParamsL (cmd);
}

//=============================================================================
EXPORT_C CSystemParameters * CEngineWrapper::GetSystemPars ()
{
	LOG( KEngineWrapperLogFile, "CEngineWrapper::GetSystemPars" );

	ComputeSystemParameters();
	
    return iSysPars;
}

//=============================================================================
void CEngineWrapper::ComputeSystemParameters ()
{
	LOG( KEngineWrapperLogFile, "CEngineWrapper::ComputeSystemParameters" );
	
	    //  Check that the model is found
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "GetSystemPars: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }

	MImageFilter * filter = iMainEngine->Filter(iMainEngine->NumFilters() - 3);
	TSize size = filter->ViewPortSize();
	iSysPars->ViewPortRect().iTl.iX = 0;
	iSysPars->ViewPortRect().iTl.iY = 0;
	iSysPars->ViewPortRect().iBr.iX = size.iWidth;
	iSysPars->ViewPortRect().iBr.iY = size.iHeight;

    LOGFMT( KEngineWrapperLogFile, "ViewPortRect().iTl.iX: %d", iSysPars->ViewPortRect().iTl.iX );
    LOGFMT( KEngineWrapperLogFile, "ViewPortRect().iTl.iY: %d", iSysPars->ViewPortRect().iTl.iY );
    LOGFMT( KEngineWrapperLogFile, "ViewPortRect().iBr.iX: %d", iSysPars->ViewPortRect().iBr.iX );
    LOGFMT( KEngineWrapperLogFile, "ViewPortRect().iBr.iY: %d", iSysPars->ViewPortRect().iBr.iY );

	iSysPars->VisibleImageRect() = filter->Rect();

    LOGFMT( KEngineWrapperLogFile, "VisibleImageRect().iTl.iX: %d", iSysPars->VisibleImageRect().iTl.iX );
    LOGFMT( KEngineWrapperLogFile, "VisibleImageRect().iTl.iY: %d", iSysPars->VisibleImageRect().iTl.iY );
    LOGFMT( KEngineWrapperLogFile, "VisibleImageRect().iBr.iX: %d", iSysPars->VisibleImageRect().iBr.iX );
    LOGFMT( KEngineWrapperLogFile, "VisibleImageRect().iBr.iY: %d", iSysPars->VisibleImageRect().iBr.iY );

    // Get the relative scale of the topmost filter
    iSysPars->RelScale() = filter->Scale();

	LOGFMT( KEngineWrapperLogFile, "RelScale(): %f", iSysPars->RelScale() );

	TInt n = iMainEngine->NumFilters();
	if ( n > 3 )
	{
	
		TRect rect;
	
		TSize srcsize = iMainEngine->Filter(n - 3)->Rect().Size();
		TSize tgtsize = iMainEngine->Filter(n - 1)->Rect().Size();

		//	Compute aspect ratio of the source
		TInt ars = (TReal)(srcsize.iWidth << KScaleBits) / srcsize.iHeight + 0.5;

		//	Compute aspect ratio of the target
		TInt art = (TReal)(tgtsize.iWidth << KScaleBits) / tgtsize.iHeight + 0.5;

		//	Select scale so that aspect ratio is preserved
		if ( ars >= art )
		{
			TInt scale = (TReal)(tgtsize.iWidth << KScaleBits) / srcsize.iWidth + 0.5;
			rect.iTl.iX = 0;
			rect.iBr.iX = tgtsize.iWidth;
			TInt h = (srcsize.iHeight * scale) >> KScaleBits;
			rect.iTl.iY = (TReal)(tgtsize.iHeight - h) / 2 + 0.5;
			rect.iBr.iY = (TReal)(tgtsize.iHeight + h) / 2 + 0.5;
		}
		else
		{
			TInt scale = (TReal)(tgtsize.iHeight << KScaleBits) / srcsize.iHeight + 0.5;
			rect.iTl.iY = 0;
			rect.iBr.iY = tgtsize.iHeight;
			TInt w = (srcsize.iWidth * scale) >> KScaleBits;
			rect.iTl.iX = (TReal)(tgtsize.iWidth - w) / 2 + 0.5;
			rect.iBr.iX = (TReal)(tgtsize.iWidth + w) / 2 + 0.5;
		}
   	
		iSysPars->VisibleImageRectPrev() = rect;
   		
	    LOGFMT( KEngineWrapperLogFile, "VisibleImageRectPrev().iTl.iX: %d", iSysPars->VisibleImageRectPrev().iTl.iX );
	    LOGFMT( KEngineWrapperLogFile, "VisibleImageRectPrev().iTl.iY: %d", iSysPars->VisibleImageRectPrev().iTl.iY );
	    LOGFMT( KEngineWrapperLogFile, "VisibleImageRectPrev().iBr.iX: %d", iSysPars->VisibleImageRectPrev().iBr.iX );
	    LOGFMT( KEngineWrapperLogFile, "VisibleImageRectPrev().iBr.iY: %d", iSysPars->VisibleImageRectPrev().iBr.iY );
	}
}



//=============================================================================
EXPORT_C void CEngineWrapper::GetOutputImageSize ( TInt& /*aWidth*/, TInt& /*aHeight*/ ) const
{

}

//=============================================================================
EXPORT_C void CEngineWrapper::SetJpegCommentL (const TDesC8& /*aComment*/)
{
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::SetJpegComment" );

    //  Check that the model is found
    if ( !iMainEngine )
    {
        LOG ( KEngineWrapperLogFile, "SetJpegComment: iMainEngine not created!" );
        User::Panic (KEnginePanic, KEnginePanicAllocation);
    }
}

//=============================================================================
void CEngineWrapper::AddRotateFilterL()
{
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::AddRotateFilterL()" );

    //  Add filter to engine
    AddFilterL (KFilterRotate);
}

//=============================================================================
TRect CEngineWrapper::ComputeViewPort (const TInt /*aStartInd*/)
{
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::ComputeViewPort()" );
    return TRect();
}

//=============================================================================
TRect CEngineWrapper::ComputeVisibleViewPort (const TRect & /*aViewPort*/)
{
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::ComputeVisibleViewPort()" );
	return TRect(0,0,0,0);
}

//=============================================================================
TPoint CEngineWrapper::ComputeNewPanValue (const TRect & /*aVvpOld*/)
{
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::ComputeNewPanValue()" );

	return TPoint();
}

//=============================================================================
void CEngineWrapper::CopyBufferL (TUint32 * aBuffer)
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::CopyBufferL" );
    
	iScreenBitmap->LockHeapLC();

	TSize size = iScreenBitmap->SizeInPixels();
	TDisplayMode dmode = iScreenBitmap->DisplayMode();
	TInt ws = iScreenBitmap->ScanLineLength (size.iWidth, dmode);

	TUint8 * tpos = (TUint8*)( iScreenBitmap->DataAddress() );
	TUint8 * tp;

	for ( TInt y = 0; y < size.iHeight; y++ )
	{
	
		tp = tpos;
		tpos += ws;
		
		for ( TInt x = 0; x < size.iWidth; x++ )
		{
			TUint32 c = *aBuffer++;
			*tp++ = c & 0xFF;
			c >>= 8;
			*tp++ = c & 0xFF;
			c >>= 8;
			*tp++ = c & 0xFF;
		}
	}

	CleanupStack::PopAndDestroy(); // iScreenBitmap->LockHeapLC	
}

//=============================================================================
TBool CEngineWrapper::IsSameString (
	const TUint8 *		aString1, 
	const TUint8 *		aString2
	)
{
	TPtrC8 s1( (TUint8*)aString1 );
	TPtrC8 s2( (TUint8*)aString2);
	
	if ( s1.Compare( s2 ) == 0 )
	{
		return ETrue;
	}
	else
	{
		return EFalse;
	}		
}

//=============================================================================
void CEngineWrapper::UpdateCropRectL ()
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateCropRect" );

    TSize screensize = iScreenSize;
    TSize imagesize = iBoundingRect.Size();
	TRect croprect = iBoundingRect;

	if (iScale != 1.0)
	{
	    //  check rotation
	    TInt angle = ComputeRotationL();
	    TSize cropsize = imagesize;
	    TReal sw = 0.0;
	    TReal sh = 0.0;
	    if ((angle == 90) || (angle == 270))
	    {
	        cropsize.iWidth = imagesize.iWidth;
	        cropsize.iHeight = (TReal)(screensize.iWidth * imagesize.iWidth) / screensize.iHeight + 0.5;
	        TReal ari = (TReal)cropsize.iWidth / cropsize.iHeight;    
	        TReal ars = (TReal)screensize.iWidth / screensize.iHeight;    
	        if (ari >= ars)
	        {
	            sw = 0.5 * iScale * cropsize.iWidth;
	            sh = 0.5 * iScale * cropsize.iHeight;
	        }
	        else
	        {
	            sh = 0.5 * iScale * cropsize.iHeight;
	            sw = 0.5 * iScale * cropsize.iWidth;
	        }
	    }
	    else
	    {
	        TReal ari = (TReal)cropsize.iWidth / cropsize.iHeight;    
	        TReal ars = (TReal)screensize.iWidth / screensize.iHeight;    
	        if (ari >= ars)
	        {
	            sw = 0.5 * iScale * cropsize.iWidth;
	            sh = 0.5 * iScale * (cropsize.iWidth / ars);
	        }
	        else
	        {
	            sh = 0.5 * iScale * cropsize.iHeight;
	            sw = 0.5 * iScale * (cropsize.iHeight * ars);
	        }
	    }

	    //  clip pan
	    if (iPanX - sw < iBoundingRect.iTl.iX)
	    {
	    	if (iScale == 1.0)
	    	{
	    		iPanX = (iBoundingRect.iTl.iX + iBoundingRect.iBr.iX) / 2;
	    	}
	    	else
	    	{
	        	iPanX = iBoundingRect.iTl.iX + sw;
	    	}
	    }
	    else if (iPanX + sw > iBoundingRect.iBr.iX)
	    {   
	    	if (iScale == 1.0)
	    	{
	    		iPanX = (iBoundingRect.iTl.iX + iBoundingRect.iBr.iX) / 2;
	    	}
	    	else
	    	{
	        	iPanX = iBoundingRect.iBr.iX - sw;
	    	}
	    }
	    if (iPanY - sh < iBoundingRect.iTl.iY)
	    {
	    	if (iScale == 1.0)
	    	{
	    		iPanY = (iBoundingRect.iTl.iY + iBoundingRect.iBr.iY) / 2;
	    	}
	    	else
	    	{
	        	iPanY = iBoundingRect.iTl.iY + sh;
	    	}
	    }
	    else if (iPanY + sh > iBoundingRect.iBr.iY)
	    {   
	    	if (iScale == 1.0)
	    	{
	    		iPanY = (iBoundingRect.iTl.iY + iBoundingRect.iBr.iY) / 2;
	    	}
	    	else
	    	{
	        	iPanY = iBoundingRect.iBr.iY - sh;
	    	}
	    }

	    //  scale rect
	    croprect = TRect(iPanX - sw, iPanY - sh, iPanX + sw, iPanY + sh);
	    if (croprect.iTl.iX < iBoundingRect.iTl.iX)
	    {
	        croprect.iTl.iX = iBoundingRect.iTl.iX;
	    }
	    if (croprect.iTl.iY < iBoundingRect.iTl.iY)
	    {
	        croprect.iTl.iY = iBoundingRect.iTl.iY;
	    }
	    if (croprect.iBr.iX > iBoundingRect.iBr.iX)
	    {
	        croprect.iBr.iX = iBoundingRect.iBr.iX;
	    }
	    if (croprect.iBr.iY > iBoundingRect.iBr.iY)
	    {
	        croprect.iBr.iY = iBoundingRect.iBr.iY;
	    }

	}
	
	if (croprect != iOldCropRect)
	{
		//	Update crop rectangle
		iCmd.Format( _L("ulc %d ulr %d lrc %d lrr %d"), croprect.iTl.iX, croprect.iTl.iY, 
			croprect.iBr.iX, croprect.iBr.iY);
		LOGDES (KEngineWrapperLogFile, iCmd);			
		iMainEngine->FunctionL ( 0, iCmd );
		iRenderScaleBuffer = ETrue;
		iOldCropRect = croprect;
	}
}


//=============================================================================
TPtrC8 CEngineWrapper::UpdateExifThumbnailL ()
{
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifThumbnailL" );

	//	Create Jpeg encoder for memory buffer
	MJpegSave * encoder = JpegSaveFactory::CreateJpegSaveLC (0,0);

    LOG ( KEngineWrapperLogFile, "UpdateExifThumbnailL::0" );
 	
	// 	Initialize saving
	encoder->StartSaveL (iThumbSize, TPtr8(0,0), 65536, KThumbnailJpegQuality);

    LOG ( KEngineWrapperLogFile, "UpdateExifThumbnailL::1" );

	// 	Save in blocks
	TBitmapHandle bmBlock;
	bmBlock.iData = new (ELeave) TUint32 [16 * 8];

    LOG ( KEngineWrapperLogFile, "UpdateExifThumbnailL::2" );
	
	for (TInt y = 0; y < iThumbSize.iHeight; y += 8)
	{
		for (TInt x = 0; x < iThumbSize.iWidth; x += 16)
		{
		
			TInt lastY = y + 8;
			if (lastY >= iThumbSize.iHeight)
			{
				lastY = iThumbSize.iHeight - 1;
			}
			TInt lastX = x + 16;
			if (lastX >= iThumbSize.iWidth)
			{
				lastX = iThumbSize.iWidth - 1;			
			}
			
			TUint32 * pDOS = (TUint32 *)bmBlock.iData;
			TUint32 * pSOS = iThumb + y * iThumbSize.iWidth + x;
			
			for (TInt yy = y; yy < lastY; yy++ )
			{
			
				TUint32 * pD = pDOS;
				pDOS += 16;

				TUint32 * pS = pSOS;
				pSOS += iThumbSize.iWidth;
				
				for (TInt xx = x; xx < lastX; xx++ )
				{
					*pD++ = *pS++;
				}
			}
		
		
			encoder->SaveBlock (bmBlock);
		}
	}

    LOG ( KEngineWrapperLogFile, "UpdateExifThumbnailL::3" );
	
	delete (TUint32 *)bmBlock.iData;
	bmBlock.iData = NULL;

    LOG ( KEngineWrapperLogFile, "UpdateExifThumbnailL::4" );

	TPtrC8 thumbNail = encoder->Finalize();

    LOG ( KEngineWrapperLogFile, "UpdateExifThumbnailL::5" );

	TPtrC8 exif = iExifParser->SaveL (thumbNail);

    LOG ( KEngineWrapperLogFile, "UpdateExifThumbnailL::6" );

	CleanupStack::PopAndDestroy (); // encoder

	return exif;
}    

//=============================================================================
void CEngineWrapper::UpdateExifTagsL()
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL" );

    // DateTime 
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- DateTime" );
    TBuf8<64> dateTimeBuf;
    GetCurrentDateTime ( dateTimeBuf );
    iExifParser->DeleteTag ( CExifParser::EIfd0, 0x0132);
    iExifParser->AddTagL ( CExifParser::EIfd0, 0x0132, dateTimeBuf.PtrZ());

    TInt n = iMainEngine->NumFilters();
    MImageFilter * filter = iMainEngine->Filter(n - 2);
	
	TInt targetWidth = (TReal)(filter->ViewPortSize().iWidth * iSysPars->Scale()) + 0.5;
	TInt targetHeight = (TReal)(filter->ViewPortSize().iHeight * iSysPars->Scale()) + 0.5;

    // ImageWidth - not needed  
    iExifParser->DeleteTag ( CExifParser::EIfd0, 0x0100);

    // ImageHeight - not needed  
    iExifParser->DeleteTag ( CExifParser::EIfd0, 0x0101);

    // Orientation
    if ( !iExifParser->TagExist (CExifParser::EIfd0, 0x0112) )
    {
    	LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- Orientation" );
    	iExifParser->AddTagL ( CExifParser::EIfd0, 0x0112, (TUint16)1);
    }

    // EXIF version 
    TUint8 exifVersion [] = "0220";
    if ( !iExifParser->TagExist (CExifParser::ESubIfd, 0x9000) )
    {
	   	LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- ExifVersion" );
	    iExifParser->AddTagL ( CExifParser::ESubIfd, 0x9000, TPtrC8 (exifVersion));
    }
    
    // FlashPixVersion
    TUint8 flashPixVersion [] = "0100";
    if ( !iExifParser->TagExist (CExifParser::ESubIfd, 0xA000) )
    {
	    LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- FlashPixVersion" );
	    iExifParser->DeleteTag ( CExifParser::ESubIfd, 0xA000);
	    iExifParser->AddTagL ( CExifParser::ESubIfd, 0xA000, TPtrC8 (flashPixVersion));
    }
    
    // ComponentsConfiguration
    TUint8 exifComponentsConfiguration [4] = {1,2,3,0};
    LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- ComponentsConfiguration" );
    iExifParser->DeleteTag ( CExifParser::ESubIfd, 0x9101);
    iExifParser->AddTagL ( CExifParser::ESubIfd, 0x9101, TPtrC8(exifComponentsConfiguration, 4));

    // ColorSpace 
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- ColorSpace" );
    iExifParser->DeleteTag ( CExifParser::ESubIfd, 0xA001);
    iExifParser->AddTagL ( CExifParser::ESubIfd, 0xA001, (TUint16)1);

    // Interoperability index 
    iExifParser->DeleteTag ( CExifParser::EInteroperability, 0x0001);

    // PixelXResolution 
    LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- PlaneXResolution" );
    iExifParser->DeleteTag ( CExifParser::ESubIfd, 0xA002);
    iExifParser->AddTagL ( CExifParser::ESubIfd, 0xA002,(TUint32)targetWidth);

    // PixelYResolution 
    LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- PlaneYResolution" );
    iExifParser->DeleteTag ( CExifParser::ESubIfd, 0xA003);
    iExifParser->AddTagL ( CExifParser::ESubIfd, 0xA003, (TUint32)targetHeight);

    // FocalPlaneResolutionUnit
    iExifParser->DeleteTag ( CExifParser::ESubIfd, 0xA210);

    // FocalPlaneXResolution 
    iExifParser->DeleteTag ( CExifParser::ESubIfd, 0xA20E);
    
    // FocalPlaneYResolution 
    iExifParser->DeleteTag ( CExifParser::ESubIfd, 0xA20F);

    // User Comment 
    TBuf8<256> comment;
    TUint8 KCharCode[8] = {0x41,0x53,0x43,0x49,0x49,0x00,0x00,0x00}; // ASCII char code
	comment.Append(TPtrC8(KCharCode, 8));
    comment.AppendFormat(KImageEditorExifComment, my_version_major, my_version_minor, my_version_build);

    LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- UserComment" );
    iExifParser->DeleteTag ( CExifParser::ESubIfd, 0x9286);
    iExifParser->AddTagL ( CExifParser::ESubIfd, 0x9286, comment);

    TBuf8<128> make;
    TBuf8<128> model;
    ImageEditorUtils::GetMakeAndModelL ( make, model );

    // Make
    if ( !iExifParser->TagExist (CExifParser::EIfd0, 0x010F) )
    {
    	LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- Make updated" );
	    iExifParser->AddTagL ( CExifParser::EIfd0, 0x010F, make.PtrZ());
    }

    // Model
    if ( !iExifParser->TagExist (CExifParser::EIfd0, 0x0110) )
    {
    	LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- Model updated" );
	    iExifParser->AddTagL ( CExifParser::EIfd0, 0x0110, model.PtrZ());
    }

    // DateTimeOriginal
    if ( !iExifParser->TagExist (CExifParser::ESubIfd, 0x9003) )
    {
    	LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- DateTimeOriginal updated" );
	    iExifParser->AddTagL ( CExifParser::ESubIfd, 0x9003, dateTimeBuf.PtrZ());
    }

    // DateTimeDigitized
    if ( !iExifParser->TagExist (CExifParser::ESubIfd, 0x9004) )
    {
    	LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- DateTimeDigitized updated" );
	    iExifParser->AddTagL ( CExifParser::ESubIfd, 0x9004, dateTimeBuf.PtrZ());
    }
        
    // XResolution
    LOG ( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- XResolution" );
    if ( !iExifParser->TagExist (CExifParser::EIfd0, 0x011A) )
    {
    	iExifParser->AddTagL ( CExifParser::EIfd0, 0x011A, (TUint32)300, (TUint32)1);	
    }
    
    // YResolution
    LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- YResolution" );
    if ( !iExifParser->TagExist (CExifParser::EIfd0, 0x011B) )
    {
    	iExifParser->AddTagL ( CExifParser::EIfd0, 0x011B, (TUint32)300, (TUint32)1);	
    }
    
    // ResolutionUnit
    LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- ResolutionUnit" );
    if ( !iExifParser->TagExist (CExifParser::EIfd0, 0x0128) )
    {
    	iExifParser->AddTagL ( CExifParser::EIfd0, 0x0128, (TUint16)2 ); 	
    }
    
    // YCbCrPositioning
    LOG( KEngineWrapperLogFile, "CEngineWrapper::UpdateExifTagsL() -- YCbCrPositioning" );
    if ( !iExifParser->TagExist (CExifParser::EIfd0, 0x0213) )
    {
    	iExifParser->AddTagL ( CExifParser::EIfd0, 0x0213, (TUint16)1 ); 	
    }
  
    
}

//=============================================================================
void CEngineWrapper::GetCurrentDateTime (TDes8 & aDateTimeBuf) const
{
    // Time in microseconds since 0 AD nominal Gregorian
    TTime time;
    // year-month-day-hour-minute-second-microsecond
    TDateTime dateTime;

    // Get current local time
    time.HomeTime();

    // Convert to fields
    dateTime = time.DateTime();

    // Create descriptors for the components.
    // This is needed because the leading zeros 
    // cannotbe suppressed.
    TBuf8<2> month;
    TBuf8<2> day;
    TBuf8<2> hour;
    TBuf8<2> minute;
    TBuf8<2> second;

    if( TInt(dateTime.Month()) < 9 )
    {
        month.AppendNum( 0 );
    }
    month.AppendNum( TInt(dateTime.Month()+1) );

    if( dateTime.Day() < 9 )
    {
        day.AppendNum( 0 );
    }
    day.AppendNum( dateTime.Day()+1 );

    if( dateTime.Hour() < 10 )
    {
        hour.AppendNum( 0 );
    }
    hour.AppendNum( dateTime.Hour() );

    if( dateTime.Minute() < 10 )
    {
        minute.AppendNum( 0 );
    }
    minute.AppendNum( dateTime.Minute() );

    if( dateTime.Second() < 10 )
    {
        second.AppendNum( 0 );
    }
    second.AppendNum( dateTime.Second() );

    // Format the time to the format
    // "YYYY:MM:DD HH:MM:SS"
    _LIT8(KFormatTxt,"%d:%S:%S %S:%S:%S");
    aDateTimeBuf.Format(
        KFormatTxt,
        dateTime.Year(),
        &month, 
        &day, 
        &hour, 
        &minute, 
        &second
        );
}

//=============================================================================
EXPORT_C void CEngineWrapper::SetBitmap (CFbsBitmap * aBitmap)
{
	iScreenBitmap = aBitmap;
}


//=============================================================================
EXPORT_C void CEngineWrapper::CreateExifThumbNailL ()
{
    LOG( KEngineWrapperLogFile, "CEngineWrapper::CreateExifThumbNailL" );

#ifdef EXIF_SUPPORT    
    
    //	Delete old thumbnail
    delete[] iThumb;
    iThumb = NULL; 

	//	Re-load the zoomed image if needed
	if ( iRenderScaleBuffer )
	{
		iMainEngine->FunctionL ( 0, _L("loadimage"));
		iRenderScaleBuffer = EFalse;
	}
	for ( TInt i = 0; i < iMainEngine->NumFilters(); ++i)
	{
		iMainEngine->FunctionL (i, _L("nop"));
	}

	TReal scale = 1.0;
	TSize imagesize = iMainEngine->Filter(iMainEngine->NumFilters() - 3)->Rect().Size();
	if (imagesize.iWidth > imagesize.iHeight)
	{
		scale = (TReal)imagesize.iWidth / KThumbnailMaxDimension;
	}
	else
	{
		scale = (TReal)imagesize.iHeight / KThumbnailMaxDimension;
	}

	//	Crete new thumbnail
	iThumbSize.iWidth = (TInt)((imagesize.iWidth / scale) + 0.5);
	iThumbSize.iHeight = (TInt)((imagesize.iHeight / scale) + 0.5);
	
	// Just ensure that dimension are never 0
	if (iThumbSize.iWidth == 0)
	{
		iThumbSize.iWidth = 1;
	}
	if (iThumbSize.iHeight == 0)
	{
		iThumbSize.iHeight = 1;
	}
	
    LOGFMT( KEngineWrapperLogFile, "iThumbSize.iWidth = %d", iThumbSize.iWidth);
    LOGFMT( KEngineWrapperLogFile, "iThumbSize.iHeight = %d", iThumbSize.iHeight);

	iThumb = new (ELeave) TUint32 [iThumbSize.iWidth * iThumbSize.iHeight];
	
	//	Set thumbnail size to buffer filter
	iCmd.Format( _L("width %d height %d"), iThumbSize.iWidth, iThumbSize.iHeight);
	iMainEngine->FunctionL ( iMainEngine->NumFilters() - 1, iCmd );
	iMainEngine->FunctionL ( iMainEngine->NumFilters() - 2, _L("nop"));

	//	Copy data to the new thumbnail
	TUint32 * pBuffer = (TUint32 *)iMainEngine->FunctionL (iMainEngine->NumFilters() - 1, _L("getbitmap"));
	Mem::Copy (iThumb, pBuffer, iThumbSize.iWidth * iThumbSize.iHeight * sizeof(TUint32));

    LOG( KEngineWrapperLogFile, "EXIF thumbnail created!" );
	
#endif
}


//=============================================================================
TInt CEngineWrapper::ComputeRotationL ()
{
	
	TInt angle = 0;
	TInt ind = 0;
	while ( ind < iMainEngine->NumFilters() )
	{
	    MImageFilter * filter = iMainEngine->Filter(ind);
	    char * cmpstr = "rotate";
	    TPtrC8 type ( (const TUint8 *)filter->Type() );
	    TPtrC8 typecmp ((const TUint8 *)cmpstr);
	    if ( type == typecmp )
	    {
	        angle += filter->CmdL(_L("getangle"));
	    }
	    ind++;
	}
	angle %= 360;
	return angle;
}


//=============================================================================
void CEngineWrapper::ComputeBoundingRectL()
{
    MImageFilter * pFilter = iMainEngine->Filter(iMainEngine->NumFilters() - 3);
    TReal relscale = pFilter->Scale();
    TRect rect = pFilter->Rect();
    TSize size = iMainEngine->Filter(0)->ViewPortSize();
    rect.iTl.iX = (TInt)((rect.iTl.iX / relscale) + 0.5);
    rect.iTl.iY = (TInt)((rect.iTl.iY / relscale) + 0.5);
    rect.iBr.iX = (TInt)((rect.iBr.iX / relscale) + 0.5);
    rect.iBr.iY = (TInt)((rect.iBr.iY / relscale) + 0.5);
    TInt angle = ComputeRotationL();
 
    if (angle == 90 || angle == 270)
    {
        iBoundingRect.iTl.iX = rect.iTl.iY;
        iBoundingRect.iTl.iY = rect.iTl.iX;
        iBoundingRect.iBr.iX = rect.iBr.iY;
        iBoundingRect.iBr.iY = rect.iBr.iX;
    }
    else
    {
        iBoundingRect = rect;
    }

    size = iBoundingRect.Size();
    iPanX = (iBoundingRect.iTl.iX + iBoundingRect.iBr.iX) / 2;
    iPanY = (iBoundingRect.iTl.iY + iBoundingRect.iBr.iY) / 2;
}


// End of File
