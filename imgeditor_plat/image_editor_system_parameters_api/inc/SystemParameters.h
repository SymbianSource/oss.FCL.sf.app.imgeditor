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
* System parameters in plug-in framework.
*
*/


#ifndef SYSTEMPARAMETERS_H
#define SYSTEMPARAMETERS_H


#include <e32base.h>

/**
 *  CSystemParameters represents system parameters in plug-in framework
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib SystemParameters.lib
 *  @since S60 v5.0
 */
class CSystemParameters :	public CBase
{

public:

    /**
    * Destructor.
    */
    IMPORT_C ~CSystemParameters();


    /**
     * Getter for source image size, non-const
     *
     * @since S60 v5.0
     * @param -
     * @return Source image size
     */
     IMPORT_C TSize & SourceSize();

    /**
     * Getter for source image size, const
     *
     * @since S60 v5.0
     * @param -
     * @return Source image size
     */
     IMPORT_C const TSize & SourceSize() const;


    /**
     * Getter for view port rect, non-const
     *
     * @since S60 v5.0
     * @param -
     * @return View port rect
     */
     IMPORT_C TRect & ViewPortRect();

    /**
     * Getter for view port rect, const
     *
     * @since S60 v5.0
     * @param -
     * @return View port rect
     */
     IMPORT_C const TRect & ViewPortRect() const;

    /**
     * Getter for visible image rect, non-const
     *
     * @since S60 v5.0
     * @param -
     * @return Visible image rect
     */
     IMPORT_C TRect & VisibleImageRect();

    /**
     * Getter for visible image rect, const
     *
     * @since S60 v5.0
     * @param -
     * @return Visible image rect
     */
     IMPORT_C const TRect & VisibleImageRect() const;

    /**
     * Getter for visible image rect in preview, non-const
     *
     * @since S60 v5.0
     * @param -
     * @return Visible image rect in preview
     */
     IMPORT_C TRect & VisibleImageRectPrev();

    /**
     * Getter for visible image rect in preview, const
     *
     * @since S60 v5.0
     * @param -
     * @return Visible image rect in preview
     */
     IMPORT_C const TRect & VisibleImageRectPrev() const;

    /**
     * Getter for output scale, non-const
     *
     * @since S60 v5.0
     * @param -
     * @return Output scale
     */
     IMPORT_C TReal & Scale();

    /**
     * Getter for output scale, const
     *
     * @since S60 v5.0
     * @param -
     * @return Output scale
     */
     IMPORT_C const TReal & Scale() const;

    /**
     * Getter for relative scale, non-const
     *
     * @since S60 v5.0
     * @param -
     * @return Relative scale
     */
     IMPORT_C TReal & RelScale();

    /**
     * Getter for relative scale, const
     *
     * @since S60 v5.0
     * @param -
     * @return Relative scale
     */
     IMPORT_C const TReal & RelScale() const;

protected:

private: // data

    /**
     * Source image size
     */
    TSize	iSourceSize;

    /**
     * View port rectangle 
     */
    TRect	iViewPortRect;
	
    /**
     * Visible image rect (from view port)
     */	
    TRect	iVisibleImageRect;
	
    /**
     * Visible image rect (from view port) in preview image
     */
    TRect	iVisibleImageRectPrev;
	
    /**
     * Scale
     */	
	  TReal	iScale;
	
    /**
     * Scale relative to the source image
     */	
    TReal	iRelScale;

};


#endif // SYSTEMPARAMETERS_H


