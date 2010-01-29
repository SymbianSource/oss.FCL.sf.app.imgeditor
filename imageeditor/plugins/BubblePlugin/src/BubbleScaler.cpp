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

#include "BubbleScaler.h"
#include "ImageEditorError.h"


//=============================================================================
CBubbleScaler::CBubbleScaler (MBubbleScalerNotifier* aNotifier)
: CActive(0), iNotifier(aNotifier)
    {
    }

//=============================================================================
CBubbleScaler::~CBubbleScaler ()
    {
    Cancel();
    delete iBitmapScaler;
    iBitmapScaler = 0;    
    }

//=============================================================================
void CBubbleScaler::ConstructL ()
    {
    CActiveScheduler::Add(this);
    iBitmapScaler = CBitmapScaler::NewL();
    }

//=============================================================================
void CBubbleScaler::ScaleBitmapL(CFbsBitmap* aBitmap, TSize aNewSize)
    {
    iBitmapScaler->Scale(&iStatus, *aBitmap, aNewSize);
    SetActive();
    }

//=============================================================================
void CBubbleScaler::DoCancel()
    {
    if(IsActive())
        {
        iBitmapScaler->Cancel();
        }
    }
 
//=============================================================================
void CBubbleScaler::RunL()
    {
    if (iStatus == KErrNone)
        {
        iNotifier->BubbleScalerOperationReadyL(KErrNone); 
        }
    else
        {
        iNotifier->BubbleScalerOperationReadyL(KSIEEInternal);        
        }
    }

// End of File
