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
* Horizontal slider panics.
*
*/



#ifndef __CUSTOMCONTROLPANICS_PAN__
#define __CUSTOMCONTROLPANICS_PAN__

enum THorizontalSliderPanic
    {
    EHorizontalSliderPanicMinMax = -0x100,
    EHorizontalSliderPanicBitmapsNotLoaded,
    EHorizontalSliderPanicStepNotPositive,
    EHorizontalSliderPanicIndexUnderflow,
    EHorizontalSliderPanicIndexOverflow,
    };

void Panic(TInt aCategory)
    {
    _LIT(KComponentName, "HSLIDER");
    User::Panic(KComponentName, aCategory);
    }

#endif __CUSTOMCONTROLPANICS_PAN__
