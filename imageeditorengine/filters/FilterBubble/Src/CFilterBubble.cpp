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


#include "CFilterBubble.h"

#include <e32math.h>

const TInt KScaleBits = 12;
const TInt KCoordBits = 8;

EXPORT_C TInt CFilterBubble::Create()
	{
	CFilterBubble* ptr = NULL;
	TRAPD( error, ptr = NewL() );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterBubble* CFilterBubble::NewL()
	{
	CFilterBubble* self = new( ELeave )CFilterBubble();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterBubble::~CFilterBubble()
	{
	delete[] iData;
	}



CFilterBubble::CFilterBubble()
	{

	}



void CFilterBubble::ConstructL()
	{
	iScale.iX = 1 << 8;
	iScale.iY = 0;
	iZoom = 1000;
	iAngle = 0;
	}



TRect CFilterBubble::Rect()
	{
	return iChild->Rect();
	}

TReal CFilterBubble::Scale()
	{
	return iChild->Scale();
	}

TSize CFilterBubble::ViewPortSize()
{
    return iChild->ViewPortSize();
}

TBlock * CFilterBubble::GetBlockL ( const TRect & aRect )
{
    TBlock * pB = iChild->GetBlockL (aRect);
    if (!pB) return NULL;
    TUint32 * pD = pB->iData;

    for (TInt i = pB->iRect.iTl.iY; i < pB->iRect.iBr.iY; ++i)
    {
        for (TInt j = pB->iRect.iTl.iX; j < pB->iRect.iBr.iX; ++j, ++pD)
        {

	        TUint32 c = *pD;
	        
	        TPoint pos = iCorrectPosition + TPoint (j << KCoordBits, i << KCoordBits);
	        TInt x = ( pos.iX * iScale.iX + pos.iY * iScale.iY ) >> ( KScaleBits + KCoordBits );
	        TInt y = ( pos.iY * iScale.iX - pos.iX * iScale.iY ) >> ( KScaleBits + KCoordBits );

	        if ( (x >= 0) && (y >= 0) && (x < iSize.iWidth) && (y < iSize.iHeight) )
            {

		        TUint32 c2 = iData[ x + y * iSize.iWidth ];
		        TInt ca = c2 >> 24;

                if ( ca == 255 )
                {
			        *pD = c2 & 0xffffff;
			    }
                else
                {
		            TInt cc1 = (c2 & 0xff00ff) * ca + (c & 0xff00ff) * (255 - ca);
		            TInt cc2 = (c2 & 0xff00) * ca + (c & 0xff00) * (255 - ca);
		            *pD = (( cc1 >> 8) & 0xff00ff) + ((cc2 >> 8) & 0xff00);
                }
		    }
        }
    }
    return pB;
}


void CFilterBubble::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}



void CFilterBubble::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterBubble::CmdL( const TDesC16& aCmd )
	{

	TLex lex( aCmd );

	while( ! lex.Eos() )
		{
		TPtrC token = lex.NextToken();
		if( token.Compare( _L("file") ) == 0 )
			{
			TPtrC namec = lex.NextToken();
			TPtrC name( namec.Ptr()+1, namec.Length()-2 );
			iFileName.Copy (name);
			}
		else if( token.Compare( _L("load") ) == 0 )
			{
			LoadImageL();
			}
		else if( token.Compare( _L("bubble") ) == 0 )
			{
			lex.Inc();
			lex.Val( iBubble );
			}
		else if( token.Compare( _L("mask") ) == 0 )
			{
			lex.Inc();
			lex.Val( iMask );
			}
		else if( token.Compare( _L("x") ) == 0 )
			{
            TReal relscale = iChild->Scale();
            TInt param = 0;
			lex.Inc ();
			lex.Val (param);
            iPosition.iX = (TInt)((param / relscale) + 0.5);
			}
		else if( token.Compare( _L("y") ) == 0 )
			{
            TReal relscale = iChild->Scale();
            TInt param = 0;
			lex.Inc ();
			lex.Val (param);
            iPosition.iY = (TInt)((param / relscale) + 0.5);
			}
		else if( token.Compare( _L("width") ) == 0 )
			{
            TReal relscale = iChild->Scale();
            TInt param = 0;
			lex.Inc ();
			lex.Val (param);
            TInt width = (TInt)((param / relscale) + 0.5);
            iZoom = (width * 1000) / iSize.iWidth;
			}
		else if( token.Compare( _L("height") ) == 0 )
			{
            TReal relscale = iChild->Scale();
            TInt param = 0;
			lex.Inc ();
			lex.Val (param);
            TInt height = (TInt)((param / relscale) + 0.5);
            iZoom = (height * 1000) / iSize.iHeight;
			}
		else if( token.Compare( _L("zoom") ) == 0 )
			{
			lex.Inc();
			lex.Val( iZoom );
			}
		else if( token.Compare( _L("angle") ) == 0 )
			{
			lex.Inc();
			lex.Val( iAngle );
			}
		}

    TReal relscale = iChild->Scale();
    TInt scaledx = (TInt)(iPosition.iX * relscale + 0.5);
    TInt scaledy = (TInt)(iPosition.iY * relscale + 0.5);
    TInt scaledz = (TInt)(iZoom * relscale + 0.5);
    
    TReal zoom = 0.001 * scaledz;
	TReal angle = 0.001 * iAngle;
	TReal rad = KPi * angle / 180.0;
	TReal r;
	TReal res;
	
	Math::Cos( r, rad );
	r *= 1 << KScaleBits;
	//r /= zoom;
	Math::Round( res, r, 0 );
	iScale.iX = (TInt)res;

	Math::Sin( r, rad );
	r *= 1 << KScaleBits;
	//r /= zoom;
	Math::Round( res, r, 0 );
	iScale.iY = (TInt)res;

	// helpers
	TPoint mid( iSize.iWidth/2, iSize.iHeight/2 );
	TPoint sca( (TInt)(zoom * iScale.iX), (TInt)(zoom * iScale.iY) );
	TInt xo = ( mid.iX * sca.iX - mid.iY * sca.iY ) >> ( KScaleBits - KCoordBits );
	TInt yo = ( mid.iY * sca.iX + mid.iX * sca.iY ) >> ( KScaleBits - KCoordBits );

	//
	// iCorrectPosition is bubble mid position corrected with
	// rotation ( otherwise rotation would be around topleft corner )
	//
	iCorrectPosition = TPoint( xo,yo ) - TPoint( scaledx << KCoordBits, scaledy << KCoordBits );

	//
	// with iScale screen coordinates are translated to texture coordinates
	// 
	iScale.iX = (TInt)(1.0 / zoom * iScale.iX);
	iScale.iY = (TInt)(1.0 / zoom * iScale.iY);
	
	return 0;
	}



void CFilterBubble::LoadImageL ()
	{

	//	create and load bubble
	CFbsBitmap * bubble = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bubble);
	User::LeaveIfError ( bubble->Load (iFileName, iBubble) );

	//	create and load mask
	CFbsBitmap * mask = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(mask);
	User::LeaveIfError ( mask->Load (iFileName, iMask) );

	//	bubble size
    iSize = bubble->SizeInPixels();

	//	create new buffer
	delete[] iData;
	iData = new (ELeave) TUint32 [iSize.iWidth * iSize.iHeight];

	//	copy bubble data
	bubble->LockHeapLC();

	TInt wsc = bubble->ScanLineLength (iSize.iWidth, bubble->DisplayMode());
	TInt wsm = mask->ScanLineLength (iSize.iWidth, mask->DisplayMode());
	TUint8 * pcos = (TUint8 *)bubble->DataAddress();
	TUint8 * pmos = (TUint8 *)mask->DataAddress();

	for ( TInt i = 0, k = 0; i < iSize.iHeight; ++i )
	{

		TUint8 * pc = pcos;
		pcos += wsc;

		TUint8 * pm = pmos;
		pmos += wsm;

		for ( TInt j = 0; j < iSize.iWidth; ++j )
		{
			iData[k++] = (TUint32)(*pc  | (*(pc + 1) << 8) | (*(pc + 2) << 16) | (*pm++ << 24));
			pc += 3;
		}
	}

	CleanupStack::PopAndDestroy(3); // LockHeapLC(), mask, bubble

}



const char* CFilterBubble::Type()
	{
	return "bubble";
	}
	
	
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
