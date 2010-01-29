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

#include "ImageEditorPluginStorage.h"
#include "PluginInfo.h"

//=============================================================================
CPluginStorage * CPluginStorage::NewL ()
{
	CPluginStorage * self = new (ELeave) CPluginStorage;
	CleanupStack::PushL (self);
	self->ConstructL ();
	CleanupStack::Pop (); // self
	return self;
}

//=============================================================================
CPluginStorage::~CPluginStorage ()
{
    iPlugins.ResetAndDestroy();
}

//=============================================================================
TInt CPluginStorage::CountPlugins() const
{
    return iPlugins.Count();
}

//=============================================================================
void CPluginStorage::ResetAndDestroy ()
{
    iPlugins.ResetAndDestroy();
}

//=============================================================================
TInt CPluginStorage::FindPluginDll(const TDesC& aPluginDll, TInt& aPos ) const
{
    for (TInt i = 0, c = iPlugins.Count(); i < c; ++i)
    {
        aPos = i;
        TPtrC plugin = iPlugins[i]->PluginDll()->Des();
        if ( plugin.CompareF(aPluginDll) == 0 )
        {
			return 0;
        }
    }
    return KErrNotFound;
}

//=============================================================================
CPluginInfo * CPluginStorage::GetPluginInfo (const TInt	aID)
{
    for (TInt i = 0, c = iPlugins.Count(); i < c; ++i)
    {
        if (iPlugins[i]->PluginRID() == aID)
        {
			return iPlugins[i];
        }
    }
	return 0;
}

//=============================================================================
CPluginStorage::CPluginStorage ()
{

}

//=============================================================================
void CPluginStorage::ConstructL ()
{
    LOG(KImageEditorLogFile,"CPluginStorage: Storage created");
}

//=============================================================================
void CPluginStorage::AddPluginInfoL (const CPluginInfo * aPluginInfo)
{
    //  Add plug-in to storage
    TLinearOrder<CPluginInfo> order (CPluginInfo::ComparePluginOrder);
    iPlugins.InsertInOrderAllowRepeats (aPluginInfo, order);
}

//=============================================================================
void CPluginStorage::InternalizeL ( RReadStream& aStream )
{
    LOG(KImageEditorLogFile,"CPluginStorage::InternalizeL");

    TRAPD ( err, 

        // Internalize the contents of iPlugins
        TInt count = aStream.ReadInt32L();
        for ( TInt i=0; i<count; i++ )
        {
            CPluginInfo* info = CPluginInfo::NewLC();
            aStream >> *info;
            User::LeaveIfError( iPlugins.Append( info ) );
            CleanupStack::Pop( info );
        } 
    );

    if (err)
    {
        LOGFMT(KImageEditorLogFile, "CPluginLocator: Internalize failed: %d", err);
        iPlugins.ResetAndDestroy();
    }
}

//=============================================================================
void CPluginStorage::ExternalizeL ( RWriteStream& aStream ) const
{
    LOG(KImageEditorLogFile,"CPluginStorage::ExternalizeL");

    TRAPD ( err, 

        // Externalize the contents of iPlugins
        aStream.WriteInt32L( iPlugins.Count() );
        for ( TInt i=0; i< iPlugins.Count(); i++ )
        {
            CPluginInfo* info = iPlugins[i];
            aStream << *info;
        }
    );

    if (err)
    {
        LOGFMT(KImageEditorLogFile, "CPluginLocator: Externalize failed: %d", err);
    }
}

// End of File
