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



#include "CJpeg.h"
#include <e32svr.h>


const TUint8 KQuantIndex[ 3 ] = { 0,1,1 };
const TUint8 KHuffIndex[ 3 ] = { 0,1,1 };




const TUint8 KZigZag[] = {
						 0, 8,   1,  2,  9, 16, 24, 17,
						10, 3,   4, 11, 18, 25, 32, 40,
						33, 26, 19, 12,  5,  6, 13, 20,
						27, 34, 41, 48, 56, 49, 42, 35,
						28, 21, 14,  7, 15, 22, 29, 36,
						43, 50, 57, 58, 51, 44, 37, 30,
						23, 31, 38, 45, 52, 59, 60, 53,
						46, 39, 47, 54, 61, 62, 55, 63
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


CJpeg* CJpeg::NewL()
	{
	CJpeg* self = NewLC();
	CleanupStack::Pop( self );
	return self;
	}


CJpeg* CJpeg::NewLC()
	{
	CJpeg* self = new( ELeave )CJpeg();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}


CJpeg::~CJpeg()
	{
	delete iHuffman[ 0 ];
	delete iHuffman[ 1 ];
	delete iHuffman[ 2 ];
	delete iHuffman[ 3 ];

	delete iQt[ 0 ];
	delete iQt[ 1 ];
	delete iQt[ 2 ];
	delete iQt[ 3 ];

	iCurrentQt = NULL;
	iExifData = NULL;

	//iBlock.Reset();
	delete iBlock;

	delete iBuffer;
	delete iC[ 0 ];
	delete iC[ 1 ];
	delete iC[ 2 ];

	//iDebug.Close();
	//iFs.Close();
	}

CJpeg::CJpeg()
	{

	}

void CJpeg::ConstructL()
	{
	}




void CJpeg::OpenL( const TFileName& aFile )
	{
	if (iBuffer)
		{
		delete iBuffer;
		iBuffer = NULL;
		}
	
	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );
	
	RFile file;
	TInt err = file.Open( fs, aFile, EFileRead | EFileShareReadersOnly );
	if (KErrNone != err)
		{
		User::LeaveIfError( file.Open( fs, aFile, EFileRead | EFileShareAny ) );
		}
	CleanupClosePushL( file );
	
	TInt size;
	file.Size( size );
	iBuffer = new( ELeave )TUint8[ size + 2 ];
	TPtr8 ptr( iBuffer, size );
	file.Read( ptr );
	
	CleanupStack::PopAndDestroy( 2 ); // file, fs
	OpenL( ptr );
	}



