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



#ifndef IMAGEEDITORPLUGINMANAGER_H
#define IMAGEEDITORPLUGINMANAGER_H

//	INCLUDES
#include <e32base.h>

#include "imageeditordebugutils.h"
#include "commondefs.h"

/// FORWARD DECLARATIONS
class CCoeControl;
class CFbsBitmap;
class CPluginLoader;
class CEditorImage;
class CEngineWrapper;
class CSystemParameters;
class CArrayFixedFlat;

#ifdef RD_CONTENTNOTIFICATION
class CContentNotification;
#endif

/*	CLASS: CImageEditorPluginManager
*
*	CImageEditorPluginManager functions as Model of the application in 
*	MVC design pattern sense. It includes an instance of Scalado's MIA
*	image processing engine and manages plug-in communication with the
*	application controller.
*/
class CImageEditorPluginManager : public CBase
{
public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param -
	*	@return pointer to created CImageEditorPluginManager object
	*/
	IMPORT_C static CImageEditorPluginManager * NewL ();

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C virtual ~CImageEditorPluginManager ();

	/** OpenPluginL
	*
	*	Loads and opens plug-in
	*	
	*	@param aFileName - plug-in file name
	*	@return -
	*/
	IMPORT_C void OpenPluginL (const TDesC & aFileName);

	/** CreatePluginControlL
	*
	*	Creates UI control for the plug-in in question.
	*	
	*	@param aRect - control rectangle
	*	@param aParent - parent control 
	*	@param aPluginControl - reference to plug-in control
	*	@return pointer to plug-in control
	*/
	IMPORT_C TInt CreatePluginControlL (
		const TRect &	aRect,
		CCoeControl *	aParent,
		CCoeControl *&	aPluginControl
		);

	/** ClosePlugin
	*
	*	Deletes current plug-in and unloads it.
	*	
	*	@param -
	*	@return -
	*/
	IMPORT_C void ClosePlugin ();

	/** ShowPluginPopup
	*
	*	Shows plugin-related popup note.
	*	
	*	@param -
	*	@return -
	*/
	IMPORT_C void ShowPluginPopup ();

	/** UndoL
	*
	*	Undoes last operation performed to the image.
	*	
	*	@param -
	*	@return -
	*/
	IMPORT_C void UndoL();

	/** RedoL
	*
	*	Redoes the last removed operation to the image.
	*	
	*	@param -
	*	@return -
	*/
	IMPORT_C void RedoL();

	/** LoadImageL
	*
	*	Loads image with MIA engine.
	*	
	*	@param aUseIPECodec - ImageEditor's own codec should be used
	*	@return -
	*/
	IMPORT_C void LoadImageL (const TBool aUseIPECodec);

    /** SaveImageL
	*
	*	Saves image with MIA engine.
	*	
	*	@param aQuality - JPEG quality factor
	*	@param aSize - if not NULL includes the custom output size
	*	@return -
	*/
	IMPORT_C void SaveImageL (
        const TInt      aQuality,
        const TSize *   aSize = NULL    
        );

	/** PrepareAsyncSaveL
    *
	*	Prepares to save the image by block
	*	
	*	@param aBlockCount - Number of blocks to save
	*	@param aQuality - JPEG quality factor
	*	@param aSize - if not NULL includes the custom output size
	*	@return -
	*/
	IMPORT_C void PrepareAsyncSaveL (
        const TInt      aQuality,
        const TSize *   aSize = NULL
        );

	/** SaveBlock
	*
	*	Saves a block 
	*	
	*	@param - 
	*	@return - returns percentage of image data saved, 100 when ready
	*/
	IMPORT_C TInt SaveBlockL();

	/** SaveAbortL
	*
	*	Must be called after aborting block saving
	*	
	*	@param - 
	*/
	EXPORT_C void SaveAbortL();

	/** FinishAsyncSave
	*
	*	Finishes asynchronous saving with MIA engine.
	*	
	*	@param  - 
	*	@return -
	*/
	IMPORT_C void FinishAsyncSaveL();

	/** ProcessImageL
    *
	*	Processes image with MIA engine.
	*	
	*	@param -
	*	@return -
	*/
	IMPORT_C void ProcessImageL();

	/** PrepareAsyncProcessL
    *
	*	Prepares to process the image by block
	*	
	*	@param aBlockCount - Number of blocks to process
	*	@return -
	*/
	IMPORT_C void PrepareAsyncProcessL(TInt& aBlockCount);

	/** ProcessBlockL
	*
	*	Render a block 
	*	
	*	@param - 
	*	@return - error code, KErrNone until whole image is saved and KErrCompletion
    *             when ready
	*/
	IMPORT_C TInt ProcessBlockL();

	/** FinishAsyncProcessL
	*
	*	Finishes asynchronous processing with MIA engine.
	*	
	*	@param  - 
	*	@return -
	*/
	IMPORT_C void FinishAsyncProcess();

    /** GetPreviewImage
	*
	*	Getter for preview bitmap.
	*
	*	@param -
	*	@return pointer to CFbsBitmap
	*/
	IMPORT_C CFbsBitmap * GetPreviewImage();

    /** GetFullImage
	*
	*	Getter for bfull size bitmap.
	*
	*	@param -
	*	@return pointer to CFbsBitmap
	*/
	IMPORT_C CFbsBitmap * GetFullImage();

    /** GetImageName
	*
	*	Getter for image file name
	*
	*	@param -
	*	@return reference to the file name
	*/
	IMPORT_C const TDesC & GetImageName();

