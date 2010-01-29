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


#include <aknquerydialog.h> 

#include "ImageEditorFramePlugin.h"
#include "ImageEditorFrameControl.h"
#include "ImageEditorPluginBaseDefs.h"
#include "DrawUtils.h"

//	CONSTANTS
_LIT (KPgnResourceFile, "frame.rsc");


//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorFramePlugin * plugin = new (ELeave) CImageEditorFramePlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorFramePlugin::CImageEditorFramePlugin ()
{

}

//=============================================================================
void CImageEditorFramePlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorFramePlugin::~CImageEditorFramePlugin ()
{
    ReleasePlugin();
}

//=============================================================================
TInt CImageEditorFramePlugin::SetProperty (
	TInt		/*aPropertyId*/, 
	TDesC &		/*aPropertyValue*/
	)
{
	return KErrNotSupported;
}

//=============================================================================
TInt CImageEditorFramePlugin::GetProperty (
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
    		LOG(KFramePluginLogFile, "CImageEditorFrameControl::GetProperty()");
			aPropertyValue.Copy (((CImageEditorFrameControl*)iControl)->GetParam());
			LOGDES (KFramePluginLogFile, aPropertyValue);
			return KErrNone;
		}
        case KCapGlobalZoomDisabled:
        {
            aPropertyValue.AppendNum((TInt)ETrue);
            return KErrNone;
        }
		default:
		{
			return CImageEditorPluginBase::GetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorFramePlugin::InitPluginL (
	const TRect &		aRect,
	CCoeControl *		aParent,
	CCoeControl *&		aPluginControl
	)
{
    // create log file if the logging is enabled
	LOG_INIT(KFramePluginLogFile);

    //  Delete previous control
	ReleasePlugin();

    // Create new control
    iControl = CImageEditorFrameControl::NewL (aRect, aParent);
    aPluginControl = iControl;

    return KErrNone;
}

//=============================================================================
void CImageEditorFramePlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{

}

//=============================================================================
void CImageEditorFramePlugin::ReleasePlugin ()
{
	delete iControl;
	iControl = 0;
}

// End of File