void CJpeg::OpenL( const TPtr8& aData )
	{
	//iFs.Connect();
	//iDebug.Replace( iFs, _L("e:\\TestData\\jpdeb.txt"), EFileWrite );

	EnableRgvConv();

	iBuffer = (TUint8*)aData.Ptr();
	
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

	//iDebug.Write( _L8("d1\n") );

	//
	// This is only needed if no huffman tables found
	//
	//CreateDefaultHuffmanL();


	iQt[ 0 ] = 0;
	iQt[ 1 ] = 0;
	iQt[ 2 ] = 0;
	iQt[ 3 ] = 0;

	//

	//TBitmapHandle bm;
	//bm.iData = 0;

	TBool moreChunks = ETrue;
	TBool possibleChunk = EFalse;

//	TInt pos;

	while( moreChunks )
		{
		TUint8 b = iBuffer[ iBufPos++ ];
		if( possibleChunk )
			{
			possibleChunk = EFalse;
			switch( b )
				{
				case 0xd8: // start of image
					{
				    //RDebug::Print( _L("--Jpeg start of image") );
					//iDebug.Write( _L8("d2\n") );
					break;
					}
				case 0xe0: // JFIF application segment
					{
					//RDebug::Print( _L("--Jpeg application segment") );
					//iDebug.Write( _L8("d3\n") );
					break;
					}
				case 0xe1: // APP1 segment, possible EXIF
					{
					//iDebug.Write( _L8("d4\n") );
					//RDebug::Print( _L("--Jpeg app1 segment") );
					TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					l -= 2;

					// this could be used to read exif data:
					//TPtrC8 exifData( iBuffer + iBufPos, l );

					// Check that there is "Exif" header in the data
					const TUint8* pos = &iBuffer[ iBufPos ];
					if (Mem::Compare(pos, 2, &KExifHeader[0], 2 ))
					{
						iBufPos += l;
						break;
					}

					// suppose there is exif here
					// exif header is 6 bytes ( 45 78 69 66 00 00 "Exif.." )
					iBufPos += 6;
					l -= 6;

					iExifData = iBuffer + iBufPos;
					iExifDataLength = l;

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
					//iDebug.Write( _L8("d5\n") );
					//
					// Unused segment, skip
					// 
					//RDebug::Print( _L("--Jpeg unused tag %x"), b );
					TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;

					iBufPos += l-2;
					break;
					}
				case 0xdb: // Quantization table
					{
					//iDebug.Write( _L8("d6\n") );
					
					//RDebug::Print( _L("--Jpeg quant table") );
					TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					////RDebug::Print( _L("Start:%x length:%d"), iBufPos-2, l );
					TInt nqt = l / 65;
					TInt i;

					// one or more quantization table
					for( i=0; i<nqt; i++ )
						{
						TUint8 t = iBuffer[ iBufPos++ ];
						TInt n = t & 15;

						// not used:
						// TInt precision = n >> 4; 

						if (iQt[ n ])
						{
							delete iQt[ n ];
							iQt[ n ] = NULL;							
						}
						
						iQt[ n ] = new( ELeave )TUint8[ 64 ];
						TInt j;
						for( j=0; j<64; j++ )
							{
							iQt[ n ][ j ] = iBuffer[ iBufPos++ ];
							//RDebug::Print( _L("Quant %d:first=%d"), j, iQt[ n ][ j ] );
							}
						}

					break;
					}
				case 0xc0: // start of frame ( SOF )
					{
					//iDebug.Write( _L8("d7\n") );

					//RDebug::Print( _L("--Jpeg start of frame") );

					//TInt length = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; 
					iBufPos += 2;

					////RDebug::Print( _L("Start:%x length:%d"), iBufPos-2, length );

					// not used:
					// TInt precision = iBuffer[ iBufPos ];
					iBufPos++; 
					
					// height
					TInt t = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					iData.iSize.iHeight = t;
					
					// width
					t = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					iData.iSize.iWidth = t;
					
					// component data
					iNumComponents = iBuffer[ iBufPos++ ];
					
					TInt i;
					for( i=0; i<iNumComponents; i++ )
						{
						TInt compID = iBuffer[ iBufPos++ ];
						
						TInt samplingFactor = iBuffer[ iBufPos++ ];
						TInt quantizationTable = iBuffer[ iBufPos++ ];
						iComponent[ i ].iXFactor = samplingFactor >> 4;
						iComponent[ i ].iYFactor = samplingFactor & 15;
						iComponent[ i ].iQuantTable = quantizationTable;
						iComponent[ i ].iID = compID;
						//RDebug::Print( _L("component %d = %d"), i, compID );
						//RDebug::Print( _L("component %d quanttable=%d"), i, quantizationTable );
						//RDebug::Print( _L("component %d factor = %d,%d"), iComponent[ i ].iXFactor, iComponent[ i ].iYFactor );
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
					//iDebug.Write( _L8("d8\n") );
					//RDebug::Print( _L("--Jpeg tag 0xc1, extended sequential, unsupported") );

					// Extended sequential Jpeg, not supported
					User::Leave( KErrNotSupported );
					break;
					};
				case 0xc2:
					{
					//iDebug.Write( _L8("d9\n") );
					//RDebug::Print( _L("--Jpeg tag 0xc2, Progressive DCT, unsupported") );

					// Progressive DCT jpeg, not supported
					User::Leave( KErrNotSupported );
					break;
					};
				case 0xc3:
					{
					//iDebug.Write( _L8("d10\n") );
					
					//RDebug::Print( _L("--Jpeg tag 0xc1, Lossless, unsupported") );
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
					//iDebug.Write( _L8("d11\n") );
					
					//RDebug::Print( _L("--Jpeg tag %x, unknown"), b );
					User::Leave( KErrNotSupported );
					break;
					};
				case 0xc4: // huffman table
					{
					//iDebug.Write( _L8("d12\n") );

					//RDebug::Print( _L("--Jpeg huffman table") );
					TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					
					TInt table = 0;

					TInt n = 0;

					// one or more huffman tables
					while( n<l-2 )
						{
						THuffman* huff = new( ELeave )THuffman;
					
						table = iBuffer[ iBufPos++ ];
						n++;
						TInt huffSize[ 16 ];
						TInt numSymbols = 0;
						TInt i;

						for( i=0; i<16; i++ )
							{
							TInt size = iBuffer[ iBufPos++ ];
							huffSize[ i ] = size;
							numSymbols += size;
							n++;
							}

						////RDebug::Print( _L("       # of symbols %d"), numSymbols );
						
						for( i=0; i<numSymbols; i++ )
							{
							TUint8 v = iBuffer[ iBufPos++ ];
							huff->iSymbol[ i ] = v;
							n++;
							}
						

						// Generate huffman lookup tables ( huffSize, table )


						TInt ll;
						TInt p = 0;
						for( ll=0; ll<16; ll++ )
							{
							for( i=0; i<huffSize[ ll ]; i++ )
								{
								huff->iLength[ p++ ] = ll+1;
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

						ll = 65536;
						for( i=lastP-1; i>=0; i-- )
							{
							TInt t = 16 - huff->iLength[ i ];
							TInt k = hc[ i ] * ( 1 << t );
							TInt j;
							for( j=k; j<ll; j++ )
								{
								huff->iSearch[ j ] = i;
								}
							ll = k;
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
					//iDebug.Write( _L8("d13\n") );

					//RDebug::Print( _L("--Jpeg start of scan") );
					//TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					iBufPos += 2;
					
					TInt numComponents = iBuffer[ iBufPos++ ];
					//RDebug::Print( _L("JPEG: number of components=%d"), numComponents );
					if( numComponents != iNumComponents )
						{
						// non-interleaved( planar ) not supported
						User::Leave( KErrNotSupported );
						}
					//RDebug::Print( _L("Number of components=%d"), numComponents );
					
					TInt componentHt[ 3 ];
					
					TInt i;
					for( i=0; i<numComponents; i++ )
						{
						TInt componentId = iBuffer[ iBufPos++ ];
						// find index by ID
						TInt index = -1;
						for( TInt j=0; j<iNumComponents; j++ )
							{
							if( iComponent[ j ].iID == componentId )
								{
								index = j;
								}
							}
						if( index == -1 )
							{
							// ID not found
							User::Leave( KErrNotSupported );
							}
						componentHt[ index ] = iBuffer[ iBufPos++ ];
						//RDebug::Print( _L("id=%d, ht=%d"), componentId, componentHt[ i ] );
						}
						
					if( componentHt[ 1 ] == 0 || componentHt[ 2 ] == 0 )
						{
						// images with same huffman for all components
						// not yet supported
						//RDebug::Print( _L("jpeg not supported") );
						User::Leave( KErrNotSupported );
						}						
					

					if( iHuffman[ 0 ] == NULL )
						{
						// no huffman tables found, create default tables
						CreateDefaultHuffmanL();
						}
					

					iBufPos += 3;

					iImageDataStart = iBufPos;
					iRandomScanned = false;
					
					moreChunks = EFalse;

					break;
					}
				case 0xd9: // end of image ( EOI )
					{
					//RDebug::Print( _L("--Jpeg end of image") );
					// not really used for anything
					// will exit if picture data is read.
					break;
					}				
				case 0xdd: // define restart interval
					{
					//RDebug::Print( _L("--Jpeg define restart interval") );
					
					//TInt l = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					iBufPos += 2;
					
					TInt interval = 256 * iBuffer[ iBufPos ] + iBuffer[ iBufPos + 1 ]; iBufPos += 2;
					//iBufPos += 2;
					//RDebug::Print( _L("       interval = %d"), interval );
					iResetInterval = interval;
					
					// not really used for anything
					// restart markers are just handled when they come
					break;
					}
				case 0x00: // escaped 0xff
					{
					// only comes if file is broken somehow
					break;
					}
				default:
					{
					// unknown jpeg tag
					//RDebug::Print( _L("unknown jpeg tag %x"), b );
					break;
					}
				}
			}
		else if( b == 255 )
			{
			possibleChunk = ETrue;
			}
			
		}

	PrepareLoadBlockL();
	}



void CJpeg::PrepareLoadBlockL()
	{
	if( iLoadBlockPrepared )
		{
		return;
		}

	// prepare rgb block bitmap
	iBm.iSize = iData.iBlockSize;
	iBm.iDrawRect = iBm.iSize;
	iBlkPixels = iData.iBlockSize.iWidth * iData.iBlockSize.iHeight;
	iBm.iData = NULL;
	iBm.iType = E16MColor;
	

	// prepare yuv buffers
	iC[ 0 ] = new( ELeave )TUint8[ 64 * iComponent[ 0 ].iXFactor * iComponent[ 0 ].iYFactor ];
	iC[ 1 ] = new( ELeave )TUint8[ 64 * iComponent[ 1 ].iXFactor * iComponent[ 1 ].iYFactor ];
	iC[ 2 ] = new( ELeave )TUint8[ 64 * iComponent[ 2 ].iXFactor * iComponent[ 2 ].iYFactor ];

	// prepare yuv->rgb scale
	TInt bw = iBm.iSize.iWidth;
	TInt bh = iBm.iSize.iHeight;
	iYxa = 256 * ( 8 * iComponent[ 0 ].iXFactor ) / bw;
	iYya = 256 * ( 8 * iComponent[ 0 ].iYFactor ) / bh;
	iUxa = 256 * ( 8 * iComponent[ 1 ].iXFactor ) / bw;
	iUya = 256 * ( 8 * iComponent[ 1 ].iYFactor ) / bh;
	iVxa = 256 * ( 8 * iComponent[ 2 ].iXFactor ) / bw;
	iVya = 256 * ( 8 * iComponent[ 2 ].iYFactor ) / bh;

	//
	// Select YUV -> RGB function
	//
	iYuv2rgbFunc = Yuv2RgbFree;	// default, handles all combinations
	
	if( iComponent[ 0 ].iXFactor == 2 && iComponent[ 0 ].iYFactor == 1 &&
		iComponent[ 1 ].iXFactor == 1 && iComponent[ 1 ].iYFactor == 1 &&
		iComponent[ 2 ].iXFactor == 1 && iComponent[ 2 ].iYFactor == 1 )
		{
		iYuv2rgbFunc = Yuv2Rgb21_11_11;
		}
	else if( iComponent[ 0 ].iXFactor == 2 && iComponent[ 0 ].iYFactor == 2 &&
			 iComponent[ 1 ].iXFactor == 1 && iComponent[ 1 ].iYFactor == 1 &&
			 iComponent[ 2 ].iXFactor == 1 && iComponent[ 2 ].iYFactor == 1 )
		{
		iYuv2rgbFunc = Yuv2Rgb22_11_11;
		}

	iLoadBlockPrepared = true;
	}



void CJpeg::ScanRandomL()
	{
	if( iRandomScanned )
		{
		return;
		}
	//
	// Set file read position
	//
	iBufPos = iImageDataStart;
	iBufBits = 0;
	iBuf = 0;

	//
	// Reserve memory for random access tables
	//
	TInt numBlocks = iData.iSizeInBlocks.iWidth * iData.iSizeInBlocks.iHeight;
	iBlock = new( ELeave )TJpegBlock[ numBlocks ];
						
	
	TInt bw = iData.iSizeInBlocks.iWidth;
	TInt bh = iData.iSizeInBlocks.iHeight;

	TInt bx;
	TInt by;

	TInt c1 = 0;
	TInt c2 = 0;
	TInt c3 = 0;

	//TInt blockNum = 0;

	for( by=0; by<bh; by++ )
		{
		////RDebug::Print( _L("Jpeg scan %d/%d"), by+1,bh );
		
		for( bx=0; bx<bw; bx++ )
			{

			if( iRst )
				{
				iRst = false;
				iBuf = 0;
				iBufBits = 0;
				
				c1 = 0;
				c2 = 0;
				c3 = 0;
				}
			
			// store information for every block 
			TJpegBlock& block = iBlock[ iNumBlocks++ ];
			
			block.iY = c1;
			block.iU = c2;
			block.iV = c3;
			block.iOffset = iBufPos;
			block.iBuf = iBuf;
			block.iBufBits = iBufBits;
			
			//iBlock.Append( block );
			
			TInt i;
			
			// go fast trough all huffman data
			iCurrentHuffman = 0;
			iCurrentQt = iQt[ 0 ];
			
			// Y-component
			TInt n = iComponent[ 0 ].iXFactor * iComponent[ 0 ].iYFactor;
			iDct[ 0 ] = c1;
			for( i=0; i<n; i++ )
				{
				DecodeBlock();
				}
			c1 = iDct[ 0 ];
			
			iCurrentHuffman = 1;
			iCurrentQt = iQt[ 1 ];
			
			// U-component
			n = iComponent[ 1 ].iXFactor * iComponent[ 1 ].iYFactor;
			iDct[ 0 ] = c2;
			for( i=0; i<n; i++ )
				{
				DecodeBlock();
				}
			c2 = iDct[ 0 ];

			// V-component
			n = iComponent[ 1 ].iXFactor * iComponent[ 1 ].iYFactor;
			iDct[ 0 ] = c3;
			for( i=0; i<n; i++ )
				{
				DecodeBlock();
				}
			c3 = iDct[ 0 ];

			}
		}

	iRandomScanned = true;
	
	}



TBitmapHandle CJpeg::LoadBlockL( const TPoint& aBlock )
	{
	TBitmapHandle bm = iBm;
	bm.iData = new( ELeave )TUint32[ iBlkPixels ];
	CleanupStack::PushL(bm.iData);
	TUint32* rgb = (TUint32*) bm.iData;
	
	TInt blkNum = aBlock.iX + aBlock.iY * iData.iSizeInBlocks.iWidth;

	if( aBlock.iX < 0 || aBlock.iX >= iData.iSizeInBlocks.iWidth )
		{
		blkNum = -1;
		}
	//if( blkNum < 0 || blkNum >= iBlock.Count() )
	if( blkNum < 0 || blkNum >= iNumBlocks )
		{
		Mem::FillZ( bm.iData, iBlkPixels * sizeof( TUint32 ) );
		return bm;
		}

	TJpegBlock& blk = iBlock[ blkNum ];

	iBuf = blk.iBuf;
	iBufBits = blk.iBufBits;
	iBufPos = blk.iOffset;
	
	//
	// Decode block
	//

	TInt c1 = blk.iY;
	TInt c2 = blk.iU;
	TInt c3 = blk.iV;
	
	
	///////////
	TInt x;
	TInt y;
	TInt xx;
	TInt yy;
	TInt w;
	TInt* p;

	// Y-component
	iCurrentHuffman = 0;
	iCurrentQt = iQt[ 0 ];	
	xx = iComponent[ 0 ].iXFactor;
	yy = iComponent[ 0 ].iYFactor;
	w = xx * 8;
	p = iBlk;
	iDct[ 0 ] = c1;

	for( y=0; y<yy; y++ )
		{
		for( x=0; x<xx; x++ )
			{
			DecodeBlock2();
			TUint8* tp = iC[ 0 ] + x*8 + y * 8 * w;
			p = iBlk;

			for( TInt ty=0; ty<8; ty++ )
				{
				for( TInt tx=0; tx<8; tx++ )
					{
					tp[ tx + ty * w ] = *p++;
					}
				}
			}
		}
	

	// U-component
	iCurrentHuffman = 1;
	iCurrentQt = iQt[ 1 ];	
	xx = iComponent[ 1 ].iXFactor;
	yy = iComponent[ 1 ].iYFactor;
	w = xx * 8;
	p = iBlk;
	iDct[ 0 ] = c2;

	for( y=0; y<yy; y++ )
		{
		for( x=0; x<xx; x++ )
			{
			DecodeBlock2();
			TUint8* tp = iC[ 1 ] + x*8 + y * 8 * w;
			p = iBlk;

			for( TInt ty=0; ty<8; ty++ )
				{
				for( TInt tx=0; tx<8; tx++ )
					{
					tp[ tx + ty * w ] = *p++;
					}
				}
			}
		}

	// V-component
	iCurrentHuffman = 1;
	iCurrentQt = iQt[ 1 ];	
	xx = iComponent[ 2 ].iXFactor;
	yy = iComponent[ 2 ].iYFactor;
	w = xx * 8;
	iDct[ 0 ] = c3;
	
	for( y=0; y<yy; y++ )
		{
		for( x=0; x<xx; x++ )
			{
			DecodeBlock2();
			TUint8* tp = iC[ 2 ] + x*8 + y * 8 * w;
			
			p = iBlk;
			for( TInt ty=0; ty<8; ty++ )
				{
				for( TInt tx=0; tx<8; tx++ )
					{
					tp[ tx + ty * w ] = *p++;
					}
				}
			}
		}
	///////////

	//
	// Scaled blit YUV->RGB
	//

	TInt bw = bm.iSize.iWidth;
	TInt bh = bm.iSize.iHeight;

	
	TInt y1y = 0;
	TInt y1u = 0;
	TInt y1v = 0;

	TInt yw = iComponent[ 0 ].iXFactor * 8;
	TInt uw = iComponent[ 1 ].iXFactor * 8;
	TInt vw = iComponent[ 2 ].iXFactor * 8;
	
	TUint32* prgb = rgb;
	for( y=0; y<bh; y++ )
		{
		TInt txy = 0;
		TInt txu = 0;
		TInt txv = 0;
		TUint8* ccy = iC[ 0 ] + ( y1y/256 ) * yw;
		TUint8* ccu = iC[ 1 ] + ( y1u/256 ) * uw;
		TUint8* ccv = iC[ 2 ] + ( y1v/256 ) * vw;

		for( x=0; x<bw; x++ )
			{

			TInt cy = ccy[ txy / 256 ];
			TInt cu = ccu[ txu / 256 ] - 128;
			TInt cv = ccv[ txv / 256 ] - 128;
			
			txy += iYxa;
			txu += iUxa;
			txv += iVxa;

			TInt cr = cy + ( 91881 * cv ) / 65536;
			TInt cg = cy - ( 22554 * cu  +  46802 * cv ) / 65536;
			TInt cb = cy + ( 116130 * cu ) / 65536;
			
			if( cr < 0 ) cr = 0;
			if( cg < 0 ) cg = 0;
			if( cb < 0 ) cb = 0;
			if( cr > 255 ) cr = 255;
			if( cg > 255 ) cg = 255;
			if( cb > 255 ) cb = 255;

			//rgb[ x + y * bw ] = cr * 65536 + cg * 256 + cb;
			*prgb++ = cr * 65536 + cg * 256 + cb;
			}
		y1y += iYya;
		y1u += iUya;
		y1v += iVya;
		}

	CleanupStack::Pop();
	return bm;
	}




void CJpeg::DecodeBlock()
	{
	// //RDebug::Print( _L("DecodeBlock()") );
	//
	// Dummy version of block decode
	// only traverses through huffman data
	// and collects DC-values
	//
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
	}



void CJpeg::DecodeBlock2()
	{
	//
	// Real block decoder
	// fills iDct table and calls Idct() function
	//
	THuffman* h = iHuffman[ iCurrentHuffman ];

	TInt k;
	
	for( k=1; k<64; k++ )
		{
		iDct[ k ] = 0;
		}

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
				iDct[ KZigZag[ k ] ] = v * iCurrentQt[ k ];
				}
			}
		else
			{
			iDct[ 0 ] += v * iCurrentQt[ 0 ];
			h = iHuffman[ iCurrentHuffman+2 ];
			}

		}
	Idct();
	}



void CJpeg::DecodeBlock3()
	{
	//
	// Real block decoder
	// fills iDct table and calls Idct() function
	//
	THuffman* h = iHuffman[ iCurrentHuffman ];

	TInt k;
	
	for( k=1; k<64; k++ )
		{
		iDct[ k ] = 0;
		}

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
				iDct[ KZigZag[ k ] ] = v * iCurrentQt[ k ];
				}
			}
		else
			{
			iDct[ 0 ] += v * iCurrentQt[ 0 ];
			h = iHuffman[ iCurrentHuffman+2 ];
			}

		}
	}




