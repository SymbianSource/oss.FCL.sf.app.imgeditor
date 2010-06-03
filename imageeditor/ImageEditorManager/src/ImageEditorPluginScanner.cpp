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



//	INCLUDES
#include <f32file.h> 
#include <eikenv.h> 
#include <bautils.h> 
#include <utf.h>
#include <eikmenup.h>
#include <aknutils.h>
#include <AknIconUtils.h>
#include <AknUtils.h>

#include "ImageEditorPluginScanner.h"
#include "ImageEditorPluginStorage.h"
#include "PluginLoader.h"
#include "PluginInfo.h"
#include "ImageEditorPluginBaseDefs.h"
#include "ImageEditorManagerDef.h"
#include "ImageEditorError.h"

//	PLUGIN TYPE DEFINITIONS
#include "plugintypedef.h"

// DEBUG LOGGER
#include "imageeditordebugutils.h"

//=============================================================================
CPluginScanner * CPluginScanner::NewL ()
{
    CPluginScanner * self = new (ELeave) CPluginScanner;
    CleanupStack::PushL (self);
    self->ConstructL ();
    CleanupStack::Pop();
    return self;
}

//=============================================================================
CPluginScanner::~CPluginScanner ()
{
    iStorage = NULL;
    LOG(KImageEditorLogFile, "CPluginScanner: ~CPluginScanner");
}

//=============================================================================
void CPluginScanner::ScanPluginsL ( TBool& aPluginStorageNeedsUpdate )
{
    // Check if the plugin storage internalized from disk is 
    // valid. If the plugins have changed, we need to perform 
    // full scan. Otherwise continue with the internalized content.
    if ( PluginStorageNeedsUpdateL() )
    {
        aPluginStorageNeedsUpdate = ETrue;
        DoScanPluginsL();
    }

    // Now the plugin infos are almost ready. Only icon bitmaps
    // are missing...
    LoadIconBitmapsL();
}

//=============================================================================
TBool CPluginScanner::PluginStorageNeedsUpdateL () const
{
    LOG(KImageEditorLogFile, "CPluginScanner: Checking for plugin storage validity");

    // First check if the plug-in storage is empty, which probably means that
    // there was no ini file to internalize (could also mean there are no plug-ins...)
    if ( ! iStorage->CountPlugins() )
    {
        LOG(KImageEditorLogFile, "CPluginScanner: No internalized plugin storage");
        return ETrue;
    }

    RFs & fs = CEikonEnv::Static()->FsSession();

    TInt matchCount = 0; // number of files from the plug-in storage matching with scanned files
    TInt scanCount  = 0; // number of files located from the directory scan

	// Find all plugins in installation directory
    TFindFile file_finder (fs); 
    CDir * file_list = 0; 
         
    TBuf<32> wildCard;
    wildCard.Append(KResourceWild);
    
    TInt err = file_finder.FindWildByDir (
		wildCard, 
		KPluginResourcePath, 
		file_list
		);

    // if .RSC file does not found, try to find language specific file
    if (err)
    {
        // Find out what is the current language
        TFileName resourceFile(KImageEditorResourceFile);
 		
 		CompleteWithAppPath(resourceFile);
 		
        BaflUtils::NearestLanguageFile(fs, resourceFile);

        TInt extPosition = resourceFile.LocateReverse(TChar('.'));

        wildCard.Zero();
        wildCard.Append('*');
        wildCard.Append(resourceFile.Mid(extPosition));

        err = file_finder.FindWildByDir (
    		wildCard, 
    		KPluginResourcePath, 
    		file_list
    		);        
    }

	//	Go through all drives
    while (err == KErrNone)
    {
        CleanupStack::PushL (file_list);
        scanCount += file_list->Count();

		//	Check all plug-in candidates
        for (TInt i = 0; i < file_list->Count(); ++i) 
        {

            /*
            *
            *	GET PLUG-IN FILE NAME
            *
            */

            //	Create a full file name for plugin
            TParse fullentry;
            TPtrC name = (*file_list)[i].iName;
            const TDesC* related = &(file_finder.File());
            fullentry.Set ( name, related, 0 );
            TFileName filename(fullentry.Name());            
            filename.Append(KPluginExtension);

            /*
            *
            *	SEE IF PLUG-IN FILE NAME IS IN  THE STORAGE
            *
            */

            TInt pos; // not used
            if ( 0 == iStorage->FindPluginDll(filename, pos) )
            {
                matchCount++;
            }

        }

        CleanupStack::PopAndDestroy(); /// file_list

		//	Try once again
        err = file_finder.FindWild (file_list); 
    }

    // All plug-in files have been scanned. See the result.
    if( scanCount == matchCount && matchCount == iStorage->CountPlugins() )
    {
        LOG(KImageEditorLogFile, "CPluginScanner: plug-in storage up to date");
        return ETrue; // no update needed
    }
    else
    {
        LOG(KImageEditorLogFile, "CPluginScanner: plug-in storage update needed");
        return ETrue;
    }
}

