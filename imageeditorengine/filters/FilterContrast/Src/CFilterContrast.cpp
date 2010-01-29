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


#include <e32math.h>
#include "CFilterContrast.h"

EXPORT_C TInt CFilterContrast::Create()
	{
	CFilterContrast* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterContrast* CFilterContrast::NewL()
	{
	CFilterContrast* self = new( ELeave )CFilterContrast();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterContrast::~CFilterContrast()
	{
	}



CFilterContrast::CFilterContrast()
	{

	}



void CFilterContrast::ConstructL()
	{
	ComputeContrastStretchL();
	}



TRect CFilterContrast::Rect()
	{
	return iChild->Rect();
	}

TReal CFilterContrast::Scale()
	{
	return iChild->Scale();
	}

TSize CFilterContrast::ViewPortSize()
{
    return iChild->ViewPortSize();
}


TBlock * CFilterContrast::GetBlockL ( const TRect & aRect )
{
    TBlock * pB = iChild->GetBlockL (aRect);
    if (!pB) return NULL;
    TUint32 * pD = pB->iData;
    for (TInt i = 0; i < pB->iDataLength; ++i)
    {
        TUint32 c = *pD;
        TUint8 b = iLUT[c & 0xFF];
	    c >>= 8;
	    TUint8 g = iLUT[c & 0xFF];
        c >>= 8;
	    TUint8 r = iLUT[c & 0xFF];
        *pD++ = 0 | b | (g << 8) | (r << 16);
    }
    return pB;
}



void CFilterContrast::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}




void CFilterContrast::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterContrast::CmdL( const TDesC16& aCmd )
{

	TLex lex (aCmd);

	while ( ! lex.Eos() )
    {
		TPtrC token = lex.NextToken();

        if ( token.Compare( _L("contrast") ) == 0)
        {

		    lex.Inc();
		    lex.Val (iContrast);

	        //	Check parameter
	        if ( iContrast < -100)
	        {
		        iContrast = -100;
	        }
	        else if ( iContrast > 100)
	        {
		        iContrast = 100;
	        }
            ComputeContrastStretchL();
		}
    }
    return 0;
}

const char* CFilterContrast::Type()
	{
	return "contrast";
	}

void CFilterContrast::ComputeContrastStretchL()
	{
        //  Compute luminance histogram, if not computed
	    if ( iContrast >= 0 )
	    {
		    TInt val_low = (iContrast * 127) / 100;
		    TInt val_high = 255 - val_low;
		    TInt range = val_high - val_low + 1;

		    TInt i = 0;
		    for ( ; i <= val_low; i++)
		    {
			    iLUT[i] = 0;
		    }
		    for ( ; i < val_high; ++i)
		    {
			    iLUT[i] = (TUint8)(((i - val_low) * 255) / range);
		    }
		    for ( ; i <= 255; i++)
		    {
			    iLUT[i] = 255;
		    }
	    }
	    else
	    {

		    TInt val_low = (-iContrast * 127) / 100;
		    TInt val_high = 255 - val_low;
		    TInt range = val_high - val_low + 1;

		    for ( TInt i = 0; i <= 255; i++ )
		    {
			    iLUT[i] = (TUint8)(val_low + (i * range) / 255);
		    }
	    }
	}

#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
