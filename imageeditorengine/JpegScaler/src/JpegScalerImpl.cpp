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


#include "JpegScalerImpl.h"
#include "CJpegScale.h"

CJpegScalerImpl* CJpegScalerImpl::NewL(RFs & aFsSession)
{
    CJpegScalerImpl* impl = new(ELeave) CJpegScalerImpl(aFsSession);
    CleanupStack::PushL(impl);
    impl->ConstructL();
    CleanupStack::Pop();
    return impl;
}

CJpegScalerImpl::~CJpegScalerImpl()
{
    Cancel();
    delete iScale;
}
	
void CJpegScalerImpl::Scale(TRequestStatus* aRequestStatus, const TFileName& aSrcFileName, const TFileName& aTrgFileName, const TSize& aDestinationSize)
{
    iScale->iSourceName = aSrcFileName;
    iScale->iTargetName = aTrgFileName;
    iScale->iTargetSize = aDestinationSize;
    iCallerReqStatus = aRequestStatus;
    *iCallerReqStatus = KRequestPending;
    Call();
}

CJpegScalerImpl::CJpegScalerImpl(RFs& aFsSession)
: CAsyncOneShot(EPriorityStandard), iFsSession(aFsSession)
{
    
}

void CJpegScalerImpl::ConstructL()
{
    iScale = new(ELeave) CJpegScale();
}

void CJpegScalerImpl::RunL()
{
    TInt err = KErrNone;
    if (iStatus == KErrNone)
    {
        TRAP(err,iScale->ScaleL());    
    }
    else
    {
        err = iStatus.Int();
    }
    User::RequestComplete(iCallerReqStatus,err);
}



// End of File
