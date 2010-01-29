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



/// INCLUDES
#include <e32std.h>
#include <bautils.h>
#include <ImplementationProxy.h>
#include "ImageEditorProvider.h"
#include "ImageEditorUids.hrh"
#include "ImageRotaterProvider.h"
#include "ImageEditorProvider.hrh"

const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY (UID_AIW_ECOM_IMPL,CEditorProvider::NewL),

#if defined( JPEG_ROTATOR_AIW_PROVIDER_SUPPORT )	
	IMPLEMENTATION_PROXY_ENTRY (KAiwCmdRotate,CRotateProvider::NewL)
#endif
	};


// The one and only exported function that is the ECom entry point
EXPORT_C const TImplementationProxy* ImplementationGroupProxy (TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

#ifndef EKA2
/**
 * Standard Symbian OS DLL entry point.
 */
TBool E32Dll(TDllReason)
	{
	return ETrue;
	}
#endif

// End of File
