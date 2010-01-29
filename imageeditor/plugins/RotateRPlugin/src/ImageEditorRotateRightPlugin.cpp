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



#include "ImageEditorRotateRightPlugin.h"

/// CONSTANTS
_LIT (KPgnResourceFile, "rotateright.rsc");


//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorRotateRightPlugin * plugin = new (ELeave) CImageEditorRotateRightPlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorRotateRightPlugin::CImageEditorRotateRightPlugin () :
CImageEditorPluginBase()
{

}

//=============================================================================
void CImageEditorRotateRightPlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorRotateRightPlugin::~CImageEditorRotateRightPlugin ()
{
    ReleasePlugin();
}

//=============================================================================
TInt CImageEditorRotateRightPlugin::SetProperty (
	TInt			aPropertyId, 
	TDesC &			aPropertyValue
	)
{
    return CImageEditorPluginBase::SetProperty(aPropertyId, aPropertyValue);
}

//=============================================================================
TInt CImageEditorRotateRightPlugin::GetProperty (
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
			aPropertyValue.Copy (_L("rotatecw"));
			return KErrNone;
		}
		default:
		{
			return CImageEditorPluginBase::GetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorRotateRightPlugin::InitPluginL (
	const TRect &		/*aRect*/,
	CCoeControl *		/*aParent*/,
	CCoeControl *&		aPluginControl
	)
{
	aPluginControl = 0;
	return KErrNone;
}

//=============================================================================
void CImageEditorRotateRightPlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{
	// Own image processing functionality here
}

//=============================================================================
void CImageEditorRotateRightPlugin::ReleasePlugin ()
{

}

// End of File