void CJpeg::Idct()
	{
	TInt even[ 4 ];
	TInt odd[ 4 ];
	TInt res[ 64 ];

	TInt* p = iDct;
    
    for( TInt x=0; x<8; x++ )
		{
		TInt x0 = *p++;
		TInt x1 = *p++;
		TInt x2 = *p++;
		TInt x3 = *p++;
		TInt x4 = *p++;
		TInt x5 = *p++;
		TInt x6 = *p++;
		TInt x7 = *p++;
        
        TInt tx0 = x0;
        x0 = (x0 + x4) * 181;
        x4 = (tx0 - x4) * 181;
        
        TInt tx2 = x2;
        TInt tx6 = x6;
        x2 = tx2 * 236 + tx6 * 98;
        x6 = tx2 * 98 - tx6 * 236;
        
        even[0] = x0 + x2;
        even[1] = x4 + x6;
        even[2] = x4 - x6;
        even[3] = x0 - x2;
        
        odd[0] = x1 * 251 + x5 * 142 + x3 * 212 + x7 * 49;
        odd[1] = x1 * 213 - x5 * 251 - x3 * 50 - x7 * 142;
        odd[2] = x1 * 142 + x5 * 50 - x3 * 251 + x7 * 213;
        odd[3] = x1 * 50 + x5 * 213 - x3 * 142 - x7 * 251;
                
        res[x + 0] = even[0] + odd[0];
        res[x + 8] = even[1] + odd[1];
        res[x + 16] = even[2] + odd[2];
        res[x + 24] = even[3] + odd[3];
        res[x + 32] = even[3] - odd[3];
        res[x + 40] = even[2] - odd[2];
        res[x + 48] = even[1] - odd[1];
        res[x + 56] = even[0] - odd[0];
		}
    
    p = res;
	TInt* tp = iBlk;

    for( TInt y=0; y<8; y++ )
		{
        TInt x0 = *p++;
        TInt x1 = *p++;
        TInt x2 = *p++;
        TInt x3 = *p++;
        TInt x4 = *p++;
        TInt x5 = *p++;
        TInt x6 = *p++;
        TInt x7 = *p++;
        
        TInt tx0 = x0;
        x0 = (x0 + x4) * 181;
        x4 = (tx0 - x4) * 181;
        
        TInt tx2 = x2;
        TInt tx6 = x6;
        x2 = tx2 * 236 + tx6 * 98;
        x6 = tx2 * 98 - tx6 * 236;
        
        even[0] = x0 + x2;
        even[1] = x4 + x6;
        even[2] = x4 - x6;
        even[3] = x0 - x2;
        
        odd[0] = x1 * 251 + x5 * 142 + x3 * 212 + x7 * 49;
        odd[1] = x1 * 213 - x5 * 251 - x3 * 50 - x7 * 142;
        odd[2] = x1 * 142 + x5 * 50 - x3 * 251 + x7 * 213;
        odd[3] = x1 * 50 + x5 * 213 - x3 * 142 - x7 * 251;
                
        *tp++ = (even[0] + odd[0]) / 262144 + 128;
        *tp++ = (even[1] + odd[1]) / 262144 + 128;
        *tp++ = (even[2] + odd[2]) / 262144 + 128;
        *tp++ = (even[3] + odd[3]) / 262144 + 128;
        *tp++ = (even[3] - odd[3]) / 262144 + 128;
        *tp++ = (even[2] - odd[2]) / 262144 + 128;
        *tp++ = (even[1] - odd[1]) / 262144 + 128;
        *tp++ = (even[0] - odd[0]) / 262144 + 128;
		}

	//
	// Clamp
	//
	tp = iBlk;
	TInt* tpe = tp + 64;
	while( tp < tpe )
		{
		if( *tp < 0 ) *tp = 0;
		if( *tp > 255 ) *tp = 255;
		tp++;
		}
	}



