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
* Resize plugin plugin base class.
*
*/



/// INCLUDES
#include <resize.mbg>
#include <resize.rsg>
#include <aknutils.h> 
#include <aknnotewrappers.h> 
#include <ConeResLoader.h> 
#include <stringloader.h>
#include "ImageEditorResizePlugin.h"
#include "ImageEditorPluginBaseDefs.h"
#include "ResizeControl.h"
#include "SystemParameters.h"

/// CONSTANTS
_LIT (KPgnResourceFile, "resize.rsc");

//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
	{
	CImageEditorResizePlugin * plugin = new (ELeave) CImageEditorResizePlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
	return plugin;
	}

//=============================================================================
CImageEditorResizePlugin::CImageEditorResizePlugin () :
		CImageEditorPluginBase()
	{

	}

//=============================================================================
void CImageEditorResizePlugin::ConstructL ()
	{
	CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
	}

//=============================================================================
CImageEditorResizePlugin::~CImageEditorResizePlugin ()
	{
	ReleasePlugin();
    iSysPars = NULL;
    iControl = NULL;
	}

//=============================================================================
TInt CImageEditorResizePlugin::SetProperty (
    TInt            aPropertyId, 
    TDesC &        	aPropertyValue
    )
{

    TLex	parser;

	//	Copy data
	switch (aPropertyId) 
	{
		case KCapSystemParameters:
		{
			parser.Assign (aPropertyValue);
            TInt tempval = 0;
            parser.Val (tempval);
            iSysPars = (CSystemParameters *)tempval;
			return KErrNone;
		}
		default:
		{
            return CImageEditorPluginBase::SetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorResizePlugin::GetProperty (
	TInt 			aPropertyId, 
	TDes	& 		aPropertyValue
	)
{
	//	Clear buffer
	aPropertyValue.Zero();

	//	Copy data
	switch (aPropertyId) 
	{
		case KCapGlobalZoomDisabled:
		{
			aPropertyValue.AppendNum ( (TInt)ETrue );
			return KErrNone;
		}
		default:
		{
			return CImageEditorPluginBase::GetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorResizePlugin::InitPluginL (
    const TRect &       aRect, 
    CCoeControl *	    aParent,
    CCoeControl* &  aPluginControl
    )
	{
	ReleasePlugin();

    //  Store image size
    TInt width = 
        (iSysPars->ViewPortRect().iBr.iX - 
        iSysPars->ViewPortRect().iTl.iX);
    TInt height = 
        (iSysPars->ViewPortRect().iBr.iY - 
        iSysPars->ViewPortRect().iTl.iY);
	TInt origSize = 0;
	// Landscape
	if (height < width)
		{
    	origSize = (TInt)(iSysPars->Scale() * width + 0.5);
		}
	else
		{
    	origSize = (TInt)(iSysPars->Scale() * height + 0.5);
		}

#ifdef RESIZE_CIF_SIZES_SUPPORTED
	const TInt KMinSize(128);
#else
	const TInt KMinSize(320);
#endif

	if (origSize <= KMinSize)
		{

	    //	Read resource
		TFileName resourcefile;
		resourcefile.Append(KPgnResourcePath);
		resourcefile.Append(KPgnResourceFile);
	    User::LeaveIfError( CompleteWithAppPath( resourcefile ) );

	    //  Get reference to application file session
	    RFs & fs = CEikonEnv::Static()->FsSession();
	    
	    // Implementation of RConeResourceLoader uses BaflUtils::NearestLanguageFile 
	    // to search for a localised resource in proper search order
	    RConeResourceLoader resLoader( *CEikonEnv::Static() );
	    resLoader.OpenL ( resourcefile );
	    CleanupClosePushL(resLoader);

    	TBuf<256> readbuf;
    	CEikonEnv::Static()->ReadResourceAsDes16L(readbuf, R_SIE_TOO_SMALL_TO_RESIZE);
	    CleanupStack::PopAndDestroy(); // resLoader

        CAknInformationNote* infoNote = new( ELeave ) CAknInformationNote( ETrue );
        infoNote->ExecuteLD( readbuf ); // ignore return value, not used

		return KErrNotSupported;
		}

	iControl = CResizeControl::NewL (aRect, aParent);
    ((CResizeControl *)iControl)->SetSystemParameters (iSysPars, this);
	aPluginControl = iControl;

	return KErrNone;
	}

//=============================================================================
void CImageEditorResizePlugin::ProcessImageL (CEditorImage * /*aImage*/ )
	{
	// Own image processing functionality here
	}

//=============================================================================
void CImageEditorResizePlugin::ReleasePlugin()
	{
	delete iControl;
	iControl = NULL;
	}

void CImageEditorResizePlugin::ShowPopupNote ()
	{
	((CResizeControl *)iControl)->ShowPopupNote();
	}

// End of File

