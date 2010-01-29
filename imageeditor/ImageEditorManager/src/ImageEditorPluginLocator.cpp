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



// INCLUDES
#include <s32file.h>
#include <bautils.h>
#include <eikenv.h>

#include "ImageEditorPluginLocator.h"
#include "ImageEditorPluginScanner.h"
#include "ImageEditorPluginStorage.h"
#include "PluginInfo.h"
#include "ImageEditorUIDs.hrh"

// CONSTANTS
_LIT( KPluginStorageExternalizeFile, "c:\\private\\101FFA91\\PluginStorage.ini");


//=============================================================================
EXPORT_C CPluginLocator * CPluginLocator::NewL ()
{
	CPluginLocator * self = new (ELeave) CPluginLocator;
	CleanupStack::PushL (self);
	self->ConstructL ();
	CleanupStack::Pop (); // self
	return self;
}

//=============================================================================
EXPORT_C CPluginLocator::~CPluginLocator ()
{
	delete iScanner;
	delete iStorage;
}

//=============================================================================
EXPORT_C void CPluginLocator::ScanPluginsL ()
{
    LOG(KImageEditorLogFile, "CPluginLocator: Scanning plugins");

    ASSERT( iScanner );
    ASSERT( iStorage );

    //	Internalize the plug-in storage
    RFs& fs = CEikonEnv::Static()->FsSession();
    BaflUtils::EnsurePathExistsL( fs, KPluginStorageExternalizeFile() );
    if( BaflUtils::FileExists(fs, KPluginStorageExternalizeFile()) )
    {
        RFileReadStream stream;
        stream.PushL();

        LOGFMT(KImageEditorLogFile, "CPluginLocator: Internalizing plug-in storage from %S", &KPluginStorageExternalizeFile());

        User::LeaveIfError (
            stream.Open (
            fs,
            KPluginStorageExternalizeFile(),
            EFileRead | EFileShareReadersOnly
            ));

        stream >> *iStorage;

        stream.Release();
        stream.Pop();
    }
    
    // Scan for plug-ins
    TBool pluginStorageNeedsUpdate = EFalse;
	iScanner->ScanPluginsL ( pluginStorageNeedsUpdate );

    // If the plugin content has changed compared to the previously
    // internalized one, we need to externalize the plug-in storage
    if( pluginStorageNeedsUpdate )
    {
        LOGFMT(KImageEditorLogFile, "CPluginLocator: Externalizing plug-in storage to %S", &KPluginStorageExternalizeFile());

        RFileWriteStream stream;
        stream.PushL();

        User::LeaveIfError (
            stream.Replace (
            fs,
            KPluginStorageExternalizeFile(),
            EFileWrite
            ));

        stream << *iStorage;

        stream.Close();
        stream.Pop();
    }

    // iScanner is not needed after this
    delete iScanner;
    iScanner = NULL;
}

//=============================================================================
EXPORT_C TInt CPluginLocator::CountPlugins() const
{
	return iStorage->CountPlugins();
}

//=============================================================================
EXPORT_C CPluginInfo * CPluginLocator::GetPluginInfo (const TInt aID)
{
    return iStorage->GetPluginInfo (aID);
}

//=============================================================================
CPluginLocator::CPluginLocator ()
{

}

//=============================================================================
void CPluginLocator::ConstructL ()
{
    LOG(KImageEditorLogFile, "CPluginLocator: Creating locator");

	//	Create a new plug-in storage
	iStorage = CPluginStorage::NewL ();

	//	Create a new plug-in locator
	iScanner = CPluginScanner::NewL ();

	//	Set storage
	iScanner->SetPluginStorage (iStorage);
}

// End of File