void CJpeg::IdctHalf()
	{
	TInt even[ 4 ];
	TInt odd[ 4 ];
	TInt res[ 64 ];

	TInt* p = iDct;
    
    for( TInt x=0; x<4; x++ )
		{
		TInt x0 = *p++;
		TInt x1 = *p++;
		TInt x2 = *p++;
		TInt x3 = *p++;
		p += 4;

        x0 = x0 * 181;
        
		TInt x6 = x2 * 98;
        x2 = x2 * 236;
        
        even[0] = x0 + x2;
        even[1] = x0 + x6;
        even[2] = x0 - x6;
        even[3] = x0 - x2;
        
        odd[0] = x1 * 251 + x3 * 212;
        odd[1] = x1 * 213 - x3 * 50;
        odd[2] = x1 * 142 - x3 * 251;
        odd[3] = x1 * 50  - x3 * 142;
                
        res[x + 0] = even[0] + odd[0];
        res[x + 4] = even[2] + odd[2];
        res[x + 8] = even[3] - odd[3];
        res[x + 12] = even[1] - odd[1];
		}
    
	p = res;
	TInt* tp = iBlk;

    for( TInt y=0; y<4; y++ )
		{
        TInt x0 = *p++;
        TInt x1 = *p++;
        TInt x2 = *p++;
        TInt x3 = *p++;
        
        x0 = x0 * 181;
        
        TInt x6 = x2 * 98;
        x2 = x2 * 236;
        
        even[0] = x0 + x2;
        even[1] = x0 + x6;
        even[2] = x0 - x6;
        even[3] = x0 - x2;
        
        odd[0] = x1 * 251  + x3 * 212;
        odd[1] = x1 * 213  - x3 * 50;
        odd[2] = x1 * 142  - x3 * 251;
        odd[3] = x1 * 50   - x3 * 142;
                
        *tp++ = (even[0] + odd[0]) / 262144 + 128;
        *tp++ = (even[2] + odd[2]) / 262144 + 128;
        *tp++ = (even[3] - odd[3]) / 262144 + 128;
        *tp++ = (even[1] - odd[1]) / 262144 + 128;
		}

	//
	// Clamp
	//
	tp = iBlk;
	TInt* tpe = tp + 16;
	while( tp < tpe )
		{
		if( *tp < 0 ) *tp = 0;
		if( *tp > 255 ) *tp = 255;
		tp++;
		}
	}




