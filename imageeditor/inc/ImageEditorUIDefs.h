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



#ifndef IMAGEEDITORUIDEFS_H
#define IMAGEEDITORUIDEFS_H

/// CONSTANTS

/// The image editor UI icons
_LIT(KImageEditorUiMifFile, "\\resource\\apps\\ImageEditorUi.mif");

const TInt KStandardCrosshairWidth  = 15;
const TInt KStandardCrosshairHeight = 15;
const TInt KQVGACrosshairWidth      = 20;
const TInt KQVGACrosshairHeight     = 20;
const TInt KDoubleCrosshairWidth    = 25;
const TInt KDoubleCrosshairHeight   = 25;
const TInt KHVGACrosshairWidth      = 24;
const TInt KHVGACrosshairHeight     = 24;
const TInt KVGACrosshairWidth       = 32;
const TInt KVGACrosshairHeight      = 32;

/// some common scan codes
const TInt  KScanCode_Edit              = EStdKeyRightShift;
const TInt  KScanCode_Clear             = EStdKeyBackspace;
const TInt  KScanCode_Apps              = EStdKeyApplication0;
const TInt  KScanCode_Soft_Left			= EStdKeyDevice0;
const TInt  KScanCode_Soft_Right		= EStdKeyDevice1;

/// Time that the tooltip is shown in the view
const TInt KTimeTooltipInView = 1500; // 1.5s

/// INLINE FUNCTIONS

/*	min_2 
*
*   Utility function for getting minimum of two numbers.
*/
inline TInt min_2 (TInt aX, TInt aY)
{
    return (aX < aY) ? (aX) : (aY);
}

/*	max_2 
*
*   Utility function for getting maximum of two numbers.
*/
inline TInt max_2 (TInt aX, TInt aY)
{
    return (aX > aY) ? (aX) : (aY);
}

#endif

// End of File
