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



#include <e32std.h>
#include <eikenv.h>

#include "PluginLoader.h"
#include "ImageEditorError.h"

#include "../../ImageEditor/inc/plugintypedef.h"

//=============================================================================
CPluginLoader * CPluginLoader::NewL (
	const TDesC	&	aFileName,
	const TUid *	aUID2,
	const TUid *	aUID3
	)
{
	CPluginLoader * self = CPluginLoader::NewLC (aFileName, aUID2, aUID3);
	CleanupStack::Pop (); // self
	return self;
}

//=============================================================================
CPluginLoader * CPluginLoader::NewLC (
	const TDesC	&	aFileName,
	const TUid *	aUID2,
	const TUid *	aUID3
	)
{
	CPluginLoader * self = new (ELeave) CPluginLoader;
	CleanupStack::PushL (self);
	self->ConstructL (aFileName, aUID2, aUID3);
	return self;
}

//=============================================================================
CPluginLoader::~CPluginLoader ()
{
	delete iPlugin;
	iLibrary.Close();
}

//=============================================================================
CPluginLoader::CPluginLoader ()
{

}

//=============================================================================
void CPluginLoader::ConstructL (
	const TDesC	&	aFileName,
	const TUid *	aUID2,
	const TUid *	aUID3
	)
{
    LOGFMT(KImageEditorLogFile, "CPluginLoader: Loading plugin %S", &aFileName);
    
    //	Dynamically load DLL
	TInt err = iLibrary.Load (aFileName);
    if (err != KErrNone)
        {
        LOGFMT(KImageEditorLogFile,"CPluginLoader: Failed to load file: %S", &aFileName);
        User::Leave (KSIEEInternal);
        }

	//	Check UID2
	if (aUID2)
	{
		if ( iLibrary.Type()[1] != *aUID2 )
		{
            LOG(KImageEditorLogFile, "CPluginLoader: Invalid plugin UID");
			User::Leave (KSIEEInternal);
		}
		
	}

    //	Check UID3
	if (aUID3)
	{
        if ( iLibrary.Type()[2] != *aUID3 )
		{
            LOG(KImageEditorLogFile, "CPluginLoader: Invalid plugin UID");
			User::Leave (KSIEEInternal);
		}
	}

    //	Create a plugin by calling ordinal 1 factory method
    TLibraryFunction entrypoint = iLibrary.Lookup (1);
    iPlugin = (CPluginType *) entrypoint();

}

//=============================================================================
CPluginType * CPluginLoader::GetPlugin () const
{
	return iPlugin;
}

//=============================================================================
const RLibrary & CPluginLoader::GetPluginDll () const
{
	return iLibrary;
}


// End of File
