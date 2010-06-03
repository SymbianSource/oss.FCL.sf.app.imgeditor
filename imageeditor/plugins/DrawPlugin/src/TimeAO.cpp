/*
 ============================================================================
 Name		: TimeAO.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CTimeAO implementation
 ============================================================================
 */

#include "TimeAO.h"
#include "MCountTime.h"
// ---------------------------------------------------------------------------
// CTimeAO()
// ---------------------------------------------------------------------------
//
CTimeAO::CTimeAO() :
	CActive(EPriorityStandard) // Standard priority
	{
	}
// ---------------------------------------------------------------------------
// NewLC()
// ---------------------------------------------------------------------------
//
CTimeAO* CTimeAO::NewLC()
	{
	CTimeAO* self = new (ELeave) CTimeAO();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
// ---------------------------------------------------------------------------
// NewL()
// ---------------------------------------------------------------------------
//
CTimeAO* CTimeAO::NewL()
	{
	CTimeAO* self = CTimeAO::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}
// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
//
void CTimeAO::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal());
	CActiveScheduler::Add(this); // Add to scheduler
	}
// ---------------------------------------------------------------------------
// ~CTimeAO()
// ---------------------------------------------------------------------------
//
CTimeAO::~CTimeAO()
	{
	Cancel(); // Cancel any request, if outstanding
	iTimer.Close(); // Destroy the RTimer object
	// Delete instance variables if any
	}
// ---------------------------------------------------------------------------
// DoCancel()
// ---------------------------------------------------------------------------
//
void CTimeAO::DoCancel()
	{
	iTimer.Cancel();
	}
// ---------------------------------------------------------------------------
// StartL()
// ---------------------------------------------------------------------------
//
void CTimeAO::StartL()
	{
	Cancel(); // Cancel any request, just to be sure
	iTimer.After(iStatus, 1000000); // Set for later
	SetActive(); // Tell scheduler a request is active
	}
// ---------------------------------------------------------------------------
// SetObserver()
// ---------------------------------------------------------------------------
//
void CTimeAO::SetObserver(MCountTime* aObserver)
	{
	iObserver = aObserver;
	}
// ---------------------------------------------------------------------------
// RunL()
// ---------------------------------------------------------------------------
//
void CTimeAO::RunL()
	{
	
	if (iObserver != NULL)
		{
		iObserver->Notify();
		}
	}
// ---------------------------------------------------------------------------
// RunError()
// ---------------------------------------------------------------------------
//
TInt CTimeAO::RunError(TInt aError)
	{
	return aError;
	}
