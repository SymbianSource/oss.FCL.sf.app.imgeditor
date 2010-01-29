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


#ifndef __CJPROTATE_H__
#define __CJPROTATE_H__

#include <e32base.h>
//#include <TBitmapHandle.h>
#include <f32file.h>

class THuffman;


class TJpegData
	{
	public:
		TSize iSize;
		TSize iBlockSize;
		TSize iSizeInBlocks;
	};



class TJpegBasicBlock
	{
	public:
		TUint32 iOffset;
		TUint32 iBuf;
		TUint8 iBufBits;
		TInt16 iDc;
	};
	
	
	
/// Jpeg component information struct
/// one for each color component
class TComponent
	{
	public:
		char iType;
		char iXFactor;
		char iYFactor;
		char iQuantTable;
	};


/// save huffman
class TSHuffman
	{
	public:
		TInt8 iLength[ 256 ];
		TInt8 iCode[ 256 ];
	};


class MJpRotateCallBack;




/// Main Jpeg decode class
class CJpRotate
	: public CBase
	{
	public:
		static CJpRotate* NewL( RFs& aFs, RFile* aSaveFile, TInt aSaveBufSize );
		static CJpRotate* NewLC( RFs& aFs, RFile* aSaveFile, TInt aSaveBufSize );
		~CJpRotate();
		
	private:
		CJpRotate( RFs& aFs, RFile* aSaveFile, TInt aSaveBufSize );
		void ConstructL();

	public:
		
		void SetCallBack( MJpRotateCallBack* aCallBack );
		void RotateL( const TFileName& aFile, TBool aRotate, TBool aFlip, TBool aMirror );
		void RotateL( const TPtrC8& aData, TPtrC8& aTarget, TBool aRotate, TBool aFlip, TBool aMirror );
		void RotateL( const TPtrC8& aData, TBool aRotate, TBool aFlip, TBool aMirror );
		const TJpegData& Info();
		TPtrC8 ExifData();
		void Cancel();

	private:
		void BufFwd( TInt aBits );
		TInt BufBits( TInt aBits );
		TInt Buf16();
		void BufLoad8();
		void DecodeBlockL( TInt aDc );
		void DecodeBlock2L();

		void WriteHuffmanL( TInt aValue );
		void WriteBits( TUint32 aValue, TInt aNumBits );
		void CreateSaveHuffmanL( TSHuffman* aHuffman, const TUint8* aBits, const TUint8* aVal );
		void ConvertQuants( TUint8* aSrc, TUint8* aTgt );
		void WriteSaveBuffer( const TUint8* aSrc, TInt aBytes );
		void WriteSaveBuffer( TUint8 aValue );
		void WriteSaveBuffer( TUint16 aValue );

		void SaveBlocks();

		void FlushSaveBuf();
		
	private:

		TUint8* iBuffer;			// contains whole jpeg file now
		TInt iBufPos;

		TUint32 iBuf;				// Huffman bit buffer
		TInt iBufBits;				// number of bits in bitbuffer

		THuffman* iHuffman[ 4 ];	// huffman lookup tables
		TInt iCurrentHuffman;

		TUint8* iQt[ 4 ];			// Quantization tables
		TUint8* iCurrentQt;

		TInt iDct[ 64 ];			// DCT coefficients

		TBool iRst;					// RST flag, might occur on some jpegs
									// now only supports RST after line of blocks

        TBool iCancelled;

		bool iEOF;

		TJpegData iData;
		
		TComponent iComponent[ 4 ];
		TInt iNumComponents;

		TUint8* iExifData;
		TInt iExifDataLength;

		TSHuffman* iSaveHuffman[ 4 ];
		TSHuffman* iCurrentSaveHuffman;

		TUint8* iSaveBuf;
		TInt iSaveBufPos;
		TInt iSaveBufBitPos;
		TUint8 iSaveByte;

		bool iRotate;
		bool iFlip;
		bool iMirror;

		RArray< TJpegBasicBlock >iBasicBlock;

		RFs& iFs;
		RFile& iSaveFile;
		TInt iSaveBufSize;

		MJpRotateCallBack* iCallBack;

		bool iOwnBuffer;
		
	};

#endif
