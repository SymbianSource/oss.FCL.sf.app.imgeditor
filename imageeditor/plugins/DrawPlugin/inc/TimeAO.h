/*
 ============================================================================
 Name		: TimeAO.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CTimeAO declaration
 ============================================================================
 */

#ifndef TIMEAO_H
#define TIMEAO_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib

class MCountTime;

class CTimeAO : public CActive
	{
public:
	// Cancel and destroy
	~CTimeAO();

	// Two-phased constructor.
	static CTimeAO* NewL();

	// Two-phased constructor.
	static CTimeAO* NewLC();

public:
	// New functions
	// Function for making the initial request
	void StartL();
	
	void SetObserver(MCountTime* aObserver);

private:
	// C++ constructor
	CTimeAO();

	// Second-phase constructor
	void ConstructL();

private:
	// From CActive
	// Handle completion
	void RunL();

	// How to cancel me
	void DoCancel();

	// Override to handle leaves from RunL(). Default implementation causes
	// the active scheduler to panic.
	TInt RunError(TInt aError);

private:

private:
	RTimer iTimer; // Provides async timing service
	MCountTime* iObserver;
	};

#endif // TIMEAO_H
