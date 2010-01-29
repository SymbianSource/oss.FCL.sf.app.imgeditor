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


//
// Jpeg encoder class

#include "CJpegSave.h"
#include <fbs.h>
#include <e32math.h>

const TUint8 KZigZag[] = {
						0,8,1,2,9,16,24,17,
						10,3,4,11,18,25,32,40,
						33,26,19,12,5,6,13,20,
						27,34,41,48,56,49,42,35,
						28,21,14,7,15,22,29,36,
						43,50,57,58,51,44,37,30,
						23,31,38,45,52,59,60,53,
						46,39,47,54,61,62,55,63
						};



//Table K.1 from JPEG specification
const int jpeg_luma_quantizer[64] = {
        16, 11, 10, 16, 24, 40, 51, 61,
        12, 12, 14, 19, 26, 58, 60, 55,
        14, 13, 16, 24, 40, 57, 69, 56,
        14, 17, 22, 29, 51, 87, 80, 62,
        18, 22, 37, 56, 68, 109, 103, 77,
        24, 35, 55, 64, 81, 104, 113, 92,
        49, 64, 78, 87, 103, 121, 120, 101,
        72, 92, 95, 98, 112, 100, 103, 99
};


// Table K.2 from JPEG specification
const int jpeg_chroma_quantizer[64] = {
        17, 18, 24, 47, 99, 99, 99, 99,
        18, 21, 26, 66, 99, 99, 99, 99,
        24, 26, 56, 99, 99, 99, 99, 99,
        47, 66, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99,
        99, 99, 99, 99, 99, 99, 99, 99
};



// Set up the standard Huffman tables (cf. JPEG standard section K.3)
// IMPORTANT: these are only valid for 8-bit data precision!

