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
* Dialog for Bubble plugin.
*
*/


#ifndef BUBBLESELECTIONDIALOG_H
#define BUBBLESELECTIONDIALOG_H

#include <AknDialog.h>
#include <aknprogressdialog.h>

#include "Bubblescaler.h"
#include "CallbackMethod.h"

// FORWARD DECLARATION
class CBubbleSelectionGrid;
class CGulIcon;
class CDir;
class CAknIconArray;
class CBubbleScaler;
class CObCallback;


class CBubbleSelectionDialog : public CEikDialog, 
                                public MBubbleScalerNotifier, 
                                public MProgressDialogCallback,
                                public MEikListBoxObserver,
                                public MObCallbackMethod
    {

    class TBubbleListItem
        {
    public:
        TFileName iFilename;
        TInt iIndex;
        };

    typedef RArray<TBubbleListItem> RBubbleList;
    
    enum TState
        {
        EIdle,
        EScalingBitmap,
        EScalingMask
        };

public:

  	/** Constructor
	*
	*	@param aBubbleFileName - Bubble filename
	*   @param aBubbleFileNameIndex - Bubble filename index
	*	@return -
	*/
    CBubbleSelectionDialog(TFileName& aBubbleFileName, TInt& aBubbleFileNameIndex);

  	/** Destructor
	*
	*	@param  -
	*	@return -
	*/
    virtual ~CBubbleSelectionDialog();

  	/** ConstructL
	*
	*	Second phase constructor
	*
	*   @param aParent - Parent control
	*	
	*/
	void ConstructL(MCoeControlObserver* aParent);

    /** OkToExitL
    * 
    * From CAknDialog update member variables .
    * @param aButtonId The ID of the button that was activated.
    * @return Should return ETrue if the dialog should exit,
    *    and EFalse if it should not
    */
    TBool OkToExitL( TInt aButtonId );

	/**	SetBusy
	*
	* @param aBusy - Set control busy
	*
    */
    void SetBusy(TBool aBusy);

    /** SetSizeAndPosition
    *   
    *   @see CEikDialog
    *   
    */
	void SetSizeAndPosition( const TSize& /*aSize*/ );

    /** BubbleFilenameL
	*
    *   Get select Bubble filename and index
    *
	*	@param aFilename - Bubble filename with full path
    *   @param aIndex    - Bubble index in the MBM file 
    *	@return -
	*/
    void BubbleFilenameL(TDes& aFilename, TInt& aIndex);

    /** BubbleScalerOperationReadyL
	*
    *   Called by the Bubble scaler when operation is ready
    *
	*	@see MBubbleScalerNotifier
    *
	*/
    void BubbleScalerOperationReadyL(TInt aError) ;

    /** DialogDismissedL
    *   
    *   @see MProgressDialogCallback
    *   
    */
    void DialogDismissedL( TInt aButtonId );

    /**
    *   
    *   @see MObCallbackMethod
    *   
    */
    TBool CallbackMethodL( TInt aParam );

    /** PreLayoutDynInitL
    *   
    *   @see CEikDialog
    *   
    */
	void PreLayoutDynInitL();

    /** CreateCustomControlL
    *   
    *   @see CEikDialog
    *   
    */
	SEikControlInfo CreateCustomControlL(TInt aControlType);
    
    /** OfferKeyEventL
    *   
    *   @see CEikDialog
    *   
    */
	virtual TKeyResponse OfferKeyEventL (
        const TKeyEvent &   aKeyEvent,
        TEventCode          aType
        );
        
	virtual void HandleDialogPageEventL(TInt /*aEventId*/);

	virtual void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

	void HandleResourceChange(TInt aType);
private:

	/*	SetupGrid
	*
	*	Sets up grid every time the grid size changes.
	*
	*   @param -
	*   @return -
	*/
	void SetupGrid();

	/**	FindBubblesL
	*
	*	Find all Bubbles in directories specified in the 
    *   resource file
	*
	*	@param -
	*	@return -
	*/
    void FindBubblesL();

    /** LoadBitmapsL
	*
    *   Loads Bubble bitmap and mask
    *
    *   @param - 
    *	@return -
	*/
    void LoadBitmapsL();

    /** AddIconL
	*
    *   Adds icon to grid
    *
    *   @param - 
    *	@return -
	*/
    void AddIconL();

    /** ExternalizeIconArrayL
	*
    *   Externalize icon array
    *
    *   @param - 
    *	@return -
	*/
    void ExternalizeIconArrayL();

    /** InternalizeIconArrayL
	*
    *   Internalize icon array
    *
    *   @param - 
    *	@return -
	*/
    void InternalizeIconArrayL();

private:

    /// Selection grid
    CBubbleSelectionGrid*  iGrid;
    TFileName&				iBubbleFileName;
    TInt&					iBubbleFileNameIndex;
    
    CAknIconArray* 			iIconArray;
    CDesC16Array*			iTextArray;
    
    CAknProgressDialog*     iProgressDialog;
    CBubbleScaler*         iBubbleScaler;
    RBubbleList            iBubbleList; 
    CObCallback*            iCallback;
    CFbsBitmap*             iCurrentBitmap;
    CFbsBitmap*             iCurrentMask;
    TInt                    iBubbleGridCellWidth;
    TInt                    iBubbleGridCellHeight;
    TInt                    iCurrentIcon;
    TInt                    iState;
    TBool                   iBusy;    
    TBool                   iDoubleClick;
    TBool                   iClick; 

    };


#endif

// End of File
