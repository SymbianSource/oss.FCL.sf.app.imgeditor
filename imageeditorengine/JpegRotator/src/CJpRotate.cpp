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





#include "CJpRotate.h"
#include "MJpRotateCallBack.h"
#include "CExifParser.h"

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

const TUint8 KReZig[] = {
						0,2,1,5,4,3,9,8,
						7,6,14,13,12,11,10,20,
						19,18,17,16,15,27,26,25,
						24,23,22,21,35,34,33,32,
						31,30,29,28,42,41,40,39,
						38,37,36,48,47,46,45,44,
						43,53,52,51,50,49,57,56,
						55,54,60,59,58,62,61,63 
						};




/* Set up the standard Huffman tables (cf. JPEG standard section K.3) */
/* IMPORTANT: these are only valid for 8-bit data precision! */

const TUint8 bits_dc_luminance[17] =
{ /* 0-base */ 0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
const TUint8 val_dc_luminance[] =
{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

const TUint8 bits_dc_chrominance[17] =
{ /* 0-base */ 0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
const TUint8 val_dc_chrominance[] =
{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

const TUint8 bits_ac_luminance[17] =
{ /* 0-base */ 0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
const TUint8 val_ac_luminance[] =
{ 0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
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

const TUint8 bits_ac_chrominance[17] =
{ /* 0-base */ 0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };

const TUint8 val_ac_chrominance[] =
{ 0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
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

const TUint8 KExifHeader[] = "Exif";

class THuffman
	{
	public:
		TInt8 iLength[ 256 ];
		TUint8 iSymbol[ 256 ];
		TUint8 iSearch[ 65536 ];
	};


CJpRotate* CJpRotate::NewL( RFs& aFs, RFile* aSaveFile, TInt aSaveBufSize )
	{
	CJpRotate* self = NewLC( aFs, aSaveFile, aSaveBufSize );
	CleanupStack::Pop( self );
	return self;
	}


CJpRotate* CJpRotate::NewLC( RFs& aFs, RFile* aSaveFile, TInt aSaveBufSize )
	{
	CJpRotate* self = new( ELeave )CJpRotate( aFs, aSaveFile, aSaveBufSize );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}


CJpRotate::~CJpRotate()
	{
	delete iHuffman[ 0 ];
	delete iHuffman[ 1 ];
	delete iHuffman[ 2 ];
	delete iHuffman[ 3 ];

	delete iSaveHuffman[ 0 ];
	delete iSaveHuffman[ 1 ];
	delete iSaveHuffman[ 2 ];
	delete iSaveHuffman[ 3 ];

	delete iQt[ 0 ];
	delete iQt[ 1 ];
	delete iQt[ 2 ];
	delete iQt[ 3 ];

	if( iOwnBuffer )
		{
		delete iBuffer;
		}
	delete iSaveBuf;
	iBasicBlock.Reset();

	if( &iSaveFile != NULL )
		{
		iSaveFile.Flush();
		iSaveFile.Close();
		}
	}

CJpRotate::CJpRotate( RFs& aFs, RFile* aSaveFile, TInt aSaveBufSize )
	: iCancelled(EFalse)
	, iFs( aFs )
	, iSaveFile( *aSaveFile )
	, iSaveBufSize( aSaveBufSize )
	{

	}

void CJpRotate::ConstructL()
	{
	}



void CJpRotate::SetCallBack( MJpRotateCallBack* aCallBack )
	{
	iCallBack = aCallBack;
	}
		
void CJpRotate::Cancel()
    {
    iCancelled = ETrue;
    }



void CJpRotate::RotateL( const TFileName& aFile, TBool aRotate, TBool aFlip, TBool aMirror )
	{
	
	iCancelled = EFalse;
	
	if (iBuffer)
	{
		delete iBuffer;
		iBuffer = NULL;		
	}
	
	RFile file;
	TInt err = file.Open( iFs, aFile, EFileRead | EFileShareReadersOnly );
	if (KErrNone != err)
		{
		User::LeaveIfError( file.Open( iFs, aFile, EFileRead | EFileShareAny ) );
		}
	CleanupClosePushL( file );
	
	TInt size;
	file.Size( size );
	iOwnBuffer = true;
	iBuffer = new( ELeave )TUint8[ size + 2 ];
	TPtr8 ptr( iBuffer, size );
	file.Read( ptr );
	
	CleanupStack::PopAndDestroy( 1 ); // file
	RotateL( ptr, aRotate, aFlip, aMirror );
	}



void CJpRotate::RotateL( const TPtrC8& aData, TPtrC8& aTarget, TBool aRotate, TBool aFlip, TBool aMirror )
	{
	iOwnBuffer = false;
	RotateL( aData, aRotate, aFlip, aMirror );

	aTarget.Set( iSaveBuf, iSaveBufPos );
	}



void CJpRotate::RotateL( const TPtrC8& aData, TBool aRotate, TBool aFlip, TBool aMirror )
	{
	
	iFlip = aFlip;
	iMirror = aMirror;
	iRotate = aRotate;
	if( iRotate )
		{
		if( iFlip )
			{
			iFlip = false;
			}
		else
			{
			iFlip = true;
			}
		}
	
	
	iBuffer = (TUint8*)aData.Ptr();

	iSaveBuf = new( ELeave )TUint8[ iSaveBufSize ];
	//Mem::FillZ( iSaveBuf, iS );
	iSaveByte = 0;
	iSaveBufPos = 0;
	iSaveBufBitPos = 0;
	
	// if used again, all variables should be cleared
	iBuf = 0;
	iBufBits = 0;
	
	delete iHuffman[ 0 ];
	delete iHuffman[ 1 ];
	delete iHuffman[ 2 ];
	delete iHuffman[ 3 ];
	iHuffman[ 0 ] = 0;
	iHuffman[ 1 ] = 0;
	iHuffman[ 2 ] = 0;
	iHuffman[ 3 ] = 0;
	//CreateDefaultHuffmanL();


	iQt[ 0 ] = 0;
	iQt[ 1 ] = 0;
	iQt[ 2 ] = 0;
	iQt[ 3 ] = 0;


	TBool moreChunks = ETrue;
	TBool possibleChunk = EFalse;

	while( moreChunks )
		{
		if (iCancelled)
		    {
		    return;
		    }
		    
		TUint8 b = iBuffer[ iBufPos++ ];
		if( possibleChunk )
			{
			possibleChunk = EFalse;
			switch( b )
				{
				case 0xd8: // start of image
					{
					WriteSaveBuffer( (TUint16)0xffd8 );
					break;
					}
				case 0xe0: // JFIF application segment
					{
					WriteSaveBuffer( (TUint16)0xffe0 );
					TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					WriteSaveBuffer( (TUint16)l );
					WriteSaveBuffer( iBuffer + iBufPos, l-2 );
					iBufPos += l-2;
					break;
					}
				case 0xe1: // APP1 segment, possible EXIF
					{
					
					TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; 
					iBufPos += 2;
					l -= 2;
					
					// Check that there is "Exif" header in the data
					const TUint8* pos = &iBuffer[ iBufPos ];
					if (Mem::Compare(pos, 2, &KExifHeader[0], 2 ))
					{
						iBufPos += l;
						break;
					}
					
					iBufPos += 6;
					l -= 6;

					iExifData = iBuffer + iBufPos;
					iExifDataLength = l;
					
					CExifParser* p = CExifParser::NewLC();
					
					TRAP_IGNORE(p->ParseL( TPtrC8( iExifData, iExifDataLength ) ););
						
					TPtrC8 thumb( p->ThumbData() );
					TPtrC8 saveThumb( 0,0 );
					TUint8* thumbPtr = 0;
					
						
					if( iRotate )
						{
						//
						// swap width and height ( if exists )
						//
						if( p->TagExist( CExifParser::ESubIfd, 0xa002 ) &&
							p->TagExist( CExifParser::ESubIfd, 0xa003 ) )
							{
							TUint16 width = p->TagValue( CExifParser::ESubIfd, 0xa002 );
							TUint16 height = p->TagValue( CExifParser::ESubIfd, 0xa003 );

							p->DeleteTag( CExifParser::ESubIfd, 0xa002 );
							p->DeleteTag( CExifParser::ESubIfd, 0xa003 );

							p->AddTagL( CExifParser::ESubIfd, 0xa002, height );
							p->AddTagL( CExifParser::ESubIfd, 0xa003, width );
							}
						}

					//
					// Rotate thumbnail ( if exists )
					//
					if( thumb.Ptr() )
						{
						CJpRotate* r = CJpRotate::NewLC( iFs, NULL, 0x10000 );

						TPtrC8 target;
						r->RotateL( thumb, target, aRotate, aFlip, aMirror );
						
						thumbPtr = new( ELeave )TUint8[ target.Length() ];
						Mem::Copy( thumbPtr, target.Ptr(), target.Length() );
						saveThumb.Set( thumbPtr, target.Length() );

						CleanupStack::PopAndDestroy( r );
						CleanupStack::PushL( thumbPtr );
						}


					TPtrC8 exif = p->SaveL( saveThumb );
					
					WriteSaveBuffer( (TUint16)0xffe1 );	// APP1 segment

					TUint16 l2 = exif.Length() + 6 + 2; // +header+tagsize
					WriteSaveBuffer( l2 );

					// exif header is 6 bytes ( 45 78 69 66 00 00 "Exif.." )
					WriteSaveBuffer( (TUint8)0x45 );
					WriteSaveBuffer( (TUint8)0x78 );
					WriteSaveBuffer( (TUint8)0x69 );
					WriteSaveBuffer( (TUint8)0x66 );
					WriteSaveBuffer( (TUint8)0x00 );
					WriteSaveBuffer( (TUint8)0x00 );

					WriteSaveBuffer( exif.Ptr(), exif.Length() );

					if( thumbPtr )
						{
						CleanupStack::PopAndDestroy( thumbPtr );
						}
					CleanupStack::PopAndDestroy( p );


					iBufPos += l;
					break;
					}
				case 0xe2:
				case 0xe3:
				case 0xe4:
				case 0xe5:
				case 0xe6:
				case 0xe7:
				case 0xe8:
				case 0xe9:
				case 0xea:
				case 0xeb:
				case 0xec:
				case 0xed:
				case 0xee:
				case 0xef:
					{
					WriteSaveBuffer( (TUint8)0xff );
					WriteSaveBuffer( (TUint8)b );
					
					TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					WriteSaveBuffer( (TUint16)l );
					WriteSaveBuffer( iBuffer + iBufPos, l-2 );
					
					iBufPos += l-2;
					break;
					}
				case 0xdb: // Quantization table
					{
					TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					
					WriteSaveBuffer( (TUint16)0xffdb );
					WriteSaveBuffer( (TUint16)l );

					TInt nqt = l / 65;
					TInt i;

					// one or more quantization table
					for( i=0; i<nqt; i++ )
						{
						TUint8 t = iBuffer[ iBufPos++ ];
						TInt n = t & 15;
						WriteSaveBuffer( t );

						// not used:
						// TInt precision = n >> 4; 

						if (iQt[n])
						{
							delete iQt[ n ];
							iQt[ n ] = NULL;
						}

						iQt[ n ] = new( ELeave )TUint8[ 64 ];
						TInt j;
						for( j=0; j<64; j++ )
							{
							iQt[ n ][ j ] = iBuffer[ iBufPos++ ];
							}


						TUint8 qt[ 64 ];
						ConvertQuants( iQt[ n ], qt );
						WriteSaveBuffer( qt, 64 );
						}


					break;
					}
				case 0xc0: // start of frame ( SOF )
					{
					TInt length = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					
					WriteSaveBuffer( (TUint16)0xffc0 );
					WriteSaveBuffer( (TUint16)length );

					// not used:
					TUint8 precision = iBuffer[ iBufPos ];
					WriteSaveBuffer( precision );
					iBufPos++; 
					
					// height
					TUint16 h = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					iData.iSize.iHeight = h;
					
					// width
					TUint16 w = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					iData.iSize.iWidth = w;

					if( iRotate )
						{
						WriteSaveBuffer( w );
						WriteSaveBuffer( h );
						}
					else
						{
						WriteSaveBuffer( h );
						WriteSaveBuffer( w );
						}
					
					// component data
					iNumComponents = iBuffer[ iBufPos++ ];
					WriteSaveBuffer( (TUint8)iNumComponents );
					
					TInt i;
					for( i=0; i<iNumComponents; i++ )
						{
						TUint8 comp = iBuffer[ iBufPos++ ] - 1;
						TUint8 samplingFactor = iBuffer[ iBufPos++ ];
						TUint8 quantizationTable = iBuffer[ iBufPos++ ];
						iComponent[ comp ].iXFactor = samplingFactor >> 4;
						iComponent[ comp ].iYFactor = samplingFactor & 15;
						iComponent[ comp ].iQuantTable = quantizationTable;
						

						comp++;


						if( iRotate )
							{
							samplingFactor = samplingFactor/16 + ( samplingFactor & 15 )*16;
							}

						WriteSaveBuffer( comp );
						WriteSaveBuffer( samplingFactor );
						WriteSaveBuffer( quantizationTable );
						}
					
					TInt xBlocks = 0;
					TInt yBlocks = 0;

					for( i=0; i<iNumComponents; i++ )
						{
						TInt x = iComponent[ i ].iXFactor;
						TInt y = iComponent[ i ].iYFactor;
						if( x > xBlocks )
							{
							xBlocks = x;
							}
						if( y > yBlocks )
							{
							yBlocks = y;
							}
						}
					
					iData.iBlockSize.iWidth = xBlocks * 8;
					iData.iBlockSize.iHeight = yBlocks * 8;

					TSize size = iData.iSize;
					size.iWidth /= iData.iBlockSize.iWidth;
					size.iHeight /= iData.iBlockSize.iHeight;

					if( iData.iSize.iWidth & ( iData.iBlockSize.iWidth - 1 ) )
						{
						size.iWidth++;
						}

					if( iData.iSize.iHeight & ( iData.iBlockSize.iHeight - 1 ) )
						{
						size.iHeight++;
						}
					
					iData.iSizeInBlocks = size;


					break;
					}
				case 0xc1:
					{
					// Extended sequential Jpeg, not supported
					User::Leave( KErrNotSupported );
					break;
					};
				case 0xc2:
					{
					// Progressive DCT jpeg, not supported
					User::Leave( KErrNotSupported );
					break;
					};
				case 0xc3:
					{
					// Lossless ( sequential ) Jpeg, not supported
					User::Leave( KErrNotSupported );
					break;
					};
	
				case 0xc5:
				case 0xc6:
				case 0xc7:
				case 0xc8:
				case 0xc9:
				case 0xca:
				case 0xcb:
				case 0xcc:
				case 0xcd:
				case 0xce:
				case 0xcf:
					{
					User::Leave( KErrNotSupported );
					break;
					};
				case 0xc4: // huffman table
					{
					TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					

					//
					// Write default huffman tables
					//
					WriteSaveBuffer( (TUint16)0xffc4 );
					TUint16 saveLen = 2+1+16+12+1+16+12+1+16+162+1+16+162;
					WriteSaveBuffer( saveLen );
					
					WriteSaveBuffer( (TUint8)0 );
					WriteSaveBuffer( bits_dc_luminance+1, 16 );
					WriteSaveBuffer( val_dc_luminance, 12 );
					
					WriteSaveBuffer( (TUint8)1 );
					WriteSaveBuffer( bits_dc_chrominance+1, 16 );
					WriteSaveBuffer( val_dc_chrominance, 12 );
					
					WriteSaveBuffer( (TUint8)16 );
					WriteSaveBuffer( bits_ac_luminance+1, 16 );
					WriteSaveBuffer( val_ac_luminance, 20*8+2 );
					
					WriteSaveBuffer( (TUint8)17 );
					WriteSaveBuffer( bits_ac_chrominance+1, 16 );
					WriteSaveBuffer( val_ac_chrominance, 20*8+2 );
					
					TInt table = 0;

					TInt n = 0;

					// one or more huffman tables
					while( n<l-2 )
						{
						THuffman* huff = new( ELeave )THuffman;
					
						table = iBuffer[ iBufPos++ ];
						n++;
						TUint8 huffSize[ 16 ];
						TInt numSymbols = 0;
						TInt i;

						for( i=0; i<16; i++ )
							{
							TInt size = iBuffer[ iBufPos++ ];
							huffSize[ i ] = size;
							numSymbols += size;
							n++;
							}
						
						for( i=0; i<numSymbols; i++ )
							{
							TUint8 v = iBuffer[ iBufPos++ ];
							huff->iSymbol[ i ] = v;
							n++;
							}

						i = ( table & 16 ) / 8 + ( table & 15 );
												

						// Generate huffman lookup tables ( huffSize, table )


						TInt l;
						TInt p = 0;
						for( l=0; l<16; l++ )
							{
							for( i=0; i<huffSize[ l ]; i++ )
								{
								huff->iLength[ p++ ] = l+1;
								}
							}

						TInt code = 0;
						huff->iLength[ p ] = 0;
						TInt si = huff->iLength[ 0 ];
						TInt lastP = p;						
						
						TInt hc[ 256 ];
						p = 0;

						while( huff->iLength[ p ] )
							{
							while( huff->iLength[ p ] == si )
								{
								hc[ p++ ] = code++;
								}
							code *= 2;
							si++;
							}

						l = 65536;
						for( i=lastP-1; i>=0; i-- )
							{
							TInt t = 16 - huff->iLength[ i ];
							TInt k = hc[ i ] * ( 1 << t );
							TInt j;
							for( j=k; j<l; j++ )
								{
								huff->iSearch[ j ] = i;
								}
							l = k;
							}

						i = ( table & 16 ) / 8 + ( table & 15 );
						delete iHuffman[ i ];						
						iHuffman[ i ] = huff;
						}

					break;
					}
//
				case 0xda: // start of scan ( SOS )
					{
					// write restart interval before SOS
					/*
					WriteSaveBuffer( (TUint16)0xffdd );
					WriteSaveBuffer( (TUint16)0x0004 );
					WriteSaveBuffer( (TUint16)1 ); // reset every (1) macroblock
					*/
					// SOS now:
					WriteSaveBuffer( (TUint16)0xffda );
					TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;

					WriteSaveBuffer( (TUint16)l );

					TInt c1 = 0;
					TInt c2 = 0;
					TInt c3 = 0;
					
					TInt numComponents = iBuffer[ iBufPos++ ];
					WriteSaveBuffer( (TUint8)3 );
					TInt i;
					for( i=0; i<numComponents; i++ )
						{
						TInt componentId = iBuffer[ iBufPos++ ];
						TInt componentHt = iBuffer[ iBufPos++ ];
						}
					WriteSaveBuffer( (TUint8)1 );	// component 1
					WriteSaveBuffer( (TUint8)0 );
					
					WriteSaveBuffer( (TUint8)2 );	// component 2
					WriteSaveBuffer( (TUint8)17 );
					
					WriteSaveBuffer( (TUint8)3 );	// component 3
					WriteSaveBuffer( (TUint8)17 );

					WriteSaveBuffer( (TUint8)0 );
					WriteSaveBuffer( (TUint8)63 );	// dctsize - 1
					WriteSaveBuffer( (TUint8)0 );

					iBufPos += 3;
					
					//
					// Create default huffman tables for save
					//
					TSHuffman* huff = new( ELeave )TSHuffman;
					CreateSaveHuffmanL( huff, bits_dc_luminance, val_dc_luminance );
					iSaveHuffman[ 0 ] = huff;

					huff = new( ELeave )TSHuffman;
					CreateSaveHuffmanL( huff, bits_dc_chrominance, val_dc_chrominance );
					iSaveHuffman[ 1 ] = huff;

					huff = new( ELeave )TSHuffman;
					CreateSaveHuffmanL( huff, bits_ac_luminance, val_ac_luminance );
					iSaveHuffman[ 2 ] = huff;
					
					huff = new( ELeave )TSHuffman;
					CreateSaveHuffmanL( huff, bits_ac_chrominance, val_ac_chrominance );
					iSaveHuffman[ 3 ] = huff;

					

					// decode
					
					TInt bw = iData.iSizeInBlocks.iWidth;
					TInt bh = iData.iSizeInBlocks.iHeight;

					TInt bx;
					TInt by;

					for( by=0; by<bh; by++ )
						{
						for( bx=0; bx<bw; bx++ )
							{
							if( iRst )
								{
								// reset marker zeroes DC-values
								c1 = 0;
								c2 = 0;
								c3 = 0;
								iRst = EFalse;
								}
							
							TInt i;
							
							// go fast trough all huffman data
							iCurrentHuffman = 0;
							iCurrentQt = iQt[ 0 ];
							
							// Y-component
							TInt n = iComponent[ 0 ].iXFactor * iComponent[ 0 ].iYFactor;
							iDct[ 0 ] = c1;
							for( i=0; i<n; i++ )
								{
								DecodeBlock2L();
								}
							c1 = iDct[ 0 ];

							iCurrentHuffman = 1;
							
							iCurrentQt = iQt[ 1 ];
							
							// U-component
							n = iComponent[ 1 ].iXFactor * iComponent[ 1 ].iYFactor;
							iDct[ 0 ] = c2;
							for( i=0; i<n; i++ )
								{
								DecodeBlock2L();
								}
							c2 = iDct[ 0 ];

							iCurrentSaveHuffman = iSaveHuffman[ 1 ];
							// V-component
							n = iComponent[ 1 ].iXFactor * iComponent[ 1 ].iYFactor;
							iDct[ 0 ] = c3;
							for( i=0; i<n; i++ )
								{
								DecodeBlock2L();
								}
							c3 = iDct[ 0 ];

							// take care of RST0's leftover bits:
							if( iRst )
								{
								TInt goodBits = ( iBufBits >> 3 ) << 3;
								TInt badBits = iBufBits - goodBits;
								iBuf <<= badBits;
								iBufBits = goodBits;
								}
							}
						}
					
					moreChunks = EFalse;
					
					SaveBlocks();

					// flush last bits to save buffer
					if( iSaveBufBitPos )
						{
						WriteBits( 0, 8-iSaveBufBitPos );
						}

					// EOI
					


					//iSaveBuf[ iSaveBufPos++ ] = 0xff;
					//iSaveBuf[ iSaveBufPos++ ] = 0xd9;
					WriteSaveBuffer( (TUint16)0xffd9 );

					FlushSaveBuf();
					

					
					break;
					}
				case 0xd9: // end of image ( EOI )
					{
					// not really used for anything
					// will exit if picture data is read.
					break;
					}				
				case 0xdd: // define restart interval
					{
					//TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; 
					iBufPos += 2;
					//TInt interval = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; 
					iBufPos += 2;
					
					break;
					}
				case 0x00: // escaped 0xff
					{
					// only comes if file is broken somehow
					break;
					}
				case 0xfe: // jpeg comment
					{
					TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					WriteSaveBuffer( (TUint16)0xfffe );
					WriteSaveBuffer( (TUint16)l );
					WriteSaveBuffer( iBuffer + iBufPos, l-2 );
					iBufPos += l-2;
					break;
					};
				default:
					{
					// unknown block
					break;
					}
				}
			}
		else if( b == 255 )
			{
			possibleChunk = ETrue;
			}
			
		}
	
	}




void CJpRotate::BufFwd( TInt aBits )
	{
	iBuf <<= aBits;
	iBufBits -= aBits;
	}



TInt CJpRotate::BufBits( TInt aBits )
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



TInt CJpRotate::Buf16()
	{
	while( iBufBits < 16 )
		{
		BufLoad8();
		}
	TInt val = iBuf >> ( 32-16 );
	return val;
	}



void CJpRotate::BufLoad8()
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
		else if( v == 0xd9 ) // EOI
			{
			iEOF = true;
			v = 0;
			}
		else
			{
			// here we have probably discarded a RST0 or similar
			v = iBuffer[ iBufPos++ ];
			iRst = ETrue;
			}
		}

	v <<= ( 24 - iBufBits );
	iBuf |= v;
	iBufBits += 8;
	}



void CJpRotate::DecodeBlockL( TInt aDc )
	{
	
	//
	// Dummy version of block decode
	// only traverses through huffman data
	// and collects DC-values
	//
	TInt16 vals[ 64 ];
	TUint16 bits[ 64 ];
	TUint8 lens[ 64 ];

	Mem::FillZ( vals, 64*2 );
	Mem::FillZ( bits, 64*2 );
	Mem::FillZ( lens, 64 );

	//for( TInt i=0; i<64; i++ ) lens[ i ] = 255;


	THuffman* h = iHuffman[ iCurrentHuffman ];

	
	TInt k;
	for( k=0; k<64; k++ )
		{
		
		TInt v = Buf16();
		TInt index = h->iSearch[ v ];
		TInt symbol = h->iSymbol[ index ];
		BufFwd( h->iLength[ index ] );
		
		TInt nullCount = 0;

		if( k>0 )
			{
			nullCount = symbol >> 4;
			symbol &= 15;
			}
	

		v = BufBits( symbol );
		
		TUint16 currentBits = v;
		
		if( v < ( 1 << ( symbol-1 ) ) )
			{
			v += 1 - ( 1 << symbol );
			}


		if( k )
			{
			if( nullCount==0 && v==0 )
				{
				k = 64;
				break;
				}
			else if( nullCount==15 && v==0 )
				{
				k += 15;
				}
			else
				{
				k += nullCount;
				if( k > 63 ) break;
				TInt pos = k;
				if( iRotate ) pos = KReZig[ pos ];
				lens[ pos ] = symbol;
				vals[ pos ] = v;
				bits[ pos ] = currentBits;
				}
			}
		else
			{
			h = iHuffman[ iCurrentHuffman+2 ]; // DC -> AC huffman

			lens[ 0 ] = symbol;
			vals[ 0 ] = aDc / iCurrentQt[ 0 ];
			//vals[ 0 ] = aDc;
			bits[ 0 ] = currentBits;
			}
		}
	
	
	
	TInt count;
	for( count=63; count>=0; count-- )
		{
		if( lens[ count ] != 0 ) break;
		}
	if( count < 0 ) count = 0;

	TInt nullCount = 0;

	//
	// Write DC
	//
		
	TInt value = vals[ 0 ];
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
			
	WriteHuffmanL( vl );
	WriteBits( value, vl );
		
	// DC -> AC huffman
	if( iCurrentSaveHuffman == iSaveHuffman[ 0 ] )
		{
		iCurrentSaveHuffman = iSaveHuffman[ 2 ];
		}
	else if( iCurrentSaveHuffman == iSaveHuffman[ 1 ] )
		{
		iCurrentSaveHuffman = iSaveHuffman[ 3 ];
		}
	//
	// Write AC
	//
	for( k=1; k<=count; k++ )
		{

		if( vals[ k ] == 0 )
			{
			nullCount++;
			if( nullCount == 16 )
				{
				// write nullCount 15 with value 0
				WriteHuffmanL( 15 * 16 );
				nullCount = 0;
				}
			}
		else
			{
			// write bits & nullCount huffman coded
			
			TUint8 neg = 0;
			TInt p = KZigZag[ k ];
			if( p & 8 && iMirror ) neg = 1;
			if( p & 1 && iFlip ) neg ^= 1;
			
			TUint16 b = bits[ k ];
			TInt l = lens[ k ];
			TInt v = vals[ k ];
			if( neg )
				{
				if( v>0 )
					{
					b = v ^ ( ( 1 << l ) - 1 );	
					}
				else
					{
					b = -v;
					}
							
				}

			WriteHuffmanL( l + nullCount * 16 );
			WriteBits( b,l );
			
			nullCount = 0;
			}
		}
	if( count != 63 )
		{
		WriteHuffmanL( 0 );
		}

	}



void CJpRotate::DecodeBlock2L()
	{

	THuffman* h = iHuffman[ iCurrentHuffman ];

	TInt k;
	
	TJpegBasicBlock bl;
	bl.iOffset = iBufPos;
	bl.iBuf = iBuf;
	bl.iBufBits = iBufBits;


	for( k=0; k<64; k++ )
		{

		TInt v = Buf16();
		TInt index = h->iSearch[ v ];
		TInt symbol = h->iSymbol[ index ];
		BufFwd( h->iLength[ index ] );
		
		TInt nullCount = 0;
		if( k>0 )
			{
			nullCount = symbol >> 4;
			symbol &= 15;
			}
		
		v = BufBits( symbol );
					
		if( v < ( 1 << ( symbol-1 ) ) )
			{
			v += 1 - ( 1 << symbol );
			}
		
		if( k )
			{
			if( nullCount==0 && v==0 )
				{
				k = 64;
				break;
				}
			else if( nullCount==15 && v==0 )
				{
				k += 15;
				}
			else
				{
				k += nullCount;
				}
			}
		else
			{
			iDct[ 0 ] += v * iCurrentQt[ 0 ];
			h = iHuffman[ iCurrentHuffman+2 ];
			}

		}

	bl.iDc = iDct[ 0 ];
	
	User::LeaveIfError(iBasicBlock.Append( bl ));

	//Idct();
	}




const TJpegData& CJpRotate::Info()
	{
	return iData;
	}


TPtrC8 CJpRotate::ExifData()
	{
	return TPtrC8( iExifData, iExifDataLength );
	}




void CJpRotate::WriteHuffmanL( TInt aValue )
	{
	TInt code = iCurrentSaveHuffman->iCode[ aValue ];
	TInt len = iCurrentSaveHuffman->iLength[ aValue ];
	if( aValue != 0 && len == 0 )
		{
		//
		// The value which we tried to write doesn't exist in
		// the huffman table. Therefore the output would be
		// corrupted. Thus the leave.
		// actually this should not happen ever.
		//
		User::Leave( KErrCorrupt );
		}
	WriteBits( code, len );
	}



void CJpRotate::WriteBits( TUint32 aValue, TInt aNumBits )
	{

	aValue &= ( ( 1 << aNumBits ) - 1 );
	while( aNumBits > 0 )
		{
		TInt bitroom = 8 - iSaveBufBitPos;
		//iSaveBuf[ iSaveBufPos ] |= ( ( aValue << ( 24+bitroom-aNumBits ) ) >> 24 );
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


void CJpRotate::CreateSaveHuffmanL( TSHuffman* aHuffman, const TUint8* aBits, const TUint8* aVal )
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


void CJpRotate::ConvertQuants( TUint8* aSrc, TUint8* aTgt )
	{
	if( ! iRotate )
		{
		Mem::Copy( aTgt, aSrc, 64 );
		return;
		}

	for( TInt i=0; i<64; i++ )
		{
		aTgt[ KReZig[ i ] ] = aSrc[ i ];
		}
	}



void CJpRotate::WriteSaveBuffer( const TUint8* aSrc, TInt aBytes )
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


void CJpRotate::WriteSaveBuffer( TUint8 aValue )
	{
	iSaveBuf[ iSaveBufPos++ ] = aValue;
	if( iSaveBufPos == iSaveBufSize )
		{
		FlushSaveBuf();
		}
	}


void CJpRotate::WriteSaveBuffer( TUint16 aValue )
	{
	WriteSaveBuffer( (TUint8) ( aValue / 256 ) );
	WriteSaveBuffer( (TUint8) ( aValue & 255 ) );
	}


void CJpRotate::SaveBlocks()
	{
	TInt yBlocks = iComponent[ 0 ].iXFactor * iComponent[ 0 ].iYFactor;
	TInt uBlocks = iComponent[ 1 ].iXFactor * iComponent[ 1 ].iYFactor;
	TInt vBlocks = iComponent[ 2 ].iXFactor * iComponent[ 2 ].iYFactor;

	TInt blocks = yBlocks + uBlocks + vBlocks;
	TInt blockOffset[ 16 ];

	TInt x;
	TInt y;

	TInt i = 0;
	TInt b = 0;
	for( TInt comp = 0; comp<iNumComponents; comp++ )
		{
		TComponent& c = iComponent[ comp ];

		for( y=0; y<c.iYFactor; y++ )
			{
			for( x=0; x<c.iXFactor; x++ )
				{
				TInt xx = x;
				TInt yy = y;
				TInt fx = c.iXFactor;
				TInt fy = c.iYFactor;
				

				if( iRotate )
					{
					TInt t = xx; xx = yy; yy = t;
					if( iFlip ) yy = fx - 1 - yy;
					if( iMirror ) xx = fy - 1 - xx;
					t = fx; fx=fy; fy = t;
					}
				else
					{
					if( iFlip ) yy = fy - 1 - yy;
					if( iMirror ) xx = fx - 1 - xx;
					}

				TInt v = xx + yy * fx; //1302
				//while( v >= n ) v -= n;
				//blockOffset[ i++ ] = b + v;
				blockOffset[ b+v ] = i++;
				}
			}
		b += c.iXFactor*c.iYFactor;
		}
	TInt bw = iData.iSizeInBlocks.iWidth;
	TInt bh = iData.iSizeInBlocks.iHeight;
	
	TInt yDc = 0;
	TInt uDc = 0;
	TInt vDc = 0;

	TInt count = 0;
	
	TInt blockXAdd;
	TInt blockYAdd;
	TInt blockPos;

	if( iRotate )
		{
		if( iMirror )
			{
			if( iFlip )
				{
				blockPos = blocks * ( bw * bh - 1 );
				blockXAdd = -blocks * bw;
				blockYAdd = blocks * bw * bh - blocks;
				}
			else
				{
				blockPos = blocks * bw * ( bh - 1 );
				blockXAdd = -blocks * bw;
				blockYAdd = blocks * bw * bh + blocks;
				}
			}
		else
			{
			if( iFlip )
				{
				blockPos = blocks * ( bw - 1 );
				blockXAdd = blocks * bw;
				blockYAdd = -blocks * bw * bh - blocks;
				}
			else
				{
				blockPos = 0;
				blockXAdd = blocks * bw;
				blockYAdd = -blocks * ( bw * bh - 1 );
				}
			}
		TInt t = bw; bw = bh; bh = t;
		}
	else
		{
		if( iMirror )
			{
			if( iFlip )
				{
				blockPos = blocks * ( bw * bh - 1 );
				blockXAdd = -blocks;
				blockYAdd = 0;
				}
			else
				{
				blockPos = blocks * ( bw-1 );
				blockXAdd = -blocks;
				blockYAdd = bw * blocks * 2;
				}
			}
		else
			{
			if( iFlip )
				{
				blockPos = blocks * bw * ( bh-1 );
				blockXAdd = blocks;
				blockYAdd = -blocks*bw*2; 
				}
			else
				{
				blockPos = 0;
				blockXAdd = blocks;
				blockYAdd = 0;
				}
			}
		}

	TInt numMacroBlocks = bw*bh;

	for( y=0; y<bh; y++ )
		{
		for( x=0; x<bw; x++ )
			{

			iCurrentQt = iQt[ 0 ];
			TInt i = 0;
			TInt n;
			for( n=0; n<yBlocks; n++ )
				{
				TInt bo = blockOffset[ i++ ];
				TJpegBasicBlock& bl = iBasicBlock[ blockPos + bo ];
				iBuf = bl.iBuf;
				iBufPos = bl.iOffset;
				iBufBits = bl.iBufBits;

				iCurrentHuffman = 0;
				iCurrentSaveHuffman = iSaveHuffman[ 0 ];
				DecodeBlockL( bl.iDc - yDc );
				yDc = bl.iDc;
				}

			iCurrentQt = iQt[ 1 ];

			for( n=0; n<uBlocks; n++ )
				{
				TJpegBasicBlock& bl = iBasicBlock[ blockPos + blockOffset[ i++ ] ];
				iBuf = bl.iBuf;
				iBufPos = bl.iOffset;
				iBufBits = bl.iBufBits;

				iCurrentHuffman = 1;
				iCurrentSaveHuffman = iSaveHuffman[ 1 ];
				DecodeBlockL( bl.iDc - uDc );
				uDc = bl.iDc;
				}

			for( n=0; n<vBlocks; n++ )
				{
				TJpegBasicBlock& bl = iBasicBlock[ blockPos + blockOffset[ i++ ] ];
				iBuf = bl.iBuf;
				iBufPos = bl.iOffset;
				iBufBits = bl.iBufBits;

				iCurrentHuffman = 1;
				iCurrentSaveHuffman = iSaveHuffman[ 1 ];
				DecodeBlockL( bl.iDc - vDc );
				vDc = bl.iDc;
				}


			//
			// Rst ( 0xFFD0..0xFFD7 ) after each macroblock
			//
			/*
			if( iSaveBufBitPos ) 
				{
				WriteBits( 0,8-iSaveBufBitPos );
				}
			WriteSaveBuffer( (TUint16)( 0xffd0 + (count & 7 ) ) );
			
			yDc = 0;
			uDc = 0;
			vDc = 0;
			
			count++;
			*/

			blockPos += blockXAdd;

			if( iCallBack )
				{
				iCallBack->JpRotateStatus( count, numMacroBlocks );
				}

			}
		blockPos += blockYAdd;
		}

	//RDebug::Print( _L("last access block :%d"), mx );
	}



void CJpRotate::FlushSaveBuf()
	{
	if( &iSaveFile == NULL ) return;		// no flushing without file
	TPtr8 ptr( iSaveBuf, iSaveBufPos );
	ptr.SetLength( iSaveBufPos );
	iSaveFile.Write( ptr );
	iSaveBufPos = 0;
	}
