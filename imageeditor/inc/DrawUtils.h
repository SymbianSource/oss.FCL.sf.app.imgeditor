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
* Draw utility class header file.
*
*/


#ifndef DRAWUTILS_H
#define DRAWUTILS_H

// INCLUDES
#include <gdi.h>
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CAknWaitDialog;
class CAknProgressDialog;
class MProgressDialogCallback;
class CAknInfoPopupNoteController;

/*  CLASS: SDrawUtils
*
*	SDrawUtils wraps often used UI drawing utilities into single class
*/ 
class SDrawUtils
{

public:

/** @name TypeDefs:*/
//@{
    enum TGuideType
    {
        //  Invalid
        EGuideTypeMin,
        //  Guide arrow up
        EGuideTypeUp,
        //  Guide arrow down
        EGuideTypeDown,
        //  Guide arrow left
        EGuideTypeLeft,
        //  Guide arrow right
        EGuideTypeRight,
        //  Invalid
        EGuideTypeMax
    };

//@}

/** @name Methods:*/
//@{

	/** DrawGuide
	*
    *   Draws arrow guide to CWindowGc.
    *
	*	@param aGc -		grapics context
	*	@param aRect -		surrouding rectangle
	*	@param aType -		guide type
	*	@param aColor -		guide color
	*	@return -
	*/
    IMPORT_C static void DrawGuide (
        CGraphicsContext &  aGc,
        const TRect &       aRect,
        const TGuideType    aType,
        const TRgb &        aColor
        );

	/** DrawBeveledRect
	*
    *   Draws a 3D beveled rectangle.
    *
	*	@param aGc -		grapics context
	*	@param aRect -		surrouding rectangle (includes the borders)
	*	@param aColor -		rectangle color
	*	@param aLight -		light color
	*	@param aShadow -	shadow color
	*	@return -
	*/
    IMPORT_C static void DrawBeveledRect (
        CGraphicsContext &  aGc,
        const TRect &       aRect,
        const TRgb &        aColor,
        const TRgb &        aLight,
        const TRgb &        aShadow
        );

	/** DrawFlatRect
	*
    *   Draws a 3D flat rectangle.
    *
	*	@param aGc -		grapics context
	*	@param aRect -		surrouding rectangle (includes the borders)
	*	@param aColor -		rectangle color
	*	@param aLight -		light color
	*	@param aShadow -	shadow color
	*	@return -
	*/
    IMPORT_C static void DrawFlatRect (
        CGraphicsContext &  aGc,
        const TRect &       aRect,
        const TRgb &        aColor,
        const TRgb &        aLight,
        const TRgb &        aShadow
        );

	/** LaunchQueryDialogL
	*
    *   Launches a confirmation query dialog.
    *
	*	@param aPrompt - dialog prompt descriptor
	*	@return -
	*/
    IMPORT_C static TInt LaunchQueryDialogL (const TDesC & aPrompt);

	/** LaunchQueryDialogOkOnlyL
	*
    *   Launches a confirmation query dialog with only OK softkey
    *   available.
    *
	*	@param aPrompt - dialog prompt descriptor
	*	@return -
	*/
    IMPORT_C static TInt LaunchQueryDialogOkOnlyL (const TDesC & aPrompt);

	/** LaunchTextQueryDialogL
	*
    *   Launches a text query dialog and returns the queried descriptor.
	*	aData includes the data shown in the edit window and on return,
	*	includes the edited text data.
    *
	*	@param aData -		data descriptor
	*	@param aPrompt -	dialog prompt descriptor
	*	@return -
	*/
    IMPORT_C static TInt LaunchTextQueryDialogL (
        TDes &			aData,
	    const TDesC &	aPrompt
        );

	/** LaunchMultiLineTextQueryDialogL
	*
    *   Launches a multiline text query dialog and returns the queried descriptor.
	*	aData includes the data shown in the edit window and on return,
	*	includes the edited text data.
    *
	*	@param aData -		data descriptor
	*	@param aPrompt -	dialog prompt descriptor
	*	@return -
	*/
    IMPORT_C static TInt LaunchMultiLineTextQueryDialogL (
        TDes &			aData,
	    const TDesC &	aPrompt
        );

    /** LaunchListQueryDialogL
	*
    *   Launches a list query dialog and returns the selected index.
	*	aData includes the descriptor array of selection items.
    *
	*	@param aTextItems - text item descriptor array
	*	@param aPrompt - dialog prompt descriptor
	*	@return -
	*/
    IMPORT_C static TInt LaunchListQueryDialogL (
		MDesCArray *		aTextItems,
		const TDesC &		aPrompt
        );

