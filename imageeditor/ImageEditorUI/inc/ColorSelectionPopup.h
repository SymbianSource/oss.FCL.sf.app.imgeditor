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
* Color selection popup dialog header file.
*
*/


#ifndef COLORSELECTIONPOPUP_H
#define COLORSELECTIONPOPUP_H

// INCLUDES
#include <coecntrl.h>
#include <AknDialog.h>

#include "PreviewControlBase.h"

// FORWARD DECLARATIONS
class CFbsBitmap;
class TAknLayoutText;
#ifdef RD_TACTILE_FEEDBACK 
class MTouchFeedback; 
#endif /* RD_TACTILE_FEEDBACK  */
   
class CColorSelectionPopup :public CPreviewControlBase
{

public:

    /** Default constructor, cannot leave.
	*
	*   @since S60 5.0
	*   @param -
	*	@return -
	*/
	CColorSelectionPopup();

    /** Destructor
	*
	*   @since S60 5.0
	*	@param -
	*	@return -
	*/
	virtual ~CColorSelectionPopup ();

    /** Second phase constructor
	*
	*   @since S60 5.0
	*	@param aParent - Parent control
	*	@return -
	*/
	virtual void ConstructL ( CCoeControl* aParent );
	
	/**	OfferKeyEventL	
	*
	*	@see CCoeControl
	*/
    virtual TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                               TEventCode aType );
                                               
    /**	HandlePointerEventL	
	*
	*	@see CCoeControl
	*/
    virtual void HandlePointerEventL( const TPointerEvent &aPointerEvent );

public: //new methods
      
    /** GridRect
    *
    * Returns popup component's rectangle area
    *
    * @since S60 5.0
    * @param -
    * @return TRect The area of the popup
    */
    TRect GridRect() const;
    
    /** HighlightedItemPressed
    *
    *   This function can be used to check whether user has pressed (by stylus)
    *   currently highlighted color item. 
    *
    *   @since S60 5.0
    *   @param aPosition position value to be checked.
    *   @return ETrue if aPosition is in the rect area of currently highlighted
    *          plugin item, EFalse otherwise.
    */
    TBool HighlightedItemPressed( TPoint aPosition ) const;
    
    /** GetSelectedColor
    *
    * Returns current highlighted color
    *
    * @since S60 5.0
    * @param -
    * @return TRgb Currently selected color item in RGB format
    */
    TRgb GetSelectedColor() const;


	void SetSelectedColor(TRgb aRgb);


private:                                                   
    
    /** CreateColorBitmapsL
    *
    * Creates a bitmap array
    *
    * @since S60 5.0
    * @param aSize Size that color bitmaps should be created to
    * @return -
    */
    void CreateColorBitmapsL( TSize aSize ); 
    
protected:

   	/**	SizeChanged	
	*
	*	@see CCoeControl
	*/
    virtual void SizeChanged();

	/**	Draw	
	*
	*	@see CCoeControl
	*/
    virtual void Draw( const TRect& aRect ) const;
    

private:
        
    // Popup graphic bitmaps
    CFbsBitmap*     iPopupCenter;
    CFbsBitmap*     iPopupCenterMask;
    CFbsBitmap*     iPopupCornerTL;
    CFbsBitmap*     iPopupCornerTLMask;
    CFbsBitmap*     iPopupCornerTR;
    CFbsBitmap*     iPopupCornerTRMask;
    CFbsBitmap*     iPopupCornerBL;
    CFbsBitmap*     iPopupCornerBLMask;
    CFbsBitmap*     iPopupCornerBR;
    CFbsBitmap*     iPopupCornerBRMask;
    CFbsBitmap*     iPopupSideL;
    CFbsBitmap*     iPopupSideLMask;
    CFbsBitmap*     iPopupSideR;
    CFbsBitmap*     iPopupSideRMask;
    CFbsBitmap*     iPopupSideT;
    CFbsBitmap*     iPopupSideTMask;
    CFbsBitmap*     iPopupSideB;
    CFbsBitmap*     iPopupSideBMask;
         
    // Popup rects
    TRect           iPopupCenterRect;
    TRect           iPopupCornerTLRect;
    TRect           iPopupCornerTRRect;
    TRect           iPopupCornerBLRect;
    TRect           iPopupCornerBRRect;
    TRect           iPopupSideLRect;
    TRect           iPopupSideRRect;
    TRect           iPopupSideTRect;
    TRect           iPopupSideBRect;    
    TRect           iHeadingRect;    
    
    // Format and layout information of the heading text
    TAknLayoutText  iLayoutTextHeading;
    
    // Contains rectangles of all grid highlight items
    RArray< TRect > iHighlightRectsArray;
    
    // Contains rectangles of all grid color items
    RArray< TRect > iGridRectsArray;
    
    // Color bitmaps
    RPointerArray < CFbsBitmap > iColorBitmapsArray;
    
    // Used rgb values
    RArray< TRgb > iRGBArray;
    
    TInt            iCurrentItem;

        
};


class CColorSelectionDialog : public CAknDialog
    {
public:
    
    /** RunDlgLD
	*
	*	@param aBitmap - background bitmap
	*	@param aRect - Rect for the dialog
    *	@param aRgb - Color value to be returned
	*	@return - ExecuteLD() return value
	*/
	static TInt RunDlgLD( const CFbsBitmap* aBitmap, 
	                      const TRect& aRect, 
	                      TRgb& aRgb );

  	/** Constructor
	*
    *	@param aRgb - Color value to be returned
	*	@return -
	*/
    CColorSelectionDialog( TRgb& aRgb );

  	/** Destructor
	*
	*	@param  -
	*	@return -
	*/
    virtual ~CColorSelectionDialog();

  	/** 2nd phase constructor
	*
	*	@param aBitmap - background bitmap
	*	@param aRect - Rect for the dialog
	*	@return -
	*/
    void ConstructL( const CFbsBitmap* aBitmap, const TRect& aRect);

    /** OkToExitL
    * 
    *   From CAknDialog update member variables .
    *   @param aButtonId The ID of the button that was activated.
    *   @return Should return ETrue if the dialog should exit,
    *           and EFalse if it should not
    */
    TBool OkToExitL( TInt aButtonId );

    /** Draw
    * 
    * 
    * @see CAknDialog
    *
    */
    void Draw(const TRect& aRect) const;

    /** SizeChanged
    * 
    * 
    * @see CCoeControl
    *
    */
    virtual void SizeChanged();

    /** OfferKeyEventL
    *  
    * @see CCoeControl
    *
    */
    virtual TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType);
    
    /**	HandlePointerEventL	
	*
	*	@see CCoeControl
	*/
    virtual void HandlePointerEventL( const TPointerEvent &aPointerEvent );
    
    /** PreLayoutDynInitL
    *  
    * @see CEikDialog
    *
    */
    virtual void PreLayoutDynInitL();

    /** CountComponentControls
    *  
    * @see CCoeControl
    *
    */
    TInt CountComponentControls() const;

    /** ComponentControl
    *  
    * @see CCoeControl
    *
    */
    virtual CCoeControl* ComponentControl(TInt aIndex) const;

private:
    
    // popup component
    CColorSelectionPopup*   iPopup;
    
    // Selected color   
    TRgb&                   iCurrentRgb;
    
    	// Feedback for screen touch:
#ifdef RD_TACTILE_FEEDBACK 
	MTouchFeedback* iTouchFeedBack;
#endif /* RD_TACTILE_FEEDBACK  */ 
    
    };

#endif
