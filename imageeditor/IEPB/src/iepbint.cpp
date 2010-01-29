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
#include "iepbint.h"
#include "iepbres.h"

#include <aknutils.h>

//=============================================================================
CImageEditorPluginBaseInt * CImageEditorPluginBaseInt::NewL (
    const TDesC & aResourcePath,
    const TDesC & aResourceFile
    )
{
	CImageEditorPluginBaseInt * self = new (ELeave) CImageEditorPluginBaseInt;
	CleanupStack::PushL (self);
	self->ConstructL (aResourcePath, aResourceFile);
	CleanupStack::Pop(); // self
    return self;
}

//=============================================================================
CImageEditorPluginBaseInt::~CImageEditorPluginBaseInt ()
{
    delete iResource;
}

//=============================================================================
CPluginResource * CImageEditorPluginBaseInt::GetPluginResource() const
{
    return iResource;
}

//=============================================================================
CImageEditorPluginBaseInt::CImageEditorPluginBaseInt ()
{

}

//=============================================================================
void CImageEditorPluginBaseInt::ConstructL (
    const TDesC &   aResourcePath,
    const TDesC &   aResourceFile
    )
{
    TFileName resourcePath ( aResourcePath );
    User::LeaveIfError( CompleteWithAppPath( resourcePath ) );
    iResource = CPluginResource::NewL (resourcePath, aResourceFile);
}

