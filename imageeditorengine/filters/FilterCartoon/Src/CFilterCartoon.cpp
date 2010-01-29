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
#include "CFilterCartoon.h"

const TInt KMaxColors = 10;
const TInt KColorBits = 8;

const TUint8 ByteMask[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};

/*
*
*   CLASS: CFilterCartoon
*
*/

//=============================================================================
EXPORT_C TInt CFilterCartoon::Create()
	{
	CFilterCartoon* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}

//=============================================================================
CFilterCartoon* CFilterCartoon::NewL()
	{
	CFilterCartoon* self = new( ELeave )CFilterCartoon();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

//=============================================================================
CFilterCartoon::~CFilterCartoon()
{
	delete[] i16BitIndLut;
	delete[] iRGB;
}

//=============================================================================
CFilterCartoon::CFilterCartoon()
	{

	}

//=============================================================================
void CFilterCartoon::ConstructL()
	{

	}

//=============================================================================
TRect CFilterCartoon::Rect()
	{
	return iChild->Rect();
	}

//=============================================================================
TReal CFilterCartoon::Scale()
	{
	return iChild->Scale();
	}

//=============================================================================
TSize CFilterCartoon::ViewPortSize()
{
    return iChild->ViewPortSize();
}

//=============================================================================
TBlock * CFilterCartoon::GetBlockL ( const TRect & aRect )
{
    TBlock * pB = iChild->GetBlockL (aRect);
    if (!pB)
    {
        return NULL;
    }

    TUint32 * pD	= pB->iData;
    TUint32 c		= 0;
	TUint32	r,g,b;
    for (TInt i = pB->iDataLength; i != 0 ; --i)
    {
        c = *pD;
		b = c & 0xFF;
		c >>= 8;
		g = c & 0xFF;
		c >>= 8;
		r = c & 0xFF;
		c = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
		*pD++ = iRGB[i16BitIndLut[c]];

    }
    return pB;
}


//=============================================================================
void CFilterCartoon::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}

//=============================================================================
void CFilterCartoon::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}

//=============================================================================
TInt CFilterCartoon::CmdL( const TDesC16& aCmd )
{

	TLex lex (aCmd);

	while ( !lex.Eos() )
	{
		TPtrC token = lex.NextToken();
		if( token.Compare( _L("quantcolors") ) == 0 )
		{
            CreateOctreePaletteL();
		}
    }
	return 0;
}

//=============================================================================
const char* CFilterCartoon::Type()
	{
	return "cartoon";
	}

//=============================================================================
void CFilterCartoon::CreateOctreePaletteL()
{

    //  Create new quantizer
    COTreeQuant * quant = new (ELeave) COTreeQuant;
    CleanupStack::PushL( quant );
    
    //  Octree quantize colors
    TRect rect = iChild->Rect();
    TBlock * pB = iChild->GetBlockL(rect);
    TUint32 * pD = pB->iData;
    TInt i;
    for (i = 0; i < pB->iDataLength; ++i, pD++) 
    {
        quant->AddColorL (*pD);
        while ( quant->iLeaves > KMaxColors )
        {
            quant->ReduceTree ();
        }
    }

    delete pB;
    pB = NULL;

    //  Create palette
    quant->GetColorTable();

    //  Get new iCount
    iCount = quant->iPalette.Count();

    //  Create 16 bit indexing array
	delete[] i16BitIndLut;
	i16BitIndLut = NULL;
    i16BitIndLut = new (ELeave) TInt [65536];

	delete[] iRGB;
	iRGB = NULL;
    iRGB = new (ELeave) TUint32 [iCount];


    //  Enumerate indexing array
    for (i = 0; i < 65536; ++i)
    {
		TUint32 c = i;
		TUint32 b = c & 0x1F;
		c >>= 5;
		TUint32 g = c & 0x3F;
		c >>= 6;
		TUint32 r = c & 0x1F;

		TInt md = 99999999;

		for (TInt j = 0; j < iCount; ++j)
		{
			c = quant->iPalette[j];
			TUint32 bb = c & 0xFF;
			bb >>= 3;

			c >>= 8;
			TUint32 gg = c & 0xFF;
			gg >>= 2;
			
			c >>= 8;
			TUint32 rr = c & 0xFF;
			rr >>= 3;

			TInt d = (b-bb) * (b-bb) + (g-gg) * (g-gg) + (r-rr) * (r-rr);
			if (d < md)
			{
				md = d;
				i16BitIndLut[i] = j;
			}
			else if (d == md)
			{
/*
				// compute i in 24 bit coordinates
				c = i;
				b = (c & 0x1F) << 3;
				c >>= 5;
				g = (c & 0x3F) << 2;
				c >>= 6;
				r = (c & 0x1F) << 3;

				//	compute distance of old min to i
				c = iRGB[i16BitIndLut[i]];
				bb = (c & 0xFF);
				c >>= 8;
				gg = (c & 0xFF);
				c >>= 8;
				rr = (c & 0xFF);
				d = (b-bb) * (b-bb) + (g-gg) * (g-gg) + (r-rr) * (r-rr);

				//	compute distance of the new min to i
				c = iRGB[j];
				bb = (c & 0xFF);
				c >>= 8;
				gg = (c & 0xFF);
				c >>= 8;
				rr = (c & 0xFF);
				TInt d2 = (b-bb) * (b-bb) + (g-gg) * (g-gg) + (r-rr) * (r-rr);

				if (d2 < d)
				{
					i16BitIndLut[i] = j;
				}
*/				
			}
		}

    }

	for (TInt j = 0; j < iCount; ++j)
	{
		iRGB[j] = quant->iPalette[j];
	}
    
    CleanupStack::PopAndDestroy( quant );
}


