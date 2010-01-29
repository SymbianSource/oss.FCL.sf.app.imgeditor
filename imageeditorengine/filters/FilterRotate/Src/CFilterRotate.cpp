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


#include <fbs.h>
#include <e32math.h>
#include "CFilterRotate.h"


EXPORT_C TInt CFilterRotate::Create()
	{
	CFilterRotate* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterRotate* CFilterRotate::NewL()
	{
	CFilterRotate* self = new( ELeave )CFilterRotate();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterRotate::~CFilterRotate()
	{
	}



CFilterRotate::CFilterRotate()
	{

	}



void CFilterRotate::ConstructL()
	{

	}



TRect CFilterRotate::Rect()
{
	if ( (ERotation90 == iRotation) || (ERotation270 == iRotation))
	{
		TRect r = iChild->Rect();
		//	swap x and y coordinates
		r.iTl.iX ^= r.iTl.iY;
		r.iTl.iY ^= r.iTl.iX;
		r.iTl.iX ^= r.iTl.iY;

		r.iBr.iX ^= r.iBr.iY;
		r.iBr.iY ^= r.iBr.iX;
		r.iBr.iX ^= r.iBr.iY;

		return r;
	}
	else
	{
		return iChild->Rect();
	}
}

TReal CFilterRotate::Scale()
{
	return iChild->Scale();
}

TSize CFilterRotate::ViewPortSize()
{
	TSize size = iChild->ViewPortSize();

    if ( (ERotation90 == iRotation) || (ERotation270 == iRotation))
	{
        size.iWidth ^= size.iHeight;
        size.iHeight ^= size.iWidth;
        size.iWidth ^= size.iHeight;
		return size;
	}
	else
	{
        return size;
	}
}

TBlock * CFilterRotate::GetBlockL ( const TRect & aRect )
{

	
    if (iRotation == ERotation90)
	{

        TRect cr = iChild->Rect();

        //  Load block
        TRect rr;
        rr.iTl.iX = aRect.iTl.iY;
        rr.iTl.iY = cr.iTl.iY + cr.iBr.iY - aRect.iBr.iX;
        rr.iBr.iX = aRect.iBr.iY;
        rr.iBr.iY = cr.iTl.iY + cr.iBr.iY - aRect.iTl.iX;
        TBlock * pS = iChild->GetBlockL(rr);
        if (!pS)
        {
            return NULL;
        }

        //  Create new block
        rr.iTl.iX = cr.iTl.iY + cr.iBr.iY - pS->iRect.iBr.iY;
        rr.iTl.iY = pS->iRect.iTl.iX;
        rr.iBr.iX = cr.iTl.iY + cr.iBr.iY - pS->iRect.iTl.iY;
        rr.iBr.iY = pS->iRect.iBr.iX;
        TBlock * pD = new (ELeave) TBlock (rr);

        //  Rotate data to new block
        TUint32 * ps = pS->iData;
        TUint32 * pddos = pD->iData + pD->iWidth - 1;
        for (TInt i = 0; i < pS->iHeight; ++i)
        {
            TUint32 * pdd = pddos;
            pddos--;
            for (TInt j = 0; j < pS->iWidth; ++j)
            {
                *pdd = *ps++;
                pdd += pD->iWidth;
            }
        }
        delete pS;
        pS = NULL;

        return pD;

    }
	else if (iRotation == ERotation180)
	{
        TRect cr = iChild->Rect();
        TRect rr;
        rr.iTl.iX = cr.iTl.iX + cr.iBr.iX - aRect.iBr.iX;
        rr.iTl.iY = cr.iTl.iY + cr.iBr.iY - aRect.iBr.iY;
        rr.iBr.iX = cr.iTl.iX + cr.iBr.iX - aRect.iTl.iX;
        rr.iBr.iY = cr.iTl.iY + cr.iBr.iY - aRect.iTl.iY;

        //  Load block
        TBlock * pS = iChild->GetBlockL(rr);
        
        if (!pS)
        {
            return NULL;
        }

        //  Create new block
        rr.iTl.iX = cr.iTl.iX + cr.iBr.iX - pS->iRect.iBr.iX;
        rr.iTl.iY = cr.iTl.iY + cr.iBr.iY - pS->iRect.iBr.iY;
        rr.iBr.iX = cr.iTl.iX + cr.iBr.iX - pS->iRect.iTl.iX;
        rr.iBr.iY = cr.iTl.iY + cr.iBr.iY - pS->iRect.iTl.iY;
        TBlock * pD = new (ELeave) TBlock (rr);

        //  Rotate data to new block
        TUint32 * ps = pS->iData;
        TUint32 * pd = pD->iData + pD->iDataLength - 1;
        for (TInt i = 0; i < pS->iDataLength; ++i)
        {
            *pd-- = *ps++;
        }
        delete pS;
        pS = NULL;

        return pD;
	}
	else if (iRotation == ERotation270)
	{
        TRect cr = iChild->Rect();

        //  Load block
        TRect rr;
        rr.iTl.iX = cr.iTl.iX + cr.iBr.iX - aRect.iBr.iY;
        rr.iTl.iY = aRect.iTl.iX;
        rr.iBr.iX = cr.iTl.iX + cr.iBr.iX - aRect.iTl.iY;
        rr.iBr.iY = aRect.iBr.iX;
        TBlock * pS = iChild->GetBlockL(rr);
        if (!pS)
        {
            return NULL;
        }

        //  Create new block
        rr.iTl.iX = pS->iRect.iTl.iY;
        rr.iTl.iY = cr.iTl.iX + cr.iBr.iX - pS->iRect.iBr.iX;
        rr.iBr.iX = pS->iRect.iBr.iY;
        rr.iBr.iY = cr.iTl.iX + cr.iBr.iX - pS->iRect.iTl.iX;
        TBlock * pD = new (ELeave) TBlock (rr);

        //  Rotate data to new block
        TUint32 * ps = pS->iData;
        TUint32 * pddos = pD->iData + (pD->iHeight - 1) * pD->iWidth;
        for (TInt i = 0; i < pS->iHeight; ++i)
        {
            TUint32 * pdd = pddos;
            pddos++;
            for (TInt j = 0; j < pS->iWidth; ++j)
            {
                *pdd = *ps++;
                pdd -= pD->iWidth;
            }
        }
        delete pS;
        pS = NULL;

        return pD;
	}
	else
	{
		return iChild->GetBlockL (aRect);
	}
}


void CFilterRotate::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}

void CFilterRotate::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterRotate::CmdL( const TDesC16& aCmd )
{

	TLex lex (aCmd);

    //  Get rotation parameter
	while ( ! lex.Eos() )
    {
		TPtrC token = lex.NextToken();
        if ( token.Compare( _L("angle") ) == 0)
        {
			TInt angle = 0;
			lex.Inc();
			lex.Val (angle);
            if (angle == 0)
            {
                iRotation = ERotation0;
            }
            else if (angle == 90 || angle == -270)
            {
                iRotation = ERotation90;
            }
            else if (angle == 180 || angle == -180)
            {
                iRotation = ERotation180;
            }
            else if (angle == 270 || angle == -90)
            {
                iRotation = ERotation270;
            }
            else
            {
                iRotation = ERotationMin;
            }
		}
        else if ( token.Compare( _L("rotateccw") ) == 0)
        {
			iRotation = ERotation270;
		}
        else if ( token.Compare( _L("rotatecw") ) == 0)
        {
			iRotation = ERotation90;
		}
        else if ( token.Compare( _L("getangle") ) == 0)
        {
            TInt angle = 0;
            if (iRotation == ERotation90)
            {
                angle = 90;
            }
            else if (iRotation == ERotation180)
            {
                angle = 180;
            }
            else if (iRotation == ERotation270)
            {
                angle = 270;
            }
            return angle;
		}
    }
    return 0;
}

const char* CFilterRotate::Type()
	{
	return "rotate";
	}
	
	
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
