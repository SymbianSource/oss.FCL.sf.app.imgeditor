/*
 ============================================================================
 Name		: AnimationAO.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CAnimationAO declaration
 ============================================================================
 */

#ifndef ROLLAO_H
#define ROLLAO_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib

class MRollObserver
	{
public:
	virtual void Roll() = 0;
	~MRollObserver() {}
protected:
	MRollObserver() {}
	};





class CRollAO : public CActive
	{
public:
	// Cancel and destroy
	~CRollAO();

	// Two-phased constructor.
	static CRollAO* NewL();

	// Two-phased constructor.
	static CRollAO* NewLC();

public:
	// New functions
	// Function for making the initial request
	void StartL(TTimeIntervalMicroSeconds32 aDelay);
	void SetObserver(MRollObserver *aObserver);
	
private:
	// C++ constructor
	CRollAO();

	// Second-phase constructor
	void ConstructL();
	
	

protected:
	// From CActive
	// Handle completion
	void RunL();

	// How to cancel me
	void DoCancel();

	// Override to handle leaves from RunL(). Default implementation causes
	// the active scheduler to panic.
	TInt RunError(TInt aError);

private:
	RTimer iTimer; // Provides async timing service
	MRollObserver *iObserver;
	};

#endif // ANIMATIONAO_H
