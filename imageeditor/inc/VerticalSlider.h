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



#ifndef __VERTICALSLIDER_H__
#define __VERTICALSLIDER_H__

#include <aknview.h>

class CFbsBitmap;

/*! 
  @class CVerticalSlider
  @discussion Simple control which draws a vertical slider
  */

class CVerticalSlider : public CCoeControl
    {
	public: 

		/*!
		  @function NewL
		  @discussion Create a CVerticalSlider object, which will draw itself to aRect
		  @param aRect the rectangle this view will be drawn to
		  @return a pointer to the created instance of CVerticalSlider
		  */
		IMPORT_C static CVerticalSlider* NewL(const TRect& aRect, const CCoeControl& aControl);

		/*!
		  @function NewLC
		  @discussion Create a CVerticalSlider object, which will draw itself to aRect
		  @param aRect the rectangle this view will be drawn to
		  @return a pointer to the created instance of CVerticalSlider
		  */
		IMPORT_C static CVerticalSlider* NewLC(const TRect& aRect, const CCoeControl& aControl);

		/*!
		  @function ~CVerticalSlider
		  @discussion Destroy the object and release all memory objects
		  */
		IMPORT_C ~CVerticalSlider();


	public:
		// new functions

		/*!
		  @function SetMinimum
		  @param aValue the new minimum value
		  @discussion Sets the minimum value of the slider
		  */
		IMPORT_C void SetMinimum(TInt aValue);

		/*!
		  @function SetMaximum
		  @param aValue the new maximum value
		  @discussion Sets the maximum value of the slider
		  */
		IMPORT_C void SetMaximum(TInt aValue);

		/*!
		  @function SetStepL
		  @param aValue the new step value
		  @discussion Sets the step of the slider
		  */
		IMPORT_C void SetStep(TUint aValue);

		/*!
		  @function SetStepAmount
		  @param aValue the new step amount
		  @discussion Sets the number of steps in the slider
		  */
		IMPORT_C void SetStepAmount(TUint8 aValue);

		/*!
		  @function SetPosition
		  @discussion Sets the position of the slider. Panics if the position is out of bounds.
		  */
		IMPORT_C void SetPosition(TInt aValue);

		/*!
		  @function Minimum
		  @discussion Gets the minimum value of the slider
		  @return minimum value
		  */
		IMPORT_C TInt Minimum() const;

		/*!
		  @function Maximum
		  @discussion Gets the maximum value of the slider
		  @return maximum value
		  */
		IMPORT_C TInt Maximum() const;

		/*!
		  @function Step
		  @discussion Gets the step of the slider
		  @return current step
		  */
		IMPORT_C TInt Step() const;

		/*!
		  @function Position
		  @discussion Gets the position of the slider
		  @return current position
		  */
		IMPORT_C TInt Position() const;

		/*!
		  @function Increment
		  @discussion Increments the slider
		  */
		IMPORT_C void Increment();

		/*!
		  @function Decrement
		  @discussion Decrements the slider
		  */
		IMPORT_C void Decrement();

	private:

		/*!
		  @fuction ConstructL
		  @discussion Perform the second phase construction of a CVerticalSlider object
		  @param aRect Frame rectangle for container.
		  */
		void ConstructL(const TRect& aRect, const CCoeControl& aControl);

		/*!
		  @function CVerticalSlider
		  @discussion Constructs this object
		  */
		CVerticalSlider();

		/*!
		  @function LoadBitmapL
		  @discussion Loads one bitmap and its mask
		  */
		void LoadBitmapL( 
            CFbsBitmap*& aBitmap, 
            CFbsBitmap*& aMask, 
            TInt aBitmapIndex, 
            TInt aMaskIndex 
            ) const;

		/*!
		  @function SetBitmapSize
		  @discussion Set size of the bitmaps to reflect the current screen resolution
		  */
        void SetBitmapSize() const;


	public: // from CoeControl
		/*!
		  @function CountComponentControls
		  @return Number of component controls 
		  */
		IMPORT_C TInt CountComponentControls() const;

		/*!
		  @function ComponentControl.
		  @param aIndex index of the component control
		  @return Pointer to component control
		  */
		IMPORT_C CCoeControl* ComponentControl(TInt aIndex) const;

		/*!
		  @function Draw
		  @discussion Draw this CVerticalSlider to the screen
		  @param aRect the rectangle of this view that needs updating
		  */
		IMPORT_C void Draw(const TRect& aRect) const;

		/*!
		  @function MinimumSize
		  @discussion Gets the minimum size of this component
		  @return a minimum size of the control
		  */
		IMPORT_C TSize MinimumSize();

	private:

		/// minimum value of the slider
		TInt iMinimumValue;

		/// maximum value of the slider
		TInt iMaximumValue;

		/// step value
		TUint iStep;

		/// number of steps
		TUint8 iNumberOfSteps;

		/// current position
		TInt iPosition;

		/// bitmap holding the slider background
		CFbsBitmap* iSliderBg;

		/// mask for the slider background
		CFbsBitmap* iSliderBgMask;
	
		/// bitmap holding the slider tab that moves
		CFbsBitmap* iSliderTab;

		/// mask for the slider tab
		CFbsBitmap* iSliderTabMask;
    };

#endif // __VERTICALSLIDER_H__

// End of File


