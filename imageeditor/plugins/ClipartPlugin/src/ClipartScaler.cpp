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



// INCLUDES
#include <BitmapTransforms.h>

#include "ClipartScaler.h"
#include "ImageEditorError.h"


//=============================================================================
CClipartScaler::CClipartScaler (MClipartScalerNotifier* aNotifier)
: CActive(0), iNotifier(aNotifier)
    {
    }

//=============================================================================
CClipartScaler::~CClipartScaler ()
    {
    Cancel();
    delete iBitmapScaler;
    iBitmapScaler = 0;    
    }

//=============================================================================
void CClipartScaler::ConstructL ()
    {
    CActiveScheduler::Add(this);
    iBitmapScaler = CBitmapScaler::NewL();
    }

//=============================================================================
void CClipartScaler::ScaleBitmapL(CFbsBitmap* aBitmap, TSize aNewSize)
    {
    iBitmapScaler->Scale(&iStatus, *aBitmap, aNewSize);
    SetActive();
    }

//=============================================================================
void CClipartScaler::DoCancel()
    {
    if(IsActive())
        {
        iBitmapScaler->Cancel();
        }
    }
 
//=============================================================================
void CClipartScaler::RunL()
    {
    if (iStatus == KErrNone)
        {
        iNotifier->ClipartScalerOperationReadyL(KErrNone); 
        }
    else
        {
        iNotifier->ClipartScalerOperationReadyL(KSIEEInternal);        
        }
    }

// End of File
