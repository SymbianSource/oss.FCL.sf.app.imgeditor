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


#include "CFilterRedEye.h"


const TInt KHueRedLimitLow	= 15;
const TInt KHueRedLimitHigh = 245;
const TInt KSatLimitLow		= 80; 

//=============================================================================
EXPORT_C TInt CFilterRedEye::Create()
	{
	CFilterRedEye * ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}

//=============================================================================
CFilterRedEye* CFilterRedEye::NewL()
	{
	CFilterRedEye* self = new( ELeave )CFilterRedEye();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

//=============================================================================
CFilterRedEye::~CFilterRedEye()
	{
	}


//=============================================================================
CFilterRedEye::CFilterRedEye()
	{

	}

//=============================================================================
void CFilterRedEye::ConstructL()
	{

	}

//=============================================================================
TRect CFilterRedEye::Rect()
	{
	return iChild->Rect();
	}

//=============================================================================
TReal CFilterRedEye::Scale()
	{
	return iChild->Scale();
	}

//=============================================================================
TSize CFilterRedEye::ViewPortSize()
{
    return iChild->ViewPortSize();
}

//=============================================================================
TBlock * CFilterRedEye::GetBlockL ( const TRect & aRect )
{
    TBlock * pB = iChild->GetBlockL(aRect);
    if (!pB) return NULL;
    if ( iRedEyeRectSc.Intersects (aRect) )
    {
        TUint32 * pD = pB->iData;
        TInt column = 0;
        TInt rows = 0;
        for (TInt i = 0; i < pB->iDataLength; ++i, ++pD)
        {        
		    TUint8 hue = Hue (*pD);
		    TUint8 sat = Saturation (*pD);
		    if ( ((hue < KHueRedLimitLow) || (hue > KHueRedLimitHigh)) && (sat > KSatLimitLow) )
		    {
				// Check if the pixel is inside the intersection of 
				// the requested block and the red eye rect
		        if (aRect.iTl.iX + column >= iRedEyeRectSc.iTl.iX &&
		            aRect.iTl.iY + rows >= iRedEyeRectSc.iTl.iY &&
                    aRect.iTl.iX + column < iRedEyeRectSc.iBr.iX &&
                    aRect.iTl.iY + rows < iRedEyeRectSc.iBr.iY)
		        {
					// Check if the pixel is inside the cirle that fits 
					// into the red eye rect (assuming it is square).
					TInt r = static_cast<TInt> ( (TReal)(iRedEyeRectSc.iBr.iX - iRedEyeRectSc.iTl.iX) / 2 + 0.5);
					TInt a = (iRedEyeRectSc.iTl.iX + r) - (aRect.iTl.iX + column);
					TInt b = (iRedEyeRectSc.iTl.iY + r) - (aRect.iTl.iY + rows);
					if ( (a*a + b*b) <= r*r )
					{
						TUint32 c = *pD;
						TUint8 cb = (TUint8)(c & 0xFF);
						c >>= 8;
						TUint8 cg = (TUint8)(c & 0xFF);
						c >>= 8;
						TUint8 cr = (TUint8)(c & 0xFF);
						TUint8 y = (TUint8)((306 * cr + 587 * cg + 117 * cb) >> 10);
						*pD = (y | (y << 8) | (y << 16));
					}
		        }
		    }

		    ++column;
		    if (column >= pB->iWidth)
		    {
		        column = 0;
		        ++rows;
		    }
        }
    }
    return pB;
}


//=============================================================================
void CFilterRedEye::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}

//=============================================================================
void CFilterRedEye::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}

