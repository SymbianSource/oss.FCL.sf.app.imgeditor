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


#include "CFilterBuffer.h"

EXPORT_C TInt CFilterBuffer::Create()
	{
	CFilterBuffer* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterBuffer* CFilterBuffer::NewL()
	{
	CFilterBuffer* self = new( ELeave )CFilterBuffer();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterBuffer::~CFilterBuffer()
	{
	delete iBuffer;
	}



CFilterBuffer::CFilterBuffer()
	{

	}



void CFilterBuffer::ConstructL()
	{

	}



TRect CFilterBuffer::Rect()
	{
    return iRect;
	}

TReal CFilterBuffer::Scale()
	{
    return iChild->Scale();
	}

TSize CFilterBuffer::ViewPortSize()
{
    return iChild->ViewPortSize();
}


TBlock * CFilterBuffer::GetBlockL ( const TRect & aRect )
{

    if ( !aRect.Intersects(iRect) )
    {
        return NULL;
    }

    TRect rect = aRect;
    rect.Intersection(iRect);

    TBlock * pB = new (ELeave) TBlock (rect);

    TUint32 * pS = iBuffer + rect.iTl.iY * iSize.iWidth + rect.iTl.iX;
    TUint32 * pD = pB->iData;
    
    for (TInt i = rect.iTl.iY; i < rect.iBr.iY; ++i)
    {
        Mem::Copy(pD, pS, pB->iWidth * sizeof(TUint32));
        pD += pB->iWidth;
        pS += iSize.iWidth;
    }
    return pB;
}



void CFilterBuffer::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}



void CFilterBuffer::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterBuffer::CmdL( const TDesC16& aCmd )
{

	TSize oldSize = iSize;

	TInt rv = 0;

	TLex lex (aCmd);

	while ( !lex.Eos() )
	{
		TPtrC token = lex.NextToken();
		
		if( token.Compare( _L("width") ) == 0 )
		{
			lex.Inc();
			lex.Val( iSize.iWidth );
		}
		else if( token.Compare( _L("height") ) == 0 )
		{
			lex.Inc();
			lex.Val( iSize.iHeight );			
		}
		else if( token.Compare( _L("getbitmap") ) == 0 )
		{

            Mem::FillZ(iBuffer, iSize.iWidth * iSize.iHeight * sizeof(TUint32));

            TPoint tl (0,0);
            TPoint br (16,16);
            
            while ( tl.iY < iRect.iBr.iY )
            {

                tl.iX = 0;
                br.iX = 16;

                while ( tl.iX < iRect.iBr.iX )
                {
                    if (br.iX >= iSize.iWidth) br.iX = iSize.iWidth;
                    if (br.iY >= iSize.iHeight) br.iY = iSize.iHeight;

                    TBlock * pB = iChild->GetBlockL(TRect (tl,br) );
                
                    if (pB)
                    {
                        TUint32 * ps = pB->iData;
                        TUint32 * pd = iBuffer + tl.iY * iSize.iWidth + tl.iX;

                        for (TInt i = 0; i < pB->iHeight; ++i )
                        {
                            Mem::Copy (pd, ps, pB->iWidth * sizeof (TUint32));
                            ps += pB->iWidth;
                            pd += iSize.iWidth;
                        }
                        delete pB;
                        pB = NULL;
                    }
                   else
                    {
                        TUint32 * pd = iBuffer + tl.iY * iSize.iWidth + tl.iX;
                        TInt datalength = (br.iX - tl.iX) * sizeof (TUint32);
                        for (TInt i = tl.iY; i < br.iY; ++i )
                        {
                            Mem::FillZ (pd, datalength);
                            pd += iSize.iWidth;
                        }
                    }

                    tl.iX += 16;
                    br.iX += 16;
                }
        
                tl.iY += 16;
                br.iY += 16;

            }
/*
            CFbsBitmap * bitmap = new (ELeave) CFbsBitmap;
            CleanupStack::PushL(bitmap);
            User::LeaveIfError(bitmap->Create(iSize, EColor16M));
            TBitmapUtil bm (bitmap);
            bm.Begin(TPoint(0,0));
            TUint8 * pDOS = (TUint8 *)bitmap->DataAddress();
            TUint32 * pS = iBuffer;
            TInt ws = CFbsBitmap::ScanLineLength(iSize.iWidth, EColor16M);
            for (TInt i = 0; i < iSize.iHeight; ++i)
            {
                TUint8 * pD = pDOS;
                pDOS += ws;
                for (TInt j = 0; j < iSize.iWidth; ++j)
                {
                    TUint32 c = *pS++;
                    *pD++ = c & 0xFF;
                    c >>= 8;
                    *pD++ = c & 0xFF;
                    c >>= 8;
                    *pD++ = c & 0xFF;
                }
            }
            bm.End();
            TFileName filename;
            filename.Copy (_L("c:\\data\\images\\test_"));
            filename.AppendNum((TInt)iBuffer);
            filename.Append(_L(".mbm"));
            bitmap->Save(filename);

            CleanupStack::PopAndDestroy(); // bitmap
*/
			
			rv = (TInt)iBuffer;
		}

		if( iSize != oldSize )
		{
			if( iSize.iWidth > 0 && iSize.iHeight > 0 )
			{
				delete[] iBuffer;
				iBuffer = NULL;
				iBuffer = new (ELeave) TUint32 [iSize.iWidth * iSize.iHeight];
			}
			oldSize = iSize;
			iRect.iTl.iX = 0;
			iRect.iTl.iY = 0;
			iRect.iBr.iX = iSize.iWidth;
			iRect.iBr.iY = iSize.iHeight;
		}


	}

	return rv;

}



const char* CFilterBuffer::Type()
	{
	return "buffer";
	}
	
	
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
