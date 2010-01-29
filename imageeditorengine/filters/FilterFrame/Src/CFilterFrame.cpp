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
#include "CFilterFrame.h"

const TInt KScaleBits = 12;

EXPORT_C TInt CFilterFrame::Create()
	{
	CFilterFrame* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterFrame* CFilterFrame::NewL()
	{
	CFilterFrame* self = new( ELeave )CFilterFrame();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterFrame::~CFilterFrame()
	{
	delete[] iData;
	}



CFilterFrame::CFilterFrame()
	{

	}



void CFilterFrame::ConstructL()
	{

	}



TRect CFilterFrame::Rect()
	{
	return iChild->Rect();
	}

TReal CFilterFrame::Scale()
	{
	return iChild->Scale();
	}

TSize CFilterFrame::ViewPortSize()
{
    return iChild->ViewPortSize();
}

TBlock * CFilterFrame::GetBlockL ( const TRect & aRect )
{
    TBlock * pB = iChild->GetBlockL (aRect);
    if (!pB) return NULL;
    TUint32 * pD = pB->iData;

    for (TInt i = pB->iRect.iTl.iY; i < pB->iRect.iBr.iY; ++i)
    {

        TInt y = ((i - iOffset.iY) * iScale.iY) >> KScaleBits;

        for (TInt j = pB->iRect.iTl.iX; j < pB->iRect.iBr.iX; ++j, ++pD)
        {

	        TInt x = ((j - iOffset.iX) * iScale.iX) >> KScaleBits;

	        if ( x >= 0 && x < iSize.iWidth && y >= 0 && y < iSize.iHeight )
	        {
	            TUint32 pixel = *pD;
		        TUint32 frame = iData[y * iSize.iWidth + x];
		        TUint8 alpha = (TUint8)(iData[y * iSize.iWidth + x] >> 24);

		        if (alpha == 255)
		        {
			        *pD = frame;
		        }
		        else 
		        {
		            TInt cc1 = (frame & 0xff00ff) * alpha + (pixel & 0xff00ff) * (255 - alpha);
		            TInt cc2 = (frame & 0xff00) * alpha + (pixel & 0xff00) * (255 - alpha);
		            *pD = (( cc1 >> 8 ) & 0xff00ff) + ((cc2 >> 8) & 0xff00);
		        }
	        }
        }
    }
    return pB;
}



void CFilterFrame::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}




void CFilterFrame::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterFrame::CmdL( const TDesC16& aCmd )
{
	TLex lex (aCmd);

	//	Handle parameters
	while ( !lex.Eos() )
	{
		TPtrC token = lex.NextToken();

		if ( token.Compare( _L("file") ) == 0 )
		{
			TPtrC namec = lex.NextToken();
			TPtrC name (namec.Ptr() + 1, namec.Length() - 2);
			iFile.Copy (name);
		}
		else if( token.Compare( _L("frame") ) == 0 )
		{
			lex.Inc ();
			lex.Val( iFrameInd );
		}
		else if( token.Compare( _L("mask") ) == 0 )
		{
			lex.Inc();
			lex.Val( iMaskInd );
		}
		else if( token.Compare( _L("load") ) == 0 )
		{
			LoadFrameL();
            iOrigRect = iChild->Rect();
            TReal relscale = iChild->Scale();
            iOrigRect.iTl.iX = (TInt)((iOrigRect.iTl.iX / relscale) + 0.5);
            iOrigRect.iTl.iY = (TInt)((iOrigRect.iTl.iY / relscale) + 0.5);
            iOrigRect.iBr.iX = (TInt)((iOrigRect.iBr.iX / relscale) + 0.5);
            iOrigRect.iBr.iY = (TInt)((iOrigRect.iBr.iY / relscale) + 0.5);
		}
	}
    
    TReal relscale = iChild->Scale();
	TRect rect = iChild->Rect();
    rect.iTl.iX = (TInt)((rect.iTl.iX / relscale) + 0.5);
    rect.iTl.iY = (TInt)((rect.iTl.iY / relscale) + 0.5);
    rect.iBr.iX = (TInt)((rect.iBr.iX / relscale) + 0.5);
    rect.iBr.iY = (TInt)((rect.iBr.iY / relscale) + 0.5);
    
    TRect framerect;
    framerect.iTl.iX = ((rect.iTl.iX - iOrigRect.iTl.iX) * (iSize.iWidth - 1)) / iOrigRect.Width();
    framerect.iTl.iY = ((rect.iTl.iY - iOrigRect.iTl.iY) * (iSize.iHeight - 1)) / iOrigRect.Height();
    framerect.iBr.iX = ((rect.iBr.iX - iOrigRect.iTl.iX) * (iSize.iWidth - 1)) / iOrigRect.Width();
    framerect.iBr.iY = ((rect.iBr.iY - iOrigRect.iTl.iY) * (iSize.iHeight - 1)) / iOrigRect.Height();

	rect = iChild->Rect();
    iScale.iX = (framerect.Width() << KScaleBits) / rect.Width();
	iScale.iY = (framerect.Height() << KScaleBits) / rect.Height();

    iOffset.iX = (TInt)(iOrigRect.iTl.iX * relscale + 0.5);
    iOffset.iY = (TInt)(iOrigRect.iTl.iY * relscale + 0.5);

    return 0;
}

const char* CFilterFrame::Type()
	{
	return "frame";
	}

void CFilterFrame::LoadFrameL()
{

	CFbsBitmap * frame = new (ELeave) CFbsBitmap;
	CleanupStack::PushL (frame);
	User::LeaveIfError ( frame->Load (iFile, iFrameInd) );

	CFbsBitmap * mask = new (ELeave) CFbsBitmap;
	CleanupStack::PushL (mask);
	User::LeaveIfError ( mask->Load (iFile, iMaskInd) );

	//	Get frame size
	iSize = frame->SizeInPixels();

	//	Create frame buffer
	delete iData;
	iData = NULL;
	iData = new (ELeave) TUint32 [iSize.iWidth * iSize.iHeight];

	//	Copy frame data
	frame->LockHeapLC();

	TInt wsf = frame->ScanLineLength (iSize.iWidth, frame->DisplayMode());
	TInt wsm = mask->ScanLineLength (iSize.iWidth, mask->DisplayMode());

	TUint8 * pfos = (TUint8 *)frame->DataAddress();
	TUint8 * pmos = (TUint8 *)mask->DataAddress();
	for ( TInt i = 0, k = 0; i < iSize.iHeight; ++i )
	{
		TUint8 * pd = pfos;
		pfos += wsf;

		TUint8 * pm = pmos;
		pmos += wsm;

		for ( TInt j = 0; j < iSize.iWidth; ++j )
		{
			iData[k++] = (TUint32)(*pd  | (*(pd + 1) << 8) | (*(pd + 2) << 16) | (*pm++ << 24));
			pd += 3;
		}
	}

	CleanupStack::PopAndDestroy(3); // LockHeapLC, mask, frame
}

	
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