const TUint8 bits_dc_luminance[17] = { 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
const TUint8 val_dc_luminance[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
const TUint8 bits_dc_chrominance[17] = { 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
const TUint8 val_dc_chrominance[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

const TUint8 bits_ac_luminance[17] = {  0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
const TUint8 val_ac_luminance[] =
	{
	0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
	0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
	0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
	0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
	0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
	0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
	0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
	0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
	0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
	0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
	0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
	0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
	0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
	0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
	0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
	0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
	0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
	0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
	0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa 
	};

const TUint8 bits_ac_chrominance[17] = { 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };

const TUint8 val_ac_chrominance[] =
	{ 
	0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
	0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
	0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
	0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
	0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
	0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
	0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
	0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
	0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
	0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
	0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
	0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
	0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
	0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
	0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
	0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
	0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
	0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
	0xf9, 0xfa 
	};


// Jpeg store huffman lookup
class TSHuffman
	{
	public:
		TUint8 iLength[ 256 ];
		TInt8 iCode[ 256 ];
	};



CJpegSave* CJpegSave::NewL( RFs* aFs, RFile* aFile )
	{
	CJpegSave* self = NewLC( aFs, aFile );
	CleanupStack::Pop( self );
	return self;
	}
	
	
	
CJpegSave* CJpegSave::NewLC( RFs* aFs, RFile* aFile )
	{
	CJpegSave* self = new( ELeave )CJpegSave( aFs, aFile );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}	



CJpegSave::~CJpegSave()
	{
	delete iHuffman[ 0 ];
	delete iHuffman[ 1 ];
	delete iHuffman[ 2 ];
	delete iHuffman[ 3 ];
	delete iSaveBuf;
	iFs = NULL;
	iSaveFile = NULL;
	iCurrentHuffman = NULL;
	iCurrentQuant = NULL;
	}



CJpegSave::CJpegSave( RFs* aFs, RFile* aFile )
	: iFs( aFs )
	, iSaveFile( aFile )
	{
	}



void CJpegSave::ConstructL()
	{
	TSHuffman* huff = new( ELeave )TSHuffman;
	CleanupStack::PushL(huff);
	CreateHuffmanL( huff, bits_dc_luminance, val_dc_luminance );
	iHuffman[ 0 ] = huff;
	CleanupStack::Pop();

	huff = new( ELeave )TSHuffman;
	CleanupStack::PushL(huff);
	CreateHuffmanL( huff, bits_dc_chrominance, val_dc_chrominance );
	iHuffman[ 1 ] = huff;
	CleanupStack::Pop();

	huff = new( ELeave )TSHuffman;
	CleanupStack::PushL(huff);
	CreateHuffmanL( huff, bits_ac_luminance, val_ac_luminance );
	iHuffman[ 2 ] = huff;
	CleanupStack::Pop();
	
	huff = new( ELeave )TSHuffman;
	CleanupStack::PushL(huff);
	CreateHuffmanL( huff, bits_ac_chrominance, val_ac_chrominance );
	iHuffman[ 3 ] = huff;
	CleanupStack::Pop();
	
	iSaveByte = 0;
	iSaveBufPos = 0;
	iSaveBufBitPos = 0;

	}



void CJpegSave::StartSaveL( const TSize& aSize, TPtr8 aExif, TInt aSaveBufferSize, TInt aQuality )
	{
	iSaveBufSize = aSaveBufferSize;
	if (iSaveBuf)
		{
		delete iSaveBuf;
		iSaveBuf = NULL;
		}
		
	iSaveBuf = new( ELeave )TUint8[ iSaveBufSize ];

	// create quantization tables
	MakeTables( aQuality, iYQuant, iUVQuant );


	TSize size = aSize;
	TInt len;
	
	// D8 Start Of Image

	WriteSaveBuffer( (TUint16)0xffd8 );

	

	//
	// Exif ( if any )
	// 
	
	if( aExif.Length() != 0 )
		{
		WriteSaveBuffer( (TUint16) 0xffe1 );
		
		TUint32 l = aExif.Length() + 6 + 2; // +header+tagsize
		WriteSaveBuffer( (TUint16) l );

		// exif header is 6 bytes ( 45 78 69 66 00 00 "Exif.." )
		WriteSaveBuffer( (TUint8) 0x45 );
		WriteSaveBuffer( (TUint8) 0x78 );
		WriteSaveBuffer( (TUint8) 0x69 );
		WriteSaveBuffer( (TUint8) 0x66 );
		WriteSaveBuffer( (TUint8) 0x00 );
		WriteSaveBuffer( (TUint8) 0x00 );

		l -= ( 6+2 ); // minus header, minus tag size
		
		// save exif chunck
		WriteSaveBuffer( aExif.Ptr(), l );
		}
	else
		{
		//
		// if no exif, standard application segment here
		//

		// E0 jpeg application segment
		WriteSaveBuffer( (TUint16)0xffe0 );
		
		// segment length
		WriteSaveBuffer( (TUint16)16 );

		// 0x4a, 0x46, 0x49, 0x46, 0x00 JFIF #0
		WriteSaveBuffer( (TUint8) 0x4a );
		WriteSaveBuffer( (TUint8) 0x46 );
		WriteSaveBuffer( (TUint8) 0x49 );
		WriteSaveBuffer( (TUint8) 0x46 );
		WriteSaveBuffer( (TUint8) 0x00 );

		WriteSaveBuffer( (TUint8) 0x01 );	// major revision number
		WriteSaveBuffer( (TUint8) 0x01 );	// minor revision number

		// 0=aspect ratio, 1=dots/inch, 2=dots/cm
		WriteSaveBuffer( (TUint8) 0x01 );

		TInt dpi = 72;
		// x dpi
		WriteSaveBuffer( (TUint16) dpi );

		// y dpi
		WriteSaveBuffer( (TUint16) dpi );

		// thumbnail width
		WriteSaveBuffer( (TUint8) 0 );
		
		// thumbnail height
		WriteSaveBuffer( (TUint8) 0 );


		}
	


	//
	// quant table
	//

	TUint8 quant[ 2 ][ 64 ];

	TInt i;
	TInt n;
	for( i=0; i<64; i++ )
		{
		quant[ 0 ][ i ] = iYQuant[ i ];
		quant[ 1 ][ i ] = iUVQuant[ i ];
		}


	for( n=0; n<2; n++ )
		{
		WriteSaveBuffer( (TUint16) 0xffdb );

		len = 2 + 1 * 65;
		// chunk length
		WriteSaveBuffer( (TUint16) len );

		// quantization table #
		WriteSaveBuffer( (TUint8) n );
		
		// quantization table
		WriteSaveBuffer( quant[ n ], 64 );
		}





	//
	// Huffman Table
	//

	const TUint8* nBits[ 4 ];
	const TUint8* values[ 4 ];
	
	nBits[ 0 ] = bits_dc_luminance;
	nBits[ 1 ] = bits_dc_chrominance;
	nBits[ 2 ] = bits_ac_luminance;
	nBits[ 3 ] = bits_ac_chrominance;
	values[ 0 ] = val_dc_luminance;
	values[ 1 ] = val_dc_chrominance;
	values[ 2 ] = val_ac_luminance;
	values[ 3 ] = val_ac_chrominance;

	
	for( n=0; n<4; n++ )
		{
		WriteSaveBuffer( (TUint16) 0xffc4 );
		
		TInt numSymbols = 0;
		for( i=0; i<16; i++ )
			{
			TUint8 size = nBits[ n ][ i + 1 ];
			numSymbols += size;
			}
		
		len = 2 + 1 + 16 + numSymbols;
		// chunk length
		WriteSaveBuffer( (TUint16) len );
		
		// huffman table #
		WriteSaveBuffer( (TUint8)( ( n / 2 ) * 16 + ( n & 1 ) ) );

		// bit lengths for huffman table codes
		for( i=0; i<16; i++ )
			{
			TUint8 size = nBits[ n ][ i + 1 ];
			WriteSaveBuffer( size );
			}
		
		// huffman table symbols
		WriteSaveBuffer( values[ n ], numSymbols );
		}
	




	//
	// Start of frame
	//
	WriteSaveBuffer( (TUint16)0xffc0 );
	
	len = 8 + 3 * 3;
	// chunk length
	WriteSaveBuffer( (TUint16)len );
	
	// precision
	WriteSaveBuffer( (TUint8)8 );

	// image height
	WriteSaveBuffer( (TUint16)size.iHeight );

	// image width
	WriteSaveBuffer( (TUint16)size.iWidth );
	
	// number of components
	WriteSaveBuffer( (TUint8)3 );

	// component 0:
	
	WriteSaveBuffer( (TUint8)1 );				// number of component
	WriteSaveBuffer( (TUint8) ( 1 + 16*2 ) );	// sampling factor
	WriteSaveBuffer( (TUint8)0 );				// number of quantization table

	// component 1:
	WriteSaveBuffer( (TUint8)2 );				// number of component
	WriteSaveBuffer( (TUint8) ( 1 + 16*1 ) );	// sampling factor
	WriteSaveBuffer( (TUint8)1 );				// number of quantization table

	// component 2:
	WriteSaveBuffer( (TUint8)3 );				// number of component
	WriteSaveBuffer( (TUint8) ( 1 + 16*1 ) );	// sampling factor
	WriteSaveBuffer( (TUint8)1 );				// number of quantization table


	//
	// Start Of Scan ( SOS )
	//
	WriteSaveBuffer( (TUint16)0xffda );
	
	len = 3 + 3*2 + 3;
	// chunk length
	WriteSaveBuffer( (TUint16)len );
	
	// number of components
	WriteSaveBuffer( (TUint8)3 );	

	/// component1:
	// component ID
	WriteSaveBuffer( (TUint8)1 );
	// component huffman table ac(low4) dc(hi4)
	WriteSaveBuffer( (TUint8)0 );

	/// component2:
	// component ID
	WriteSaveBuffer( (TUint8)2 );
	// component huffman table ac(low4) dc(hi4)
	WriteSaveBuffer( (TUint8)17 );

	/// component3:
	// component ID
	WriteSaveBuffer( (TUint8)3 );
	// component huffman table ac(low4) dc(hi4)
	WriteSaveBuffer( (TUint8)17 );
	
	// stuffing ( actually dctsize-1 can be stored )
	WriteSaveBuffer( (TUint8)0 );
	WriteSaveBuffer( (TUint8)63 );	// dctsize - 1
	WriteSaveBuffer( (TUint8)0 );

	iDy = 0;
	iDu = 0;
	iDv = 0;
	}



void CJpegSave::SaveBlock( const TBitmapHandle& aBitmap )
	{
	//
	// Macroblocks:
	//
	TInt by[ 256 ];
	TInt bu[ 64 ];
	TInt bv[ 64 ];
	TInt dct[ 64 ];

	
	Rgb2Yuv( (TUint32*)aBitmap.iData, aBitmap.iSize.iWidth, by, bu, bv );
	
	iCurrentQuant = (TUint8*)iYQuant;
	
	Dct( by, dct );
	dct[ 0 ] -= iDy;
	iDy += WriteDct( dct, iHuffman[ 0 ], iHuffman[ 2 ] );
	
	Dct( by+64, dct );
	dct[ 0 ] -= iDy;
	iDy += WriteDct( dct, iHuffman[ 0 ], iHuffman[ 2 ] );
	
	iCurrentQuant = (TUint8*)iUVQuant;
	Dct( bu, dct );
	dct[ 0 ] -= iDu;
	iDu += WriteDct( dct, iHuffman[ 1 ], iHuffman[ 3 ] );
	
	Dct( bv, dct );
	dct[ 0 ] -= iDv;
	iDv += WriteDct( dct, iHuffman[ 1 ], iHuffman[ 3 ] );

	}



void CJpegSave::FinalizeSave()
	{
	
	//
	// End Of Image ( EOI )
	//

	// flush last bits to save buffer
	if( iSaveBufBitPos )
		{
		WriteBits( 0, 8-iSaveBufBitPos );
		}

	// end of image tag
	WriteSaveBuffer( (TUint16)0xffd9 );

	// there might still be stuff in save buffer
	FlushSaveBuf();		
	}


TPtrC8 CJpegSave::Finalize()
	{
	
	//
	// End Of Image ( EOI )
	//

	// flush last bits to save buffer
	if( iSaveBufBitPos )
		{
		WriteBits( 0, 8-iSaveBufBitPos );
		}

	// end of image tag
	WriteSaveBuffer( (TUint16)0xffd9 );
	
	return TPtrC8( iSaveBuf, iSaveBufPos );
	}



void CJpegSave::WriteHuffman( TInt aValue )
	{
	TInt code = iCurrentHuffman->iCode[ aValue ];
	TInt len = iCurrentHuffman->iLength[ aValue ];
	WriteBits( code, len );
	}



void CJpegSave::CreateHuffmanL( TSHuffman* aHuffman, const TUint8* aBits, const TUint8* aVal )
	{
	TInt huffSize[ 16 ];

	TUint32 huffCode[ 256 ];
	TInt huffValue[ 256 ];
	TInt huffLength[ 256 ];

	TInt numSymbols = 0;
	TInt i;

	for( i=0; i<16; i++ )
		{
		TInt size = aBits[ i+1 ];
		huffSize[ i ] = size;
		numSymbols += size;
		}

	for( i=0; i<numSymbols; i++ )
		{
		huffValue[ i ] = aVal[ i ];
		}
	
	TInt l;
	TInt p = 0;
	for( i=0; i<256; i++ ) huffLength[ i ] = -1;
	for( l=0; l<16; l++ )
		{
		for( i=0; i<huffSize[ l ]; i++ )
			{
			huffLength[ p++ ] = l+1;
			}
		}

	TInt code = 0;
	huffLength[ p ] = 0;
	TInt si = huffLength[ 0 ];
	
	p = 0;

	while( huffLength[ p ] )
		{
		while( huffLength[ p ] == si )
			{
			huffCode[ p++ ] = code++;
			}
		code *= 2;
		si++;
		}

	for( i=0; i<numSymbols; i++ )
		{
		TInt v = huffValue[ i ];
		aHuffman->iLength[ v ] = huffLength[ i ];
		aHuffman->iCode[ v ] = huffCode[ i ];
		}
	}



void CJpegSave::WriteBits( TUint32 aValue, TInt aNumBits )
	{

	aValue &= ( ( 1 << aNumBits ) - 1 );
	while( aNumBits > 0 )
		{
		TInt bitroom = 8 - iSaveBufBitPos;
		iSaveByte |= ( ( aValue << ( 24+bitroom-aNumBits ) ) >> 24 );

		if( aNumBits < bitroom )
			{
			iSaveBufBitPos += aNumBits;
			}
		else
			{
			iSaveBufBitPos += bitroom;
			}

		if( iSaveBufBitPos == 8 ) 
			{
			iSaveBufBitPos = 0;
			iSaveBuf[ iSaveBufPos ] = iSaveByte;

			if( iSaveByte == 255 )
				{
				iSaveBufPos++;
				if( iSaveBufPos == iSaveBufSize )
					{
					FlushSaveBuf();
					}
				iSaveBuf[ iSaveBufPos ] = 0;	// 255,0 = 255 ( escaped 255 )
				}
			iSaveByte = 0;
			iSaveBufPos++;

			if( iSaveBufPos == iSaveBufSize )
				{
				FlushSaveBuf();
				}

			}
		aNumBits -= bitroom;
		}
	}



TInt CJpegSave::WriteDct( TInt* aDct, TSHuffman* aDc, TSHuffman* aAc )
	{
	TInt rv = 0;
	TInt nullCount = 0;
	bool theEnd = false;

	for( TInt i=0; i<64; i++ )
		{
		if( i == 0 )
			{
			iCurrentHuffman = aDc;
			}
		else
			{
			iCurrentHuffman = aAc;
			}

		TInt value = aDct[ KZigZag[ i ] ] / iCurrentQuant[ i ];
		if( i==0 ) rv = value * iCurrentQuant[ 0 ];
			
		bool doSave = true;

		if( i > 0 && value == 0 )
			{
			
			if( nullCount < 14 )
				{
				nullCount++;
				doSave = false;
				}
			else
				{
				value = 0; // end of block, too many zeros
				nullCount = 0;
				theEnd = true;
				}

			}
		
		if( i == 63 && nullCount > 0 )
			{
			doSave = true;
			value = 0;
			nullCount = 0;
			theEnd = true;
			}

		if( doSave )
			{
			bool minus = false;
			if( value < 0 )
				{
				minus = true;
				value = -value;
				}
			TInt v = value;
			TInt vl = 0;
			
			while( v )
				{
				v >>= 1;
				vl++;
				}

			if( minus ) 
				{
				TInt a = ( 1 << vl ) - 1;
				value ^= a;
				}

			WriteHuffman( vl + nullCount * 16 );
			WriteBits( value, vl );
			if( theEnd ) // this is the end
				{
				break;
				}
			nullCount = 0;
			}
		}
	return rv;
	}



void CJpegSave::Dct( TInt* aSrc, TInt* aTgt )
	{
	const TInt w1 = 237;
	const TInt w2 = 98;
	const TInt w3 = 181;
	const TInt w4 = 251;
	const TInt w5 = 50;
	const TInt w6 = 213;
	const TInt w7 = 142;

	TInt s[8];
	TInt t[6];
	TInt r[4];
	
	int j;

	
	TInt* tp = aTgt;

	for( j=0; j<8; j++ )
		{
		s[0] = aSrc[j +  0] + aSrc[j + 56];
		s[1] = aSrc[j +  8] + aSrc[j + 48];
		s[2] = aSrc[j + 16] + aSrc[j + 40];
		s[3] = aSrc[j + 24] + aSrc[j + 32];
		s[4] = aSrc[j + 24] - aSrc[j + 32];
		s[5] = aSrc[j + 16] - aSrc[j + 40];
		s[6] = aSrc[j +  8] - aSrc[j + 48];
		s[7] = aSrc[j +  0] - aSrc[j + 56];
		
		t[0] = s[0] + s[3];
		t[1] = s[1] + s[2];
		t[2] = s[1] - s[2];
		t[3] = s[0] - s[3];
		t[4] = (s[6] - s[5]) * w3;
		t[5] = (s[6] + s[5]) * w3;
		
		r[0] = s[4] * 256 + t[4];
		r[1] = s[4] * 256 - t[4];
		r[2] = s[7] * 256 - t[5];
		r[3] = s[7] * 256 + t[5];
		
		*tp++ = (t[0] + t[1]) * w3 / 256;
		*tp++ = (r[0] * w5 + r[3] * w4) / 256 / 256;
		*tp++ = (t[2] * w2 + t[3] * w1) / 256;
		*tp++ = (r[2] * w6 - r[1] * w7) / 256 / 256;
		*tp++ = (t[0] - t[1]) * w3 / 256;
		*tp++ = (r[1] * w6 + r[2] * w7) / 256 / 256;
		*tp++ = (t[3] * w2 - t[2] * w1) / 256;
		*tp++ = (r[3] * w5 - r[0] * w4) / 256 / 256;
		}

	for( j=0; j<8; j++ )
		{
		s[0] = aTgt[j + 0] + aTgt[j + 56];
		s[1] = aTgt[j + 8] + aTgt[j + 48];
		s[2] = aTgt[j + 16] + aTgt[j + 40];
		s[3] = aTgt[j + 24] + aTgt[j + 32];
		s[4] = aTgt[j + 24] - aTgt[j + 32];
		s[5] = aTgt[j + 16] - aTgt[j + 40];
		s[6] = aTgt[j + 8] - aTgt[j + 48];
		s[7] = aTgt[j + 0] - aTgt[j + 56];
		
		t[0] = s[0] + s[3];
		t[1] = s[1] + s[2];
		t[2] = s[1] - s[2];
		t[3] = s[0] - s[3];
		t[4] = (s[6] - s[5]) * w3;
		t[5] = (s[6] + s[5]) * w3;
		
		r[0] = s[4] * 256 + t[4];
		r[1] = s[4] * 256 - t[4];
		r[2] = s[7] * 256 - t[5];
		r[3] = s[7] * 256 + t[5];
		
		aTgt[ 0 + j] = (t[0] + t[1]) * w3 / 256 / 4;
		aTgt[32 + j] = (t[0] - t[1]) * w3 / 256 / 4;
		aTgt[ 8 + j] = (r[0] * w5 + r[3] * w4) / 256 / 256 / 4;
		aTgt[56 + j] = (r[3] * w5 - r[0] * w4) / 256 / 256 / 4;
		aTgt[24 + j] = (r[2] * w6 - r[1] * w7) / 256 / 256 / 4;
		aTgt[40 + j] = (r[1] * w6 + r[2] * w7) / 256 / 256 / 4;
		aTgt[16 + j] = (t[2] * w2 + t[3] * w1) / 256 / 4;
		aTgt[48 + j] = (t[3] * w2 - t[2] * w1) / 256 / 4;
		}

	}



void CJpegSave::Rgb2Yuv( TUint32* aRgb, TInt /*aWidth*/, TInt* aY, TInt* aU, TInt* aV )
	{
	TInt pos = 0;
	TInt yy;
	TInt xx;
	TInt y;
	for( yy=0; yy<1; yy++ )
	for( xx=0; xx<2; xx++ )
	for( y=0; y<8; y++ )
		{
		TInt cpos = xx*4 + y*8;
		TInt x;
		for( x=0; x<8; x++ )
			{
			TUint32 c = aRgb[ x + xx*8 + y*16 + yy * 128 ];
			TInt r = ( c >> 16 ) & 0xff;
			TInt g = ( c >> 8 ) & 0xff;
			TInt b = c & 0xff;

			TInt cy = ( 19595*r + 38470*g + 7471*b ) >> 16;
			//TInt cy = 0.29900*r + 0.58700*g + 0.11400*b;
			//if( y<0 ) cy=0;
			//if( y>255 ) cy=255;
			aY[ pos++ ] = cy - 128;

			if( x & 1 )
				{
				TInt t = ( ( -11059*r - 21710*g + 32768*b ) >> 16 );
				//TInt t = -0.16874*r - 0.33126*g + 0.50000*b + 128;
				//if( t<-128 ) t=-128;
				//if( t>127 ) t=127;
				aU[ cpos ] = t;

				t = ( ( 32768*r - 27439*g - 5329*b ) >> 16 );
				//t = 0.50000*r - 0.41869*g - 0.08131*b + 128;
				//if( t<-128 ) t=-128;
				//if( t>127 ) t=127;
				aV[ cpos++ ] = t;
				}
			}
		}


	// 1:1:1 macroblock version
	/*
	TUint32* p = aRgb;
	TInt mod = ( aWidth - 8 );
	TInt pos = 0;
	TInt y;
	
	for( y=0; y<8; y++ )
		{
		TInt x;
		for( x=0; x<8; x++ )
			{
			TUint32 c = *p++;
			TInt r = c >> 16;
			TInt g = ( c >> 8 ) & 0xff;
			TInt b = c & 0xff;

			TInt y = ( 19595*r + 38470*g + 7471*b ) >> 16;
			//TInt y = 0.29900*r + 0.58700*g + 0.11400*b;
			if( y<0 ) y=0;
			if( y>255 ) y=255;
			aY[ pos ] = y - 128;
			

			TInt t = ( ( -11059*r - 21710*g + 32768*b ) >> 16 );
			//TInt t = -0.16874*r - 0.33126*g + 0.50000*b + 128;
			if( t<-128 ) t=-128;
			if( t>127 ) t=127;
			aU[ pos ] = t;

			t = ( ( 32768*r - 27439*g - 5329*b ) >> 16 );
			//t = 0.50000*r - 0.41869*g - 0.08131*b + 128;
			if( t<-128 ) t=-128;
			if( t>127 ) t=127;
			aV[ pos ] = t;
			pos++;
			}
		p += mod;
		}
*/

	}



void CJpegSave::MakeTables( TInt aQuality, TUint8* aY, TUint8* aUV )
	{
	TInt i;
	TInt factor = aQuality;

	if( aQuality < 1 ) factor = 1;
	if( aQuality > 99 ) factor = 99;
	if( aQuality < 50 )
		{
		aQuality = 5000 / factor;
		}
	else
		{
		aQuality = 200 - factor*2;
		}


	for( i=0; i < 64; i++ )
		{
		TInt y = ( jpeg_luma_quantizer[i] * aQuality + 50) / 100;
		TInt uv = ( jpeg_chroma_quantizer[i] * aQuality + 50) / 100;

		if( y < 1) y = 1;
		if( uv < 1) uv = 1;
		if( y > 255) y = 255;
		if( uv > 255) uv = 255;

		aY[ i ] = y;
		aUV[ i ] = uv;
		}
	}



void CJpegSave::WriteSaveBuffer( const TUint8* aSrc, TInt aBytes )
	{
	TInt pos = 0;
	while( aBytes )
		{
		TInt bytes = aBytes;
		if( iSaveBufPos + bytes > iSaveBufSize )
			{
			bytes = iSaveBufSize - iSaveBufPos;
			}
		Mem::Copy( iSaveBuf + iSaveBufPos, aSrc + pos, bytes );
		iSaveBufPos += bytes;
		if( iSaveBufPos == iSaveBufSize )
			{
			FlushSaveBuf();
			}
		aBytes -= bytes;
		pos += bytes;
		}
	}



void CJpegSave::WriteSaveBuffer( TUint8 aValue )
	{
	iSaveBuf[ iSaveBufPos++ ] = aValue;
	if( iSaveBufPos == iSaveBufSize )
		{
		FlushSaveBuf();
		}
	}



void CJpegSave::WriteSaveBuffer( TUint16 aValue )
	{
	WriteSaveBuffer( (TUint8) ( aValue / 256 ) );
	WriteSaveBuffer( (TUint8) ( aValue & 255 ) );
	}



void CJpegSave::FlushSaveBuf()
	{
	if( &iSaveFile == NULL ) return;		// no flushing without file
	TPtr8 ptr( iSaveBuf, iSaveBufPos );
	ptr.SetLength( iSaveBufPos );
	if( iSaveFile )iSaveFile->Write( ptr );
	iSaveBufPos = 0;
	}

