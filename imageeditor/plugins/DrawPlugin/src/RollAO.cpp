/*
 ============================================================================
 Name		: AnimationAO.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CAnimationAO implementation
 ============================================================================
 */

#include "RollAO.h"
// ---------------------------------------------------------------------------
// CRollAO()
// ---------------------------------------------------------------------------
//
CRollAO::CRollAO() :
	CActive(EPriorityStandard) // Standard priority
	{
	}
// ---------------------------------------------------------------------------
// NewLC()
// ---------------------------------------------------------------------------
//
CRollAO* CRollAO::NewLC()
	{
	CRollAO* self = new (ELeave) CRollAO();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
// ---------------------------------------------------------------------------
// NewL()
// ---------------------------------------------------------------------------
//
CRollAO* CRollAO::NewL()
	{
	CRollAO* self = CRollAO::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}
// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
//
void CRollAO::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal()); // Initialize timer
	CActiveScheduler::Add(this); // Add to scheduler
	}
// ---------------------------------------------------------------------------
// ~CRollAO()
// ---------------------------------------------------------------------------
//
CRollAO::~CRollAO()
	{
	Cancel(); // Cancel any request, if outstanding
	iTimer.Close(); // Destroy the RTimer object
	// Delete instance variables if any
	}
// ---------------------------------------------------------------------------
// DoCancel()
// ---------------------------------------------------------------------------
//
void CRollAO::DoCancel()
	{
	iTimer.Cancel();
	}
// ---------------------------------------------------------------------------
// StartL()
// ---------------------------------------------------------------------------
//
void CRollAO::StartL(TTimeIntervalMicroSeconds32 aDelay)
	{
	Cancel(); // Cancel any request, just to be sure
	iTimer.After(iStatus, aDelay); // Set for later
	SetActive(); // Tell scheduler a request is active
	}
// ---------------------------------------------------------------------------
// RunL()
// ---------------------------------------------------------------------------
//
void CRollAO::RunL()
	{
	iTimer.After(iStatus, 300); // Set for 1 sec later
	SetActive(); 
	iObserver->Roll();
	}
// ---------------------------------------------------------------------------
// RunError()
// ---------------------------------------------------------------------------
//
TInt CRollAO::RunError(TInt aError)
	{
	return aError;
	}
// ---------------------------------------------------------------------------
// SetObserver()
// ---------------------------------------------------------------------------
//
void CRollAO::SetObserver(MRollObserver *aObserver)
	{
	iObserver = aObserver;
	}
