/*
 ============================================================================
 Name		: AnimationAO.cpp
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CAnimationAO implementation
 ============================================================================
 */

#include "AnimationAO.h"
// ---------------------------------------------------------------------------
// CAnimationAO()
// ---------------------------------------------------------------------------
//
CAnimationAO::CAnimationAO() :
	CActive(EPriorityStandard) // Standard priority
	{
	}
// ---------------------------------------------------------------------------
// NewLC()
// ---------------------------------------------------------------------------
//
CAnimationAO* CAnimationAO::NewLC()
	{
	CAnimationAO* self = new (ELeave) CAnimationAO();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
// ---------------------------------------------------------------------------
// NewL()
// ---------------------------------------------------------------------------
//
CAnimationAO* CAnimationAO::NewL()
	{
	CAnimationAO* self = CAnimationAO::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}
// ---------------------------------------------------------------------------
// ConstructL()
// ---------------------------------------------------------------------------
//
void CAnimationAO::ConstructL()
	{
	User::LeaveIfError(iTimer.CreateLocal()); // Initialize timer
	CActiveScheduler::Add(this); // Add to scheduler
	}
// ---------------------------------------------------------------------------
// CAnimationAO()
// ---------------------------------------------------------------------------
//
CAnimationAO::~CAnimationAO()
	{
	Cancel(); // Cancel any request, if outstanding
	iTimer.Close(); // Destroy the RTimer object
	}
// ---------------------------------------------------------------------------
// DoCancel()
// ---------------------------------------------------------------------------
//
void CAnimationAO::DoCancel()
	{
	iTimer.Cancel();
	}
// ---------------------------------------------------------------------------
// StartL()
// ---------------------------------------------------------------------------
//
void CAnimationAO::StartL(TTimeIntervalMicroSeconds32 aDelay)
	{
	Cancel(); // Cancel any request, just to be sure
	iTimer.After(iStatus, aDelay); // Set for later
	SetActive(); // Tell scheduler a request is active
	}
// ---------------------------------------------------------------------------
// RunL()
// ---------------------------------------------------------------------------
//
void CAnimationAO::RunL()
	{
	iTimer.After(iStatus, 33333); // Set for 1 sec later
	SetActive(); 
	iObserver->Notify();
	}
// ---------------------------------------------------------------------------
// RunError()
// ---------------------------------------------------------------------------
//
TInt CAnimationAO::RunError(TInt aError)
	{
	return aError;
	}
// ---------------------------------------------------------------------------
// SetObserver()
// ---------------------------------------------------------------------------
//
void CAnimationAO::SetObserver(MAnimationObserver *aObserver)
	{
	iObserver = aObserver;
	}
