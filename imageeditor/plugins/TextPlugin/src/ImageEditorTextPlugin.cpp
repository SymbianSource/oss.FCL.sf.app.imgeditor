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
* Text plugin class.
*
*/



#include <aknquerydialog.h> 

#include "ImageEditorTextPlugin.h"
#include "ImageEditorTextControl.h"
#include "ImageEditorPluginBaseDefs.h"
#include "DrawUtils.h"
#include "SystemParameters.h"

#ifdef TEXT_INSERTION_IN_PORTRAIT_ONLY
#include <aknappui.h>
#endif

//	CONSTANTS
_LIT (KPgnResourceFile, "text.rsc");


//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorTextPlugin * plugin = new (ELeave) CImageEditorTextPlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorTextPlugin::CImageEditorTextPlugin () : iLandscapeEnabled(EFalse)
{

}

//=============================================================================
void CImageEditorTextPlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorTextPlugin::~CImageEditorTextPlugin ()
{
    ReleasePlugin();
    iSysPars = NULL;
}

//=============================================================================
TInt CImageEditorTextPlugin::SetProperty (
	TInt		aPropertyId, 
	TDesC &		aPropertyValue
	)
{

    TInt err = KErrNone;
    TLex parser;

	//	Copy data
	switch (aPropertyId) 
	{
		case KCapSystemParameters:
		{
			parser.Assign (aPropertyValue);
            TInt tempval = 0;
            parser.Val (tempval);
            iSysPars = (const CSystemParameters *)tempval;
			err = KErrNone;
            break;
		}
		default:
		{
			err = CImageEditorPluginBase::SetProperty (aPropertyId, aPropertyValue);
            break;
		}
	}
    return err;
}

//=============================================================================
TInt CImageEditorTextPlugin::GetProperty (
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
				aPropertyValue.Copy ( ((CImageEditorTextControl *)iControl)->GetParam() );
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
            TBool readytorender = ETrue;
            if (iControl)
                {
                readytorender = ((CImageEditorTextControl *)iControl)->IsReadyToRender();
                }
			aPropertyValue.AppendNum ((TInt)readytorender);
			return KErrNone;
		}
        case KCapIsLandscapeEnabled:
		{
			aPropertyValue.AppendNum ((TInt)iLandscapeEnabled);
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
TInt CImageEditorTextPlugin::InitPluginL (
	const TRect &		aRect,
	CCoeControl *		aParent,
	CCoeControl *&		aPluginControl
	)
{
    //  Delete previous control
	ReleasePlugin();

    TInt ret = KErrNone;

	//	Get pointer to the parameter descriptor array
	TBuf<256>  readbuf;  
	TLex       parser;
	User::LeaveIfError ( CImageEditorPluginBase::GetProperty (KCapPluginParamNames, readbuf) );
	parser.Assign (readbuf);
	TInt tempval = 0;
	parser.Val ( tempval );
	CDesCArray * pars = (CDesCArray *)tempval;

	CEikonEnv* env = CEikonEnv::Static();

#ifdef TEXT_INSERTION_IN_PORTRAIT_ONLY
    // Text input is always insterted in portrait mode.
	// Store the original orientation before forcing to portrait.
    CAknAppUiBase* appUi = static_cast<CAknAppUiBase *>( env->EikAppUi() );
	CAknAppUiBase::TAppUiOrientation originalOrientation = appUi->Orientation();
    appUi->SetOrientationL(CAknAppUiBase::EAppUiOrientationPortrait);
#endif

    TRAPD(err,

    TBuf<256> text;
    TBuf<64> prompt;
    prompt.Copy ( (*pars)[0] );

    TInt ok = SDrawUtils::LaunchMultiLineTextQueryDialogL ( text, prompt );

    //  Create new control
    if (ok)
    {
        iControl = CImageEditorTextControl::NewL (aRect, aParent);
        aPluginControl = iControl;
        
        //  Set system parameters
        ((CImageEditorTextControl*)iControl)->SetSystemParameters (iSysPars);

        //  Set the text
        ((CImageEditorTextControl*)iControl)->SetTextL  ( text );

        iLandscapeEnabled = ETrue;
        
        ret = KErrNone;
    }
    else
    {
        aPluginControl = 0;
        ret = KErrNotSupported;
    }

    ) // TRAP


#ifdef TEXT_INSERTION_IN_PORTRAIT_ONLY
    // Set always back to original orientation, even if leaves
	CAknAppUiBase::TAppUiOrientation orientation = appUi->Orientation();

	if (orientation != originalOrientation)
		{
		appUi->SetOrientationL(originalOrientation);

		// Send screen device change event to validate screen
		TWsEvent event;

		RWsSession& rws = env->WsSession();
		event.SetType( EEventScreenDeviceChanged );
		event.SetTimeNow(); 
		event.SetHandle( rws.WsHandle() ); 

		User::LeaveIfError( rws.SendEventToAllWindowGroups(event) );
		}
#else
#ifdef LANDSCAPE_ONLY

	appUi->SetOrientationL(CAknAppUiBase::EAppUiOrientationLandscape);

	// Send screen device change event to validate screen
	TWsEvent event;

	RWsSession& rws = env->WsSession();
	event.SetType( EEventScreenDeviceChanged );
	event.SetTimeNow(); 
	event.SetHandle( rws.WsHandle() ); 

	User::LeaveIfError( rws.SendEventToAllWindowGroups(event) );
	
#endif
#endif


    if (err)
        {
        User::Leave(err);
        }

    return ret;
}

//=============================================================================
void CImageEditorTextPlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{

}

//=============================================================================
void CImageEditorTextPlugin::ReleasePlugin ()
{
	delete iControl;
	iControl = NULL;
}

// End of File