const TJpegData& CJpeg::Info()
	{
	return iData;
	}


TPtrC8 CJpeg::ExifData()
	{
	return TPtrC8( iExifData, iExifDataLength );
	}



void CJpeg::CreateHuffmanL( THuffman* aHuffman, const TUint8* aBits, const TUint8* aVal )
	{
	//
	// default huffman lookup table generator
	//

	TInt huffSize[ 16 ];
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
		aHuffman->iSymbol[ i ] = aVal[ i ];
		}
	
	TInt l;
	TInt p = 0;
	for( l=0; l<16; l++ )
		{
		for( i=0; i<huffSize[ l ]; i++ )
			{
			aHuffman->iLength[ p++ ] = l+1;
			}
		}

	TInt code = 0;
	aHuffman->iLength[ p ] = 0;
	TInt si = aHuffman->iLength[ 0 ];
	TInt lastP = p;
	
	TInt hc[ 256 ];
	p = 0;

	while( aHuffman->iLength[ p ] )
		{
		while( aHuffman->iLength[ p ] == si )
			{
			hc[ p++ ] = code++;
			}
		code *= 2;
		si++;
		}
	
	l = 65536;
	for( i=lastP-1; i>=0; i-- )
		{
		TInt t = 16 - aHuffman->iLength[ i ];
		TInt k = hc[ i ] * ( 1 << t );
		TInt j;
		for( j=k; j<l; j++ )
			{
			aHuffman->iSearch[ j ] = i;
			}
		l = k;
		}
	
	}



void CJpeg::CreateDefaultHuffmanL()
	{
	//
	// Creates default huffman tables
	// needed if jpeg file doesn't have huffman tables
	// for example motion jpeg file doesn't have.
	//
	THuffman* huff = new( ELeave )THuffman;
	CleanupStack::PushL( huff );
	CreateHuffmanL( huff, bits_dc_luminance, val_dc_luminance );
	iHuffman[ 0 ] = huff;
	CleanupStack::Pop( huff );

	huff = new( ELeave )THuffman;
	CleanupStack::PushL( huff );
	CreateHuffmanL( huff, bits_dc_chrominance, val_dc_chrominance );
	iHuffman[ 1 ] = huff;
	CleanupStack::Pop( huff );

	huff = new( ELeave )THuffman;
	CleanupStack::PushL( huff );
	CreateHuffmanL( huff, bits_ac_luminance, val_ac_luminance );
	iHuffman[ 2 ] = huff;
	CleanupStack::Pop( huff );

	huff = new( ELeave )THuffman;
	CleanupStack::PushL( huff );
	CreateHuffmanL( huff, bits_ac_chrominance, val_ac_chrominance );
	iHuffman[ 3 ] = huff;
	CleanupStack::Pop( huff );
	}



void CJpeg::DecRgb1_1L( const TBitmapHandle& aBitmap, const TRect& aBlockRect )
	{
	PrepareLoadBlockL();

	//
	// Set file read position
	//
	iBufPos = iImageDataStart;
	iBufBits = 0;
	iBuf = 0;

	TInt c[ 3 ];
	c[ 0 ] = 0;
	c[ 1 ] = 0;
	c[ 2 ] = 0;

	TInt w = aBitmap.iSize.iWidth;
	TUint32* rgb = (TUint32*)aBitmap.iData;

	//
	// Store data for yuv->rgb conversion
	//
	iYuvConv.iBlkSize = iData.iBlockSize;
	iYuvConv.iRgbWidth = w;
	iYuvConv.iBlkPixels = 8;

	TInt blw = aBlockRect.Size().iWidth;
	TInt blh = aBlockRect.Size().iHeight;

	for( TInt by=0; by<blh; by++ )
		{

		if( iRandomScanned )
			{
			TPoint pos( aBlockRect.iTl.iX, aBlockRect.iTl.iY + by );
			TInt blkNum = pos.iX + pos.iY * iData.iSizeInBlocks.iWidth;
			TJpegBlock& blk = iBlock[ blkNum ];

			iBuf = blk.iBuf;
			iBufBits = blk.iBufBits;
			iBufPos = blk.iOffset;
			c[ 0 ] = blk.iY;
			c[ 1 ] = blk.iU;
			c[ 2 ] = blk.iV;
			iRst = false;
			}

		//TInt blockNum = aBlockRect.iTl.iX + ( aBlockRect.iTl.iY + by ) * iData.iSizeInBlocks.iWidth;

		for( TInt bx=0; bx<blw; bx++ )
			{
			iYuvConv.iRgb = rgb + bx * iData.iBlockSize.iWidth;
			iYuvConv.iRgb += by * iData.iBlockSize.iHeight * w;
		
			if( iRst )
				{
				iRst = false;
				iBuf = 0;
				iBufBits = 0;
				
				c[ 0 ] = 0;
				c[ 1 ] = 0;
				c[ 2 ] = 0;
				}
			//blockNum++;

			
			TInt xx;
			TInt yy;
			TInt* p;

			for( TInt i=0; i<iNumComponents; i++ )
				{
				iCurrentQt = iQt[ KQuantIndex[ i ] ];	
				xx = iComponent[ i ].iXFactor;
				yy = iComponent[ i ].iYFactor;
				TInt dw = xx * 8;
				p = iBlk;
				iDct[ 0 ] = c[ i ];

				for( TInt y=0; y<yy; y++ )
					{
					for( TInt x=0; x<xx; x++ )
						{
						iCurrentHuffman = KHuffIndex[ i ];
						DecodeBlock3();
						Idct();
						TUint8* tp = iC[ i ] + x*8 + y*8 * dw;
						p = iBlk;

						for( TInt ty=0; ty<8; ty++ )
							{
							for( TInt tx=0; tx<8; tx++ )
								{
								tp[ tx + ty * dw ] = *p++;
								}
							}
						}
					}
				c[ i ] = iDct[ 0 ];
				}

			//
			// Scaled blit YUV->RGB
			//
			if( iRgbConv )
				{
				iYuv2rgbFunc( this );
				}

			}
		}		
	}




