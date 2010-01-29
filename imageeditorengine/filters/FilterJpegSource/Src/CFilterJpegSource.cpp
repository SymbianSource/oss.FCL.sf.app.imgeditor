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
#include "CFilterJpegSource.h"
#include "JpegLoadFactory.h"
#include "MJpegLoad.h"

const TInt KBlockBufferSize = 256;	// should be enough for every picture...
const TInt KScaleBits = 12;
const TInt KDestBufferMaxDim = 352;

//=============================================================================
EXPORT_C TInt CFilterJpegSource::Create()
	{
	CFilterJpegSource* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}

//=============================================================================
CFilterJpegSource* CFilterJpegSource::NewL()
	{
	CFilterJpegSource* self = new( ELeave )CFilterJpegSource();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

//=============================================================================
CFilterJpegSource::~CFilterJpegSource()
{
    delete iJpeg;
    delete iDestBuffer;
    delete[] iIndexMap;
    if ( iBlockBuffer )
    {
	    for (TInt i = 0; i < KBlockBufferSize; ++i)
	    {
	        delete iBlockBuffer[i];
	        iBlockBuffer[i] = NULL;
	    }
    }
	delete[] iBlockBuffer;
    iBlockBuffer = NULL;
}

//=============================================================================
CFilterJpegSource::CFilterJpegSource()
	{

	}

//=============================================================================
void CFilterJpegSource::ConstructL()
	{

	}

//=============================================================================
TRect CFilterJpegSource::Rect()
{
    if (iOutputState == EDirect)
    {
	    return iRect;
    }
    else
    {
        return iScaledRect;
    }
}

//=============================================================================
TReal CFilterJpegSource::Scale()
{
    if (iOutputState == EDirect)
    {
        return 1.0;
    }
    else
    {
	    return iRelScale;
    }
}

//=============================================================================
TSize CFilterJpegSource::ViewPortSize()
{
    return iFullSize;
}

//=============================================================================
TBlock * CFilterJpegSource::GetBlockL ( const TRect & aRect )
{
    if (iOutputState == EBuffer)
    {
        if ( !aRect.Intersects(iScaledRect) )
        {
            return NULL;
        }
        TRect rect = aRect;
        rect.Intersection(iScaledRect);

        TBlock * pB = new (ELeave) TBlock (rect);

        TUint32 * pS = iDestBuffer + (rect.iTl.iY - iScaledRect.iTl.iY) * iDestSize.iWidth + 
            (rect.iTl.iX - iScaledRect.iTl.iX);
        TUint32 * pD = pB->iData;
   
        for (TInt i = rect.iTl.iY; i < rect.iBr.iY; ++i)
        {
            Mem::Copy(pD, pS, pB->iWidth * sizeof(TUint32));
            pD += pB->iWidth;
            pS += iDestSize.iWidth;
        }
        return pB;
    }
    else
    {

		if (!aRect.Intersects(iRect))
		{
			return NULL;
		}

		//  Clip rectangle to image
		TRect rect = aRect;
		rect.Intersection(iRect);

        //  Compute region of interest in blocks
        TRect blockRect = rect; 
        blockRect.iTl.iX /= iBlockSize.iWidth;
        blockRect.iTl.iY /= iBlockSize.iHeight;
        blockRect.iBr.iX /= iBlockSize.iWidth;
        blockRect.iBr.iY /= iBlockSize.iHeight;
        
        if (blockRect.iBr.iX < iSizeInBlocks.iWidth)
        {
        	blockRect.iBr.iX++;	
        }
        if (blockRect.iBr.iY < iSizeInBlocks.iHeight)
        {
        	blockRect.iBr.iY++;	
        }
        
        //  Create block for pixel data
        TBlock * pB = new (ELeave) TBlock;
        CleanupStack::PushL(pB);
        pB->iRect = blockRect;
        pB->iRect.iTl.iX *= iBlockSize.iWidth;
        pB->iRect.iTl.iY *= iBlockSize.iHeight;
        pB->iRect.iBr.iX *= iBlockSize.iWidth;
        pB->iRect.iBr.iX += iBlockSize.iWidth;
        pB->iRect.iBr.iY *= iBlockSize.iHeight;
        pB->iRect.iBr.iY += iBlockSize.iHeight;
        pB->iWidth = (pB->iRect.iBr.iX - pB->iRect.iTl.iX);
        pB->iHeight = (pB->iRect.iBr.iY - pB->iRect.iTl.iY);
        pB->iDataLength = pB->iWidth * pB->iHeight;
        pB->iData = new (ELeave) TUint32 [pB->iDataLength];

        //  Read all blocks in region of interest
        for (TInt i = blockRect.iTl.iY; i < blockRect.iBr.iY; ++i)
        {

            for (TInt j = blockRect.iTl.iX; j < blockRect.iBr.iX; ++j)
            {

                //  Get block index
                TInt index = i * iSizeInBlocks.iWidth + j;

                //  If not found, load new block, add to cache and update index table
                if ( iIndexMap[index] < 0 )
                {

                    //  Load block
		            TBlockEntry * e = new (ELeave) TBlockEntry;
		            CleanupStack::PushL(e);
                    e->iRect = TRect ( TPoint (j * iBlockSize.iWidth, i * iBlockSize.iHeight), iBlockSize);
		            e->iBitmap = iJpeg->LoadBlockL (TPoint(j,i));

                    //  Remove index of the oldest block from the map
                    if (iBlockBuffer [iOldestBlock])
                    {
                        TPoint blockPos = iBlockBuffer[iOldestBlock]->iRect.iTl;
	                    blockPos.iX /= iBlockSize.iWidth;
	                    blockPos.iY /= iBlockSize.iHeight;
                        iIndexMap[blockPos.iY * iSizeInBlocks.iWidth + blockPos.iX] = -1;

                        //  Delete oldest block
		                delete iBlockBuffer [iOldestBlock];
                        iBlockBuffer [iOldestBlock] = NULL;
                    }

                    //  Insert new block to buffer and update map
                    iBlockBuffer[iOldestBlock] = e;
                    iIndexMap[index] = iOldestBlock;
                    CleanupStack::Pop(e);

                    //  Update old block index
                    iOldestBlock++;
                    if (iOldestBlock >= KBlockBufferSize) iOldestBlock = 0;

                }

                //  Store pixel data to block
	            TUint32 * pS = (TUint32*)( iBlockBuffer[ iIndexMap[index] ]->iBitmap.iData );
                TUint32 * pD = 
                    pB->iData + 
                    (i - blockRect.iTl.iY) * iBlockSize.iHeight * pB->iWidth + 
                    (j - blockRect.iTl.iX) * iBlockSize.iWidth;
                for (TInt ib = 0; ib < iBlockSize.iHeight; ++ib)
                {
                    Mem::Copy (pD, pS, iBlockSize.iWidth * sizeof(TUint32));
                    pS += iBlockSize.iWidth;
                    pD += pB->iWidth;
                }
            }
        }
        CleanupStack::Pop( pB );
        return pB;
    }
}


//=============================================================================
void CFilterJpegSource::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}

