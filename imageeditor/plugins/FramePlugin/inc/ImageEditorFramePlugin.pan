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



#ifndef __IMAGEEDITORFRAMEPLUGIN_PAN__
#define __IMAGEEDITORFRAMEPLUGIN_PAN__

#include <e32def.h>

void Panic(TInt aCategory)
{
    _LIT(KComponentName, "ImageEditorFramePlugin");
    User::Panic(KComponentName, aCategory);
}

enum TFramePluginPanicIds
{
    EFrameBitmapIndexOutOfBounds = 0,
    EFrameBitmapNotReady,
    EFrameAlphaBitmapNotReady,
    EFrameUnsupportedDisplayMode
};

#endif __IMAGEEDITORFRAMEPLUGIN_PAN__

// End of File
