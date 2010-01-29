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
* Contrast plugin plugin base class.
*
*/


/// INCLUDES
#include <eikenv.h>
#include <contrast.mbg>
#include "ImageEditorContrastPlugin.h"
#include "SingleParamControl.h"

/// CONSTANTS
_LIT (KPgnResourceFile, "contrast.rsc");

const long KParamMin   = -100;
const long KParamMax   = 100;
const long KParamStep  = 10;
const long KParamDef   = 0;
const TInt KContrastHSTitleIndex = 0;

_LIT (KContrastTag, "contrast ");
_LIT (KResourceDir, "\\resource\\apps\\");

//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorContrastPlugin * plugin = new (ELeave) CImageEditorContrastPlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorContrastPlugin::CImageEditorContrastPlugin () :
CImageEditorPluginBase()
{

}

//=============================================================================
void CImageEditorContrastPlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorContrastPlugin::~CImageEditorContrastPlugin ()
{
    ReleasePlugin();
}

//=============================================================================
TInt CImageEditorContrastPlugin::SetProperty (
	TInt		/*aPropertyId*/,
	TDesC &		/*aPropertyValue*/
	)
{
	return KErrNotSupported;
}

//=============================================================================
TInt CImageEditorContrastPlugin::GetProperty (
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
        	aPropertyValue.Copy (KContrastTag);
        	aPropertyValue.AppendNum (iContrast);
			return KErrNone;
		}
		default:
		{
			return CImageEditorPluginBase::GetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
TInt CImageEditorContrastPlugin::InitPluginL (
	const TRect &	aRect,
	CCoeControl *	aParent,
	CCoeControl *&	aPluginControl
	)
{
	ReleasePlugin();
	iControl = CSingleParamControl::NewL (aRect, aParent);
    ((CSingleParamControl *)iControl)->SetParObserver ( (MSingleParControlObserver*)this );
	aPluginControl = iControl;

	// initialize horizontal slider
	((CSingleParamControl *)iControl)->SetSliderMinimumAndMaximum(KParamMin, KParamMax);
    ((CSingleParamControl *)iControl)->SetSliderStep(KParamStep);
	((CSingleParamControl *)iControl)->SetSliderPosition(KParamDef);
    TFileName iconFile (KResourceDir);
	TBuf<256> readbuf;  
	User::LeaveIfError ( GetProperty (KCapIconName, readbuf) );
	iconFile.Append(readbuf);
    CEikImage* icon = new (ELeave) CEikImage;
	icon->CreatePictureFromFileL(iconFile,
									EMbmContrastQgn_indi_imed_contrast_super,
									EMbmContrastQgn_indi_imed_contrast_super_mask);
	((CSingleParamControl *)iControl)->SetIcon(icon);

	// Get caption from plugin properties
	User::LeaveIfError ( GetProperty (KCapPluginParamNames, readbuf) );
	TLex parser;
	parser.Assign (readbuf);
    TInt tempval = 0;
	parser.Val ( tempval );
    CDesCArray * parameters = (CDesCArray *)tempval;
    ((CSingleParamControl *)iControl)->SetCaption( (*parameters)[KContrastHSTitleIndex] );

	return KErrNone;
}

//=============================================================================
void CImageEditorContrastPlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{
	// Own image processing functionality here
}

//=============================================================================
void CImageEditorContrastPlugin::ReleasePlugin ()
{
		delete iControl;
		iControl = 0;
}

//=============================================================================
void CImageEditorContrastPlugin::ParamOperation (const TParamOperation aOperation)
{
    switch (aOperation)
    {
        case EParamOperationSubtract:
        {
            iContrast -= KParamStep;
            if (iContrast < KParamMin)
            {
                iContrast = KParamMin;
            }
    	    break;
        }
        case EParamOperationAdd:
        {
            iContrast += KParamStep;
            if ( iContrast > KParamMax )
            {
                iContrast = KParamMax;
            }
    	    break;
        }
        case EParamOperationDefault:
        {
	        iContrast = KParamDef;
    	    break;
        }
        default:
        {
    	    break;
        }
    }
}

//=============================================================================
TReal CImageEditorContrastPlugin::GetParam () const
{
    return (TReal)(iContrast) / (KParamMax - KParamMin);
}

// End of File