void CJpeg::DecRgb1_2L( const TBitmapHandle& aBitmap, const TRect& aBlockRect )
	{
	PrepareLoadBlockL();

	//
	// Set file read position
	//
	iBufPos = iImageDataStart;
	iBufBits = 0;
	iBuf = 0;

	TInt c[ 3 ];
	c[ 0 ] = 0;
	c[ 1 ] = 0;
	c[ 2 ] = 0;

	TInt w = aBitmap.iSize.iWidth;
	TUint32* rgb = (TUint32*)aBitmap.iData;

	TInt bw = iData.iBlockSize.iWidth / 2;
	TInt bh = iData.iBlockSize.iHeight / 2;
	//
	// Store data for yuv->rgb conversion
	//
	iYuvConv.iBlkSize = TSize( bw, bh );
	iYuvConv.iRgbWidth = w;
	iYuvConv.iBlkPixels = 4;

	TInt blw = aBlockRect.Size().iWidth;
	TInt blh = aBlockRect.Size().iHeight;

	for( TInt by=0; by<blh; by++ )
		{

		if( iRandomScanned )
			{
			TPoint pos( aBlockRect.iTl.iX, aBlockRect.iTl.iY + by );
			TInt blkNum = pos.iX + pos.iY * iData.iSizeInBlocks.iWidth;
			TJpegBlock& blk = iBlock[ blkNum ];

			iBuf = blk.iBuf;
			iBufBits = blk.iBufBits;
			iBufPos = blk.iOffset;
			c[ 0 ] = blk.iY;
			c[ 1 ] = blk.iU;
			c[ 2 ] = blk.iV;
			iRst = false;
			}

		//TInt blockNum = aBlockRect.iTl.iX + ( aBlockRect.iTl.iY + by ) * iData.iSizeInBlocks.iWidth;

		for( TInt bx=0; bx<blw; bx++ )
			{
			iYuvConv.iRgb = rgb + bx * bw;
			iYuvConv.iRgb += by * bh * w;

			if( iRst )
				{
				iRst = false;
				iBuf = 0;
				iBufBits = 0;
				
				c[ 0 ] = 0;
				c[ 1 ] = 0;
				c[ 2 ] = 0;
				}
			//blockNum++;


			
			///////////
			TInt x;
			TInt y;
			TInt xx;
			TInt yy;
			TInt* p;

			for( TInt i=0; i<iNumComponents; i++ )
				{
				iCurrentQt = iQt[ KQuantIndex[ i ] ];	
				xx = iComponent[ i ].iXFactor;
				yy = iComponent[ i ].iYFactor;
				TInt dw = xx * 4;
				p = iBlk;
				iDct[ 0 ] = c[ i ];

				
				for( y=0; y<yy; y++ )
					{
					for( x=0; x<xx; x++ )
						{
						iCurrentHuffman = KHuffIndex[ i ];
						DecodeBlock3();
						TUint8* tp = iC[ i ] + x*4 + y*4 * dw;
						
						//
						// 4x4 Idct
						//
						IdctHalf();

						for( TInt ty=0; ty<4; ty++ )
							{
							for( TInt tx=0; tx<4; tx++ )
								{
								tp[ tx + ty * dw ] = p[ tx + ty*4 ];
								}
							}
						}
					}
				

				c[ i ] = iDct[ 0 ];
				}

			//
			// Scaled blit YUV->RGB
			//
			if( iRgbConv )
				{
				iYuv2rgbFunc( this );
				}


			}
		}
	}



void CJpeg::DecRgb1_4L( const TBitmapHandle& aBitmap, const TRect& aBlockRect )
	{
	PrepareLoadBlockL();

	//
	// Set file read position
	//
	iBufPos = iImageDataStart;
	iBufBits = 0;
	iBuf = 0;

	TInt c[ 3 ];
	c[ 0 ] = 0;
	c[ 1 ] = 0;
	c[ 2 ] = 0;

	TInt w = aBitmap.iSize.iWidth;
	TUint32* rgb = (TUint32*)aBitmap.iData;

	TInt bw = iData.iBlockSize.iWidth / 4;
	TInt bh = iData.iBlockSize.iHeight / 4;
	//
	// Store data for yuv->rgb conversion
	//
	iYuvConv.iBlkSize = TSize( bw, bh );
	iYuvConv.iRgbWidth = w;
	iYuvConv.iBlkPixels = 2;

	TInt blw = aBlockRect.Size().iWidth;
	TInt blh = aBlockRect.Size().iHeight;

	for( TInt by=0; by<blh; by++ )
		{

		if( iRandomScanned )
			{
			TPoint pos( aBlockRect.iTl.iX, aBlockRect.iTl.iY + by );
			TInt blkNum = pos.iX + pos.iY * iData.iSizeInBlocks.iWidth;
			TJpegBlock& blk = iBlock[ blkNum ];

			iBuf = blk.iBuf;
			iBufBits = blk.iBufBits;
			iBufPos = blk.iOffset;
			c[ 0 ] = blk.iY;
			c[ 1 ] = blk.iU;
			c[ 2 ] = blk.iV;
			iRst = false;
			}

		//TInt blockNum = aBlockRect.iTl.iX + ( aBlockRect.iTl.iY + by ) * iData.iSizeInBlocks.iWidth;
		
		for( TInt bx=0; bx<blw; bx++ )
			{
			iYuvConv.iRgb = rgb + bx * bw;
			iYuvConv.iRgb += by * bh * w;

			if( iRst )
				{
				iRst = false;
				iBuf = 0;
				iBufBits = 0;
				
				c[ 0 ] = 0;
				c[ 1 ] = 0;
				c[ 2 ] = 0;
				}
			//blockNum++;

			
			TInt x;
			TInt y;
			TInt xx;
			TInt yy;

			for( TInt i=0; i<iNumComponents; i++ )
				{
				iCurrentQt = iQt[ KQuantIndex[ i ] ];	
				xx = iComponent[ i ].iXFactor;
				yy = iComponent[ i ].iYFactor;
				TInt dw = xx * 2;
				iDct[ 0 ] = c[ i ];

				for( y=0; y<yy; y++ )
					{
					for( x=0; x<xx; x++ )
						{
						iCurrentHuffman = KHuffIndex[ i ];
						DecodeBlock3();
						TUint8* tp = iC[ i ] + x*2 + y*2 * dw;
						
						//
						// 2x2 Idct
						//
						TInt v0 = 181*iDct[1];
						TInt v1 = 32761 * iDct[ 0 ];
						TInt v2 = 45431 * iDct[ 8 ];
						TInt v3 = -9050 * iDct[ 8 ];
						TInt v4 = v0 + 251*iDct[9];
						TInt v5 = v0 -  50*iDct[9];
										
						TInt v = (v1 + v2 + 251*v4 ) / 262144 + 128;
						if( v<0 ) v=0; if( v>255 ) v=255; tp[ 0 ] = v;

						v = (v1 + v2 -  50*v4 ) / 262144 + 128;
						if( v<0 ) v=0; if( v>255 ) v=255; tp[ dw ] = v;

						v = (v1 + v3 + 251*v5 ) / 262144 + 128;
						if( v<0 ) v=0; if( v>255 ) v=255; tp[ 1 ] = v;

						v = (v1 + v3 -  50*v5 ) / 262144 + 128;
						if( v<0 ) v=0; if( v>255 ) v=255; tp[ dw+1 ] = v;
						}
					}
				c[ i ] = iDct[ 0 ];
				}


			//
			// Scaled blit YUV->RGB
			//
			if( iRgbConv )
				{
				iYuv2rgbFunc( this );
				}


			}
		}
	}