	/** SetPreviewImage 
	*
	*	Setter for preview bitmap.
	*
	*	@param aBitmap - pointer to preview bitmap
	*	@return -
	*/
	IMPORT_C void SetPreviewImage (CFbsBitmap * aBitmap);

	/** SetFullImage 
	*
	*	Setter for preview bitmap.
	*
	*	@param aBitmap - pointer to full size bitmap
	*	@return -
	*/
	IMPORT_C void SetFullImage (CFbsBitmap * aBitmap);

    /** SetImageName
	*
	*	Setter for image file name
	*
	*	@param aFileName - new file name
	*	@return -
	*/
	IMPORT_C void SetImageName (const TDesC & aFileName);

	/** SetScreenSizeL
	*
	*	Set current screen size to the engine
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C void SetScreenSizeL ();

	/** ResetEngine
	*
	*	Resets MIA engine.
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C void ResetEngineL();

	/** PurgeUndoRedoHistory
	*
	*	Purges undo / redo history.
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C void PurgeUndoRedoHistory();

    /** CanUndo
	*
	*	Checks, if undoable changes are made.
	*
	*	@param -
	*	@return - ETrue if we can undo, EFalse otherwise
	*/
	IMPORT_C TBool CanUndo();

    /** IsImageChanged
	*
	*	Checks, if image is changed.
	*
	*	@param -
	*	@return - ETrue if image changed, EFalse otherwise
	*/
	IMPORT_C TBool IsImageChanged();

    /** IsLandscapeEnabledL
	*
	*	Checks, if plugin is landscape enabled.
	*
	*	@param -
	*	@return - ETrue if landscape enabled, EFalse otherwise
	*/
    IMPORT_C TBool IsLandscapeEnabledL() const;


    /** ZoomL
	*
	*	Zooms image.
	*
	*	@param aZoom - zoom mode to be set
	*	@return - 
	*/
	IMPORT_C void ZoomL (const TZoom aZoom);

    /** GetZoomMode
     * Returns current zoom mode.
     *
     * @since S60 v5.0
     * @param -
     * @return TZoomMode current zooming mode
     */
    IMPORT_C TZoomMode GetZoomMode();
    
    /** PanL
	*
	*   Pans image.
	*
	*	@param aDir - direction 
	*	@return -
    *   @see commondefs.h
	*/
	IMPORT_C void PanL (const TDirection aDir);
    
    /** Pan
	*
	*   Pans image.
	*
	*	@param aXChange x-directional change on the screen
	*	@param aYChange y-directional change on the screen
	*	@return -
	*/
	IMPORT_C void PanL( TInt aXChange, TInt aYChange );


    /** RotateL
	*
	*	Rotates image.
	*
	*	@param aRot - rotate direction 
	*	@return -
    *   @see commondefs.h
	*/
	IMPORT_C void RotateL (const TRotation aRot);

	/** AddFilterToEngineL
	*
	*	Adds filter to MIA engine. Sets undo/redo point.
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C void AddFilterToEngineL();

	/** GetSystemParameters
	*
	*	Gets system parameters.
	*
	*	@param -
	*	@return const CSystemParameters &
	*/
	IMPORT_C const CSystemParameters & GetSystemParameters() const;

    /** IsPluginLoaded
	*
	*	Check if a plug-in is loaded
    *
    *	@param -
	*	@return - ETrue if plug-in is loaded, EFalse otherwise
    */
	IMPORT_C TBool IsPluginLoaded() const;

    /** RestoreFilterL
	*
	*	Restores current filter to main engine.
    *
    *	@param -
	*	@return - 
    */
	IMPORT_C void RestoreFilterL() const;

//@}

protected:

/** @name Methods:*/
//@{

//@}

/** @name Members:*/
//@{

//@}

private:

/** @name Methods:*/
//@{

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CImageEditorPluginManager ();

	/** Second phase constructor, may leave
	*
	*	@param -
	*	@return -
	*/
	void ConstructL ();

	/** SetParameters
	*
	*	Sets engine parameters depending on the filter type
	*
	*	@param -
	*	@return -
	*/
	void SetParametersL();

	/** RenderL
	*
	*	Renders image.
	*
	*	@param aMultiSessionBlockCount - Has two purposes:
	*			1) If not null, marks a request to render 
	*			   calling RenderBlock several times
	*			2) On return, contains the number of blocks to save
	*	@return -
	*/
	void RenderL(TInt* aMultiSessionBlockCount = NULL);

	/** Cleanup
	*
	*	Deletes current plug-in, editor image, engine and cleans up wrapper
	*	parameters.
	*
	*	@param -
	*	@return -
	*/
	void Cleanup();

    /** Copy constructor, disabled
	*/
	CImageEditorPluginManager (const CImageEditorPluginManager & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorPluginManager & operator= (const CImageEditorPluginManager & rhs);

//@}

/** @name Members:*/
//@{
	/// Plug-in loader
	CPluginLoader *		iPluginLoader;
	/// Edited image as CEditorImage
	CEditorImage *		iImage;
    /// MIA engine wrapper
    CEngineWrapper *	iEngineWrapper;
    
    #ifdef RD_CONTENTNOTIFICATION
    // Notification for Media Gallery
    CContentNotification * iEventNotifier;
    #endif
    
    /// Loaded filter type
    TInt                iFilterType;
    /// Loaded UI type
    TInt                iUiType;
    /// Is global zoom disabled
    TBool               iGlobalZoomDisabled;
    /// Closing
    TBool               iOnTheWayToDestruction;
    /// Plug-in scope
    TInt                iPluginScope;
//@}

};


#endif