	/** LaunchWaitNoteL
	*
    *   Creates and launches a wait note dialog and returns a pointer to
	*	it.
	*   @param -	aSelfPtr
    *				The wait dialog is allocated at this pointer.
	*				The pointer needs to be valid when the dialog 
	*				is dismissed and must be a class member.
	*	@param 		aResourceID - resource ID for prompt
	*	@param 		aPrompt - wait dialog prompt
	*   @param 		aCallback
	*				The wait dialog callback.
	*				Pass NULL if no callback is needed.
	*	@return - pointer to wait dialog
	*/
	IMPORT_C static void LaunchWaitNoteL (
		CAknWaitDialog** 			aSelfPtr,
		TInt						aResourceID,
		const TDesC &				aPrompt,
        MProgressDialogCallback*	aCallback = NULL
		);

	/** LaunchProgressNoteL
	*
    *   Creates and launches a wait note dialog and returns a pointer to
	*	it.
	*   @param -	aSelfPtr
    *				The wait dialog is allocated at this pointer.
	*				The pointer needs to be valid when the dialog 
	*				is dismissed and must be a class member.
	*	@param 		aResourceID - resource ID for prompt
	*	@param 		aPrompt - wait dialog prompt
	*	@param 		aFinalValue     Final value for the process
	*   @param 		aCallback
	*				The wait dialog callback.
	*				Pass NULL if no callback is needed.
	*	@return - pointer to wait dialog
	*/
	IMPORT_C static void LaunchProgressNoteL (
		CAknProgressDialog** 		aSelfPtr,
		TInt						aResourceID,
		const TDesC &				aPrompt,
		TInt						aFinalValue = 0,
        MProgressDialogCallback*	aCallback = NULL
		);

	/** LaunchColorSelectionGridL
	*
    *   Launches a color selection grid inside a pop-up list. Returns 
    *   KErrNone if color was selected.
    *
	*	@param aColor - includes the selected color on return
	*	@return - KErrNone if color selected
	*/
    IMPORT_C static TInt LaunchColorSelectionGridL (TRgb & aColor);
    
    /** LaunchColorSelectionPopupL
	*
    *   Launches a semi-transparent 35 color selection popup component. 
    *   Color grid is launched via a dialog. 
    *   
    *	@param aPreview - a constant pointer to a bitmap image that should be
    *                     drawn to background
    *	@param aRect - rectangle of calling control (use main pane rect)
	*	@param aColor - includes the selected color on return
	*	@return - KErrNone if color selected
	*/
    IMPORT_C static TInt LaunchColorSelectionPopupL (const CFbsBitmap* aPreview,
                                                           TRect aRect, 
                                                           TRgb & aColor);
    
    
	/** GetIndicatorBitmapL
	*
    *   Load the indicator cursor bitmap and mask.
    *
	*	@param aChBitmap	- reference to cursor bitmap pointer
	*	@param aChMask		- reference to cursor mask pointer
	*	@param aBmpIndex	- bitmap MBM index
	*	@param aMaskIndex	- mask MBM index
	*	@return - KErrNone if successful
	*/
    IMPORT_C static TInt GetIndicatorBitmapL (
		CFbsBitmap *&	aChBitmap,
		CFbsBitmap *&	aChMask,
		TInt			aBmpIndex,
		TInt			aMaskIndex
		);
		
    /** ShowToolTip
    *
    *   Show a tooltip note on the screen. This function calculates the
    *   relative position on the screen for the tooltip.
    *
    *   @param aPopupController - pointer to popup note controller
    *   @param aCallingControl - pointer to a calling control
    *   @param aPositionRect - position rect where the tooltip should be shown
    *   @param aText - text to be shown
    *   @return - 
    */
    IMPORT_C static void ShowToolTip ( 
        CAknInfoPopupNoteController* aPopupController,
        CCoeControl* aCallingControl, 
        TRect aPositionRect,
        const TDesC& aText
        );
    
    /** ShowToolTip
    *
    *   Show a tooltip note on the screen. This function calculates the
    *   relative position on the screen for the tooltip.
    *
    *   @param aPopupController - pointer to popup note controller
    *   @param aCallingControl - pointer to a calling control
    *   @param aPosition - position where the tooltip should be shown
    *   @param aAlignment - GUI alignment to use
    *   @param aText - text to be shown
    *   @return - 
    */
    IMPORT_C static void ShowToolTip ( 
        CAknInfoPopupNoteController* aPopupController,
        CCoeControl* aCallingControl,
        TPoint aPosition, 
        TGulAlignmentValue aAlignment,
        const TDesC& aText
        );  

//@}

protected:

/** @name Methods:*/
//@{

//@}

/** @name Members:*/
//@{
	/// 
//@}

private:

/** @name Methods:*/
//@{

//@}

/** @name Members:*/
//@{

//@}

};


#endif

// End of File

