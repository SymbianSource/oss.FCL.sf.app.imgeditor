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


#include "JpegScaler.h"
#include "JpegScalerImpl.h"

EXPORT_C CJpegScaler* CJpegScaler::NewL(RFs & aFsSession)
{
    CJpegScaler* scaler = new(ELeave) CJpegScaler();
    CleanupStack::PushL(scaler);
    scaler->ConstructL(aFsSession);
    CleanupStack::Pop();
    return scaler;
}

EXPORT_C CJpegScaler::~CJpegScaler()
{
    delete iImplementation;
}
	
EXPORT_C void CJpegScaler::Scale(TRequestStatus* aRequestStatus, const TFileName& aSrcFileName, const TFileName& aTrgFileName, const TSize& aDestinationSize)
{
    iImplementation->Scale(aRequestStatus, aSrcFileName, aTrgFileName, aDestinationSize);    
}

EXPORT_C void CJpegScaler::Cancel()
{
    iImplementation->Cancel();    
}

CJpegScaler::CJpegScaler()
{
    
}

void CJpegScaler::ConstructL(RFs & aFsSession)
{
    iImplementation = CJpegScalerImpl::NewL(aFsSession);
}

// End of File