void CJpeg::DecRgb1_8L( const TBitmapHandle& aBitmap, const TRect& aBlockRect )
	{
	PrepareLoadBlockL();

	//
	// Set file read position
	//
	iBufPos = iImageDataStart;
	iBufBits = 0;
	iBuf = 0;

	TInt c[ 3 ];
	c[ 0 ] = 0;
	c[ 1 ] = 0;
	c[ 2 ] = 0;

	TInt w = aBitmap.iSize.iWidth;
	TUint32* rgb = (TUint32*)aBitmap.iData;

	TInt bw = iData.iBlockSize.iWidth / 8;
	TInt bh = iData.iBlockSize.iHeight / 8;
	
	//
	// Store data for yuv->rgb conversion
	//
	iYuvConv.iBlkSize = TSize( bw, bh );
	iYuvConv.iRgbWidth = w;
	iYuvConv.iBlkPixels = 1;

	TInt blw = aBlockRect.Size().iWidth;
	TInt blh = aBlockRect.Size().iHeight;

	for( TInt by=0; by<blh; by++ )
		{

		if( iRandomScanned )
			{
			TPoint pos( aBlockRect.iTl.iX, aBlockRect.iTl.iY + by );
			TInt blkNum = pos.iX + pos.iY * iData.iSizeInBlocks.iWidth;
			TJpegBlock& blk = iBlock[ blkNum ];

			iBuf = blk.iBuf;
			iBufBits = blk.iBufBits;
			iBufPos = blk.iOffset;
			c[ 0 ] = blk.iY;
			c[ 1 ] = blk.iU;
			c[ 2 ] = blk.iV;
			iRst = false;
			}

		//TInt blockNum = aBlockRect.iTl.iX + ( aBlockRect.iTl.iY + by ) * iData.iSizeInBlocks.iWidth;

		for( TInt bx=0; bx<blw; bx++ )
			{
			iYuvConv.iRgb = rgb + bx * bw;
			iYuvConv.iRgb += by * bh * w;

			if( iRst )
				{
				iRst = false;
				iBuf = 0;
				iBufBits = 0;
				
				c[ 0 ] = 0;
				c[ 1 ] = 0;
				c[ 2 ] = 0;
				}
			//blockNum++;

			
			TInt x;
			TInt y;
			TInt xx;
			TInt yy;

			for( TInt i=0; i<iNumComponents; i++ )
				{
				iCurrentQt = iQt[ KQuantIndex[ i ] ];	
				xx = iComponent[ i ].iXFactor;
				yy = iComponent[ i ].iYFactor;
				TInt dw = xx * 1;
				iDct[ 0 ] = c[ i ];

				for( y=0; y<yy; y++ )
					{
					for( x=0; x<xx; x++ )
						{
						iCurrentHuffman = KHuffIndex[ i ];
						DecodeBlock3();
						TUint8* tp = iC[ i ] + x*1 + y*1 * dw;

						*tp = iDct[ 0 ] / 8 + 128;
						}
					}
				c[ i ] = iDct[ 0 ];
				}

			//
			// Scaled blit YUV->RGB
			//
			if( iRgbConv )
				{
				iYuv2rgbFunc( this );
				}

			}
		}
	}



void CJpeg::SetScale( TJpegScale aScale )
	{
	iScale = aScale;
	}



void CJpeg::Yuv2Rgb22_11_11( TAny* aPtr )
	{
	CJpeg& p = *(CJpeg*)aPtr;
	
	//
	// YUV420
	//
	// YY
	// YY U V
	//
	
	TUint8* cY = p.iC[ 0 ];
	TUint8* cU = p.iC[ 1 ];
	TUint8* cV = p.iC[ 2 ];
	TUint32* rgb = p.iYuvConv.iRgb;
	TInt modulo = p.iYuvConv.iRgbWidth - p.iYuvConv.iBlkSize.iWidth;

	TInt w = p.iYuvConv.iBlkSize.iWidth;
	TInt h = p.iYuvConv.iBlkSize.iHeight;
	

	for( TInt y=0; y<h; y++ )
		{
		TInt x = 0;
		TInt cu = 0;
		TInt cv = 0;
		for( x=0; x<w; x++ )
			{
			TInt cy = *cY++;
			if( ( x & 1 ) == 0 )
				{
				// when X even, fetch new color components
				cu = *cU++ - 128;
				cv = *cV++ - 128;
				}
			

			// urgb color
			TUint32 c;

			// rgb color component
			TInt cc;
			
			// add red
			cc = cy + ( 359 * cv ) / 256;
			if( cc < 0 ) cc = 0;
			if( cc > 255 ) cc = 255;
			c = cc << 16;

			// add green
			cc = cy - ( 88 * cu  +  183 * cv ) / 256;
			if( cc < 0 ) cc = 0;
			if( cc > 255 ) cc = 255;
			c |= cc << 8;
			
			// add blue
			cc = cy + ( 454 * cu ) / 256;
			if( cc < 0 ) cc = 0;
			if( cc > 255 ) cc = 255;

			// write to bitmap
			*rgb++ = c + cc;
			
			}
		rgb += modulo;
		if( ( y & 1 ) == 0 )
			{
			// only advance color components on odd Y
			// so on even, do rewind
			cU -= x/2;
			cV -= x/2;
			}
		}	
	}



void CJpeg::Yuv2Rgb21_11_11( TAny* aPtr )
	{
	CJpeg& p = *(CJpeg*)aPtr;
	//
	// YUV422
	//
	// YY U V
	//

	TUint8* cY = p.iC[ 0 ];
	TUint8* cU = p.iC[ 1 ];
	TUint8* cV = p.iC[ 2 ];
	TUint32* rgb = p.iYuvConv.iRgb;
	TInt modulo = p.iYuvConv.iRgbWidth - p.iYuvConv.iBlkSize.iWidth;

	TInt w = p.iYuvConv.iBlkSize.iWidth;
	TInt h = p.iYuvConv.iBlkSize.iHeight;

	for( TInt y=0; y<h; y++ )
		{
		TInt cu = 0;
		TInt cv = 0;
		for( TInt x=0; x<w; x++ )
			{
			TInt cy = *cY++;
			if( ( x & 1 ) == 0 )
				{
				// for every even X, fetch new color components
				cu = *cU++ - 128;
				cv = *cV++ - 128;
				}

			// urgb color
			TUint32 c;
			
			// rgb color component
			TInt cc;
			
			// add red
			cc = cy + ( 359 * cv ) / 256;
			if( cc < 0 ) cc = 0;
			if( cc > 255 ) cc = 255;
			c = cc << 16;

			// add green
			cc = cy - ( 88 * cu  +  183 * cv ) / 256;
			if( cc < 0 ) cc = 0;
			if( cc > 255 ) cc = 255;
			c |= cc << 8;
			
			// add blue
			cc = cy + ( 454 * cu ) / 256;
			if( cc < 0 ) cc = 0;
			if( cc > 255 ) cc = 255;

			// write to bitmap
			*rgb++ = c + cc;
			}
		rgb += modulo;
		}
	}



