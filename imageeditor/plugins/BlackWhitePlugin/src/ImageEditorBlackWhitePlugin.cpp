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



#include "ImageEditorBlackWhitePlugin.h"

/// CONSTANTS
_LIT (KPgnResourceFile, "blackwhite.rsc");

//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorBlackWhitePlugin * plugin = new (ELeave) CImageEditorBlackWhitePlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorBlackWhitePlugin::CImageEditorBlackWhitePlugin () :
CImageEditorPluginBase()
{

}

//=============================================================================
void CImageEditorBlackWhitePlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorBlackWhitePlugin::~CImageEditorBlackWhitePlugin ()
{
    ReleasePlugin();
}

//=============================================================================
TInt CImageEditorBlackWhitePlugin::SetProperty (
	TInt			/*aPropertyId*/, 
	TDesC &			/*aPropertyValue*/
	)
{
	return KErrNotSupported;
}

//=============================================================================
TInt CImageEditorBlackWhitePlugin::GetProperty (
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
TInt CImageEditorBlackWhitePlugin::InitPluginL (
	const TRect &		/*aRect*/,
	CCoeControl *		/*aParent*/,
	CCoeControl *&		aPluginControl
	)
{
	aPluginControl = 0;
	return KErrNone;
}

//=============================================================================
void CImageEditorBlackWhitePlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{
	// Own image processing functionality here
    
}

//=============================================================================
void CImageEditorBlackWhitePlugin::ReleasePlugin ()
{

}

// End of File