//=============================================================================
TInt CFilterRedEye::CmdL( const TDesC16& aCmd )
	{

	TLex lex (aCmd);

	while ( !lex.Eos() )
	{
		TPtrC token = lex.NextToken();
		if( token.Compare( _L("ulc") ) == 0 )
			{
			TReal relscale = iChild->Scale();
			TInt param = 0;
			lex.Inc();
			lex.Val( param );
			iRedEyeRect.iTl.iX = (TInt)((param / relscale) + 0.5);
			}
		else if( token.Compare( _L("ulr") ) == 0 )
			{
			TReal relscale = iChild->Scale();
			TInt param = 0;
			lex.Inc();
			lex.Val( param );
			iRedEyeRect.iTl.iY = (TInt)((param / relscale) + 0.5);
			}
		else if( token.Compare( _L("lrc") ) == 0 )
			{
			TReal relscale = iChild->Scale();
			TInt param = 0;
			lex.Inc();
			lex.Val( param );
			iRedEyeRect.iBr.iX = (TInt)((param / relscale) + 0.5);
			}
		else if( token.Compare( _L("lrr") ) == 0 )
			{
			TReal relscale = iChild->Scale();
			TInt param = 0;
			lex.Inc();
			lex.Val( param );
			iRedEyeRect.iBr.iY = (TInt)((param / relscale) + 0.5);
			}
		}

    TReal relscale = iChild->Scale();
    iRedEyeRectSc.iTl.iX = (TInt)(iRedEyeRect.iTl.iX * relscale + 0.5);
    iRedEyeRectSc.iTl.iY = (TInt)(iRedEyeRect.iTl.iY * relscale + 0.5);
    iRedEyeRectSc.iBr.iX = (TInt)(iRedEyeRect.iBr.iX * relscale + 0.5);
    iRedEyeRectSc.iBr.iY = (TInt)(iRedEyeRect.iBr.iY * relscale + 0.5);

    return 0;

	}

//=============================================================================
const char* CFilterRedEye::Type()
	{
	return "negate";
	}
	
//=============================================================================
TUint8 CFilterRedEye::Hue ( TUint32 aPixel ) const
{

	TUint8 g = (TUint8)(aPixel & 0xFF); 
	aPixel >>= 8;
	TUint8 b = (TUint8)(aPixel & 0xFF); 
	aPixel >>= 8;
	TUint8 r = (TUint8)(aPixel & 0xFF);

	const TInt t1 = (1 << 10);
	const TInt t3 = (3 << 10);
	const TInt t5 = (5 << 10);
	const TInt t6 = (6 << 10);
	
	TInt h = 0;
	TInt v = ( r > g ) ? ( (r > b) ? (r) : (b) ) : ( (g > b) ? (g) : (b) );
	TInt m = ( r < g ) ? ( (r < b) ? (r) : (b) ) : ( (g < b) ? (g) : (b) );

	if (v != m) 
	{

		TInt vm = v - m;

		if (r == v) 
		{
			TInt g2 = ((v - g) << 10) / vm;
			TInt b2 = ((v - b) << 10) / vm;
			h = (g == m) ? (t5 + b2) : (t1 - g2);
		}

		else if (g == v) 
		{
			TInt r2 = ((v - r) << 10) / vm;
			TInt b2 = ((v - b) << 10) / vm;
			h = (b == m) ? (t1 + r2) : (t3 - b2);
		}

		else 
		{
			TInt r2 = ((v - r) << 10) / vm;
			TInt g2 = ((v - g) << 10) / vm;
			h = (r == m) ? (t3 + g2) : (t5 - r2);
		}

		h *= 255;
		h /= t6;
	}

	return (TUint8)h;
}

//=============================================================================
TUint8 CFilterRedEye::Saturation ( TUint32 aPixel ) const
{
	TUint8 g = (TUint8)(aPixel & 0xFF); 
	aPixel >>= 8;
	TUint8 b = (TUint8)(aPixel & 0xFF); 
	aPixel >>= 8;
	TUint8 r = (TUint8)(aPixel & 0xFF);

	TInt v = ( r > g ) ? ( (r > b) ? (r) : (b) ) : ( (g > b) ? (g) : (b) );
	TInt m = ( r < g ) ? ( (r < b) ? (r) : (b) ) : ( (g < b) ? (g) : (b) );
	TInt l = ((m + v) >> 1);
	TInt s = 0;

	if (l > 0) 
	{
		s = v - m;

		if (s > 0) {

			s *= 255;

			if (l <= 127)
			{
				s /= (v + m);
			}
			else 
			{
				s /= (512 - v - m);
			}
		}
	}
	else {
		s = 0;
	}	
	return (TUint8)s;
}

//=============================================================================
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
