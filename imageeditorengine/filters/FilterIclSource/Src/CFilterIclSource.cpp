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


#include <e32std.h>
#include <eikenv.h>
#include <fbs.h>
#include <ImageConversion.h>
#include <ICL/icl_propertyuids.h>

#include "CFilterIclSource.h"

const TInt  KScaleBits = 12;
const TInt  KBlockSize = 16;
const TInt  KDestBufferMaxDim = 352;

//=============================================================================
EXPORT_C TInt CFilterIclSource::Create()
{
	CFilterIclSource * ptr = NULL;
	TRAPD (error, ptr = NewL(); );
	if (error != KErrNone)
    {
		ptr = NULL;
	}
	return (TInt)((MImageFilter*)ptr);
}

//=============================================================================
CFilterIclSource* CFilterIclSource::NewL()
{
	CFilterIclSource * self = new (ELeave) CFilterIclSource();
	CleanupStack::PushL (self);
	self->ConstructL();
	CleanupStack::Pop (self);
	return self;
}

//=============================================================================
CFilterIclSource::~CFilterIclSource()
{
    delete iBitmap;
    delete[] iDestBuffer;
    if (iLoader)
    {
        iLoader->Cancel();
    }
    delete iLoader;
    delete[] iIndexMap;
    for (TInt i = 0; i < iBlockBufferSize; ++i)
    {
        delete iBlockBuffer[i];
        iBlockBuffer[i] = NULL;
    }
	delete[] iBlockBuffer;
    iBlockBuffer = NULL;
}

//=============================================================================
CFilterIclSource::CFilterIclSource()
{

}

//=============================================================================
void CFilterIclSource::ConstructL()
{

}

//=============================================================================
TRect CFilterIclSource::Rect()
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
TReal CFilterIclSource::Scale()
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
TSize CFilterIclSource::ViewPortSize()
{
    return iSize;
}

//=============================================================================
TBlock * CFilterIclSource::GetBlockL ( const TRect & aRect )
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

        //  Create block for pixel data
        TBlock * pB = new (ELeave) TBlock;
        pB->iRect = rect;
        pB->iWidth = (pB->iRect.iBr.iX - pB->iRect.iTl.iX);
        pB->iHeight = (pB->iRect.iBr.iY - pB->iRect.iTl.iY);
        pB->iDataLength = pB->iWidth * pB->iHeight;
        pB->iData = new (ELeave) TUint32 [pB->iDataLength];

        //  Store pixel data to block
        TBitmapUtil bm (iBitmap);
        bm.Begin(TPoint(0,0));

	    TUint8 * psos = (TUint8*)(iBitmap->DataAddress());
        psos += pB->iRect.iTl.iY * iWStep + pB->iRect.iTl.iX * 3;
        TUint32 * pd = pB->iData;
        for (TInt i = rect.iTl.iY; i < rect.iBr.iY; ++i)
        {

            TUint8 * ps = psos;
            psos += iWStep;

            for (TInt j = rect.iTl.iX; j < rect.iBr.iX; ++j)
            {
                TUint8 b = *ps++;
                TUint8 g = *ps++;
                TUint8 r = *ps++;
                *pd++ = (r << 16) | (g << 8) | b;
            }
        }

        bm.End();

        return pB;
    }
}

//=============================================================================
void CFilterIclSource::SetParent (MImageFilter * aParent)
{
	iParent = aParent;
}

//=============================================================================
void CFilterIclSource::SetChild (MImageFilter * aChild)
{
	iChild = aChild;
}

