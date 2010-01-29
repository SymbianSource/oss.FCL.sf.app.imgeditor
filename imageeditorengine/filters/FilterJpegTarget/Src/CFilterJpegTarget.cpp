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
#include "CFilterJpegTarget.h"
#include "MJpegSave.h"
#include "JpegSaveFactory.h"

const TInt KSaveBufferLength = 0x40000; // 256 kilobytes

EXPORT_C TInt CFilterJpegTarget::Create()
	{
	CFilterJpegTarget* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterJpegTarget* CFilterJpegTarget::NewL()
	{
	CFilterJpegTarget* self = new( ELeave )CFilterJpegTarget();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterJpegTarget::~CFilterJpegTarget()
	{
	delete iJpegSave;
	iFile.Close();
	iFs.Close();
	}



CFilterJpegTarget::CFilterJpegTarget()
	{
	}



void CFilterJpegTarget::ConstructL()
	{
	User::LeaveIfError( iFs.Connect() );
	}



TRect CFilterJpegTarget::Rect()
    {
	TRect rect;
	
	rect.iTl.iX = 0;			
	rect.iTl.iY = 0;			
	rect.iBr.iX = iSize.iWidth;			
	rect.iBr.iY = iSize.iHeight;			

	return rect;
    }

TReal CFilterJpegTarget::Scale()
    {
    return iChild->Scale();
    }

TSize CFilterJpegTarget::ViewPortSize()
{
    return iChild->ViewPortSize();
}

TBlock * CFilterJpegTarget::GetBlockL ( const TRect & /*aRect*/ )
{
    return NULL;
}

void CFilterJpegTarget::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}




void CFilterJpegTarget::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterJpegTarget::CmdL( const TDesC16& aCmd )
	{

	TInt exifData = 0;
	TInt exifLength = 0;

	TLex lex( aCmd );
	while( ! lex.Eos() )
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
		else if( token.Compare( _L("exifdata") ) == 0 )
			{
			lex.Inc();
			lex.Val( exifData );
			}
		else if( token.Compare( _L("exiflen") ) == 0 )
			{
			lex.Inc();
			lex.Val( exifLength );
			}
		else if( token.Compare( _L("file") ) == 0 )
			{
			//
			// Starts saving, gets filename in parenthesis: file "file.jpg"
			// !! lexer cuts with space in filename
			//
			
			if ( iSize.iWidth == 0 || iSize.iHeight == 0 )
				{
				iRect = iChild->Rect();
				iSize.iWidth = iRect.iBr.iX - iRect.iTl.iX;
		        iSize.iHeight = iRect.iBr.iY - iRect.iTl.iY;
				}
			else
				{
				iRect.iTl.iX = 0;			
				iRect.iTl.iY = 0;			
				iRect.iBr.iX = iSize.iWidth;			
				iRect.iBr.iY = iSize.iHeight;			
				}
			
			TPtrC filec = lex.NextToken();
			TPtrC file( filec.Ptr()+1, filec.Length()-2 );
			iFileName.Copy( file );
			iFile.Replace( iFs, iFileName, EFileWrite | EFileShareAny );

			iJpegSave = JpegSaveFactory::CreateJpegSaveL( &iFs, &iFile );
			
			TPtr8 exifPtr( (TUint8*)exifData, exifLength );
			exifPtr.SetLength( exifLength );

			iJpegSave->StartSaveL( iSize, exifPtr, KSaveBufferLength, 90 );
			
			iBm.iData = new( ELeave )TUint32[ 128 ];
			iBm.iSize = TSize (16,8);
            iBm.iDrawRect = iBm.iSize;
			iBm.iType = E16MColor;
			
			
			iBlockWidth = 16;
			iBlockHeight = 8;
			iBlockY = 0;
			}
		else if( token.Compare( _L("store") ) == 0 )
			{

            TInt height = iRect.iBr.iY - iRect.iTl.iY;
			TInt progress = (iBlockY * 100) / height;
			if( iBlockY >= height)
            	{
				iJpegSave->FinalizeSave();
				delete iBm.iData;
				progress = 100;
				iFile.Close();
				}
            else
            	{
                TPoint tl (iRect.iTl.iX, iBlockY + iRect.iTl.iY);
                TPoint br = tl + TPoint (iBlockWidth, iBlockHeight);

                while ( tl.iX < iRect.iBr.iX )
                	{
                    //  Get block
                    TBlock * pB = iChild->GetBlockL ( TRect(tl, br) );

                    if (pB)
                    	{
                        TUint32 * ps = 
                            pB->iData + 
                            (tl.iY - pB->iRect.iTl.iY) * pB->iWidth +
                            (tl.iX - pB->iRect.iTl.iX);
                        TUint32 * pd = (TUint32*)iBm.iData;
                        TInt datalength = iBlockWidth * sizeof(TUint32);
                        for (TInt i = 0; i < iBlockHeight; ++i)
                        	{
                            Mem::Copy (pd, ps, datalength);
                            ps += pB->iWidth;
                            pd += iBlockWidth;
                        	}
                        
                        delete pB;
                        pB = NULL;

                    	}
                    else
                    	{
                        Mem::FillZ ((TUint8 *)iBm.iData, iBlockWidth * iBlockHeight * sizeof(TUint32));
                    	}
/*
            CFbsBitmap * bitmap = new (ELeave) CFbsBitmap;
            CleanupStack::PushL(bitmap);
            TSize blocksize (iBm.iSize);
            User::LeaveIfError(bitmap->Create(blocksize, EColor16M));
            TBitmapUtil bm (bitmap);
            bm.Begin(TPoint(0,0));
            TUint8 * pDOS = (TUint8 *)bitmap->DataAddress();
            TUint32 * pS = (TUint32 *)iBm.iData;
            TInt ws = CFbsBitmap::ScanLineLength(blocksize.iWidth, EColor16M);
            for (TInt ii = 0; ii < blocksize.iHeight; ++ii)
            {
                TUint8 * pD = pDOS;
                pDOS += ws;
                for (TInt j = 0; j < blocksize.iWidth; ++j)
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
            filename.Copy (_L("c:\\nokia\\images\\sf_"));
            filename.AppendNum(tl.iY);
            filename.Append(_L("_"));
            filename.AppendNum(tl.iX);
            filename.Append(_L(".mbm"));
            bitmap->Save(filename);

            CleanupStack::PopAndDestroy(); // bitmap
*/

                    iJpegSave->SaveBlock( iBm );


                    tl.iX  += iBlockWidth;
                    br.iX  += iBlockWidth;

                	}
			    iBlockY += iBlockHeight;

            	}
			return progress;
			}
		
		else if( token.Compare( _L("abort") ) == 0 )
			{
			if (iBm.iData)
				{
				delete iBm.iData;	
				iBm.iData = NULL;
				}
			
			iFile.Close();
			}
		}
		
	return 0;
	}



const char* CFilterJpegTarget::Type()
	{
	return "jpegtarget";
	}
	
	
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
