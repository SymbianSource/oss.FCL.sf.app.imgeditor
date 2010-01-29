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


#include "CFilterGrayScale.h"



EXPORT_C TInt CFilterGrayScale::Create()
	{
	CFilterGrayScale* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterGrayScale* CFilterGrayScale::NewL()
	{
	CFilterGrayScale* self = new( ELeave )CFilterGrayScale();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterGrayScale::~CFilterGrayScale()
	{
	}



CFilterGrayScale::CFilterGrayScale()
	{

	}



void CFilterGrayScale::ConstructL()
	{

	}



TRect CFilterGrayScale::Rect()
	{
	return iChild->Rect();
	}

TReal CFilterGrayScale::Scale()
	{
	return iChild->Scale();
	}

TSize CFilterGrayScale::ViewPortSize()
{
    return iChild->ViewPortSize();
}

TBlock * CFilterGrayScale::GetBlockL ( const TRect & aRect )
{
    TBlock * pB = iChild->GetBlockL (aRect);
    if (!pB) return NULL;
    TUint32 * pD = pB->iData;
    for (TInt i = 0; i < pB->iDataLength; ++i)
    {
        TUint32 c = *pD;
	    TUint32 b = c & 0xFF;
	    c >>= 8;
	    TUint32 g = c & 0xFF;
	    c >>= 8;
	    TUint32 r = c & 0xFF;
	    TUint32 y = (306 * r + 587 * g + 117 * b) >> 10;
        *pD++ = (y | (y << 8) | (y << 16));
    }
    return pB;
}


void CFilterGrayScale::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}




void CFilterGrayScale::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterGrayScale::CmdL( const TDesC16& /*aCmd*/ )
	{
    return 0;
	}



const char* CFilterGrayScale::Type()
	{
	return "grayscale";
	}
	
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
