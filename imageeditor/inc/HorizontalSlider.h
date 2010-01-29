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


#ifndef __HORIZONTALSLIDER_H__
#define __HORIZONTALSLIDER_H__

#include <aknview.h>
#include <aknutils.h>
#include <coecntrl.h>

class CFbsBitmap;

/*! 
  @class CHorizontalSlider
  @discussion Simple control which draws a vertical slider
  */

class CHorizontalSlider : public CCoeControl
    {
	public: 

		/*!
		  @function NewL
		  @discussion Create a CHorizontalSlider object, which will draw itself to aRect
		  @param aRect the rectangle this view will be drawn to
		  @return a pointer to the created instance of CHorizontalSlider
		  */
		IMPORT_C static CHorizontalSlider* NewL(const TRect& aRect, const CCoeControl& aControl);

		/*!
		  @function NewLC
		  @discussion Create a CHorizontalSlider object, which will draw itself to aRect
		  @param aRect the rectangle this view will be drawn to
		  @return a pointer to the created instance of CHorizontalSlider
		  */
		IMPORT_C static CHorizontalSlider* NewLC(const TRect& aRect, const CCoeControl& aControl);

		/*!
		  @function ~CHorizontalSlider
		  @discussion Destroy the object and release all memory objects
		  */
		IMPORT_C ~CHorizontalSlider();


	public:
		// new functions

		/*!
		  @function SetIcon
		  @param aIcon the new icon
		  @discussion Sets the icon of the slider
		  */
		IMPORT_C void SetIcon(CEikImage* aIcon);

		/*!
		  @function SetCaption
		  @param aText the new caption text
		  @discussion Sets the caption text of the slider
		  */
		IMPORT_C void SetCaption(const TDesC& aText);

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
		  @discussion Perform the second phase construction of a CHorizontalSlider object
		  @param aRect Frame rectangle for container.
		  */
		void ConstructL(const TRect& aRect, const CCoeControl& aControl);

		/*!
		  @function CHorizontalSlider
		  @discussion Constructs this object
		  */
		CHorizontalSlider();

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
		  @discussion Draw this CHorizontalSlider to the screen
		  @param aRect the rectangle of this view that needs updating
		  */
		IMPORT_C void Draw(const TRect& aRect) const;

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

		/// popup graphics
        RPointerArray<CEikImage> iBorders;

		/// scrollbar graphics
        RPointerArray<CEikImage> iScrollBar;

		/// caption text
        TBuf<20> iText;

		/// icon graphic, owned
        CEikImage* iIcon;

    };

#endif // __HORIZONTALSLIDER_H__

// End of File


