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



#include "CFilterText.h"
#include <txtetext.h>
#include <fbs.h>
#include <bitdev.h>
#include <e32math.h>
#include <AknBidiTextUtils.h>
#include "CFbsBitmapHack.h"

const TInt KScaleBits = 12;
const TInt KCoordBits = 8;

EXPORT_C TInt CFilterText::Create()
	{
	CFilterText* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterText* CFilterText::NewL()
	{
	CFilterText* self = new( ELeave )CFilterText();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterText::~CFilterText()
	{
	delete[] iData;
	iFont = NULL;
	}



CFilterText::CFilterText()
	{

	}



void CFilterText::ConstructL()
	{
	iScale.iX = 1 << 8;
	iScale.iY = 0;
	iZoom = 1000;
	iAngle = 0;
	iColor = 0xFFFFFF;
	}



TRect CFilterText::Rect()
	{
	return iChild->Rect();
	}

TReal CFilterText::Scale()
	{
	return iChild->Scale();
	}

TSize CFilterText::ViewPortSize()
{
    return iChild->ViewPortSize();
}


TBlock * CFilterText::GetBlockL ( const TRect & aRect )
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
	        TInt x = (pos.iX * iScale.iX + pos.iY * iScale.iY) >> (KScaleBits + KCoordBits);
	        TInt y = (pos.iY * iScale.iX - pos.iX * iScale.iY) >> (KScaleBits + KCoordBits);

	        if ( (x >= 0) && (y >= 0) && (x < iSize.iWidth) && (y < iSize.iHeight) )
            {
		        TUint32 c2 = iData [x + y * iSize.iWidth];
		        TInt ca = c2 >> 24;
		        c2 &= iColor;
		        if( ca == 255 )
                {
			        *pD = c2 & 0xffffff;
			    }
		        else 
                {
		            TInt cc1 = ( c2 & 0xff00ff ) * ca + ( c & 0xff00ff ) * ( 255-ca );
		            TInt cc2 = ( c2 & 0xff00 ) * ca + ( c & 0xff00 ) * ( 255-ca );
		            *pD =  ( ( cc1 >> 8 ) & 0xff00ff ) + ( ( cc2 >> 8 ) & 0xff00 );
                }
            }
        }
    }
    return pB;
}


void CFilterText::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}




void CFilterText::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterText::CmdL( const TDesC16& aCmd )
	{
	TLex lex( aCmd );

	TInt red = iColor >> 16;
	TInt green = ( iColor >> 8 ) & 255;
	TInt blue = iColor & 255;

    while( ! lex.Eos() )
		{
		TPtrC token = lex.NextToken();
		if( token.Compare( _L("x") ) == 0 )
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
		else if( token.Compare( _L("red") ) == 0 )
			{
			lex.Inc();
			lex.Val( red );
			}
		else if( token.Compare( _L("green") ) == 0 )
			{
			lex.Inc();
			lex.Val( green );
			}
		else if( token.Compare( _L("blue") ) == 0 )
			{
			lex.Inc();
			lex.Val( blue );
			}
		else if( token.Compare( _L("textwidth") ) == 0 )
			{
			lex.Inc();
			lex.Val( iTextSize.iWidth );
			}
		else if( token.Compare( _L("textheight") ) == 0 )
			{
			lex.Inc();
			lex.Val( iTextSize.iHeight );
			}
		else if( token.Compare( _L("font") ) == 0 )
			{
            TInt point = 0;
			lex.Inc();
			lex.Val( point );
            iFont = (const CFont *)point;
			}

		else if( token.Compare( _L("text") ) == 0 )
		{
			
			//	Extract text data
			HBufC * text = HBufC::NewLC(255);
			TPtr txtptr = text->Des();
			
			//	Find the start of the string
			while (!lex.Eos()) 
			{
				if (lex.Get() == '"') break;
			}

			//	Get the text data
			while (!lex.Eos())
			{
				TChar c= lex.Get();
				if (c == '"')
				{
					break;
				}
				else
				{
					txtptr.Append(c);
				}
			}
            if (iFont)
            	{
				CreateTextL( txtptr );

            	}
			CleanupStack::PopAndDestroy(); // text
		}

		iColor = ( red << 16 ) + ( green << 8 ) + blue;
		}

    TReal relscale = iChild->Scale();
    TInt scaledx = (TInt)(iPosition.iX  * relscale + 0.5);
    TInt scaledy = (TInt)(iPosition.iY * relscale + 0.5);
    TInt scaledz = (TInt)(iZoom *  relscale + 0.5);

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
	Math::Round( res, r, 0 );
	iScale.iY = (TInt)res;

	// helpers
	TPoint mid( iSize.iWidth/2, iSize.iHeight/2 );
	TPoint sca( (TInt)(zoom * iScale.iX), (TInt)(zoom * iScale.iY) );
	TInt xo = ( mid.iX * sca.iX - mid.iY * sca.iY ) >> ( KScaleBits - KCoordBits );
	TInt yo = ( mid.iY * sca.iX + mid.iX * sca.iY ) >> ( KScaleBits - KCoordBits );

	// iCorrectPosition is clipart mid position corrected with
	// rotation ( otherwise rotation would be around topleft corner )
    iCorrectPosition = TPoint( xo,yo ) - TPoint( scaledx << KCoordBits, scaledy << KCoordBits );

	// with iScale screen coordinates are translated to texture coordinates
	iScale.iX = (TInt)(1.0 / zoom * iScale.iX);
	iScale.iY = (TInt)(1.0 / zoom * iScale.iY);
	return 0;
	}



