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
* Brightness plugin plugin base class.
*
*/


/// INCLUDES
#include "ImageEditorBrightnessPlugin.h"
#include "SingleParamControl.h"
#include <brightness.mbg>

/// CONSTANTS
_LIT (KPgnResourceFile, "brightness.rsc");

const long KParamMin   = -100;
const long KParamMax   = 100;
const long KParamStep  = 10;
const long KParamDef   = 0;
const TInt KBrightnessHSTitleIndex = 0;

//	brightness tag
_LIT (KBrightnessTag, "brightness ");
_LIT (KResourceDir, "\\resource\\apps\\");

//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorBrightnessPlugin * plugin = new (ELeave) CImageEditorBrightnessPlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorBrightnessPlugin::CImageEditorBrightnessPlugin () :
CImageEditorPluginBase()
{

}

//=============================================================================
void CImageEditorBrightnessPlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorBrightnessPlugin::~CImageEditorBrightnessPlugin ()
{
    ReleasePlugin();
}

//=============================================================================
TInt CImageEditorBrightnessPlugin::SetProperty (
	TInt		/*aPropertyId*/,
	TDesC &		/*aPropertyValue*/
	)
{
	return KErrNotSupported;
}

//=============================================================================
TInt CImageEditorBrightnessPlugin::GetProperty (
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
        	aPropertyValue.Copy (KBrightnessTag);
        	aPropertyValue.AppendNum (iBrightness);
			return KErrNone;
		}
		default:
		{
			return CImageEditorPluginBase::GetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorBrightnessPlugin::InitPluginL (
	const TRect &		aRect,
	CCoeControl *		aParent,
	CCoeControl *&		aPluginControl
	)
{
	ReleasePlugin();
	iControl = CSingleParamControl::NewL (aRect, aParent);
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
									EMbmBrightnessQgn_indi_imed_bright_super,
									EMbmBrightnessQgn_indi_imed_bright_super_mask);
	((CSingleParamControl *)iControl)->SetIcon(icon);	
	
	// Get caption from plugin properties
	User::LeaveIfError ( GetProperty (KCapPluginParamNames, readbuf) );
	TLex parser;
	parser.Assign (readbuf);
    TInt tempval = 0;
	parser.Val ( tempval );
    CDesCArray * parameters = (CDesCArray *)tempval;
    ((CSingleParamControl *)iControl)->SetCaption( (*parameters)[KBrightnessHSTitleIndex] );
    
	return KErrNone;
}

//=============================================================================
void CImageEditorBrightnessPlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{
	// Own image processing functionality here
}

//=============================================================================
void CImageEditorBrightnessPlugin::ReleasePlugin ()
{
    delete iControl;
    iControl = 0;
}

//=============================================================================
void CImageEditorBrightnessPlugin::ParamOperation (const TParamOperation aOperation)
{
    switch (aOperation)
    {
        case EParamOperationSubtract:
        {
        	
            iBrightness -= KParamStep;
            if ( iBrightness < KParamMin )
            {
                iBrightness = KParamMin;
            }
    	    break;
        }
        case EParamOperationAdd:
        {
            iBrightness += KParamStep;
            if ( iBrightness > KParamMax )
            {
                iBrightness = KParamMax;
            }
    	    break;
        }
        case EParamOperationDefault:
        {
        	iBrightness = KParamDef;
    	    break;
        }
        default:
        {
    	    break;
        }
    }
}

//=============================================================================
TReal CImageEditorBrightnessPlugin::GetParam () const
{
    return (TReal)( (TReal)iBrightness / (KParamMax - KParamMin) );
}

// End of File
