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


#include "CJpegScale.h"
#include "CJpeg.h"
#include "CJpegSave.h"
#include "CExifParser.h"


CJpegScale::CJpegScale( MJpegScaleCallBack* aCallBack )
	: iTargetScale( 0 )
	, iQuality( 95 )
	, iCallBack( aCallBack )
	{
	}
	

	
CJpegScale::~CJpegScale()
	{
	}
	

	
	
void CJpegScale::ScaleL()
	{
	const TInt KxShift = 14;
	const TInt KxMul = ( 1 << KxShift );
	const TInt KxAnd = KxMul - 1;	
	
	CJpeg* source;
	CJpegSave* target;
	
	// file server session
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	
	// encoded file
	RFile targetFile;
	User::LeaveIfError( targetFile.Replace( fs, iTargetName, EFileWrite ) );
	CleanupClosePushL( targetFile );
	
	// jpeg source
	source = CJpeg::NewL();
	CleanupStack::PushL( source );
	
	// jpeg target
	target = CJpegSave::NewL( &fs, &targetFile );
	CleanupStack::PushL( target );

	source->OpenL( iSourceName );
	
	TJpegData info = source->Info();

	TInt tw = iTargetSize.iWidth;
	TInt th = iTargetSize.iHeight;
	
	TInt sw = info.iSize.iWidth;
	TInt sh = info.iSize.iHeight;

	TInt xscale;
	TInt yscale;

	//
	// Decide scale or target size
	//
	if( iTargetScale != 0 )
		{
		xscale = (TInt)(iTargetScale * KxMul);
		yscale = xscale;
		tw = xscale * sw / KxMul;
		th = yscale * sh / KxMul;
		}
	else
		{
		yscale = KxMul * th / sh;
		xscale = KxMul * tw / sw;
		}

	if( tw>sw || th>sh )
		{
		// no upscaling yet.
		User::Leave( KErrNotSupported );
		}

	//
	// Hardscale is scaling which is done in jpeg decoder
	// possible hardscales are 1:1, 1:2, 1:4 and 1:8
	// hard scaling speeds up things a lot.
	//
	TInt hardScale = 1;
	source->SetScale( EScale1 );

	if( xscale <= KxMul/8 && yscale <= KxMul/8 )
		{
		hardScale = 8;
		source->SetScale( EScale8 );
		}

	else if( xscale <= KxMul/4 && yscale <= KxMul/4 )
		{
		hardScale = 4;
		source->SetScale( EScale4 );
		}

	else if( xscale <= KxMul/2 && yscale <= KxMul/2 )
		{
		hardScale = 2;
		source->SetScale( EScale2 );
		}
	// else hardscale = 1


	sw /= hardScale;
	sh /= hardScale;
	xscale *= hardScale;
	yscale *= hardScale;
	

	//
	// Jpeg store bitmap block
	// Jpeg encoder always uses block size of 16x8 pixels
	//
	TBitmapHandle blk;
	blk.iSize = TSize( 16,8 );
	blk.iData = new( ELeave )TUint32[ 16*8 ];
	CleanupStack::PushL( blk.iData );

	iTargetSize = TSize( tw,th );
		
	CExifParser* parser = CExifParser::NewLC();
	parser->ParseL(source->ExifData());
	
    // ImageWidth - Not used for JPEG images 
    if (parser->TagExist(CExifParser::EIfd0, 0x0100))
    {
        parser->DeleteTag ( CExifParser::EIfd0, 0x0100);
        //iExifParser->AddTagL ( CExifParser::EIfd0, 0x0100, (TUint16)iTargetSize.iWidth);        
    }

    // ImageHeight - Not used for JPEG images 
    if (parser->TagExist(CExifParser::EIfd0, 0x0101))
    {
        parser->DeleteTag ( CExifParser::EIfd0, 0x0101);
        //iExifParser->AddTagL ( CExifParser::EIfd0, 0x0101, (TUint16)iTargetSize.iHeight);
    }
	    
	// PixelXResolution 
    if (parser->TagExist(CExifParser::ESubIfd, 0xA002))
    {
        parser->DeleteTag ( CExifParser::ESubIfd, 0xA002);
        parser->AddTagL ( CExifParser::ESubIfd, 0xA002,(TUint32)iTargetSize.iWidth);
    }
    // PixelYResolution 
    if (parser->TagExist(CExifParser::ESubIfd, 0xA003))
    {
        parser->DeleteTag ( CExifParser::ESubIfd, 0xA003);
        parser->AddTagL ( CExifParser::ESubIfd, 0xA003, (TUint32)iTargetSize.iHeight);
    }

    TPtr8 exif = parser->SaveL(parser->ThumbData());
	
	//
	// Initialize jpeg encoder
	// no exif
	// save buffer ~16KB
	// use iQuality as quality ( values from 0 to 100 )
	//
	target->StartSaveL( iTargetSize, exif, 16000, iQuality );
	
	CleanupStack::PopAndDestroy(); // parser
    	
	TInt yStep = 8;		// jpeg encoder block height
	TInt yPos = 0;		// number of encoded lines
	
	//
	// Reserve scaled buffer
	//
	TInt pixels = tw * ( yStep*2 ); // room for 2 block lines

	TUint32* cr = new( ELeave )TUint32[ pixels ];
	CleanupStack::PushL( cr );
	TUint32* cg = new( ELeave )TUint32[ pixels ];
	CleanupStack::PushL( cg );
	TUint32* cb = new( ELeave )TUint32[ pixels ];
	CleanupStack::PushL( cb );
	TUint32* cm = new( ELeave )TUint32[ pixels ];
	CleanupStack::PushL( cm );

	//
	// Clear scaled buffer
	//
	Mem::FillZ( cr, pixels * sizeof( TUint32 ) );
	Mem::FillZ( cg, pixels * sizeof( TUint32 ) );
	Mem::FillZ( cb, pixels * sizeof( TUint32 ) );
	Mem::FillZ( cm, pixels * sizeof( TUint32 ) );
	

	TInt srcBlockHeight = info.iBlockSize.iHeight;
	
	TInt srcY1 = 0;
	TInt srcY2 = 0;
	TBitmapHandle bm;
	bm.iData = 0;

	for( TInt y=0; y<sh; y++ )
		{
		if( iCallBack )
			{
			iCallBack->JpegStatusCallBack( 100 * y / sh );
			}
		//
		// Fill source buffer when needed
		//
		if( srcY2 <= y )
			{
			delete bm.iData;
			TRect rect( 0,y*hardScale, info.iSize.iWidth, ( y + srcBlockHeight ) * hardScale );
			bm = source->LoadImageL( rect );
			srcY1 = y;
			srcY2 = srcY1 + bm.iSize.iHeight;
			}

		TUint32* srcRgb = (TUint32*)bm.iData;
		srcRgb += (y-srcY1) * bm.iSize.iWidth;

		TInt ty = y * yscale;
		TInt y2 = ty & KxAnd;
		TInt y1 = KxMul - y2;
		ty >>= KxShift;
		
		TInt oy = ( ty - yPos ) * tw;
		TUint32* pr = cr + oy;
		TUint32* pg = cg + oy;
		TUint32* pb = cb + oy;
		TUint32* pm = cm + oy;

		//
		// Add pixels to scaled buffer
		//
		for( TInt x=0; x<sw; x++ )
			{
			TInt tx = x * xscale;
			TInt x2 = tx & KxAnd;
			TInt x1 = KxMul - x2;
			tx >>= KxShift;
			
			TUint32 pixel = *srcRgb++;

			TUint32 m1 = x1*y1 / KxMul;
			TUint32 m2 = x2*y1 / KxMul;
			TUint32 m3 = x1*y2 / KxMul;
			TUint32 m4 = x2*y2 / KxMul;
			
			TInt red = pixel >> 16;
			pr[ tx ] += m1 * red;
			pr[ tx+1 ] += m2 * red;
			pr[ tx+tw ] += m3 * red;
			pr[ tx+tw+1 ] += m4 * red;

			TInt green = ( pixel >> 8 ) & 255;
			pg[ tx ] += m1 * green;
			pg[ tx+1 ] += m2 * green;
			pg[ tx+tw ] += m3 * green;
			pg[ tx+tw+1 ] += m4 * green;

			TInt blue = pixel & 255;
			pb[ tx ] += m1 * blue;
			pb[ tx+1 ] += m2 * blue;
			pb[ tx+tw ] += m3 * blue;
			pb[ tx+tw+1 ] += m4 * blue;

			pm[ tx ] += m1;
			pm[ tx+1 ] += m2;
			pm[ tx+tw ] += m3;
			pm[ tx+tw+1 ] += m4;
			}


		//
		// Transfer scaled buffer block-by-block to jpeg encoder
		//
		if( ty >= yPos + yStep || y == sh-1 )
			{
			yPos += yStep;

			TUint32* trgb = (TUint32*)blk.iData;
			
			for( TInt b=0; b<tw; b += 16 )
				{
				TUint32* p = trgb;
				for( TInt by=0; by<8; by++ )
				for( TInt bx=0; bx<16; bx++ )
					{
					TUint32 pos = b + bx + by*tw;
					TUint32 bcr = cr[ pos ];
					TUint32 bcg = cg[ pos ];
					TUint32 bcb = cb[ pos ];
					TUint32 m = cm[ pos ];
					if( m )
						{
						bcr /= m;
						bcg /= m;
						bcb /= m;
						}
					//trgb[ bx + by * 16 ] = bcr*65536 + bcg*256 + bcb;

					TUint32 c = bcr << 16;
					c += ( bcg << 8 );
					c += bcb;
					*p++ = c;
					}
				target->SaveBlock( blk );
				}

			//
			// move overflow line to first line in scaled buffer
			//
			TInt offset = tw * yStep;
			for( TInt rx=0; rx<tw; rx++ )
				{
				cr[ rx ] = cr[ rx + offset ];
				cg[ rx ] = cg[ rx + offset ];
				cb[ rx ] = cb[ rx + offset ];
				cm[ rx ] = cm[ rx + offset ];

				}

			//
			// clear all but first line from scaled buffer
			//
			TInt clrbytes = tw * yStep * sizeof( TUint32 );
			offset = tw;
			Mem::FillZ( cr+offset, clrbytes );
			Mem::FillZ( cg+offset, clrbytes );
			Mem::FillZ( cb+offset, clrbytes );
			Mem::FillZ( cm+offset, clrbytes );
			}
		}

	target->FinalizeSave();
	
	delete bm.iData;

	CleanupStack::PopAndDestroy( 4 ); // cm,cb,cg,cr
	CleanupStack::PopAndDestroy( blk.iData );
	CleanupStack::PopAndDestroy( target );
	CleanupStack::PopAndDestroy( source );
	CleanupStack::PopAndDestroy( 2 ); // file, fs

	if( iCallBack )
		{
		iCallBack->JpegStatusCallBack( 100 );
		}

	}
