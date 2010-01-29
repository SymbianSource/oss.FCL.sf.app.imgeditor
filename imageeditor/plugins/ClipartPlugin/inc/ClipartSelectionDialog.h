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
* Dialog for clipart plugin.
*
*/


#ifndef CLIPARTSELECTIONDIALOG_H
#define CLIPARTSELECTIONDIALOG_H

#include <AknDialog.h>
#include <aknprogressdialog.h>

#include "clipartscaler.h"
#include "CallbackMethod.h"

// FORWARD DECLARATION
class CClipartSelectionGrid;
class CGulIcon;
class CDir;
class CAknIconArray;
class CClipartScaler;
class CObCallback;


class CClipartSelectionDialog : public CEikDialog, 
                                public MClipartScalerNotifier, 
                                public MProgressDialogCallback,
                                public MEikListBoxObserver,
                                public MObCallbackMethod
    {

    class TClipartListItem
        {
    public:
        TFileName iFilename;
        TInt iIndex;
        };

    typedef RArray<TClipartListItem> RClipartList;
    
    enum TState
        {
        EIdle,
        EScalingBitmap,
        EScalingMask
        };

public:

  	/** Constructor
	*
	*	@param aClipartFileName - Clipart filename
	*   @param aClipartFileNameIndex - Clipart filename index
	*	@return -
	*/
    CClipartSelectionDialog(TFileName& aClipartFileName, TInt& aClipartFileNameIndex);

  	/** Destructor
	*
	*	@param  -
	*	@return -
	*/
    virtual ~CClipartSelectionDialog();

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

    /** ClipartFilenameL
	*
    *   Get select clipart filename and index
    *
	*	@param aFilename - Clipart filename with full path
    *   @param aIndex    - Clipart index in the MBM file 
    *	@return -
	*/
    void ClipartFilenameL(TDes& aFilename, TInt& aIndex);

    /** ClipartScalerOperationReadyL
	*
    *   Called by the Clipart scaler when operation is ready
    *
	*	@see MClipartScalerNotifier
    *
	*/
    void ClipartScalerOperationReadyL(TInt aError) ;

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

	/**	FindClipartsL
	*
	*	Find all cliparts in directories specified in the 
    *   resource file
	*
	*	@param -
	*	@return -
	*/
    void FindClipartsL();

    /** LoadBitmapsL
	*
    *   Loads clipart bitmap and mask
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
    CClipartSelectionGrid*  iGrid;
    TFileName&				iClipartFileName;
    TInt&					iClipartFileNameIndex;
    
    CAknIconArray* 			iIconArray;
    CDesC16Array*			iTextArray;
    
    CAknProgressDialog*     iProgressDialog;
    CClipartScaler*         iClipartScaler;
    RClipartList            iClipartList; 
    CObCallback*            iCallback;
    CFbsBitmap*             iCurrentBitmap;
    CFbsBitmap*             iCurrentMask;
    TInt                    iClipartGridCellWidth;
    TInt                    iClipartGridCellHeight;
    TInt                    iCurrentIcon;
    TInt                    iState;
    TBool                   iBusy;    
    TBool                   iDoubleClick;
    TBool                   iClick; 
	
    };


#endif

// End of File
