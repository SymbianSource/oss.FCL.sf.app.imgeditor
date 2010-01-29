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
#include "CFilterBrightness.h"

const TInt KBrightnessScale = 80;

EXPORT_C TInt CFilterBrightness::Create()
	{
	CFilterBrightness* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterBrightness* CFilterBrightness::NewL()
	{
	CFilterBrightness* self = new( ELeave )CFilterBrightness();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterBrightness::~CFilterBrightness()
	{
	}



CFilterBrightness::CFilterBrightness()
	{

	}



void CFilterBrightness::ConstructL()
	{

	}



TRect CFilterBrightness::Rect()
	{
	return iChild->Rect();
	}

TReal CFilterBrightness::Scale()
	{
    return iChild->Scale();
	}

TSize CFilterBrightness::ViewPortSize()
{
    return iChild->ViewPortSize();
}

TBlock * CFilterBrightness::GetBlockL ( const TRect & aRect )
{
    TBlock * pB = iChild->GetBlockL (aRect);
    if (!pB) return NULL;
    TUint32 * pD = pB->iData;
    for (TInt i = 0; i < pB->iDataLength; ++i)
    {
        TUint32 c = *pD;

        TInt r = c & 0xFF0000;
        r += iAdjR;
        if (r > 0xFF0000)
        {
            r = 0xFF0000;
        }
        else if (r < 0)
        {
            r = 0;
        }

        TInt g = c & 0xFF00;
        g += iAdjG;
        if (g > 0xFF00)
        {
            g = 0xFF00;
        }
        else if (g < 0)
        {
            g = 0;
        }

        TInt b = c & 0xFF;
        b += iAdjB;
        if (b > 0xFF)
        {
            b = 0xFF;
        }
        if (b < 0)
        {
            b = 0;
        }
        *pD++ = 0 | r | g | b;
    }
    return pB;
}



void CFilterBrightness::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}




void CFilterBrightness::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterBrightness::CmdL( const TDesC16& aCmd )
{

	TLex lex (aCmd);

    //  Get brightness parameter [-100,100]
	while ( ! lex.Eos() )
    {
		TPtrC token = lex.NextToken();

        if ( token.Compare( _L("brightness") ) == 0)
        {
            TInt bpar = 0;
			lex.Inc();
			lex.Val (bpar);
	        if (bpar < - 100)
	        {
		        bpar = -100;
	        }
	        else if (bpar > 100)
	        {
		        bpar = 100;
	        }

            iBrightness = ((bpar * KBrightnessScale) / 100);
            iAdjR = iBrightness << 16;
            iAdjG = iBrightness << 8;
            iAdjB = iBrightness;
		}
    }
    return 0;
}

const char* CFilterBrightness::Type()
	{
	return "brightness";
	}
	
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	

#endif