const char* CFilterText::Type()
	{
	return "text";
	}


void CFilterText::CreateBitmapL( const TSize& aSize )
	{
	delete iData;
	iData = NULL;
	iData = new( ELeave )TUint32[ aSize.iWidth * aSize.iHeight ];
	Mem::FillZ( iData, aSize.iWidth * aSize.iHeight * sizeof( TUint32 ) );
	iRect = aSize;
	iSize = aSize;
	}




void CFilterText::CreateTextL ( const TDesC16 & aText )
	{
    CArrayFix<TPtrC>* lines = new(ELeave) CArrayFixFlat<TPtrC>(5);
    CleanupStack::PushL (lines);
 

    HBufC* visualText = AknBidiTextUtils::ConvertToVisualAndWrapToArrayL  
	                        ( aText,
	                          iTextSize.iWidth,
	                          *iFont,
	                          *lines ); 
    CleanupStack::PushL(visualText);


    TInt width = 0;	
    for (TInt i = 0; i < lines->Count(); ++i)
    {
        if (width < iFont->TextWidthInPixels ((*lines)[i]))
        {
            width = iFont->TextWidthInPixels ((*lines)[i]);    
        }
        
    }
    

	TInt fh = iFont->HeightInPixels() + 4;
	TInt height = fh * lines->Count(); 

    //  Create a bitmap big enough to hold the text
	CFbsBitmap bitmap;
    iSize.SetSize ( width + 4, height + 4);
    User::LeaveIfError ( bitmap.Create ( iSize, EColor4K ) );

    //  Create bitmap device and context
    CFbsBitmapDevice * bitmapDevice = CFbsBitmapDevice::NewL (&bitmap); 
    CleanupStack::PushL (bitmapDevice);

	//	Create bitmap graphics context
    CFbsBitGc * bitmapContext = 0;
    User::LeaveIfError (bitmapDevice->CreateContext (bitmapContext));
    CleanupStack::PushL (bitmapContext) ;
    
    //  Set parameters for the context
    bitmapContext->UseFont( iFont );
	bitmapContext->SetBrushColor( 0 );
    bitmapContext->SetPenColor (KRgbWhite);
	bitmapContext->Clear();

    //  Render lines
    for (TInt i = 0; i < lines->Count(); i++)
        {
        TInt xoffset = (bitmap.SizeInPixels().iWidth - iFont->TextWidthInPixels ((*lines)[i])) >> 1;
		
		TInt yoffset = iFont->HeightInPixels() * (i+1) + 2; 
		bitmapContext->DrawText ((*lines)[i], TPoint (xoffset, yoffset));	
        }


	//	Create memory buffer to hold rendered image data
    delete[] iData;
    iData = NULL;
    iData = new (ELeave) TUint32 [iSize.iWidth * iSize.iHeight];
    Mem::FillZ (iData, iSize.iWidth * iSize.iHeight * sizeof (TUint32));
    
    //  Copy data to buffer and create borders
	CFbsBitmapHack bm2;
	bm2.Duplicate( bitmap.Handle() );
	bm2.DoLock();

	TInt ws = bm2.ScanLineLength(iSize.iWidth, bm2.DisplayMode()) / 2;
	TUint16 * spos = (TUint16*)(bm2.DataAddress()) + ws + 1;
    TUint32 * tp = iData + iSize.iWidth + 1;

    for (TInt y = 1; y < iSize.iHeight - 1; y++ )
    {

        TUint16 * sp = spos;
        spos += ws;       
 
		for (TInt x = 1; x < iSize.iWidth - 1; x++ )
        {
		
            TUint16 c0 = sp[ 0 ];
			TUint16 c1 = sp[ -1 ];
			TUint16 c2 = sp[ 1 ];
			TUint16 c3 = sp[ -ws ];
			TUint16 c4 = sp[ ws ];
			TUint16 c5 = sp[ -1 - ws ];
			TUint16 c6 = sp[ 1 - ws ];
			TUint16 c7 = sp[ -1 + ws ];
			TUint16 c8 = sp[ 1 + ws ];
			sp++;

			TUint32 c = 0;
			if (c0)
            {
				c = iColor;
			}
			TInt a = 0;
			if (c0) a++;
			if (c1) a++;
			if (c2) a++;
			if (c3) a++;
			if (c4) a++;
			a *= 2;
			if (c5) a++;
			if (c6) a++;
			if (c7) a++;
			if (c8) a++;
			a *= 80;
			if (a > 255) a = 255;
			a <<= 24;
			*tp++ = a + c;
        }
        tp += 2;
    }
	bm2.DoUnlock();

    bitmapContext->DiscardFont();
    CleanupStack::PopAndDestroy(4); // bitmapContext, bitmapDevice, lines, visaul text
	bitmap.Reset();

    }

#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
