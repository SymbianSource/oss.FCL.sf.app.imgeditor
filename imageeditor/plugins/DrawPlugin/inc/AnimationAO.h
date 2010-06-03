/*
 ============================================================================
 Name		: AnimationAO.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CAnimationAO declaration
 ============================================================================
 */

#ifndef ANIMATIONAO_H
#define ANIMATIONAO_H

#include <e32base.h>	// For CActive, link against: euser.lib
#include <e32std.h>		// For RTimer, link against: euser.lib

class MAnimationObserver
	{
public:
	virtual void Notify() = 0;
	~MAnimationObserver() {}
protected:
	MAnimationObserver() {}
	};





class CAnimationAO : public CActive
	{
public:
	// Cancel and destroy
	~CAnimationAO();

	// Two-phased constructor.
	static CAnimationAO* NewL();

	// Two-phased constructor.
	static CAnimationAO* NewLC();

public:
	// New functions
	// Function for making the initial request
	void StartL(TTimeIntervalMicroSeconds32 aDelay);
	void SetObserver(MAnimationObserver *aObserver);
	
private:
	// C++ constructor
	CAnimationAO();

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
	RTimer iTimer; // Provides async timing service
	MAnimationObserver *iObserver;
	};

#endif // ANIMATIONAO_H
