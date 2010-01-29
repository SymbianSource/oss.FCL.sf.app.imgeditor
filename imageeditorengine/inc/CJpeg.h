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
// Jpeg decoder class

#ifndef __CJPEG_H__
#define __CJPEG_H__

#include <e32base.h>
#include <f32file.h>
#include "TBitmapHandle.h"
#include "MJpegLoad.h"

class THuffman;




/// Main Jpeg decode class
class CJpeg
	: public CBase
	, public MJpegLoad
	{

	public:
		static CJpeg* NewL();
		static CJpeg* NewLC();
		~CJpeg();
		
	private:
		CJpeg();
		void ConstructL();

	public: // MJpegLoad
		
		void OpenL( const TFileName& aFile );
		void OpenL( const TPtr8& aData );

		void SetScale( TJpegScale aScale );
		TBitmapHandle LoadImageL( TRect& aRect );

		void ScanRandomL();			// scans file, creates random access tables
		TBitmapHandle LoadBlockL( const TPoint& aBlock );

		const TJpegData& Info();
		TPtrC8 ExifData();

	public:

		void EnableRgvConv();
		void DisableRgbConv();
		
	private:
		inline void BufFwd( TInt aBits );
		inline TInt BufBits( TInt aBits );
		inline TInt Buf16();
		inline void BufLoad8();
		void DecodeBlock();
		void DecodeBlock2();
		void DecodeBlock3();
		void Idct();
		void IdctHalf();
		void CreateHuffmanL( THuffman* aHuffman, const TUint8* aBits, const TUint8* aVal );
		void CreateDefaultHuffmanL();
		void PrepareLoadBlockL();	// must be called before first LoadBlock()
		
		//
		// Scale load functions:
		//
		void DecRgb1_1L( const TBitmapHandle& aBitmap, const TRect& aBlockRect );
		void DecRgb1_2L( const TBitmapHandle& aBitmap, const TRect& aBlockRect );
		void DecRgb1_4L( const TBitmapHandle& aBitmap, const TRect& aBlockRect );
		void DecRgb1_8L( const TBitmapHandle& aBitmap, const TRect& aBlockRect );

		//
		// Yuv -> Rgb functions
		// 
		
		// YUV420
		static void Yuv2Rgb22_11_11( TAny* aPtr );
		
		// YUV422
		static void Yuv2Rgb21_11_11( TAny* aPtr );

		// Any other YUV format
		static void Yuv2RgbFree( TAny* aPtr );
		
	private:

		/// Jpeg component information struct
		/// one for each color component
		class TComponent
			{
			public:
				char iType;
				char iXFactor;
				char iYFactor;
				char iQuantTable;
				char iID;
			};

		/// Current jpeg block search / use struct
		/// will change to type where the actual huffman data is also contained
		class TJpegBlock
			{
			public:
				TUint32 iOffset;
				TUint32 iBuf;
				TInt iBufBits;
				TInt iY;
				TInt iU;
				TInt iV;
			};

		class TYuvConv
			{
			public:
				TUint32* iRgb;
				TSize iBlkSize;
				TInt iRgbWidth;
				TInt iBlkPixels;
			};

		TUint8* iBuffer;			// contains whole jpeg file now
		TInt iBufPos;

		TUint32 iBuf;				// Huffman bit buffer
		TInt iBufBits;				// number of bits in bitbuffer

		THuffman* iHuffman[ 4 ];	// huffman lookup tables
		TInt iCurrentHuffman;

		TUint8* iQt[ 4 ];			// Quantization tables
		TUint8* iCurrentQt;

		TInt iDct[ 64 ];			// DCT coefficients
		TInt iBlk[ 64 ];			// IDCT result

		TBool iRst;					// RST flag, might occur on some jpegs
		
		bool iEOF;

		TJpegData iData;
		
		TComponent iComponent[ 4 ];
		TInt iNumComponents;

		//RArray< TJpegBlock >iBlock;
		TJpegBlock* iBlock;
		TInt		iNumBlocks;

		TUint8* iExifData;
		TInt iExifDataLength;

		// LoadBlock variables:
		TBitmapHandle	iBm;			// rgb block bitmap
		TInt			iBlkPixels;		// number of pixels in rgb block

		TUint8*			iC[ 3 ];		// Y,U,V buffer

		TInt iYxa;
		TInt iYya;
		TInt iUxa;
		TInt iUya;
		TInt iVxa;
		TInt iVya;

		bool iRgbConv;

		//RFs iFs;
		//RFile iDebug;

		TYuvConv iYuvConv;
		void (*iYuv2rgbFunc)(TAny*);


		TInt iImageDataStart;
		bool iRandomScanned;
		bool iLoadBlockPrepared;
		
		TJpegScale iScale;
		
		TInt iResetInterval;

		
	};


inline void CJpeg::BufFwd( TInt aBits )
	{
	iBuf <<= aBits;
	iBufBits -= aBits;
	}



inline TInt CJpeg::BufBits( TInt aBits )
	{
	if( aBits == 0 ) return 0;
	while( iBufBits < aBits )
		{
		BufLoad8();
		}
	TInt val = iBuf >> ( 32-aBits );
	BufFwd( aBits );
	return val;
	}



inline TInt CJpeg::Buf16()
	{
	while( iBufBits < 16 )
		{
		BufLoad8();
		}
	TInt val = iBuf >> ( 32-16 );
	return val;
	}



inline void CJpeg::BufLoad8()
	{
	TInt v;
	v = iBuffer[ iBufPos++ ];
	if( v == 255 )
		{
		v = iBuffer[ iBufPos++ ]; // escaped 0xff ?
		if( v == 0 )
			{
			v = 255;
			}
		else
			{
			//RDebug::Print( _L("Discard %x - position=%d bits=%d"), v, iBufPos, iBufBits );
			// here we have probably discarded a RST0..7
			iRst = ETrue;		// inform decoder of reset
			iBufBits = 255;		// decoder doesn't need any more real data until next mcu
			return;
			}
		}

	v <<= ( 24 - iBufBits );
	iBuf |= v;
	iBufBits += 8;		
	}




#endif
