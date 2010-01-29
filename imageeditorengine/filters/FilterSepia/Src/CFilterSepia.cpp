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


#include "CFilterSepia.h"

//  Sepia in RGB
const TInt rs = 112;
const TInt gs = 66;
const TInt bs = 20;

//  Helper variables
const TInt kr =   45808 * rs - 38446 * gs - 7362 * bs + 32768;
const TInt kg = - 19496 * rs + 26952 * gs - 3750 * bs + 32768;
const TInt kb = - 19608 * rs - 38184 * gs + 57792* bs + 32768;

EXPORT_C TInt CFilterSepia::Create()
	{
	CFilterSepia* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterSepia* CFilterSepia::NewL()
	{
	CFilterSepia* self = new( ELeave )CFilterSepia();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterSepia::~CFilterSepia()
	{
	}



CFilterSepia::CFilterSepia()
	{

	}



void CFilterSepia::ConstructL()
	{

	}



TRect CFilterSepia::Rect()
	{
	return iChild->Rect();
	}

TReal CFilterSepia::Scale()
	{
	return iChild->Scale();
	}

TSize CFilterSepia::ViewPortSize()
{
    return iChild->ViewPortSize();
}


TBlock * CFilterSepia::GetBlockL ( const TRect & aRect )
{
    TBlock * pB = iChild->GetBlockL (aRect);
    if (!pB) return NULL;
    TUint32 * pD = pB->iData;

    for (TInt i = 0; i < pB->iDataLength; ++i)
    {
        TUint32 c = *pD;

        //	Get RGB values
	    TInt b = c & 0xFF;
	    c >>= 8;
	    TInt g = c & 0xFF;
	    c >>= 8;
	    TInt r = c & 0xFF;


        TInt alpha = 19668 * r + 38442 * g + 7450 * b; 
        r = (alpha + kr) >> 16;
        if (r < 0) 
        {
            r = 0;
        }
        else if (r > 255) 
        {
            r = 255;
        }
        g = (alpha + kg) >> 16;
        if (g < 0) 
        {
            g = 0;
        }
        else if (g > 255) 
        {
            g = 255;
        }
        b = (alpha + kb) >> 16;
        if (b < 0) 
        {
            b = 0;
        }
        else if (b > 255) 
        {
            b = 255;
        }

        *pD++ = b | (g << 8) | (r << 16);
    }

    return pB;
}


void CFilterSepia::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}




void CFilterSepia::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterSepia::CmdL( const TDesC16& /*aCmd*/ )
	{
	return 0;
	}



const char* CFilterSepia::Type()
	{
	return "sepia";
	}
	
	
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
