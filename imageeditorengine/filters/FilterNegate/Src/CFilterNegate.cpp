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


#include "CFilterNegate.h"



EXPORT_C TInt CFilterNegate::Create()
	{
	CFilterNegate* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterNegate* CFilterNegate::NewL()
	{
	CFilterNegate* self = new( ELeave )CFilterNegate();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterNegate::~CFilterNegate()
	{
	}



CFilterNegate::CFilterNegate()
	{

	}



void CFilterNegate::ConstructL()
	{

	}



TRect CFilterNegate::Rect()
	{
	return iChild->Rect();
	}

TReal CFilterNegate::Scale()
	{
	return iChild->Scale();
	}

TSize CFilterNegate::ViewPortSize()
{
    return iChild->ViewPortSize();
}


TBlock * CFilterNegate::GetBlockL ( const TRect & aRect )
{
    TBlock * pB = iChild->GetBlockL (aRect);
    if (!pB) return NULL;
    for (TInt i = 0; i < pB->iDataLength; ++i)
    {
        pB->iData[i] ^= 0xFFFFFF;
    }
    return pB;
}


void CFilterNegate::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}




void CFilterNegate::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterNegate::CmdL( const TDesC16& /*aCmd*/ )
	{
	return 0;
	}



const char* CFilterNegate::Type()
	{
	return "negate";
	}
	
	
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
