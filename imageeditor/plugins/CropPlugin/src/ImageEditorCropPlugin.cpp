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



#include <eikenv.h>
#include <aknenv.h>
#include <coneresloader.h>
#include <barsread.h>
#include <bautils.h>

#include <aknutils.h>

#include <crop.rsg>
#include "ImageEditorCropPlugin.h"
#include "ImageEditorCropControl.h"
#include "ImageEditorPluginBaseDefs.h"
#include "DrawUtils.h"
#include "ResolutionUtil.h"
#include "definitions.def"

//#include "platform_security_literals.hrh"


//	CONSTANTS
_LIT (KPgnResourceFile, "crop.rsc");

const TInt KManualCropQueryIndex = 3;


//=============================================================================
EXPORT_C CImageEditorPluginBase * CreateImageEditorPlugin ()
{
	CImageEditorCropPlugin * plugin = new (ELeave) CImageEditorCropPlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
    return plugin;
}

//=============================================================================
CImageEditorCropPlugin::CImageEditorCropPlugin ()
{

}

//=============================================================================
void CImageEditorCropPlugin::ConstructL ()
{
    CImageEditorPluginBase::ConstructL (KPgnResourcePath, KPgnResourceFile);
}

//=============================================================================
CImageEditorCropPlugin::~CImageEditorCropPlugin ()
{
    iSysPars = NULL;
    ReleasePlugin();
}