//=============================================================================
void CPluginScanner::DoScanPluginsL ()
{
    LOG(KImageEditorLogFile, "CPluginScanner: Scanning plugins");

    // iStorage may contain invalidated internalized 
    // data, so it needs to be emptied first.
    iStorage->ResetAndDestroy();

	// Create a file session
    RFs & fs = CEikonEnv::Static()->FsSession();

	// Find plugin resource files private directory. It is not allowed 
	// to find plug-in DLLs from /sys/bin/ directory, because of 
	// Platform Security
    TFindFile file_finder (fs); 
    CDir * file_list = 0; 
    
    TBuf<32> wildCard;
    wildCard.Append(KResourceWild);
    
    TInt err = file_finder.FindWildByDir (
		wildCard, 
		KPluginResourcePath, 
		file_list
		);

    // if .RSC file does not found, try to find language specific file
    if (err)
    {
        // Find out what is the current language
        TFileName resourceFile(KImageEditorResourceFile);
 		
 		CompleteWithAppPath(resourceFile);
 		
        BaflUtils::NearestLanguageFile(fs, resourceFile);

        TInt extPosition = resourceFile.LocateReverse(TChar('.'));

        wildCard.Zero();
        wildCard.Append('*');
        wildCard.Append(resourceFile.Mid(extPosition));

        err = file_finder.FindWildByDir (
    		wildCard, 
    		KPluginResourcePath, 
    		file_list
    		);        
    }
    
	//	Go through all drives
    while (err == KErrNone)
    {
        CleanupStack::PushL (file_list);

		//	Check all plug-in candidates
        for (TInt i = 0; i < file_list->Count(); ++i) 
        {

            /*
            *
            *	LOAD PLUG-IN
            *
            */

            //	Create a full file name for plugin
            TParse fullentry;
            fullentry.Set (
				(*file_list)[i].iName,&
                file_finder.File(), 
				0
				); 
				
			// Use resource filename to compose DLL filename
            TFileName name(fullentry.Name());            
            name.Append(KPluginExtension);
            
            LOGFMT(KImageEditorLogFile,"CPluginScanner: Starting to load plug-in file: %S", &name);

            //	Load plug-in and perform the DLL checks
			CPluginLoader * pluginloader = 
				CPluginLoader::NewLC (name, &TUid2, 0);


			//	Get pointer to the plugin
			CPluginType * plugin = pluginloader->GetPlugin();
				
			//	Query information and construct CPluginInfo instance
            CPluginInfo * plugininfo = CPluginInfo::NewLC ();

            /*
            *
            *	STORE ALL PLUG-IN INFORMATION
            *
            */

			//	Read buffer and parser
			TBuf<256>  	readbuf;  
			TBuf<256> 	ureadbuf;  
			TLex       	parser;

            //	PLUG-IN DLL NAME
		    ureadbuf.Copy (name);
            plugininfo->PluginDll() = ( ureadbuf.AllocL() );

            //	PLUG-IN UID2
			plugininfo->Uid2() = pluginloader->GetPluginDll().Type()[1];

            //	PLUG-IN UID3
			plugininfo->Uid3() = pluginloader->GetPluginDll().Type()[2];

            //	PLUG-IN UI TYPE
			User::LeaveIfError ( plugin->GetProperty (KCapPluginUiType, readbuf) );
			parser.Assign (readbuf);
			parser.Val ( plugininfo->PluginUiType() );

            //	PLUG-IN FILTER TYPE
			User::LeaveIfError ( plugin->GetProperty (KCapPluginFilterType, readbuf) );
			parser.Assign (readbuf);
			parser.Val ( plugininfo->PluginFilterType() );

            //	PLUG-IN FILTER SCOPE
			User::LeaveIfError ( plugin->GetProperty (KCapPluginScope, readbuf) );
			parser.Assign (readbuf);
			parser.Val ( plugininfo->PluginScope() );

            //  PLUG-IN DISPLAY ORDER VALUE
			User::LeaveIfError ( plugin->GetProperty (KCapPluginDisplayOrder, readbuf) );
			parser.Assign (readbuf);
			parser.Val ( plugininfo->PluginDisplayOrder() );

            //  PLUG-IN RUN-TIME ID (RID)
            plugininfo->PluginRID() = iLastRID++;

			//	PLUG-IN NAME
			User::LeaveIfError ( plugin->GetProperty (KCapPluginName, readbuf) );
            ureadbuf.Copy( readbuf );
            plugininfo->PluginName() = ureadbuf.AllocL();

			//	PLUG-IN ICON FILE NAME
			User::LeaveIfError ( plugin->GetProperty (KCapIconName, readbuf) );
            ureadbuf.Copy( readbuf );
			plugininfo->IconFile() = ureadbuf.AllocL();

            //	PARAMETER DESCRIPTOR ARRAY
			User::LeaveIfError ( plugin->GetProperty (KCapPluginParamNames, readbuf) );
			parser.Assign (readbuf);
            TInt tempval = 0;
			parser.Val ( tempval );
            CDesCArray * parameters = (CDesCArray *)tempval;
            if (parameters)
			{
				for (TInt j = 0; j < parameters->Count(); ++j)
				{
					plugininfo->Parameters().AppendL( (*parameters)[j] );
				}
			}

			//	SOFT KEY 1 COMMAND IDS
			User::LeaveIfError ( plugin->GetProperty (KCapPluginSk1Cmd, readbuf) );
			parser.Assign (readbuf);
            tempval = 0;
			parser.Val ( tempval );
            CArrayFix<TInt> * cmdids = 0;
            cmdids = (CArrayFix<TInt> *)tempval;
            if (cmdids)
			{
				for (TInt j = 0; j < cmdids->Count(); ++j)
				{
					plugininfo->Sk1Cmds().AppendL( (*cmdids)[j] );
				}
			}

			//	SOFT KEY 1 TEXTS
			User::LeaveIfError ( plugin->GetProperty (KCapPluginSk1Text, readbuf) );
			parser.Assign (readbuf);
            tempval = 0;
			parser.Val ( tempval );
            CDesCArray * texts = 0;
            texts = (CDesCArray *)tempval;
            if (texts)
			{
				for (TInt j = 0; j < texts->Count(); ++j)
				{
					plugininfo->Sk1Texts().AppendL( (*texts)[j] );
				}
			}

			//	SOFT KEY 2 COMMAND IDS
			User::LeaveIfError ( plugin->GetProperty (KCapPluginSk2Cmd, readbuf) );
			parser.Assign (readbuf);
            tempval = 0;
			parser.Val ( tempval );
            cmdids = 0;
            cmdids = (CArrayFix<TInt> *)tempval;
            if (cmdids)
			{
				for (TInt j = 0; j < cmdids->Count(); ++j)
				{
					plugininfo->Sk2Cmds().AppendL( (*cmdids)[j] );
				}
			}

			//	SOFT KEY 2 TEXTS
			User::LeaveIfError ( plugin->GetProperty (KCapPluginSk2Text, readbuf) );
			parser.Assign (readbuf);
            tempval = 0;
			parser.Val ( tempval );
            texts = 0;
            texts = (CDesCArray *)tempval;
            if (texts)
			{
				for (TInt j = 0; j < texts->Count(); ++j)
				{
					plugininfo->Sk2Texts().AppendL( (*texts)[j] );
				}
			}
            
            //	MSK COMMAND IDS
			User::LeaveIfError ( plugin->GetProperty (KCapPluginMSKCmd, readbuf) );
			parser.Assign (readbuf);
            tempval = 0;
			parser.Val ( tempval );
            cmdids = 0;
            cmdids = (CArrayFix<TInt> *)tempval;
            if (cmdids)
			{
				for (TInt j = 0; j < cmdids->Count(); ++j)
				{
					plugininfo->MSKCmds().AppendL( (*cmdids)[j] );
				}
			}

			//	MSK TEXTS
			User::LeaveIfError ( plugin->GetProperty (KCapPluginMSKText, readbuf) );
			parser.Assign (readbuf);
            tempval = 0;
			parser.Val ( tempval );
            texts = 0;
            texts = (CDesCArray *)tempval;
            if (texts)
			{
				for (TInt j = 0; j < texts->Count(); ++j)
				{
					plugininfo->MSKTexts().AppendL( (*texts)[j] );
				}
			}
            
			//	MENU ITEMS
			User::LeaveIfError ( plugin->GetProperty (KCapPluginMenuItems, readbuf) );
			parser.Assign (readbuf);
            tempval = 0;
			parser.Val ( tempval );
            CMenuItemArray * menuitems = (CMenuItemArray *)tempval;
            if (menuitems)
            {
                for (TInt j = 0; j < menuitems->Count(); ++j)
			    {
				    plugininfo->MenuItems().AppendL ( (*menuitems)[j] );
			    }
            }

			//	Add plugininfo to storage
			iStorage->AddPluginInfoL (plugininfo);

			CleanupStack::Pop();                // plugininfo
			CleanupStack::PopAndDestroy ();     // pluginloader
        }	
    
		CleanupStack::PopAndDestroy(); /// file_list

		//	Try once again
        err = file_finder.FindWild (file_list); 
    }
}