//=============================================================================
TInt CFilterIclSource::CmdL (const TDesC16 & aCmd)
{

    TLex lex (aCmd);

	while ( !lex.Eos() )
    {
        TPtrC token = lex.NextToken();
		if (token.Compare( _L("file") ) == 0)
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

			if (iBitmap)
			{
	            delete iBitmap;
	            iBitmap = NULL;
			}
			
			if(iLoader)
			{
		        delete iLoader;
		        iLoader = NULL;				
			}

            iSize.iWidth = 0;
            iSize.iHeight = 0;

            iRect.iTl.iX = 0;
            iRect.iTl.iY = 0;
            iRect.iBr.iX = 0;
            iRect.iBr.iY = 0;

            iWStep = 0;
    	}
		else if( token.Compare( _L("loadimage") ) == 0 )
        {
            
            if ( !iBitmap )
            {
                //  Delete old bitmap and create new
                iBitmap = new (ELeave) CFbsBitmap;

                //  Create new loader and start loading image
                if (iLoader)
                {
                	delete iLoader;                	
                	iLoader = NULL;
                }

                iLoader = new (ELeave) CImageLoader (this);
                iLoader->LoadBitmapL (iFileName, iBitmap);

                //  Wait here until loading is finished
                CActiveScheduler::Start();

                //  Delete loader, not needed anymore
                delete iLoader;
                iLoader = NULL;

                //  Get image size
                iSize = iBitmap->SizeInPixels();

                //  Update region of interest
                iRect.iTl.iX = 0;
                iRect.iTl.iY = 0;
                iRect.iBr.iX = iSize.iWidth;
                iRect.iBr.iY = iSize.iHeight;

                //  Compute row offset in bytes
                iWStep = CFbsBitmap::ScanLineLength(iSize.iWidth, EColor16M);

                //  Initialize block buffer size to 2 x row size in blocks
                iBlockBufferSize = iSize.iWidth / KBlockSize;
                if (iSize.iWidth % KBlockSize > 0)
                {
                    iBlockBufferSize++;
                }
                iBlockBufferSize *= 2;

                //  Compute image size in 16 x 16 blocks
                iSizeInBlocks.iWidth = iSize.iHeight / KBlockSize;
                if (iSize.iHeight % KBlockSize > 0)
                {
                    iSizeInBlocks.iWidth++;
                }
                iSizeInBlocks.iHeight = iSize.iHeight / KBlockSize;
                if (iSizeInBlocks.iHeight % KBlockSize > 0)
                {
                    iSizeInBlocks.iHeight++;
                }

                //  Crate new block buffer, init to zero
                iBlockBuffer = new (ELeave) TBlockEntry * [iBlockBufferSize];
                Mem::FillZ (iBlockBuffer, iBlockBufferSize * sizeof(TBlockEntry *));

                //  Reset oldest block index to zero
                iOldestBlock = 0;

                //  Create index map, init to -1
                TInt blocks = iSizeInBlocks.iWidth * iSizeInBlocks.iHeight;
                delete[] iIndexMap;
                iIndexMap = NULL;
                iIndexMap = new (ELeave) TInt [blocks];
                for (TInt i = 0; i < blocks; ++i) iIndexMap[i] = -1;
                
            }
			

            iOutputState = EBuffer;

            //  set crop rectangle to full image size
            if (iRect.Size() == TSize(0,0))
            {
                iRect.iTl.iX = 0;
			    iRect.iTl.iY = 0;
			    iRect.iBr.iX = iSize.iWidth;
			    iRect.iBr.iY = iSize.iHeight;
            }

            //  load rectangle
            LoadRectL();
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
            iRect.iBr.iX = iSize.iWidth;
            iRect.iBr.iY = iSize.iHeight;
            iOutputState = EDirect;
			}
		else if( token.Compare( _L("bufferoutput") ) == 0 )
			{
            iOutputState = EBuffer;
		}
    }
	return 0;
}

//=============================================================================
const char * CFilterIclSource::Type()
{
	return "iclsource";
}

//=============================================================================
void CFilterIclSource::OperationReady (const TInt /* aError */)
{
	CActiveScheduler::Stop();
}

//=============================================================================
void CFilterIclSource::LoadRectL()
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

    //  scale buffer 

    TBitmapUtil bmtl (iBitmap);
    bmtl.Begin(TPoint(0,0));

    TInt scale = (height << KScaleBits) / iDestSize.iHeight;
    TUint32 * pD = iDestBuffer;
    TUint8 * pS = (TUint8 *)iBitmap->DataAddress() +
        iRect.iTl.iY * iWStep + iRect.iTl.iX * 3;

    for (TInt i = 0; i < iDestSize.iHeight; ++i)
    {
        TInt y = (i * scale) >> KScaleBits;

        for (TInt j = 0; j < iDestSize.iWidth; ++j)
        {
            TInt x = (j * scale) >> KScaleBits;
            TUint8 * pSS = pS + y * iWStep + x * 3;
            TUint32 c = (*(pSS + 2) << 16) | (*(pSS + 1) << 8) | *pSS;
            *pD++ = c;
        }
    }

    bmtl.End();

    iRelScale = (TReal)(iDestSize.iHeight) / (TReal) height;

    iScaledRect.iTl.iX = (TInt)(iRect.iTl.iX * iRelScale + 0.5);
    iScaledRect.iTl.iY = (TInt)(iRect.iTl.iY * iRelScale + 0.5);
    iScaledRect.iBr.iX = (TInt)(iRect.iBr.iX * iRelScale + 0.5);
    iScaledRect.iBr.iY = (TInt)(iRect.iBr.iY * iRelScale + 0.5);

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

