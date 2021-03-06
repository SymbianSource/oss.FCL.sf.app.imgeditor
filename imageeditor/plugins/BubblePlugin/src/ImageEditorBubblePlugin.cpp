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
* Bubble plugin.
*
*/


#include "ImageEditorBubblePlugin.h"
#include "ImageEditorBubbleControl.h"
#include "ImageEditorPluginBaseDefs.h"
#include "DrawUtils.h"
#include "SystemParameters.h"

//	CONSTANTS
_LIT (KPgnResourceFile, "Bubble.rsc");

//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorBubblePlugin * plugin = new (ELeave) CImageEditorBubblePlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorBubblePlugin::CImageEditorBubblePlugin ()
{

}

//=============================================================================
void CImageEditorBubblePlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorBubblePlugin::~CImageEditorBubblePlugin ()
{
    iSysPars = NULL;
    ReleasePlugin();
}

//=============================================================================
TInt CImageEditorBubblePlugin::SetProperty (
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
TInt CImageEditorBubblePlugin::GetProperty (
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
				aPropertyValue.Copy ( ((CImageEditorBubbleControl *)iControl)->GetParam() );
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
				readyToRender =  ((CImageEditorBubbleControl *)iControl)->IsReadyToRender();
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
TInt CImageEditorBubblePlugin::InitPluginL (
	const TRect &		aRect,
	CCoeControl *		aParent,
	CCoeControl *&		aPluginControl
	)
{
    //  Delete previous control
	ReleasePlugin();

    //  Create the control
    iControl = CImageEditorBubbleControl::NewL (aRect, aParent);
    aPluginControl = iControl;

    //  Set system parameters
    ((CImageEditorBubbleControl*)iControl)->SetSystemParameters (iSysPars);

    return KErrNone;
}

//=============================================================================
void CImageEditorBubblePlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{

}

//=============================================================================
void CImageEditorBubblePlugin::ReleasePlugin ()
{
	delete iControl;
	iControl = 0;
}


// End of File
