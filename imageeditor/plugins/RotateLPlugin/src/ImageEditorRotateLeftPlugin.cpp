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



#include "ImageEditorRotateLeftPlugin.h"
#include "commondefs.h"

/// CONSTANTS
_LIT (KPgnResourceFile, "rotateleft.rsc");


//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorRotateLeftPlugin * plugin = new (ELeave) CImageEditorRotateLeftPlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorRotateLeftPlugin::CImageEditorRotateLeftPlugin () :
CImageEditorPluginBase()
{

}

//=============================================================================
void CImageEditorRotateLeftPlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorRotateLeftPlugin::~CImageEditorRotateLeftPlugin ()
{
    ReleasePlugin();
}

//=============================================================================
TInt CImageEditorRotateLeftPlugin::SetProperty (
	TInt			aPropertyId, 
	TDesC &			aPropertyValue
	)
{
    return CImageEditorPluginBase::SetProperty(aPropertyId, aPropertyValue);
}

//=============================================================================
TInt CImageEditorRotateLeftPlugin::GetProperty (
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
			aPropertyValue.Copy (_L("rotateccw"));
			return KErrNone;
		}
		default:
		{
			return CImageEditorPluginBase::GetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorRotateLeftPlugin::InitPluginL (
	const TRect &		/*aRect*/,
	CCoeControl *		/*aParent*/,
	CCoeControl *&		aPluginControl
	)
{
	aPluginControl = 0;
	return KErrNone;
}

//=============================================================================
void CImageEditorRotateLeftPlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{
	// Own image processing functionality here
}

//=============================================================================
void CImageEditorRotateLeftPlugin::ReleasePlugin ()
{

}

// End of File
