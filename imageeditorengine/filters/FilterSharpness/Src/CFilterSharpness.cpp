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


#include "CFilterSharpness.h"

EXPORT_C TInt CFilterSharpness::Create()
	{
	CFilterSharpness* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterSharpness* CFilterSharpness::NewL()
	{
	CFilterSharpness* self = new( ELeave )CFilterSharpness();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterSharpness::~CFilterSharpness()
	{
	delete[] ipRCR;
	delete[] ipRCG;
	delete[] ipRCB;
	delete[] ipCCR;
	delete[] ipCCG;
	delete[] ipCCB;
    delete[] iDivLUT;
	}



CFilterSharpness::CFilterSharpness()
	{

	}



void CFilterSharpness::ConstructL()
	{
	}



TRect CFilterSharpness::Rect()
	{
	return iChild->Rect();
	}

TReal CFilterSharpness::Scale()
	{
	return iChild->Scale();
	}

TSize CFilterSharpness::ViewPortSize()
{
    return iChild->ViewPortSize();
}


TBlock * CFilterSharpness::GetBlockL ( const TRect & aRect )
{

    if (iN == 1)
    {
        return iChild->GetBlockL(aRect);
    }

    //  Read the block needed to create sharpened block aRect
    TRect rect = aRect;
    rect.iTl.iX -= iHN;
    rect.iTl.iY -= iHN;
    rect.iBr.iX += iHN;
    rect.iBr.iY += iHN;
    TBlock * pS = iChild->GetBlockL(rect);
    if (!pS) return NULL;

    //  Create resulting sharpened block
    TBlock * pB = new (ELeave) TBlock (aRect);
    TUint32 * pd = pB->iData;

    TInt i = aRect.iTl.iY;      //  pixel index y
    TInt j = aRect.iTl.iX;      //  pixel index x

    TInt sumR = 0;
    TInt sumG = 0;
    TInt sumB = 0;
    TInt rowsumR = 0;
    TInt rowsumG = 0;         
    TInt rowsumB = 0;

    TInt irc = 0;               //  row cache index
    TInt icc = 0;               //  column cache index


    //  NW corner of the block => initialize row cache
    for (TInt ii = -iHN; ii <= iHN; ++ii)
    {

        TInt y = i + ii;
        if (y < pS->iRect.iTl.iY)
        {
            y = pS->iRect.iTl.iY;
        }
        else if (y >= pS->iRect.iBr.iY)
        {
            y = pS->iRect.iBr.iY - 1;
        }
        TInt offset = (y - pS->iRect.iTl.iY) * pS->iWidth - pS->iRect.iTl.iX;

        for (TInt jj = -iHN; jj <= iHN; ++jj)
        {
        
            TInt x = j + jj;
            if (x < pS->iRect.iTl.iX)
            {
                x = pS->iRect.iTl.iX;
            }
            else if (x >= pS->iRect.iBr.iX)
            {
                x = pS->iRect.iBr.iX - 1;
            }

            TUint32 cc = *(pS->iData + offset + x);
            rowsumB += cc & 0xFF;
            ipRCB[irc] = (TUint8)(cc & 0xFF);    

            cc >>= 8;
            rowsumG += cc & 0xFF;
            ipRCG[irc] = (TUint8)(cc & 0xFF);    
            
            cc >>= 8;
            rowsumR += cc & 0xFF;
            ipRCR[irc] = (TUint8)(cc & 0xFF);

            irc++;
            if (irc >= iN2) irc = 0;

            
        }
    }

	if ( iSharpen )
	{
        TUint32 c = *(pS->iData + (i - pS->iRect.iTl.iY) * pS->iWidth + (j - pS->iRect.iTl.iX));
		
		//	Get pixel from the previous filter
		TInt bb = c & 0xFF;
		bb += (bb - iDivLUT[rowsumB]);
		if (bb < 0)
		{ 
			bb = 0;
		}
		else if (bb > 255)
		{
			bb = 255;
		}

		c >>= 8;
		TInt gg = c & 0xFF;
		gg += (gg - iDivLUT[rowsumG]);
		if (gg < 0)
		{ 
			gg = 0;
		}
		else if (gg > 255)
		{
			gg = 255;
		}

		c >>= 8;
		TInt  rr = c & 0xFF;
		rr += (rr - iDivLUT[rowsumR]);
		if (rr < 0)
		{ 
			rr = 0;
		}
		else if (rr > 255)
		{
			rr = 255;
		}
        *pd++ = (rr << 16) | (gg << 8) | bb;
	}
	else
	{
        *pd++ = ( (iDivLUT[rowsumR] << 16) |  (iDivLUT[rowsumG] << 8) | iDivLUT[rowsumB] );
	}


    //  Initialize column sum
    sumB = rowsumB;
    sumG = rowsumG;
    sumR = rowsumR;

    //  Update column cache
    for (TInt iii = 0; iii < iN; ++iii)
    {
        for (TInt jjj = 0; jjj < iN; ++jjj)
        {
            ipCCR[iii * iN + jjj] = ipRCR[jjj * iN + iii];
            ipCCG[iii * iN + jjj] = ipRCG[jjj * iN + iii];
            ipCCB[iii * iN + jjj] = ipRCB[jjj * iN + iii];
        }
    }


    //  Filter the rest of the first row
    for ( j++ ; j < aRect.iBr.iX; ++j)
    {

        TInt x = j + iHN;
        if (x < pS->iRect.iTl.iX)
        {
            x = pS->iRect.iTl.iX;
        }
        else if (x >= pS->iRect.iBr.iX)
        {
            x = pS->iRect.iBr.iX - 1;
        }
        TInt offset = x - pS->iRect.iTl.iX - pS->iRect.iTl.iY * pS->iWidth;

        for (TInt ii = -iHN; ii <= iHN; ++ii)
        {

            TInt y = i + ii;
            if (y < pS->iRect.iTl.iY)
            {
                y = pS->iRect.iTl.iY;
            }
            else if (y >= pS->iRect.iBr.iY)
            {
                y = pS->iRect.iBr.iY - 1;
            }

            TUint32 cc = *(pS->iData + y * pS->iWidth + offset);
            sumB -= ipCCB[icc];
            sumB += cc & 0xFF;
            ipCCB[icc] = (TUint8)(cc & 0xFF);    

            cc >>= 8;
            sumG -= ipCCG[icc];
            sumG += cc & 0xFF;
            ipCCG[icc] = (TUint8)(cc & 0xFF);    
            
            cc >>= 8;
            sumR -= ipCCR[icc];
            sumR += cc & 0xFF;
            ipCCR[icc] = (TUint8)(cc & 0xFF);    

            icc++;
            if (icc >= iN2) icc = 0;

        }

	    if ( iSharpen )
	    {
            TUint32 c = *(pS->iData + (i - pS->iRect.iTl.iY) * pS->iWidth + (j - pS->iRect.iTl.iX));
		    
		    //	Get pixel from the previous filter
		    TInt bb = c & 0xFF;
		    bb += (bb - iDivLUT[sumB]);
		    if (bb < 0)
		    { 
			    bb = 0;
		    }
		    else if (bb > 255)
		    {
			    bb = 255;
		    }

		    c >>= 8;
		    TInt gg = c & 0xFF;
		    gg += (gg - iDivLUT[sumG]);
		    if (gg < 0)
		    { 
			    gg = 0;
		    }
		    else if (gg > 255)
		    {
			    gg = 255;
		    }

		    c >>= 8;
		    TInt  rr = c & 0xFF;
		    rr += (rr - iDivLUT[sumR]);
		    if (rr < 0)
		    { 
			    rr = 0;
		    }
		    else if (rr > 255)
		    {
			    rr = 255;
		    }
            *pd++ = (rr << 16) | (gg << 8) | bb;
	    }
	    else
	    {
            *pd++ = ( (iDivLUT[sumR] << 16) |  (iDivLUT[sumG] << 8) | iDivLUT[sumB] );
	    }

    }


    //  Filter the rest of the block
    for ( i++; i < aRect.iBr.iY; ++i)
    {
     
        j = aRect.iTl.iX;
        icc = 0;
        
        //  New row, add new row to row cache and remove old row, update column cache
        TInt y = i + iHN;
        if (y < pS->iRect.iTl.iY)
        {
            y = pS->iRect.iTl.iY;
        }
        else if (y >= pS->iRect.iBr.iY)
        {
            y = pS->iRect.iBr.iY - 1;
        }
        TInt offset = (y - pS->iRect.iTl.iY) * pS->iWidth - pS->iRect.iTl.iX;

        for (TInt jj = -iHN; jj <= iHN; ++jj)
        {

            TInt x = j + jj;
            if (x < pS->iRect.iTl.iX)
            {
                x = pS->iRect.iTl.iX;
            }
            else if (x >= pS->iRect.iBr.iX)
            {
                x = pS->iRect.iBr.iX - 1;
            }

            TUint32 cc = *(pS->iData + offset + x);
            rowsumB -= ipRCB[irc];
            rowsumB += cc & 0xFF;
            ipRCB[irc] = (TUint8)(cc & 0xFF);    

            cc >>= 8;
            rowsumG -= ipRCG[irc];
            rowsumG += cc & 0xFF;
            ipRCG[irc] = (TUint8)(cc & 0xFF);    
    
            cc >>= 8;
            rowsumR -= ipRCR[irc];
            rowsumR += cc & 0xFF;
            ipRCR[irc] = (TUint8)(cc & 0xFF);

            irc++;
            if (irc >= iN2) irc = 0;
   
        }

	    if ( iSharpen )
	    {
            TUint32 c = *(pS->iData + (i - pS->iRect.iTl.iY) * pS->iWidth + (j - pS->iRect.iTl.iX));
		    
		    //	Get pixel from the previous filter
		    TInt bb = c & 0xFF;
		    bb += (bb - iDivLUT[rowsumB]);
		    if (bb < 0)
		    { 
			    bb = 0;
		    }
		    else if (bb > 255)
		    {
			    bb = 255;
		    }

		    c >>= 8;
		    TInt gg = c & 0xFF;
		    gg += (gg - iDivLUT[rowsumG]);
		    if (gg < 0)
		    { 
			    gg = 0;
		    }
		    else if (gg > 255)
		    {
			    gg = 255;
		    }

		    c >>= 8;
		    TInt  rr = c & 0xFF;
		    rr += (rr - iDivLUT[rowsumR]);
		    if (rr < 0)
		    { 
			    rr = 0;
		    }
		    else if (rr > 255)
		    {
			    rr = 255;
		    }
            *pd++ = (rr << 16) | (gg << 8) | bb;
	    }
	    else
	    {
            *pd++ = ( (iDivLUT[rowsumR] << 16) |  (iDivLUT[rowsumG] << 8) | iDivLUT[rowsumB] );
	    }

        sumR = rowsumR;
        sumG = rowsumG;         
        sumB = rowsumB;

        //  Update column cache
        for (TInt iii = 0; iii < iN; ++iii)
        {
            for (TInt jjj = 0; jjj < iN; ++jjj)
            {
                ipCCR[iii * iN + jjj] = ipRCR[jjj * iN + iii];
                ipCCG[iii * iN + jjj] = ipRCG[jjj * iN + iii];
                ipCCB[iii * iN + jjj] = ipRCB[jjj * iN + iii];
            }
        }

        //  Filter the rest of the row
        for ( j++; j < aRect.iBr.iX; ++j)
        {

            TInt x = j + iHN;
            if (x < pS->iRect.iTl.iX)
            {
                x = pS->iRect.iTl.iX;
            }
            else if (x >= pS->iRect.iBr.iX)
            {
                x = pS->iRect.iBr.iX - 1;
            }
            TInt offset = x - pS->iRect.iTl.iX - pS->iRect.iTl.iY * pS->iWidth;

            for (TInt ii = -iHN; ii <= iHN; ++ii)
            {

                TInt y = i + ii;
                if (y < pS->iRect.iTl.iY)
                {
                    y = pS->iRect.iTl.iY;
                }
                else if (y >= pS->iRect.iBr.iY)
                {
                    y = pS->iRect.iBr.iY - 1;
                }

                TUint32 cc = *(pS->iData + y * pS->iWidth + offset);
                sumB -= ipCCB[icc];
                sumB += cc & 0xFF;
                ipCCB[icc] = (TUint8)(cc & 0xFF);    

                cc >>= 8;
                sumG -= ipCCG[icc];
                sumG += cc & 0xFF;
                ipCCG[icc] = (TUint8)(cc & 0xFF);    
            
                cc >>= 8;
                sumR -= ipCCR[icc];
                sumR += cc & 0xFF;
                ipCCR[icc] = (TUint8)(cc & 0xFF);    

                icc++;
                if (icc >= iN2) icc = 0;

            }
            
	        if ( iSharpen )
	        {
                TUint32 c = *(pS->iData + (i - pS->iRect.iTl.iY) * pS->iWidth + (j - pS->iRect.iTl.iX));
		        
		        //	Get pixel from the previous filter
		        TInt bb = c & 0xFF;
		        bb += (bb - iDivLUT[sumB]);
		        if (bb < 0)
		        { 
			        bb = 0;
		        }
		        else if (bb > 255)
		        {
			        bb = 255;
		        }

		        c >>= 8;
		        TInt gg = c & 0xFF;
		        gg += (gg - iDivLUT[sumG]);
		        if (gg < 0)
		        { 
			        gg = 0;
		        }
		        else if (gg > 255)
		        {
			        gg = 255;
		        }

		        c >>= 8;
		        TInt  rr = c & 0xFF;
		        rr += (rr - iDivLUT[sumR]);
		        if (rr < 0)
		        { 
			        rr = 0;
		        }
		        else if (rr > 255)
		        {
			        rr = 255;
		        }
                *pd++ = (rr << 16) | (gg << 8) | bb;
	        }
	        else
	        {
                *pd++ = ( (iDivLUT[sumR] << 16) |  (iDivLUT[sumG] << 8) | iDivLUT[sumB] );
	        }
        }
    }

    delete pS;
    pS = NULL;
    return pB;
}

void CFilterSharpness::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}


void CFilterSharpness::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterSharpness::CmdL( const TDesC16 & aCmd )
{
	TLex lex (aCmd);

    //  Get sharpness [-100,100]
	while ( ! lex.Eos() )
    {
		TPtrC token = lex.NextToken();

        if ( token.Compare( _L("sharpness") ) == 0)
        {
			lex.Inc();
			lex.Val (iSharpness);
		}
    }
	if (iSharpness < -100)
	{
		iSharpness = -100;

	}
	else if (iSharpness > 100)
	{
		iSharpness = 100;
	}

	if (iSharpness < 0)
	{
		iN = (-iSharpness / 10) + 1;
		iSharpen = ETrue;
	}
	else
	{
		iN = (iSharpness / 10) + 1;
		iSharpen = EFalse;
	}
	if ( iN % 2 == 0 )
	{
		iN++;
	}
	iN2 = iN * iN;
	iHN = (iN >> 1);
	
    //  Create row caches for new filter size
	delete[] ipRCR;
	ipRCR = new (ELeave) TUint8 [iN2];
	delete[] ipRCG;
	ipRCG = new (ELeave) TUint8 [iN2];
	delete[] ipRCB;
	ipRCB = new (ELeave) TUint8 [iN2];
	
    //  Create column caches for new filter size
	delete[] ipCCR;
	ipCCR = new (ELeave) TUint8 [iN2];
	delete[] ipCCG;
	ipCCG = new (ELeave) TUint8 [iN2];
	delete[] ipCCB;
	ipCCB = new (ELeave) TUint8 [iN2];

    //  Create and compute division LUT
    delete[] iDivLUT;
    TInt maxbins = iN2 * 255 + 1;
    iDivLUT = new (ELeave) TUint8 [maxbins];
    for (TInt i = 0; i < maxbins; ++i)
    {
        iDivLUT[i] = (TUint8)(i / iN2);
    }


    return 0;
}
	
const char* CFilterSharpness::Type()
{
	return "sharpness";
}

#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