/*
    CFbsBitmap * bitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Create(iDestSize, EColor16M));
    TBitmapUtil bm (bitmap);
    bm.Begin(TPoint(0,0));
    TUint8 * pDOS = (TUint8 *)bitmap->DataAddress();
    TUint32 * ps = iDestBuffer;
    TInt ws = CFbsBitmap::ScanLineLength(iDestSize.iWidth, EColor16M);
    for (TInt ii = 0; ii < iDestSize.iHeight; ++ii)
    {
        TUint8 * pD = pDOS;
        pDOS += ws;
        for (TInt j = 0; j < iDestSize.iWidth; ++j)
        {
            TUint32 c = *ps++;
            *pD++ = c & 0xFF;
            c >>= 8;
            *pD++ = c & 0xFF;
            c >>= 8;
            *pD++ = c & 0xFF;
        }
    }
    bm.End();

    TFileName filename;
    filename.Copy (_L("c:\\nokia\\images\\sf_"));
    filename.AppendNum((TInt)iDestBuffer);
    filename.Append(_L(".mbm"));
    bitmap->Save(filename);

    CleanupStack::PopAndDestroy(); // bitmap
*/
	}






	
//=============================================================================
CImageLoader::CImageLoader(MLoaderObserver * aObserver) : 
CActive (EPriorityStandard),
iObserver (aObserver)
{
    CActiveScheduler::Add (this);
}

//=============================================================================
CImageLoader::~CImageLoader()
{
    if (iDecoder)
    {
        iDecoder->Cancel();
    }
    delete iDecoder;
}

//=============================================================================
void CImageLoader::LoadBitmapL (TDesC & aFileName, CFbsBitmap * iBitmap)
{

    // Delete old decoder
    if (iDecoder)
    {
        iDecoder->Cancel();
    }
    delete iDecoder;
    iDecoder = NULL;

    //Get the MIME type of the file to be decoded:     
    TBuf8<KMaxFileName> mimeType;
    
    RFs vFs;
    User::LeaveIfError(vFs.Connect());
    CleanupClosePushL(vFs);      	
    
    TRAPD( getMimeErr, CImageDecoder::GetMimeTypeFileL( 
                                            vFs, 
                                            aFileName,
                                            mimeType ) );
	if ( getMimeErr != KErrNone ) { }
    
    CleanupStack::PopAndDestroy(); // vFs
    
    // If MIME type contains "jpeg", then it would make sense to check 
    // for HW Codec possibility
    TUid implementationUid = KNullUid; // default value that can be used

    TInt vStartIndLocation = mimeType.Find( _L8( "jpeg" ) );
    if( vStartIndLocation != KErrNotFound )
        {
        RUidDataArray implArray;
        const TUid properties[] = { KUidHwCodec };
                                                            
        // Request existing plugins with the desired properties
        TRAPD( getIntErr, CImageDecoder::GetInterfaceImplementationsL( 
                                                            properties, 
                                                            1, 
                                                            implArray ) );   
                                                                                                                      
        if ( implArray.Count() != 0 && getIntErr == KErrNone )
            {
            // use the first HW codec from the list
            implementationUid = implArray[0];
            // HW Codec found
            }       
        }
    
    // if HW codec not found implementationUid == KNullUid ->
    // ICL selects proper codec
    TRAPD( decErr, iDecoder = CImageDecoder::FileNewL(
                                        vFs, 
                                        aFileName, 
                                        CImageDecoder::EOptionNone, 
                                        KNullUid, 
                                        KNullUid, 
                                        implementationUid ) );
	if ( decErr != KErrNone )
	    {
	    User::Leave( decErr );
	    }
	    
    //  Create new bitmap
    User::LeaveIfError ( iBitmap->Create (iDecoder->FrameInfo().iOverallSizeInPixels, EColor16M));

    //	Start decoding
    iDecoder->Convert (&iStatus, *iBitmap);
    
    if (iStatus != KRequestPending)
    {
        User::Leave(iStatus.Int());    
    }
    
    SetActive();

}

//=============================================================================
void CImageLoader::RunL()
{
	if (iStatus.Int() == KRequestPending)	
    {
        iDecoder->ContinueConvert (&iStatus);
    }
    else
    {
        iObserver->OperationReady ( iStatus.Int() );
    }
}

//=============================================================================
void CImageLoader::DoCancel()
{
    if (iDecoder)
    {
        iDecoder->Cancel();
    }
}


#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