void CJpeg::Yuv2RgbFree( TAny* aPtr )
	{
	CJpeg& p = *(CJpeg*)aPtr;

	TInt bw = p.iYuvConv.iBlkSize.iWidth;
	TInt bh = p.iYuvConv.iBlkSize.iHeight;
	
	TInt y1y = 0;
	TInt y1u = 0;
	TInt y1v = 0;

	TInt yw = p.iComponent[ 0 ].iXFactor * p.iYuvConv.iBlkPixels; 
	TInt uw = p.iComponent[ 1 ].iXFactor * p.iYuvConv.iBlkPixels;
	TInt vw = p.iComponent[ 2 ].iXFactor * p.iYuvConv.iBlkPixels;
	
	TUint32* rgb = p.iYuvConv.iRgb;
	TInt modulo = p.iYuvConv.iRgbWidth - bw;
	
	for( TInt y=0; y<bh; y++ )
		{
		TInt txy = 0;
		TInt txu = 0;
		TInt txv = 0;
		TUint8* ccy = p.iC[ 0 ] + ( y1y/256 ) * yw;
		TUint8* ccu = p.iC[ 1 ] + ( y1u/256 ) * uw;
		TUint8* ccv = p.iC[ 2 ] + ( y1v/256 ) * vw;

		for( TInt x=0; x<bw; x++ )
			{

			TInt cy = ccy[ txy / 256 ];
			TInt cu = ccu[ txu / 256 ] - 128;
			TInt cv = ccv[ txv / 256 ] - 128;
			
			txy += p.iYxa;
			txu += p.iUxa;
			txv += p.iVxa;

			// urgb color
			TUint32 c;
			
			// rgb color component
			TInt cc;
			
			// add red
			cc = cy + ( 359 * cv ) / 256;
			if( cc < 0 ) cc = 0;
			if( cc > 255 ) cc = 255;
			c = cc << 16;

			// add green
			cc = cy - ( 88 * cu  +  183 * cv ) / 256;
			if( cc < 0 ) cc = 0;
			if( cc > 255 ) cc = 255;
			c |= cc << 8;
			
			// add blue
			cc = cy + ( 454 * cu ) / 256;
			if( cc < 0 ) cc = 0;
			if( cc > 255 ) cc = 255;

			// write to bitmap
			*rgb++ = c + cc;
			}
		y1y += p.iYya;
		y1u += p.iUya;
		y1v += p.iVya;
		rgb += modulo;
		}
	}




void CJpeg::EnableRgvConv()
	{
	iRgbConv = true;
	}



void CJpeg::DisableRgbConv()
	{
	iRgbConv = false;
	}



TBitmapHandle CJpeg::LoadImageL( TRect& aRect )
	{
	//
	// Crop possible illegal rect
	//
	//aRect.BoundingRect( TRect( iData.iSize ) );
	
	if( aRect.iTl.iX < 0 ) aRect.iTl.iX = 0;
	if( aRect.iTl.iY < 0 ) aRect.iTl.iY = 0;
	if( aRect.iBr.iX < 0 ) aRect.iBr.iX = 0;
	if( aRect.iBr.iY < 0 ) aRect.iBr.iY = 0;
	if( aRect.iTl.iX > iData.iSize.iWidth ) aRect.iTl.iX = iData.iSize.iWidth;
	if( aRect.iTl.iY > iData.iSize.iHeight ) aRect.iTl.iY = iData.iSize.iHeight;
	if( aRect.iBr.iX > iData.iSize.iWidth ) aRect.iBr.iX = iData.iSize.iWidth;
	if( aRect.iBr.iY > iData.iSize.iHeight ) aRect.iBr.iY = iData.iSize.iHeight;
	
	//
	// Create rectangle of blocks that has all the pixels of aRect
	//
	TRect blockRect = aRect;
	blockRect.iTl.iX /= iData.iBlockSize.iWidth;
	blockRect.iBr.iX /= iData.iBlockSize.iWidth;
	blockRect.iTl.iY /= iData.iBlockSize.iHeight;
	blockRect.iBr.iY /= iData.iBlockSize.iHeight;
	
	if( aRect.iBr.iX & ( iData.iBlockSize.iWidth - 1 ) )
		{
		blockRect.iBr.iX++;
		}

	if( aRect.iBr.iY & ( iData.iBlockSize.iHeight - 1 ) )
		{
		blockRect.iBr.iY++;
		}

	//
	// Return the real 1:1 scale pixel rectangle back in aRect
	//
	aRect.iTl.iX = blockRect.iTl.iX * iData.iBlockSize.iWidth;
	aRect.iTl.iY = blockRect.iTl.iY * iData.iBlockSize.iHeight;
	aRect.iBr.iX = blockRect.iBr.iX * iData.iBlockSize.iWidth;
	aRect.iBr.iY = blockRect.iBr.iY * iData.iBlockSize.iHeight;


	// decode area size in blocks
	TSize blockSize = blockRect.Size();

	// create bitmap
	TBitmapHandle bm;
	bm.iSize.iWidth = iData.iBlockSize.iWidth * blockSize.iWidth;
	bm.iSize.iHeight = iData.iBlockSize.iHeight * blockSize.iHeight;
	
	//
	// If not whole image decode, random access must be initialized
	//
	if( iData.iSizeInBlocks != blockSize )
		{
		ScanRandomL();
		}

	//
	// ...and decode
	//
	switch( iScale )
		{
		case EScale1:
			{
			bm.iData = new( ELeave )TUint32[ bm.iSize.iWidth * bm.iSize.iHeight ];
			CleanupStack::PushL( bm.iData );
			DecRgb1_1L( bm, blockRect );
			CleanupStack::Pop( bm.iData );
			break;
			}
		case EScale2:
			{
			bm.iSize.iWidth /= 2;
			bm.iSize.iHeight /= 2;
			bm.iData = new( ELeave )TUint32[ bm.iSize.iWidth * bm.iSize.iHeight ];
			CleanupStack::PushL( bm.iData );
			DecRgb1_2L( bm, blockRect );
			CleanupStack::Pop( bm.iData );
			break;
			}
		case EScale4:
			{
			bm.iSize.iWidth /= 4;
			bm.iSize.iHeight /= 4;
			bm.iData = new( ELeave )TUint32[ bm.iSize.iWidth * bm.iSize.iHeight ];
			CleanupStack::PushL( bm.iData );
			DecRgb1_4L( bm, blockRect );
			CleanupStack::Pop( bm.iData );
			break;
			}
		case EScale8:
			{
			bm.iSize.iWidth /= 8;
			bm.iSize.iHeight /= 8;
			bm.iData = new( ELeave )TUint32[ bm.iSize.iWidth * bm.iSize.iHeight ];
			CleanupStack::PushL( bm.iData );
			DecRgb1_8L( bm, blockRect );
			CleanupStack::Pop( bm.iData );
			break;
			}
		}

	return bm;
	}