//=============================================================================
void CFilterJpegSource::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}

//=============================================================================
TInt CFilterJpegSource::CmdL( const TDesC16& aCmd )
	{

	TLex lex( aCmd );

	while( ! lex.Eos() )
		{

        TPtrC token = lex.NextToken();
		if( token.Compare( _L("file") ) == 0 )
			{
			iFileName.Zero();
			
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
					iFileName.Append(c);
				    }
			    }
			}
		else if( token.Compare( _L("loadimage") ) == 0 )
            {
            
            delete iJpeg;
            iJpeg = 0;
	        iJpeg = JpegLoadFactory::CreateJpegLoadL();
            
            // open
			iJpeg->OpenL( iFileName );
            iJpeg->ScanRandomL();
			
			const TJpegData & d = iJpeg->Info();

            //  set crop rectangle to full image size
            if (iRect.Size() == TSize(0,0))
            {
                iRect.iTl.iX = 0;
			    iRect.iTl.iY = 0;
			    iRect.iBr.iX = d.iSize.iWidth;
			    iRect.iBr.iY = d.iSize.iHeight;
            }
            iFullSize = d.iSize;

            //  set macroblock parameters
            iBlockSize = d.iBlockSize;
            iSizeInBlocks = d.iSizeInBlocks;

            iOutputState = EBuffer;

		    if ( iBlockBuffer )
		    {
			    for (TInt i = 0; i < KBlockBufferSize; ++i)
			    {
			        delete iBlockBuffer[i];
			        iBlockBuffer[i] = NULL;
			    }
		    }
			delete[] iBlockBuffer;
		    iBlockBuffer = NULL;
            iBlockBuffer = new (ELeave) TBlockEntry * [KBlockBufferSize];
            TInt i;
            for (i = 0; i < KBlockBufferSize; ++i) iBlockBuffer[i] = NULL;

            iSizeInBlocks = d.iSizeInBlocks;
            TInt blocks = iSizeInBlocks.iWidth * iSizeInBlocks.iHeight;
            delete[] iIndexMap;
            iIndexMap = NULL;
            iIndexMap = new (ELeave) TInt [blocks];
            for (i = 0; i < blocks; ++i) iIndexMap[i] = -1;

            iOldestBlock = 0;


            //  load rectangle
            LoadRectL();
            }
		else if( token.Compare( _L("exifdata") ) == 0 )
			{
			return (TInt)(iJpeg->ExifData().Ptr());
			}
		else if( token.Compare( _L("ulc") ) == 0 )
			{
			lex.Inc();
			lex.Val( iRect.iTl.iX );			
			}
		else if( token.Compare( _L("ulr") ) == 0 )
			{
			lex.Inc();
			lex.Val( iRect.iTl.iY );			
			}
		else if( token.Compare( _L("lrc") ) == 0 )
			{
			lex.Inc();
			lex.Val( iRect.iBr.iX );			
			}
		else if( token.Compare( _L("lrr") ) == 0 )
			{
			lex.Inc();
			lex.Val( iRect.iBr.iY );			
			}
		else if( token.Compare( _L("fileoutput") ) == 0 )
			{
            iRect.iTl.iX = 0;
            iRect.iTl.iY = 0;
            iRect.iBr.iX = iFullSize.iWidth;
            iRect.iBr.iY = iFullSize.iHeight;
            iOutputState = EDirect;
			}
		else if( token.Compare( _L("bufferoutput") ) == 0 )
			{
            iOutputState = EBuffer;
			}
		else if( token.Compare( _L("exiflength") ) == 0 )
			{
			return iJpeg->ExifData().Length();
			}
		}
	return 0;
	}

