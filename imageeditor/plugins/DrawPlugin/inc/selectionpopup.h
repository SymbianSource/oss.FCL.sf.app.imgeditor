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
* Selection popup dialog header file.
*
*/


#ifndef _SELECTIONPOPUP_H
#define _SELECTIONPOPUP_H

// INCLUDES
#include <coecntrl.h>
#include <AknDialog.h>
#include <AknUtils.h>

#include "PreviewControlBase.h"

class CFbsBitmap;
//class TAknLayoutText;
   
NONSHARABLE_CLASS( CSelectionPopup ):public CPreviewControlBase
{

public:

    /** Default constructor, cannot leave.
	*
	*   @since S60 5.0
	*   @param -
	*	@return -
	*/
	CSelectionPopup();

    /** Destructor
	*
	*   @since S60 5.0
	*	@param -
	*	@return -
	*/
	~CSelectionPopup ();

    /** Second phase constructor
	*
	*   @since S60 5.0
	*	@param aParent - Parent control
	*	@return -
	*/
	void ConstructL ( CCoeControl* aParent );
	
	/**	OfferKeyEventL	
	*
	*	@see CCoeControl
	*/
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                               TEventCode aType );
                                               
    /**	HandlePointerEventL	
	*
	*	@see CCoeControl
	*/
    void HandlePointerEventL( const TPointerEvent &aPointerEvent );

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
    
    /** SetSelectedValue
    *
    * Set currently highlighted item
    *
    * @param aSelected selected item index.    
    * @since S60 5.0
    */
    void SetSelectedValue( TInt aSelected );
    
    /** GetSelectedValue
    *
    * Returns current highlighted item value
    *
    * @since S60 5.0
    * @return TInt Currently selected value
    */
    TInt GetSelectedValue() const;
    
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
    
    // Used values
    RArray< TInt > iItemArray;
    
    TInt iCurrentItem;

        
};


NONSHARABLE_CLASS( CSelectionDialog ): public CAknDialog
    {
public:
    
    /** RunDlgLD
	*
	*	@param aBitmap - background bitmap
	*	@param aRect - Rect for the dialog
    *	@param aSelection - Selected value to be returned
	*	@return - ExecuteLD() return value
	*/
	static TInt RunDlgLD( const CFbsBitmap* aBitmap, 
	                      const TRect& aRect, 
	                      TInt& aSelection );


    virtual ~CSelectionDialog();

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
    
private: // implementation

    CSelectionDialog( TInt& aSelectino );
    void ConstructL( const CFbsBitmap* aBitmap, const TRect& aRect);

private:
    
    // Own: popup component
    CSelectionPopup* iPopup;
    
    // Ref: Selected color   
    TInt& iCurrentValue;
    
    };

#endif // _SELECTIONPOPUP_H
