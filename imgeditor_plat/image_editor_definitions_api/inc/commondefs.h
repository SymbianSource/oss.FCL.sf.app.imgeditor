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
* Common definitions.
*
*/


#ifndef COMMONDEFS_H
#define COMMONDEFS_H

    /**  Directions */
    enum TDirection
    {
      EDirectionMin   = 0,
      EDirectionUp,
      EDirectionDown,
      EDirectionLeft,
      EDirectionRight,
      EDirectionMax
    };

    /**  Rotation directions */
    enum TRotation
    {
      ERotationCounterClockwise  = 0,
      ERotationClockwise         = 1,
      ERotation90                = ERotationCounterClockwise,
      ERotation180               = 2,
      ERotation270               = ERotationClockwise
    };

    /**  Zoom directions */
    enum TZoom
        {
        EZoomMin = 0,
        EZoomIn,
        EZoomOut
        };

    /** Zoom Modes */      
    enum TZoomMode
        {
        EZoomNormal = 0,
        EZoomIn1,
        EZoomIn2,
        EZoomIn3, 
        ENumOfZooms // has to be the last one
        };
        
/**  File name extensions for known used types */
_LIT (KJpegExtension, ".jpg");
_LIT (KBmpExtension, ".bmp");

/**  Temporary file directory */
_LIT (KTempPath, "\\private\\101ffa91\\temp\\");

/**  Temporary save name for an image */
_LIT (KTempSaveFile, "_tmpfile.sav");

/**  Temporary save name for sending an image */
_LIT (KTempSentFile, "tmp_send.jpg");

/**  Default quality factor for saved images */
const TInt KDefaultSavedJpegQuality = 90;

/**  Property category Uid  */
const TUid KImageEditorProperties = {0x02341234};

/** Image editor property keys  */
enum TImageEditorPropertyKeys
{
    EPropertyFilename   // Document file name
}; 


#endif // COMMONDEFS_H