//=============================================================================
void CPluginScanner::LoadIconBitmapsL()
{
    ASSERT( iStorage );

    for ( TInt i = 0; i < iStorage->CountPlugins(); i++)
    {
        CPluginInfo* plugininfo = iStorage->GetPluginInfo(i);

		TFileName iconfile;
        iconfile.Copy ( KPluginBitmapPath );
        iconfile.Append ( plugininfo->IconFile()->Des() );

        CompleteWithAppPath( iconfile );
        if ( !BaflUtils::FileExists (CEikonEnv::Static()->FsSession(), iconfile) )
        {
            LOGFMT(KImageEditorLogFile,"CPluginScanner: File not found: %S", &iconfile);
            User::Leave (KSIEEInternal);
        }

        LOGFMT(KImageEditorLogFile,"CPluginScanner: Plug-in icon file: %S", &iconfile);

        // Check whether icon file is SVG graphic
        if (AknIconUtils::IsMifFile(iconfile))
        {
            TInt bitmapId = 0;
            TInt maskId = 0;
            // Get ids for bitmap and mask
            // NOTE: ValidateLogicalAppIconId returns same id for both!
            AknIconUtils::ValidateLogicalAppIconId(iconfile, bitmapId, maskId);
            AknIconUtils::CreateIconL(
                plugininfo->Icon(),
                plugininfo->Mask(),
                iconfile,
                bitmapId,
                maskId+1);  // TEMPORARY FIX

            //plugininfo->Icon()->Save(_L("icon.mbm"));
            //plugininfo->Mask()->Save(_L("mask.mbm"));
        }
        else
        {
            //	PLUG-IN ICON BITMAP
            CFbsBitmap * icon = new (ELeave) CFbsBitmap;
            CleanupStack::PushL (icon);
            User::LeaveIfError ( icon->Load (iconfile, 0) );
            plugininfo->Icon() = icon;
            CleanupStack::Pop(); // icon

            //	PLUG-IN MASK BITMAP
            CFbsBitmap * mask = new (ELeave) CFbsBitmap;
            CleanupStack::PushL (mask);
            User::LeaveIfError ( mask->Load (iconfile, 1) );
            plugininfo->Mask() = mask;
            CleanupStack::Pop(); // mask
        }
    }
}

//=============================================================================
CPluginScanner::CPluginScanner()
{

}
	
//=============================================================================
void CPluginScanner::ConstructL ()
{
    
}

// End of File