//=============================================================================
TInt CImageEditorCropPlugin::SetProperty (
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
TInt CImageEditorCropPlugin::GetProperty (
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
			//	Control found
			if (iControl)
			{
				aPropertyValue.Copy ( ((CImageEditorCropControl *)iControl)->GetParam() );
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
            if (iControl)
                {
                readyToRender = ((CImageEditorCropControl *)iControl)->IsReadyToRender();
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
TInt CImageEditorCropPlugin::InitPluginL (
	const TRect &		aRect,
	CCoeControl *		aParent,
	CCoeControl *&		aPluginControl
	)
{
    //  Delete previous control
	ReleasePlugin();

	//	Query dialog texts
	CDesCArray * queries = new (ELeave) CDesCArraySeg (8);
	CleanupStack::PushL (queries);

    //  Aspect ratio definition array
    RArray<TInt> aspect_ratios;

	//	Get pointer to the parameter descriptor array
    TBuf<256>  readbuf;  
	TLex       parser;
	User::LeaveIfError ( CImageEditorPluginBase::GetProperty (KCapPluginParamNames, readbuf) );
	parser.Assign (readbuf);
	TInt tempval = 0;
	parser.Val ( tempval );
	CDesCArray * pars = (CDesCArraySeg *)tempval;
    
    //  Append manual crop query
	queries->AppendL( (*pars)[KManualCropQueryIndex] );
   
    ReadWallPaperCropDefinitionsL(*queries, aspect_ratios);

#ifdef __SHOW_OPTIONAL_ASPECT_RATIOS__    
    ReadCropDefinitionsL (*queries, aspect_ratios);
#endif

    //  Create new control
    iControl = CImageEditorCropControl::NewL (aRect, aParent);

    //  Set system parameters
    ((CImageEditorCropControl*)iControl)->SetSystemParameters (iSysPars);

	//	Show selection dialog
    TPtrC query = (*pars)[0];
    TInt index = SDrawUtils::LaunchListQueryDialogL (queries, query);

    //  If canceled, return with NULL pointer and error value
    TInt ret = KErrNone;
    if (index <= -1)
    {
    	// Relasing created plugin if canceled
    	ReleasePlugin();
        ret = KErrNotSupported;
    }
    else
    {
        //  Compute and set crop mode and aspect ratio
        float ar = 1.0F;
        if (index != 0)
        {
            TInt index2 = (index - 1) * 2;
            ar = (float)aspect_ratios[index2] / (float)aspect_ratios[index2 + 1];
        }

        ((CImageEditorCropControl*)iControl)->SetCropModeL (index, ar);
    }

    if (queries)
    {
        for (TInt i = 0; i < queries->Count(); ++i)
        {
            queries->Delete(i);
        }
        queries->Reset();
    }
    aspect_ratios.Reset();
    CleanupStack::PopAndDestroy(); // queries
    aPluginControl = iControl;
    return ret;
}

//=============================================================================
void CImageEditorCropPlugin::ProcessImageL (CEditorImage * /*aImage*/ )
{

}

//=============================================================================
void CImageEditorCropPlugin::ReleasePlugin ()
{
	delete iControl;
	iControl = 0;
}

//=============================================================================
void CImageEditorCropPlugin::ReadWallPaperCropDefinitionsL (
    CDesCArray &  aCropTexts,
    RArray<TInt> &  aCropRatios
    ) const
{
    // find out screen resolution
    TRect screenRect;
    CResolutionUtil::Self()->GetScreenRect(screenRect);
    TInt screenWidth = screenRect.Width();
    TInt screenHeight = screenRect.Height();
     
    //	Read resource
	TFileName resourcefile;
	resourcefile.Append(KPgnResourcePath);
	resourcefile.Append(KPgnResourceFile);
    User::LeaveIfError( CompleteWithAppPath( resourcefile ) );

    //  Read aspect ratio definitions from resource file
    //  (RConeResourceLoader selects the language using BaflUtils::NearestLanguageFile)
    CEikonEnv *env = CEikonEnv::Static();
    RConeResourceLoader resLoader ( *env );
    CleanupClosePushL ( resLoader );
	resLoader.OpenL ( resourcefile );
    
    // To find out if in landscape or in portrait mode    
    TInt shortSize;
    TInt longSize;    
    if ( screenHeight > screenWidth )
        {
        shortSize = screenWidth;
        longSize = screenHeight;
        }
    else
        {
        longSize = screenWidth;
        shortSize = screenHeight;
        }
        
#ifndef __SHOW_TWO_WALLPAPER_ASPECT_RATIOS__

    // If only this one wallpaper option is shown crop ratio is set as portrait
    HBufC * wallpaperText = 
                    env->AllocReadResourceL( R_CROP_ASPECT_RATIO_WALLPAPER );
    CleanupStack::PushL( wallpaperText );
    aCropTexts.AppendL( wallpaperText->Des() );
    CleanupStack::PopAndDestroy();
    aCropRatios.Append( shortSize );
    aCropRatios.Append( longSize );

#else        
        
    HBufC * wallpaperText2 = 
                    env->AllocReadResourceL( R_CROP_ASPECT_RATIO_WALLPAPER_PRT );
    CleanupStack::PushL( wallpaperText2 );
    aCropTexts.AppendL( wallpaperText2->Des() );
    CleanupStack::PopAndDestroy();    
    aCropRatios.Append( shortSize );
    aCropRatios.Append( longSize );
    
    
    HBufC * wallpaperText3 = 
                    env->AllocReadResourceL( R_CROP_ASPECT_RATIO_WALLPAPER_LNDS );
    CleanupStack::PushL( wallpaperText3 );
    aCropTexts.AppendL( wallpaperText3->Des() );
    CleanupStack::PopAndDestroy();    
    aCropRatios.Append( longSize );
    aCropRatios.Append( shortSize );

#endif
   
    CleanupStack::PopAndDestroy(1); //resLoader
}


//=============================================================================
void CImageEditorCropPlugin::ReadCropDefinitionsL (
    CDesCArray &  aCropTexts,
    RArray<TInt> &  aCropRatios
    ) const
{

    //	Read resource
	TFileName resourcefile;
	resourcefile.Append(KPgnResourcePath);
	resourcefile.Append(KPgnResourceFile);
    User::LeaveIfError( CompleteWithAppPath( resourcefile ) );

    //  Read aspect ratio definitions from resource file
    //  (RConeResourceLoader selects the language using BaflUtils::NearestLanguageFile)
    CEikonEnv *env = CEikonEnv::Static();
    RConeResourceLoader resLoader ( *env );
    CleanupClosePushL ( resLoader );
	resLoader.OpenL ( resourcefile );

    HBufC8 * res8 = env->AllocReadResourceAsDes8LC (R_CROP_MODES);    
    TResourceReader resource;
    resource.SetBuffer (res8);
    
    TInt16 arraycount = (TInt16)resource.ReadInt16();
	if (arraycount > 0)
	{
		//	Read extra parameters to array
		for (TInt i = 0; i < arraycount; ++i)
		{
            //  Read the crop ratio text
            HBufC * crop_text = resource.ReadHBufCL();
            CleanupStack::PushL(crop_text);
            aCropTexts.AppendL ( crop_text->Des() );
            CleanupStack::PopAndDestroy();

            //  Add width
            aCropRatios.Append ( resource.ReadInt32() );
            
            //  Add height
            aCropRatios.Append ( resource.ReadInt32() );
		}
    }
    
    CleanupStack::PopAndDestroy(2); // res8, resLoader
}

// End of File
