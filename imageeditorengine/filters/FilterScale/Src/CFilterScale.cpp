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


#include "CFilterScale.h"
#include <e32math.h>

const TInt KScaleBits = 12;

EXPORT_C TInt CFilterScale::Create()
	{
	CFilterScale* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterScale* CFilterScale::NewL()
	{
	CFilterScale* self = new( ELeave )CFilterScale();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterScale::~CFilterScale()
	{
	}



CFilterScale::CFilterScale()
	{
	iScale = (1 << KScaleBits);
    iRelScale = 1.0;
	}



void CFilterScale::ConstructL()
	{

	}



TRect CFilterScale::Rect()
	{
	if ( iRect == TRect(0,0,0,0) )
	{
		return iChild->Rect();
	}
	else
	{
		return iRect;
	}
	}

TReal CFilterScale::Scale()
	{
        return iRelScale;
	}


TSize CFilterScale::ViewPortSize()
{
    return iChild->ViewPortSize();
}

TBlock * CFilterScale::GetBlockL ( const TRect & aRect )
{
    //  Get the block to be scaled 
    TRect rect;
    rect.iTl.iX = iOrigo.iX + (((aRect.iTl.iX * iScale) >> KScaleBits));
    rect.iTl.iY = iOrigo.iY + (((aRect.iTl.iY * iScale) >> KScaleBits));
    rect.iBr.iX = iOrigo.iX + (((aRect.iBr.iX * iScale) >> KScaleBits)) + 1;
    rect.iBr.iY = iOrigo.iY + (((aRect.iBr.iY * iScale) >> KScaleBits)) + 1;
    
    TBlock * pSource = iChild->GetBlockL(rect);
    if (!pSource) return NULL;

    //  Create the scaled block
    TBlock * pDest = new (ELeave) TBlock (aRect);
    TUint32 * pD = pDest->iData;

    rect = iChild->Rect();

    for (TInt i = aRect.iTl.iY; i < aRect.iBr.iY; ++i)
    {

	    TInt y = iOrigo.iY + ((i * iScale) >> KScaleBits);
        if ((y < rect.iTl.iY) || (y >= rect.iBr.iY))
        {
            pD += pDest->iWidth;
            continue;
        }

        for (TInt j = aRect.iTl.iX; j < aRect.iBr.iX; ++j, ++pD)
        {

	        TInt x = iOrigo.iX + ((j * iScale) >> KScaleBits);
            if ((x < rect.iTl.iX) || (x >= rect.iBr.iX))
            {
                continue;
            }
	        
            if ( (x >= pSource->iRect.iTl.iX) && (x < pSource->iRect.iBr.iX) && 
                 (y >= pSource->iRect.iTl.iY) && (y < pSource->iRect.iBr.iY) )
	        {
                *pD = *(pSource->iData + (y - pSource->iRect.iTl.iY) * pSource->iWidth + (x - pSource->iRect.iTl.iX));
	        }
        }
    }
    
    delete pSource;
    pSource = NULL;

    //  Return scaled block
    return pDest;
}



void CFilterScale::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}



void CFilterScale::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterScale::CmdL( const TDesC16& aCmd )
	{

	TLex lex( aCmd );

	while( ! lex.Eos() )
		{
		TPtrC token = lex.NextToken();
		if( token.Compare( _L("zoom") ) == 0 )
			{
			lex.Inc();
			lex.Val( iZoom );
			}
		else if( token.Compare( _L("x") ) == 0 )
			{
			lex.Inc();
			lex.Val( iPosition.iX );			
			}
		else if( token.Compare( _L("y") ) == 0 )
			{
			lex.Inc();
			lex.Val( iPosition.iY );			
			}
		}

	//	Get source and target sizes
    TRect rect = iChild->Rect();
	TSize srcsize;
    srcsize.iWidth = rect.iBr.iX - rect.iTl.iX;
	srcsize.iHeight = rect.iBr.iY - rect.iTl.iY;

    rect = iParent->Rect();
	TSize tgtsize;
    tgtsize.iWidth = rect.iBr.iX - rect.iTl.iX;
	tgtsize.iHeight = rect.iBr.iY - rect.iTl.iY;

	//	Compute aspect ratio of the source
	TInt ars = (srcsize.iWidth << KScaleBits) / srcsize.iHeight;

	//	Compute aspect ratio of the target
	TInt art = (tgtsize.iWidth << KScaleBits) / tgtsize.iHeight;

	//	Select scale so that aspect ratio is preserved
	if ( ars >= art )
	{
		iScale = (srcsize.iWidth << KScaleBits) / tgtsize.iWidth;
	    iRelScale = (TReal)tgtsize.iWidth / (TReal)srcsize.iWidth;
        iOrigo.iX = iChild->Rect().iTl.iX;
		TInt h = (srcsize.iWidth << KScaleBits) / art;
		iOrigo.iY = iChild->Rect().iTl.iY + (srcsize.iHeight - h) / 2;
	}
	else
	{
		iScale = (srcsize.iHeight << KScaleBits) / tgtsize.iHeight;
	    iRelScale = (TReal)tgtsize.iHeight / (TReal)srcsize.iHeight;
		iOrigo.iY = iChild->Rect().iTl.iY;
		TInt w = (srcsize.iHeight * art) >> KScaleBits;
		iOrigo.iX = iChild->Rect().iTl.iX + (srcsize.iWidth - w) / 2 ;
	}

    rect = iChild->Rect();
    iRect.iTl.iX = ((rect.iTl.iX * iScale) >> KScaleBits);
    iRect.iTl.iY = ((rect.iTl.iY * iScale) >> KScaleBits);
    iRect.iBr.iX = ((rect.iBr.iX * iScale) >> KScaleBits);
    iRect.iBr.iY = ((rect.iBr.iY * iScale) >> KScaleBits);
	return 0;
	}



const char* CFilterScale::Type()
	{
	return "scale";
	}
	
	
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