/*
*
*   CLASS: CNode
*
*/

//=============================================================================
CNode::CNode()
{
 
}

//=============================================================================
CNode::~CNode()
{
   for (TInt i = 0; i < 8; i++) 
    {
        delete iChild[i];
        iChild[i] = NULL;
    }
    iNext = NULL;
}

/*
*
*   CLASS: COTreeQuant
*
*/

//=============================================================================
COTreeQuant::COTreeQuant() 
{

}

//=============================================================================
COTreeQuant::~COTreeQuant()
{
    delete iTree;
    iTree = NULL;
    Mem::FillZ(iReducibleNodes, 9 * sizeof(CNode *));
    iPalette.Reset();
}

//=============================================================================
void COTreeQuant::AddColorL (TUint32 aRgb)
{
    DoAddColorL (&iTree, aRgb, 0);
}

//=============================================================================
void COTreeQuant::ReduceTree ()
{ 

    //  Find the deepest level with at least 1 reducible node
    TInt i=0;
    for (i = KColorBits - 1; (i > 0) && (!iReducibleNodes[i]); i--) {};

    //  Reduce most recent node at level i
    CNode * pNode = iReducibleNodes[i];
    iReducibleNodes[i] = pNode->iNext;

    TInt reds = 0;
    TInt greens = 0;
    TInt blues = 0;
    TInt children = 0;

    for ( i = 0; i < 8; i++) 
    {
        if ( pNode->iChild[i] ) 
        {
            reds += pNode->iChild[i]->iRedSum;
            greens += pNode->iChild[i]->iGreenSum;
            blues += pNode->iChild[i]->iBlueSum;
            pNode->iPixels += pNode->iChild[i]->iPixels;
            delete pNode->iChild[i];
            pNode->iChild[i] = NULL;
            children++;
        }
    }

    pNode->iIsLeaf = ETrue;
    pNode->iRedSum = reds;
    pNode->iGreenSum = greens;
    pNode->iBlueSum = blues;
    iLeaves -= (children - 1);
}

//=============================================================================
void COTreeQuant::GetColorTable ()
{
    GetPaletteColors ( *iTree );
}

//=============================================================================
TInt COTreeQuant::GetColorCount() const
{
    return iLeaves;
}

//=============================================================================
void COTreeQuant::DoAddColorL (CNode ** apNode, TUint32 aRgb, TInt aLevel)
{

    //  If node does not exist, create new
    if (!*apNode)
    {
        *apNode = new (ELeave) CNode;

        (*apNode)->iIsLeaf = (aLevel == KColorBits) ? ETrue : EFalse;
        if ((*apNode)->iIsLeaf)
        {
            iLeaves++;
        }
        else 
        {
            (*apNode)->iNext = iReducibleNodes[aLevel];
            iReducibleNodes[aLevel] = *apNode;
        }
    }

    TUint32 c = aRgb;
    TUint8 b = (TUint8)(c & 0xFF);
    c >>= 8;
    TUint8 g = (TUint8)(c & 0xFF);
    c >>= 8;
    TUint8 r = (TUint8)(c & 0xFF);

    //  Update colors if leaf
    if ( (*apNode)->iIsLeaf ) 
    {
        (*apNode)->iPixels++;
        (*apNode)->iRedSum += r;
        (*apNode)->iGreenSum += g;
        (*apNode)->iBlueSum += b;
    }

    //  Recurse deeper if not a leaf
    else 
    {

        TInt shift = 7 - aLevel;
        TInt index =    (((r & ByteMask[aLevel]) >> shift) << 2) |
                        (((g & ByteMask[aLevel]) >> shift) << 1) |
                        ((b & ByteMask[aLevel]) >> shift);

        DoAddColorL (&((*apNode)->iChild[index]), aRgb, aLevel + 1);
    }
    
}

//=============================================================================
void COTreeQuant::GetPaletteColors ( const CNode & aNode )
{
    if ( aNode.iIsLeaf ) 
    {
        TUint8 r = (TUint8) (aNode.iRedSum / aNode.iPixels);
        TUint8 g = (TUint8) (aNode.iGreenSum / aNode.iPixels);
        TUint8 b = (TUint8) (aNode.iBlueSum / aNode.iPixels);
        iPalette.Append ( ((r << 16) | (g << 8) | b) );
    }
    else 
    {
        for (TInt i = 0; i < 8; i++) 
        {
            if ( aNode.iChild[i] )
            {
                GetPaletteColors ( *aNode.iChild[i] );
            }
        }
    }
}


/*
*
*   ENTRY POINT
*
*/
//=============================================================================
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
