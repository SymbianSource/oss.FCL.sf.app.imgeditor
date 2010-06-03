/*
 ============================================================================
 Name		: CustomerComponet.h
 Author	  : henry
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CCustomerComponet declaration
 ============================================================================
 */

#ifndef CUSTOMERCOMPONET_H
#define CUSTOMERCOMPONET_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <coecntrl.h>
#include "CImageLabel.h"
#include <COECOBS.H> 
// CLASS DECLARATION

/**
 *  CCustomerComponet
 * 
 */

class CCustomerComponet : public CCoeControl
	{
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CCustomerComponet();

	/**
	 * Two-phased constructor.
	 */
	static CCustomerComponet* NewL(TRect & aRect, CCoeControl* aParent);

	/**
	 * Two-phased constructor.
	 */
	static CCustomerComponet* NewLC(TRect & aRect, CCoeControl* aParent);

	/**
	 * Draw() 
	 */
	void Draw(const TRect& aRect) const;

	/**
	 * Handling pointer Event.
	 */
	void HandlePointerEventL(const TPointerEvent& aPointerEvent);

	/**
	 * Set Rect for the componet.
	 */
	void SetRect(TRect& aRect);

protected:

	void SizeChanged();

private:

	/**
	 * Constructor for performing 1st stage construction
	 */
	CCustomerComponet();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL(TRect & aRect, CCoeControl* aParent);
public:
	/**
	 * SetHandlePointerObserver
	 */
	void SetHandlePointerObserver(
			MHandlePointerObserver* aHandlePointerObserver);

	TBool GetButtonType();
private:
	TInt iDrawDirection;
	CFbsBitmap* iMenu;
	CFbsBitmap* iMenuMask;
	CFbsBitmap* iRight;
	CFbsBitmap* iRightMask;
	CFbsBitmap* iLeft;
	CFbsBitmap* iTop;
	CFbsBitmap* iBottom;
	CFbsBitmap* iLeftMask;
	CFbsBitmap* iTopMask;
	CFbsBitmap* iBottomMask;
	MHandlePointerObserver* iPointerObserver;
	TBool iButtonFlag;
	};

#endif // CUSTOMERCOMPONET_H
