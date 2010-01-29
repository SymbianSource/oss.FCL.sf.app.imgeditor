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
* Clipart plugin's plugin class.
*
*/


#include "ImageEditorClipartPlugin.h"
#include "ImageEditorClipartControl.h"
#include "ImageEditorPluginBaseDefs.h"
#include "DrawUtils.h"
#include "SystemParameters.h"

//	CONSTANTS
_LIT (KPgnResourceFile, "clipart.rsc");

//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorClipartPlugin * plugin = new (ELeave) CImageEditorClipartPlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorClipartPlugin::CImageEditorClipartPlugin ()
{

}

//=============================================================================
void CImageEditorClipartPlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorClipartPlugin::~CImageEditorClipartPlugin ()
{
    iSysPars = NULL;
    ReleasePlugin();
}

//=============================================================================
TInt CImageEditorClipartPlugin::SetProperty (
	TInt		aPropertyId, 
	TDesC &		aPropertyValue
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
            iSysPars = (const CSystemParameters *)tempval;
			return KErrNone;
		}
		default:
		{
			return CImageEditorPluginBase::SetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorClipartPlugin::GetProperty (
	TInt		aPropertyId, 
	TDes &		aPropertyValue
	)
{
	//	Clean buffer
	aPropertyValue.Zero();

	//	Copy data
	switch (aPropertyId) 
	{
		case KCapParamStruct:
		{
			// Control found
			if (iControl)
			{
				aPropertyValue.Copy ( ((CImageEditorClipartControl *)iControl)->GetParam() );
				return KErrNone;
			}

			//	Control not found, return KErrNotReady
			else
			{
				return KErrNotReady;
			}
		}
        case KCapReadyToRender:
		{
            TBool readyToRender = EFalse;
			if ( iControl )
			{
				readyToRender =  ((CImageEditorClipartControl *)iControl)->IsReadyToRender();
			}
			aPropertyValue.AppendNum ((TInt)readyToRender);
			return KErrNone;
		}
        case KCapIsSlowPlugin:
		{
			aPropertyValue.AppendNum ((TInt)ETrue);
			return KErrNone;
		}
		default:
		{
			return CImageEditorPluginBase::GetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorClipartPlugin::InitPluginL (
	const TRect &		aRect,
	CCoeControl *		aParent,
	CCoeControl *&		aPluginControl
	)
{
    //  Delete previous control
	ReleasePlugin();

    //  Create the control
    iControl = CImageEditorClipartControl::NewL (aRect, aParent);
    aPluginControl = iControl;

    //  Set system parameters
    ((CImageEditorClipartControl*)iControl)->SetSystemParameters (iSysPars);

    return KErrNone;
}

//=============================================================================
void CImageEditorClipartPlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{

}

//=============================================================================
void CImageEditorClipartPlugin::ReleasePlugin ()
{
	delete iControl;
	iControl = 0;
}


// End of File