//=============================================================================
const char* CFilterJpegSource::Type()
	{
	return "jpegsource";
	}

//=============================================================================
void CFilterJpegSource::LoadRectL()
	{

    // compute destination buffer size
    TInt width = iRect.iBr.iX - iRect.iTl.iX;
    TInt height = iRect.iBr.iY - iRect.iTl.iY;

    if (width >= height)
    {
        TReal scale = (TReal)KDestBufferMaxDim / width;
        iDestSize.iWidth = KDestBufferMaxDim;
        iDestSize.iHeight = (TInt)(height * scale + 0.5);
    }
    else
    {
        TReal scale = (TReal)KDestBufferMaxDim / height;
        iDestSize.iHeight = KDestBufferMaxDim;
        iDestSize.iWidth = (TInt)(width * scale + 0.5);
    }

    //  create new destination buffer
    delete[] iDestBuffer;
    iDestBuffer = new (ELeave) TUint32 [iDestSize.iWidth * iDestSize.iHeight];

    //  compute scale
    TJpegScale scale = EScale1;
    while ((width /= 2) > iDestSize.iWidth)
        {
        if (scale == EScale1)
            {
            scale= EScale2;
            }
        else if (scale == EScale2)
            {
            scale= EScale4;
            }
        else if (scale == EScale4)
            {
            scale= EScale8;
            }
        }

    //  load scaled rectangle 
    TBitmapHandle loadbitmap;
    TRect loadrect = iRect;
    iJpeg->SetScale(scale);
    loadbitmap = iJpeg->LoadImageL(loadrect);

    //  scale buffer 
    TInt sc = (TInt) ((TReal)(height << KScaleBits) / iDestSize.iHeight + 0.5);
    TInt ws = (loadrect.iBr.iX - loadrect.iTl.iX) >> (TInt)scale;

    TUint32 * pD = iDestBuffer;
    TUint32 * pS = (TUint32 *)loadbitmap.iData +
        ((iRect.iTl.iY - loadrect.iTl.iY) >> (TInt)scale) * ws +
        ((iRect.iTl.iX  - loadrect.iTl.iX) >> (TInt)scale);

    for (TInt i = 0; i < iDestSize.iHeight; ++i)
    {
        TInt y = (i * sc) >> KScaleBits;
        y >>= (TInt)scale;
        for (TInt j = 0; j < iDestSize.iWidth; ++j)
        {
            TInt x = (j * sc) >> KScaleBits;
            x >>= (TInt)scale;
            *pD++ = *(pS + y * ws + x);
        }
    }

    delete loadbitmap.iData;

    iRelScale = (TReal)(iDestSize.iHeight) / (TReal)height;

    iScaledRect.iTl.iX = (TInt)(iRelScale * iRect.iTl.iX + 0.5);
    iScaledRect.iTl.iY = (TInt)(iRelScale * iRect.iTl.iY + 0.5);
    iScaledRect.iBr.iX = (TInt)(iRelScale * iRect.iBr.iX + 0.5);
    iScaledRect.iBr.iY = (TInt)(iRelScale * iRect.iBr.iY + 0.5);

	//	check that iScaledRect is inside buffer
	TInt tmp = iScaledRect.iBr.iX - iScaledRect.iTl.iX - iDestSize.iWidth;
	if ( tmp > 0 )
	{
		iScaledRect.iBr.iX -= tmp;
	}

	tmp = iScaledRect.iBr.iY - iScaledRect.iTl.iY - iDestSize.iHeight;
	if (tmp > 0 )
	{
		iScaledRect.iBr.iY -= tmp;
	}
	

//    CFbsBitmap * bitmap = new (ELeave) CFbsBitmap;
//    CleanupStack::PushL(bitmap);
//    User::LeaveIfError(bitmap->Create(iDestSize, EColor16M));
//    TBitmapUtil bm (bitmap);
//    bm.Begin(TPoint(0,0));
//    TUint8 * pDOS = (TUint8 *)bitmap->DataAddress();
//    pS = iDestBuffer;
//    ws = CFbsBitmap::ScanLineLength(iDestSize.iWidth, EColor16M);
//    for (TInt ii = 0; ii < iDestSize.iHeight; ++ii)
//    {
//        TUint8 * pD = pDOS;
//        pDOS += ws;
//        for (TInt j = 0; j < iDestSize.iWidth; ++j)
//        {
//            TUint32 c = *pS++;
//            *pD++ = c & 0xFF;
//            c >>= 8;
//            *pD++ = c & 0xFF;
//            c >>= 8;
//            *pD++ = c & 0xFF;
//        }
//    }
//    bm.End();
//    TFileName filename;
//    filename.Copy (_L("c:\\nokia\\images\\sf_"));
//    filename.AppendNum((TInt)iDestBuffer);
//    filename.Append(_L(".mbm"));
//    bitmap->Save(filename);
//
//    CleanupStack::PopAndDestroy(); // bitmap


	}
            
	
	
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
