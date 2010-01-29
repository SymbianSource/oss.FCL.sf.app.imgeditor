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



#include "ImageEditorCartoonizePlugin.h"

/// CONSTANTS
_LIT (KPgnResourceFile, "cartoonize.rsc");


//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorCartoonizePlugin * plugin = new (ELeave) CImageEditorCartoonizePlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorCartoonizePlugin::CImageEditorCartoonizePlugin () :
CImageEditorPluginBase()
{

}

//=============================================================================
void CImageEditorCartoonizePlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorCartoonizePlugin::~CImageEditorCartoonizePlugin ()
{
    ReleasePlugin();
}

//=============================================================================
TInt CImageEditorCartoonizePlugin::SetProperty (
	TInt			/*aPropertyId*/, 
	TDesC &			/*aPropertyValue*/
	)
{
	return KErrNotSupported;
}

//=============================================================================
TInt CImageEditorCartoonizePlugin::GetProperty (
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
			aPropertyValue.Append (_L("quantcolors"));
			return KErrNone;
		}
		default:
		{
			return CImageEditorPluginBase::GetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorCartoonizePlugin::InitPluginL (
	const TRect &		/*aRect*/,
	CCoeControl *		/*aParent*/,
	CCoeControl *&		aPluginControl
	)
{
	aPluginControl = 0;
	return KErrNone;
}

//=============================================================================
void CImageEditorCartoonizePlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{
	// Own image processing functionality here
}

//=============================================================================
void CImageEditorCartoonizePlugin::ReleasePlugin ()
{

}

// End of File
