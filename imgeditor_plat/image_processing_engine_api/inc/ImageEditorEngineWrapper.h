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
* CEngineWrapper wraps functionality of Gonzales image 
* processing engine architecture.
*
*/


#ifndef IMAGEEDITORENGINEWRAPPER_H
#define IMAGEEDITORENGINEWRAPPER_H

/// INCLUDES
#include <e32base.h>
#include "commondefs.h"


/**  Panic category */
_LIT (KEnginePanic, "ENGINE PANIC");

/**  Panic reasons */
const TInt KEnginePanicAllocation = -1;
const TInt KEnginePanicInternal = -2;
const TInt KEnginePanicParameter = -3;

/// FORWARD DECLARATIONS
class CFbsBitmap;
class CFilterStack;
class CSystemParameters;
class TFilter;
class CExifParser;


/**
 * CEngineWrapper wraps functionality of Gonzales image processing
 * engine architecture.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib EngineWrapper.lib
 *  @since S60 v5.0
 */
class CEngineWrapper : public CBase
{

public:


    /**
     * Two-phased constructor.
     * @param -
     */
     IMPORT_C static CEngineWrapper * NewL ();

    /**
     * Destructor.
     */
     IMPORT_C ~CEngineWrapper ();
   
    /**
     * Creates JPEG source to engine.
     *
     * @since S60 v5.0
     * @param aFileName source image file name
     * @return -
     */   
     IMPORT_C void CreateJpegSourceL  (const TDesC & aFileName);

    /**
     * Creates JPEG target for engine.
     *
     * @since S60 v5.0
     * @param aFileName saved image file name
     * @param aQuality saved image quality
     * @param aSize image size (width and height)
     * @return -
     */
     IMPORT_C void CreateJpegTargetL (
		                     const TDesC & 	aFileName,
                         const TInt     aQuality,
                         const TSize *   aSize = NULL
        );

    /**
     * Creates ICL source to engine.
     *
     * @since S60 v5.0
     * @param aFileName source image file name
     * @return /
     */
     IMPORT_C void CreateIclSourceL (const TDesC & aFileName);
    
    /**
     * Creates RGB888 buffer target to engine.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */    
     IMPORT_C void CreateRGB888TargetL ();

    /**
     * Sets screen size to engine.
     *
     * @since S60 v5.0
     * @param aSize screen size
     * @return -
     */
     IMPORT_C void SetScreenSizeL (const TSize & aSize);

    /**
     * Adds new filter to the engine.
     *
     * @since S60 v5.0
     * @param aFilterName filter plug-in name
     * @return -
     */
     IMPORT_C void AddFilterL (const TDesC & aFilterName);	

    /**
     * Sets filter parameters for the last (top) filter.
     *
     * @since S60 v5.0
     * @param aParam reference to parameter structure
     * @return -
     */
     IMPORT_C void SetParamsL (const TDesC & aParam);

    /**
     * Renders the engine graph.
     *
     * @since S60 v5.0
     * @param aMultiSessionBlockCount not used
     * @return -
     */
     IMPORT_C void RenderL (TInt* aMultiSessionBlockCount = NULL);

    /**
     * Renders a block
     *
     * @since S60 v5.0
     * @return return percentage of image data saved, 100 when ready
     */
     IMPORT_C TInt RenderBlockL ();

    /**
     * Aborts block rendering
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */
    IMPORT_C void RenderAbortL ();

    /**
     * Initializes undo / redo stack.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */
     IMPORT_C void InitUndoRedo();

    /**
     * Sets a new undo / redo step.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */
     IMPORT_C void AddUndoRedoStepL();

    /**
     * Undoes the last operation.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */
     IMPORT_C void UndoL();

    /**
     * Checks if it is possible to undo.
     *
     * @since S60 v5.0
     * @param -
     * @return ETrue if undo possible, EFalse otherwise 
     */
     IMPORT_C TBool CanUndo ();

    /**
     * Checks if the image is changed after last saving.
     *
     * @since S60 v5.0
     * @param -
     * @return ETrue if image is changed, EFalse otherwise
     */
     IMPORT_C TBool IsImageChanged();

    /**
     * Stores current zoom value.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */
     IMPORT_C void StoreZoomL ();

    /**
     * Restores stored zoom value.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */
     IMPORT_C void RestoreZoomL();

    /** ZoomL
     * Zooms the image.
     *
     * @since S60 v5.0
     * @param aZoom zoom direction
     * @return -
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
     
    /**
     * Pans the image.
     *
     * @since S60 v5.0
     * @param aDir direction, see commondefs.h
     * @return -
     */
	IMPORT_C void PanL (const TDirection aDir);
    
        
    /**
     * Pans the image. Used when panning is done with touch
     *
     * @since S60 v5.0
     * @param aXChange x-directional change on the screen
     * @param aYChange y-directional change on the screen
     * @return -
     */
     IMPORT_C void PanL( TInt aXChange, TInt aYChange );
    

    /**
     * Rotates the image.
     *
     * @since S60 v5.0
     * @param aRot rotation direction, see commodefs.h
     * @return -
     */
     IMPORT_C void RotateL (const TRotation aRot);

    /**
     * Gets system parameters
     *
     * @since S60 v5.0
     * @param -
     * @return pointer to the system parameters
     */
     IMPORT_C CSystemParameters * GetSystemPars ();

    /**
     * No functionality.
     *
     * @since S60 v5.0
     * @param aWidth width of the output image in pixels
     * @param aHeight height of the output image in pixels
     * @return -
     */
     IMPORT_C void GetOutputImageSize ( TInt& aWidth, TInt& aHeight ) const;

