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
* Effect plugin selection dialog that shows icons of available
* plugins.
*
*/


#ifndef PLUGINGRID_H
#define PLUGINGRID_H

#include <eikdialg.h>
#include <AknDialog.h>

// FORWARD DECLARATIONS
class CFbsBitmap;
class CPluginInfo;
class CAknInfoPopupNoteController;
#ifdef RD_TACTILE_FEEDBACK 
class MTouchFeedback; 
#endif /* RD_TACTILE_FEEDBACK  */

typedef RPointerArray<CPluginInfo> RPluginArray;

class CPluginGrid : public CCoeControl
{

public:

    /** Default constructor, cannot leave.
	*
	*	@param -
	*	@return -
	*/
	CPluginGrid (CFbsBitmap** aBitmap, 
                 const RPluginArray* aItems);

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	virtual ~CPluginGrid ();

	/**	Second phase constructor	
	*
	*	@param -
	*	@return -
	*/
    void ConstructL ();

	/**	GetSelectedItemIndex	
	*
	*	@param -
	*	@return - Current item
	*/
    TInt GetSelectedItemIndex() const;

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
    * Returns grids rectangle area
    *
    * @since S60 5.0
    * @param -
    * @return TRect The area of the grid component (text field not included)
    */
    TRect GridRect() const;
    
    /** HighlightedItemPressed
    *
    * This function can be used to check whether user has pressed (by stylus)
    * currently highlighted plugin item. 
    *
    * @since S60 5.0
    * @param aPosition position value to be checked.
    * @return ETrue if aPosition is in the rect area of currently highlighted
    *          plugin item, EFalse otherwise.
    */
    TBool HighlightedItemPressed( TPoint aPosition ) const;
    
    /** ShowTooltip
    *
    *   Shows a tooltip text on the screen next to the currently 
    *   highlighted grid item.
    *
    *   @since S60 5.0
    *   @param aPopupController - pointer to popup note controller
    *   @param aCallingControl - pointer to a calling control    
    *   @return -
    */
    void ShowTooltip( CAknInfoPopupNoteController* aPopupController,
                      CCoeControl* aCallingControl );
     
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

	/**	SizeChanged	
	*
	*	@see CCoeControl
	*/
    virtual void SizeChanged();
    
protected:

	/**	Draw	
	*
	*	@see CCoeControl
	*/
    virtual void Draw( const TRect& aRect ) const;
    
private:

    // Background image
    CFbsBitmap** iBitmap;
    // Plugin info array
    const RPluginArray* iPluginItems;
    // Current item index
    TInt iCurrentItem;
    // popup graphics
    RPointerArray<CEikImage> iBorders;
    // highlight graphics
    RPointerArray<CEikImage> iHighlight;
    // plugin icons
    RPointerArray<CEikImage> iIcons;
    // title text
    CEikLabel* iText;
	
	// Grid resolutions
	TInt iNumberOfRows;	
	TInt iNumberOfColumns;
	
    // Grid layout	
    TSize   iCellSize;
    TRect   iTouchGridRect;
    TPoint  iHighlightPos;	
	TRect   iParentRect;

};


class CPluginSelectionDialog : public CAknDialog
    {
public:

  	/** RunDlgLD
	*
	*	@param aBitmap - background bitmap
	*	@param aSelectedItem - selected item
	*	@param aItems - Plugin info item array
	*	@param aRect - Rect for the dialog
	*	@return - ExecuteLD() return value
	*/
	static TInt RunDlgLD(CFbsBitmap** aBitmap, 
                         TInt& aSelectedItem, 
                         const RPluginArray* aItems,
                         const TRect& aRect);

  	/** Constructor
	*
	*	@param aBitmap - background bitmap
	*	@param aSelectedItem - selected item
	*	@param aItems - Plugin info item array
	*	@return -
	*/
    CPluginSelectionDialog(CFbsBitmap** aBitmap, 
                           TInt& aSelectedItem, 
                           const RPluginArray* aItems);

  	/** Destructor
	*
	*	@param  -
	*	@return -
	*/
    virtual ~CPluginSelectionDialog();

  	/** 2nd phase constructor
	*
	*	@param aRect - Rect for the dialog
	*	@return -
	*/
    void ConstructL(const TRect& aRect);

    /** OkToExitL
    * 
    * From CAknDialog update member variables .
    * @param aButtonId The ID of the button that was activated.
    * @return Should return ETrue if the dialog should exit,
    *    and EFalse if it should not
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
    /// Background bitmap
    CFbsBitmap** iBitmap;
    /// Plugin info array
    const RPluginArray* iItems;
    /// Selected item
    TInt& iSelectedItem;
    /// Selection grid
    CPluginGrid* iGrid;
	// Popup controller
	CAknInfoPopupNoteController* iPopupController;
	
	// Feedback for screen touch:
#ifdef RD_TACTILE_FEEDBACK 
	MTouchFeedback* iTouchFeedBack;
#endif /* RD_TACTILE_FEEDBACK  */    
    
    };


#endif

// End of File
