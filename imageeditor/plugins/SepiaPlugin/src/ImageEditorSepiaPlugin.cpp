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


#include "ImageEditorSepiaPlugin.h"

/// CONSTANTS
_LIT (KPgnResourceFile, "sepia.rsc");


//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorSepiaPlugin * plugin = new (ELeave) CImageEditorSepiaPlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorSepiaPlugin::CImageEditorSepiaPlugin () :
CImageEditorPluginBase()
{

}

//=============================================================================
void CImageEditorSepiaPlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorSepiaPlugin::~CImageEditorSepiaPlugin ()
{
    ReleasePlugin();
}

//=============================================================================
TInt CImageEditorSepiaPlugin::SetProperty (
	TInt			/*aPropertyId*/, 
	TDesC &			/*aPropertyValue*/
	)
{
	return KErrNotSupported;
}

//=============================================================================
TInt CImageEditorSepiaPlugin::GetProperty (
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
			return KErrNone;
		}
		default:
		{
			return CImageEditorPluginBase::GetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorSepiaPlugin::InitPluginL (
	const TRect &		/*aRect*/,
	CCoeControl *		/*aParent*/,
	CCoeControl *&		aPluginControl
	)
{
	aPluginControl = 0;
	return KErrNone;
}

//=============================================================================
void CImageEditorSepiaPlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{
	// Own image processing functionality here
}

//=============================================================================
void CImageEditorSepiaPlugin::ReleasePlugin ()
{

}

// End of File
