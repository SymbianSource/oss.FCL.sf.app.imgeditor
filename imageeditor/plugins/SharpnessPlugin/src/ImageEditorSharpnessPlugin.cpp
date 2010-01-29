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
* Sharpness plugin plugin base class.
*
*/


/// INCLUDES
#include <sharpness.mbg>
#include "ImageEditorSharpnessPlugin.h"
#include "SingleParamControl.h"

/// CONSTANTS
_LIT (KPgnResourceFile, "sharpness.rsc");

const long KParamMin   = -100;
const long KParamMax   = 100;
const long KParamStep  = 10;
const long KParamDef   = 0;
const TInt KSharpnessHSTitleIndex = 0;

_LIT (KSharpnessTag, "sharpness ");
_LIT (KResourceDir, "\\resource\\apps\\");

//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorSharpnessPlugin * plugin = new (ELeave) CImageEditorSharpnessPlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorSharpnessPlugin::CImageEditorSharpnessPlugin () :
CImageEditorPluginBase()
{

}

//=============================================================================
void CImageEditorSharpnessPlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorSharpnessPlugin::~CImageEditorSharpnessPlugin ()
{
    ReleasePlugin();
    iControl = NULL;
}

//=============================================================================
TInt CImageEditorSharpnessPlugin::SetProperty (
	TInt		/*aPropertyId*/,
	TDesC &		/*aPropertyValue*/
	)
{
	return KErrNotSupported;
}

//=============================================================================
TInt CImageEditorSharpnessPlugin::GetProperty (
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
        	aPropertyValue.Copy (KSharpnessTag);
        	aPropertyValue.AppendNum (iSharpness);
			return KErrNone;
		}
		default:
		{
			return CImageEditorPluginBase::GetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorSharpnessPlugin::InitPluginL (
	const TRect &		aRect,
	CCoeControl *		aParent,
	CCoeControl *&		aPluginControl
	)
{
	ReleasePlugin();
	iControl = CSingleParamControl::NewL (aRect, aParent, ETrue);
    ((CSingleParamControl *)iControl)->SetParObserver ( (MSingleParControlObserver*)this );
	aPluginControl = iControl;

	// initialize vertical slider
	((CSingleParamControl *)iControl)->SetSliderMinimumAndMaximum(KParamMin, KParamMax);
    ((CSingleParamControl *)iControl)->SetSliderStep(KParamStep);
	((CSingleParamControl *)iControl)->SetSliderPosition(KParamDef);
    TFileName iconFile (KResourceDir);
	TBuf<256> readbuf;  
	User::LeaveIfError ( GetProperty (KCapIconName, readbuf) );
	iconFile.Append(readbuf);
    CEikImage* icon = new (ELeave) CEikImage;
	icon->CreatePictureFromFileL(iconFile,
									EMbmSharpnessQgn_indi_imed_sharpness_super,
									EMbmSharpnessQgn_indi_imed_sharpness_super_mask);
	((CSingleParamControl *)iControl)->SetIcon(icon);
	
	// Get caption from plugin properties
	User::LeaveIfError ( GetProperty (KCapPluginParamNames, readbuf) );
	TLex parser;
	parser.Assign (readbuf);
    TInt tempval = 0;
	parser.Val ( tempval );
    CDesCArray * parameters = (CDesCArray *)tempval;
    ((CSingleParamControl *)iControl)->SetCaption( (*parameters)[KSharpnessHSTitleIndex] );


	return KErrNone;
}

//=============================================================================
void CImageEditorSharpnessPlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{
	// Own image processing functionality here
}

//=============================================================================
void CImageEditorSharpnessPlugin::ReleasePlugin ()
{
	delete iControl;
	iControl = 0;
}

//=============================================================================
void CImageEditorSharpnessPlugin::ParamOperation (const TParamOperation aOperation)
{
    switch (aOperation)
    {
        case EParamOperationSubtract:
        {
            iSharpness += KParamStep;
            if (iSharpness < KParamMin)
            {
                iSharpness = KParamMin;
            }
    	    break;
        }
        case EParamOperationAdd:
        {
            iSharpness -= KParamStep;
            if (iSharpness > KParamMax)
            {
                iSharpness = KParamMax;
            }
    	    break;
        }
        case EParamOperationDefault:
        {
	        iSharpness = KParamDef;
    	    break;
        }
        default:
        {
    	    break;
        }
    }
}

//=============================================================================
TReal CImageEditorSharpnessPlugin::GetParam () const
{
    return (TReal)(iSharpness) / (KParamMax - KParamMin);
}

// End of File

