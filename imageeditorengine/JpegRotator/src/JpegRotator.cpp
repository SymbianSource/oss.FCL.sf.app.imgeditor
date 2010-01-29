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


// INCLUDE FILES
#include    "JpegRotator.h"
#include    "JpegRotatorImpl.h"

// CONSTANTS

//  debug log
#include "imageeditordebugutils.h"

//=============================================================================
EXPORT_C CJpegRotator * CJpegRotator::NewLC (RFs& aFsSession)
{
	CJpegRotator * self = new (ELeave) CJpegRotator ();
	CleanupStack::PushL (self);
	self->ConstructL (aFsSession);
	return self;
}

//=============================================================================
EXPORT_C CJpegRotator * CJpegRotator::NewL (RFs& aFsSession)
{
    CJpegRotator * self = CJpegRotator::NewLC (aFsSession);
	CleanupStack::Pop(); // self
	return self;
}

//=============================================================================
EXPORT_C CJpegRotator::~CJpegRotator ()
{
    LOG( KJpegRotatorLogFile, "CJpegRotator::~CJpegRotator");

    delete iImplementation;
}

//=============================================================================
EXPORT_C void CJpegRotator::RotateImageL (
    TRequestStatus &			aStatus,
    const TDesC &				aSourceFileName, 
    const TDesC &				aTargetFileName,
    CJpegRotator::TRotationMode aRotationMode,
    TBool						aHandleExifData,
    TBool						aForceLossyMode
	)
{
    LOGFMT( KJpegRotatorLogFile, "CJpegRotator::RotateImageL (rotation mode: %d)", aRotationMode );
    LOGFMT( KJpegRotatorLogFile, "\tSource image: %S", &aSourceFileName );
    LOGFMT( KJpegRotatorLogFile, "\tTarget image: %S", &aTargetFileName );

    iImplementation->PrepareRotateFileL (aSourceFileName, aTargetFileName, aRotationMode, aHandleExifData);
    iImplementation->StartAsyncRotate (aStatus, aForceLossyMode);
}

//=============================================================================
EXPORT_C void CJpegRotator::Cancel ()
{
    LOG( KJpegRotatorLogFile, "CJpegRotator::Cancel()");

    iImplementation->Cancel();
}

//=============================================================================
CJpegRotator::CJpegRotator ()
{
}

//=============================================================================
void CJpegRotator::ConstructL (RFs& aFsSession)
{
    LOG_INIT( KJpegRotatorLogFile );

    iImplementation = CJpegRotatorImpl::NewL (aFsSession);
}

// End of File
