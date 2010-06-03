/*
 ============================================================================
 Name		: CImageLabel.h
 Author	  : 
 Version	 : 1.0
 Copyright   : Your copyright notice
 Description : CImageLabel declaration
 ============================================================================
 */

#ifndef IMAGELABEL_H
#define IMAGELABEL_H

// INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <COECNTRL.H>
#include "MCountTime.h"

class CFbsBitmap;
class CTimeAO;


class MHandlePointerObserver
	{
public:
	virtual void HandlePointerNotify(CCoeControl* aComponent) = 0;
	virtual ~MHandlePointerObserver() {}
protected:
	MHandlePointerObserver(){}
	};



// CLASS DECLARATION

/**
 *  CCImageLabel
 * 
 */
class CImageLabel : public CCoeControl, MCountTime
	{
public:
	enum LABEL_STATE
		{
		ENone, ESelected, ESelectedWithTooltip,EMoving
		};
	enum TOOLTIP_LAYOUT
		{
		ELabelHorizontal, ELabelVertical
		};
public:
	// Constructors and destructor

	/**
	 * Destructor.
	 */
	~CImageLabel();

	/**
	 * Two-phased constructor.
	 */
	static CImageLabel* NewL(CCoeControl *aParent, const TRect& aRect, TOOLTIP_LAYOUT aLabelLayout);

	/**
	 * Two-phased constructor.
	 */
	static CImageLabel* NewLC(CCoeControl *aParent, const TRect& aRect, TOOLTIP_LAYOUT aLabelLayout);

public:
	/**
	 * From CCoeControl
	 */
	virtual void HandlePointerEventL(const TPointerEvent& aPointerEvent);

	/**
	 * From CCoeControl
	 */
	virtual void SizeChanged();

	/**
	 * From CCoeControl
	 */
	virtual void Draw(const TRect& aRect) const;
	
	virtual void Notify();

	/**
	 * Self-define 
	 */
	void SetImage(const TDesC& aMbmPackage, TInt aIndex);

	/**
	 * Self-define 
	 */
	void SetTooltip(const TDesC& aTooltip);
	
	void ResetControl();
	
	void LabelIsMoving();
	
	void SetHandlePointerObserver(MHandlePointerObserver* aHandlePointerObserver);
	
private:
	/**
	 * Constructor for performing 1st stage construction
	 */
	CImageLabel();

	/**
	 * EPOC default constructor for performing 2nd stage construction
	 */
	void ConstructL(CCoeControl *aParent, const TRect& aRect, TOOLTIP_LAYOUT aLabelLayout);
	
	void CalculateImagePoint();
	void CalculateBoundingRectStartPoint();
	void CalculateTooltipPoint();
	void CalculateZones();

private:
	LABEL_STATE iState;
	TOOLTIP_LAYOUT iLabelLayout;
	
	TBuf<20> iTooltip;
	TPoint iTooltipStartPoint;
	TPoint iImageStartPoint;
	TPoint iBoundingRectStartPoint;
	
	TSize iImageSize;
	TSize iBoundingSize;
	
	CFbsBitmap* iBitmap;
	CFbsBitmap* iBitmapMask;
	
	CTimeAO* iTimeAO;
	MHandlePointerObserver* iHandlePointerObserver;
	TBool iIsHandlePointerNotifyProcessing;
	};

#endif // CIMAGELABEL_H
