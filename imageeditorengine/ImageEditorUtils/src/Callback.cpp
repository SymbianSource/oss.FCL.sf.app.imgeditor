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


#include "callback.h"

//=============================================================================
EXPORT_C CObCallback::~CObCallback()
{
	Cancel();
    iMethod = NULL;
}

//=============================================================================
EXPORT_C CObCallback * CObCallback::NewL (MObCallbackMethod * aMethod)
{
	CObCallback * self = new (ELeave) CObCallback (aMethod);
	CleanupStack::PushL (self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}

//=============================================================================
EXPORT_C void CObCallback::DoCallback (TInt aParam)		
{
	iParam = aParam;
    CompleteRequest();
}

//=============================================================================
void CObCallback::ConstructL()
{
	CActiveScheduler::Add (this);
}

//=============================================================================
CObCallback::CObCallback (MObCallbackMethod * aMethod) : 
CActive (EPriorityStandard), 
iMethod (aMethod)
{

}

//=============================================================================
void CObCallback::RunL()
{
    if ( iMethod->CallbackMethodL (iParam) )
    {
        CompleteRequest();
    }
}

//=============================================================================
void CObCallback::DoCancel()
{

}

//=============================================================================
void CObCallback::CompleteRequest()
{
	if ( IsActive() )
	{
		Cancel();
	}
	TRequestStatus * p = &iStatus;
	SetActive();
	User::RequestComplete (p, KErrNone);
}