    /**
     * No functionality.
     *
     * @since S60 v5.0
     * @param aComment comment to be set
     * @return -
     */
     IMPORT_C void SetJpegCommentL (const TDesC8& aComment);

    /**
     * Sets the bitmap to member variable.
     *
     * @since S60 v5.0
     * @param aBitmap bitmap to be set
     * @return -
     */
     IMPORT_C void SetBitmap (CFbsBitmap * aBitmap);
    
    /**
     * Creates new EXIF thumbnail from the current filter stack.
     *
     * @since S60 v5.0
     * @param -
     * @param -
     * @return -
     */    
     IMPORT_C void CreateExifThumbNailL ();
    
protected:

private:

     CEngineWrapper ();
     
     void ConstructL ();

    /**
     * Checks if a file exists or not, on return has the correct
     * path to the checked file.
     *
     * @since S60 v5.0
     * @param aFileName file name, on return includes the correct path
     * @return KErrNone if file exists, system wide error code otherwise
     */
     TInt FileExists (TDes & aFileName) const;

    /**
     * Cleans up the engine, frees allocated memory. Does not delete
     * the engine itself.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */
     void Cleanup();

    /**
     * Computes current system parameters.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */
     void ComputeSystemParameters ();

    /**
     * No functionality.
     *
     * @since S60 v5.0
     * @param aStartInd view port start index, defaults to 2
     * @return the current view port
     */
     TRect ComputeViewPort( const TInt aStartInd = 2 );

    /**
     * No functionality.
     *
     * @since S60 v5.0
     * @param aViewPort the current view port
     * @return the current visible view port area
     */
     TRect ComputeVisibleViewPort (const TRect & aViewPort);

    /**
     * No functionality
     *
     * @since S60 v5.0
     * @param aVvpOld the old visible view port area
     * @return new pan value
     */
     TPoint ComputeNewPanValue (const TRect & aVvpOld);

    /**
     * Adds rotation filter to the engine.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */
     void AddRotateFilterL();

    /**
     * Copies rendered buffer from engine to screen bitmap.
     *
     * @since S60 v5.0
     * @param aBuffer pointer to engine buffer data
     * @return -
     */
     void CopyBufferL (TUint32 * aBuffer);
	
    /**
     * Checks if two strings are equal.
     *
     * @since S60 v5.0
     * @param aString1 first string to be compared
     * @param aString2 second string to be compared
     * @return ETrue if equal, EFalse otherwise
     */
     TBool IsSameString (const TUint8 * aString1, const TUint8 * aString2);
                             
    /**
     * Updates scale and pan to crop rectangle.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */ 
	void UpdateCropRectL ();
	
    /**
     * Updates thumbnail data to EXIF parser.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */	
     TPtrC8 UpdateExifThumbnailL ();

    /**
     * Handles tags that are always updated to the new image. If tags
     * do not exist, creates new ones.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */
     void UpdateExifTagsL();
	
    /**
     * Gets current date and time as descriptor.
     *
     * @since S60 v5.0
     * @param aDateTimeBuf contains datetime on return
     * @return -
     */	
     void GetCurrentDateTime (TDes8 & aDateTimeBuf) const;

    /**
     * Computes current total rotation of the original image
     * in degrees.
     *
     * @since S60 v5.0
     * @param -
     * @return rotation in degrees
     */
     TInt ComputeRotationL ();
	
	    /**
     * Computes bounding rectangle used from global cropping.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */
     void ComputeBoundingRectL();
	
    /**
     * Rendering mode
     */	
     enum TRenderingMode
     {
       // Invalid
       ERenderingModeMin = 0,
       // Render to jpeg
       ERenderingModeJpeg,
       // Render RGB888 buffer
       ERenderingModeRGB888,
       // Invalid
       ERenderingModeMax
      } iMode;

    /**
     * Main engine
     */
     CFilterStack * iMainEngine;

    /**
     * Engine command line
     */
     TBuf<256> iCmd;

    /**
     * Source image size
     */
     TSize iSourceSize;

    /**
     * Screen size
     */
     TSize iScreenSize;
	
    /**
     * Screen data address
     */	
     CFbsBitmap * iScreenBitmap;					

    /**
     * Target image name
     */
     TFileName iTargetFile;
	
    /**
     * System parameters
     */	
     CSystemParameters * iSysPars;

    /**
     * Undo points
     */
     RArray<TInt> iUndoPoints;
   
    /**
     * Undo buffer for scale parameter
     */   
     RArray<TReal> iScaleUndoBuf;
    
    /**
     * EXIF parser
     */
     CExifParser * iExifParser;
	
    /**
     * EXIF thumbnail buffer
     */	
     TUint32 * iThumb;
	
    /**
     * EXIF thumbnail dimensions
     */	
     TSize iThumbSize;

    /**
     * JPEG comment text
     */    
     HBufC8* iJpegComment;

    /**
     * Change counter, 0 if no changes after save
     */
     TInt iChangeCount;

    /**
     * Previous change counter
     */
     TInt iPrevChangeCount;
	
    /**
     * Scale buffer changed
     */	
     TBool iRenderScaleBuffer;

    /**
     * Crop parameters
     */	
     TReal iScale;
     TReal iMinScale;
     TReal iMaxScale;
     TReal iPanX;
     TReal iPanY;
     TReal iPanStep;
     TRect iBoundingRect;
	
    /**
     * Stored crop parameters
     */
     TReal iScaleSt;
     TReal iPanXSt;
     TReal iPanYSt;
     TRect iOldCropRect;

    /** 
     * Current zooming mode
     */
     TZoomMode iZoomMode;
	
};

#endif // IMAGEEDITORENGINEWRAPPER_H

