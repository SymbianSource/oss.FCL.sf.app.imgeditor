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
* Image editor RedEyeReduction plugin implementation.
*
*/



#include <aknquerydialog.h> 

#include "ImageEditorRedEyeReductionPlugin.h"
#include "ImageEditorRedEyeReductionControl.h"
#include "ImageEditorPluginBaseDefs.h"
#include "DrawUtils.h"

//	CONSTANTS
_LIT (KPgnResourceFile, "redeyereduction.rsc");


//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorRedEyeReductionPlugin * plugin = new (ELeave) CImageEditorRedEyeReductionPlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorRedEyeReductionPlugin::CImageEditorRedEyeReductionPlugin ()
{

}

//=============================================================================
void CImageEditorRedEyeReductionPlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorRedEyeReductionPlugin::~CImageEditorRedEyeReductionPlugin ()
{
    ReleasePlugin();
    iSysPars = NULL;
    iControl = NULL;
}

//=============================================================================
TInt CImageEditorRedEyeReductionPlugin::SetProperty (
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
TInt CImageEditorRedEyeReductionPlugin::GetProperty (
	TInt		aPropertyId, 
	TDes  &		aPropertyValue
	)
{
	//	Clean buffer
	aPropertyValue.Zero();

	//	Copy data
	switch (aPropertyId) 
	{
		case KCapParamStruct:
		{
			TRect rect = ((CImageEditorRedEyeReductionControl *)iControl)->GetParam();
			if (rect.iBr.iX <= 0 || rect.iBr.iY <= 0)
			{
				aPropertyValue.Copy (_L("nop"));
			}
			else
			{
				aPropertyValue.Copy (_L("ulc "));
				aPropertyValue.AppendNum (rect.iTl.iX);
				aPropertyValue.Append (_L(" ulr "));
				aPropertyValue.AppendNum (rect.iTl.iY);
				aPropertyValue.Append (_L(" lrc "));
				aPropertyValue.AppendNum (rect.iBr.iX);
				aPropertyValue.Append (_L(" lrr "));
				aPropertyValue.AppendNum (rect.iBr.iY);
			}
			return KErrNone;
		}
        case KCapReadyToRender:
		{
            TBool readyToRender = EFalse;
            if (iControl)
                {
                readyToRender = ((CImageEditorRedEyeReductionControl *)iControl)->IsReadyToRender();
                }
  
			aPropertyValue.AppendNum ((TInt)readyToRender);
			return KErrNone;
		}
		default:
		{
			return CImageEditorPluginBase::GetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorRedEyeReductionPlugin::InitPluginL (
	const TRect &		aRect,
	CCoeControl *		aParent,
	CCoeControl *&		aPluginControl
	)
{
    //  Delete previous control
	ReleasePlugin();

	// Create the control
    iControl = CImageEditorRedEyeReductionControl::NewL (aRect, aParent);
    aPluginControl = iControl;

    //  Set system parameters
    ((CImageEditorRedEyeReductionControl*)iControl)->SetSystemParameters (iSysPars);

    return KErrNone;
}

//=============================================================================
void CImageEditorRedEyeReductionPlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{

}

//=============================================================================
void CImageEditorRedEyeReductionPlugin::ReleasePlugin ()
{
	delete iControl;
	iControl = 0;
}

// End of File
