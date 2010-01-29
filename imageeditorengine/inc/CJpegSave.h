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


#ifndef __CJPEGSAVE_H__
#define __CJPEGSAVE_H__

#include <e32base.h>
#include <f32file.h>

#include "TBitmapHandle.h"
#include "MJpegSave.h"

class CFbsBitmap;
class TSHuffman;

class CJpegSave
	: public CBase
	, public MJpegSave
	{
	public:
		static CJpegSave* NewL( RFs* aFs, RFile* aFile );
		static CJpegSave* NewLC( RFs* aFs, RFile* aFile );
		~CJpegSave();

	private:
		CJpegSave( RFs* aFs, RFile* aFile );
		void ConstructL();

	public: // MJpegSave
		//void SaveL( const CFbsBitmap* aBitmap, const TDesC16& aFileName );
		void StartSaveL( const TSize& aSize, TPtr8 aExif, TInt aSaveBufferSize, TInt aQuality );
		void SaveBlock( const TBitmapHandle& aBitmap );
		void FinalizeSave();
		TPtrC8 Finalize();

	private:
		void WriteHuffman( TInt aValue );
		void CreateHuffmanL( TSHuffman* aHuffman, const TUint8* aBits, const TUint8* aVal );
		void WriteBits( TUint32 aValue, TInt aNumBits );
		TInt WriteDct( TInt* aDct, TSHuffman* aDc, TSHuffman* aAc );
		void Dct( TInt* aSource, TInt* aTarget );
		void Rgb2Yuv( TUint32* aRgb, TInt aWidth, TInt* aY, TInt* aU, TInt* aV );
		void MakeTables( TInt aQuality, TUint8* aY, TUint8* aUV );

		void WriteSaveBuffer( const TUint8* aSrc, TInt aBytes );
		void WriteSaveBuffer( TUint8 aValue );
		void WriteSaveBuffer( TUint16 aValue );
		void FlushSaveBuf();

	private:

		RFs* iFs;
		RFile* iSaveFile;
		TInt iSaveBufSize;
		TUint8* iSaveBuf;
		TInt iSaveBufPos;
		TInt iSaveBufBitPos;
		TUint8 iSaveByte;
/*
		TUint8* iBuffer;
		TInt iBufPos;
		TInt iBufBitPos;
*/
		TSHuffman* iHuffman[ 4 ];
		TSHuffman* iCurrentHuffman;
		TUint8* iCurrentQuant;

		TInt iDctMul[ 64 ][ 64 ];

		TInt iDy;
		TInt iDu;
		TInt iDv;

		TUint8 iYQuant[ 64 ];
		TUint8 iUVQuant[ 64 ];

	};

#endif
