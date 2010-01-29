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


#include "CExifParser.h"
#include <e32svr.h>		// for debug



enum TExifType
	{
	EByte = 1,
	EAscii,
	EShort,
	ELong,
	ERational,
	EUndefined = 7,
	ESLong = 9,
	ESRational
	};



const char* KTypeName[] = 
	{
	"Byte",
	"Ascii",
	"Short",
	"Long",
	"Rational",
	"Undefined",
	"SLong",
	"SRational"
	};



const char KTypeLength[] = 
	{
	0,
	1,
	1,
	2,
	4,
	8,
	0,
	1,
	0,
	4,
	8
	};







CExifParser* CExifParser::NewL()
	{
	CExifParser* self = NewLC();
	CleanupStack::Pop( self );
	return self;
	}



CExifParser* CExifParser::NewLC()
	{
	CExifParser* self = new( ELeave )CExifParser();
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}



CExifParser::~CExifParser()
	{
	iEntry.ResetAndDestroy();
	iParseStack.Reset();

    delete[] iSaveBuf;
	delete[] iSaveDataBuf;
	
	iThumbData = NULL;    
	}



CExifParser::CExifParser()
	{
	
	}



void CExifParser::ConstructL()
	{
	iIntelByteOrder = true;
	}



void CExifParser::ParseL( const TPtrC8& aData )
	{
	if( aData.Length() < (8+2+12) ) return; // header + 1 tag length

	iData.Set( aData );
	iThumbData = 0;
	iThumbLen = 0;

	if( iData[ 0 ] == 0x49 && iData[ 1 ] == 0x49 )
		{
		// intel byte order
		iIntelByteOrder = ETrue;
		//RDebug::Print( _L("Intel byte order selected") );
		}
	else
		{
		//RDebug::Print( _L("Motorola byte order selected") );
		}
	
	iPosition = 2;
	TUint16 v = Get16();
	if( v != 0x002a )
		{
		// must be 2a 00 in EXIF
		User::Leave( KErrNotSupported );
		}
	
	iPosition = 4;
	iParseStack.Append( TParseStack( Get32(), EIfd0 ) );
	
	while( iParseStack.Count() )
		{
		iPosition = iParseStack[ 0 ].iPosition;
		TUint8 currentIfd = iParseStack[ 0 ].iIfd;
		iParseStack.Remove( 0 );
		TInt i;
		TInt c = Get16();
		for( i=0; i<c; i++ )
			{
			ParseIfdL( currentIfd );
			}
		TUint32 link = NULL;	
		if (iPosition + 3 < iData.Length())
			{
			link = Get32();
			}
			
		if( link )
			{
			//RDebug::Print( _L(" --Link (IFD1?) %d,%d"), link, iPosition );
			iParseStack.Append( TParseStack( link, EIfd1 ) );
			}
		else
			{
			//RDebug::Print( _L(" -- NULL Link after IFD") );
			}
		}
	iParseStack.Reset();

	//RDebug::Print( _L(" -- Parse Complete") );
	iIntelByteOrder = true;
	}



TBool CExifParser::TagExist( TInt aIfd, TUint16 aTag )
	{
	if( FindTag( aIfd, aTag ) >= 0 ) return ETrue;
	return EFalse;
	}



void CExifParser::DeleteTag( TInt aIfd, TUint16 aTag )
	{
	TInt i = FindTag( aIfd, aTag );
	if( i >= 0 )
		{
		delete iEntry[ i ];
		iEntry.Remove( i );
		}
	}



TUint32 CExifParser::TagValue( TInt aIfd, TUint16 aTag )
	{
	TInt value = 0;
	TInt n = FindTag( aIfd, aTag );
	if( n<0 ) return 0;
	TExifEntry* e = iEntry[ n ];
	TUint8* valData = e->iData;
	switch( e->iType )
		{
		case EByte:
			{
			value = Get8( valData );
			break;
			}
		case EAscii:
			{
			/*
			TBuf< 256 >str;
			str.Copy( _L("value:") );
			const TUint8* p = valData;
			while( *p != 0 )
				{
				str.Append( *p );
				p++;
				}
			*/
			// not supported
			value = 0;
			break;
			}
		case EShort:
			{
			value = Get16( valData );
			////RDebug::Print( _L("value:%d"), value );
			break;
			}
		case ELong:
			{
			value = Get32( valData );
			////RDebug::Print( _L("value:%d"), value );
			break;
			}
		case ERational:
			{
			TInt nominator = Get32( valData );
			TInt denominator = Get32( valData+4 );
			////RDebug::Print( _L("value:%d/%d"), nominator, denominator );
			if( denominator != 0 )
				{
				value = nominator / denominator;
				}
			else
				{
				value = 0;
				}
			break;
			}
		case EUndefined:
			{
			
			break;
			}
		case ESLong:
			{
			TUint32 val = Get32( valData );
			value = *(TInt*)(&val);
			////RDebug::Print( _L("value:%d"), svalue );

			break;
			}
		case ESRational:
			{
			TUint32 uval = Get32( valData );
			TInt nominator = *(TInt*)(&uval);
			uval = Get32( valData+4 );
			TInt denominator = *(TInt*)(&uval);

			if( denominator != 0 )
				{
				value = nominator / denominator;
				}
			else
				{
				value = 0;
				}
			////RDebug::Print( _L("value:%d/%d"), nominator, denominator );
			break;
			}
		default:
		 	{
		 	break;
		 	}
		}	

	return value;
	}

TPtrC8 CExifParser::ThumbData()
	{
	return TPtrC8( iThumbData, iThumbLen );
	}


TPtr8 CExifParser::SaveL( const TPtrC8& aThumbData )
	{
	iIntelByteOrder = true;
	//
	// Exif data cannot take more than 64KBytes
	//
	if (iSaveBuf)
	{
	    delete[] iSaveBuf;
	    iSaveBuf = NULL;
	}
	
	iSaveBuf = new( ELeave )TUint8[ 0x10000 ];
	iSaveBufPos = 0;

	if (iSaveDataBuf)
	{
    	delete[] iSaveDataBuf;
    	iSaveDataBuf = NULL;	    
	}

	iSaveDataBuf = new( ELeave )TUint8[ 0x10000 ];
	iSaveDataBufPos = 0;

	//
	// header
	//
	Put8( 0x49 );
	Put8( 0x49 );
	Put8( 0x2a );
	Put8( 0x00 );
	Put32( 8 );

	//
	// Remove IFD0 SubIfd tag, will be regenerated
	//
	DeleteTag( EIfd0, 0x8769 );

	//
	// Save IFD0
	//
	Put16( IfdCount( EIfd0 ) + 1 );
	TInt c = iEntry.Count();
	TInt i;
	for( i=0; i<c; i++ )
		{
		TExifEntry* e = iEntry[ i ];
		if( e->iIfd == EIfd0 )
			{
			StoreEntry( i );
			}
		}

	//
	// Save link to SubIfd ( this is a tag )
	//
	TExifEntry* tagsub = new( ELeave )TExifEntry;
	CleanupStack::PushL(tagsub);
	tagsub->iTag = 0x8769;
	tagsub->iIfd = EIfd0;
	tagsub->iType = ELong;
	tagsub->iCount = 1;
	tagsub->iData = new( ELeave )TUint8[ 4 ];
	TUint8* dat = tagsub->iData;
	TUint32 offset = iSaveBufPos + 12 + 4; // just after IFD1 link
	if( iIntelByteOrder )
		{
		dat[ 0 ] = offset & 255; offset >>= 8;
		dat[ 1 ] = offset & 255; offset >>= 8;
		dat[ 2 ] = offset & 255; offset >>= 8;
		dat[ 3 ] = offset & 255;
		}
	else
		{
		dat[ 3 ] = offset & 255; offset >>= 8;
		dat[ 2 ] = offset & 255; offset >>= 8;
		dat[ 1 ] = offset & 255; offset >>= 8;
		dat[ 0 ] = offset & 255;
		}
	iEntry.Append( tagsub );
	CleanupStack::Pop();
	StoreEntry( iEntry.Count()-1 );
	
	//
	// Save link to IFD1 ( not a tag, just pointer )
	//
	TInt IFD1LinkPos = iSaveBufPos;
	Put32( 0 );

	//
	// Remove Interoperability tag, will be regenerated
	//
	DeleteTag( ESubIfd, 0xa005 );
	c = iEntry.Count();

	//
	// Save SubIfd
	//
	Put16( IfdCount( ESubIfd ) );
	//RDebug::Print( _L(" SUBIFD count %x"), IfdCount( ESubIfd ) );
	for( i=0; i<c; i++ )
		{
		TExifEntry* e = iEntry[ i ];
		if( e->iIfd == ESubIfd )
			{
			//RDebug::Print( _L("  SUBIFD %x"), e->iTag );
			StoreEntry( i );
			}
		}
/*	
	TExifEntry* tagint = new( ELeave )TExifEntry;
	tagint->iTag = 0xa005;
	tagint->iIfd = ESubIfd;
	tagint->iType = ELong;
	tagint->iCount = 1;
	tagint->iData = new( ELeave )TUint8[ 4 ];
	TUint8* dat2 = tagint->iData;
	TUint32 offset2 = iSaveBufPos + 12 + 4; // just after IFD link
	if( iIntelByteOrder )
		{
		dat2[ 0 ] = offset2 & 255; offset2 >>= 8;
		dat2[ 1 ] = offset2 & 255; offset2 >>= 8;
		dat2[ 2 ] = offset2 & 255; offset2 >>= 8;
		dat2[ 3 ] = offset2 & 255;
		}
	else
		{
		dat2[ 3 ] = offset2 & 255; offset2 >>= 8;
		dat2[ 2 ] = offset2 & 255; offset2 >>= 8;
		dat2[ 1 ] = offset2 & 255; offset2 >>= 8;
		dat2[ 0 ] = offset2 & 255;
		}
	iEntry.Append( tagint );
	StoreEntry( iEntry.Count()-1 );
*/
	Put32( 0 ); // IFD link

/*	
	//
	// Save Interoperability
	//
	Put16( IfdCount( EInteroperability ) );
	c = iEntry.Count();
	for( i=0; i<c; i++ )
		{
		TExifEntry* e = iEntry[ i ];
		if( e->iIfd == EInteroperability )
			{
			StoreEntry( i );
			}
		}
	Put32( 0 ); // IFD link
*/
	//
	// Save IFD1 ( thumbnail ) if new thumbnail given
	//
	if( aThumbData.Ptr() )
		{
		TUint32 temp = iSaveBufPos;
		iSaveBufPos = IFD1LinkPos;
		Put32( temp );
		iSaveBufPos = temp;
		}

	//
	// Remove old thumbnail tags
	//
	c = iEntry.Count();
	i = 0;
	while( i < iEntry.Count() )
		{
		TExifEntry* e = iEntry[ i ];
		if( e->iIfd == EIfd1 )
			{
			delete e;
			iEntry.Remove( i );
			}
		else
			{
			i++;
			}
		}

	//
	// Add newe thumbnail tags ( if thumbnail given )
	//
	TInt thumbData = 0;
	if( aThumbData.Ptr() )
		{
		AddTagL( EIfd1, 0x0103, (TUint16)6 );			// thumbnail = jpeg
		
		// thumbnail X-resolution 180/1
		AddTagL( EIfd1, 0x011a, (TUint32)180, (TUint32)1 );			 
		
		// thumbnail Y-resolution 180/1
		AddTagL( EIfd1, 0x011b, (TUint32)180, (TUint32)1 );
		
		// thumbnail resolution unit = inch
		AddTagL( EIfd1, 0x0128, (TUint16)2 );

		AddTagL( EIfd1, 0x0201, (TUint32)0 );			// thumb offset
		AddTagL( EIfd1, 0x0202, (TUint32)aThumbData.Length() );	// thumb length
		
	
		Put16( IfdCount( EIfd1 ) );
		c = iEntry.Count();
		for( i=0; i<c; i++ )
			{
			TExifEntry* e = iEntry[ i ];
			if( e->iIfd == EIfd1 )
				{
				StoreEntry( i );
				}
			}
		

		thumbData = iSaveBufPos - 12 - 4; // tag 0x0201 data position

		Put32( 0 ); // IFD link
		}

	//
	// Copy tag data to same buffer with tags
	//
	Mem::Copy( iSaveBuf + iSaveBufPos, iSaveDataBuf, iSaveDataBufPos );
	TInt totalLength = iSaveBufPos + iSaveDataBufPos;

	//
	// Copy thumbnail data to same buffer
	//
	if( thumbData )
		{
		TInt temp = iSaveBufPos;
		iSaveBufPos = thumbData;
		Put32( totalLength );
		iSaveBufPos = temp;
		
		Mem::Copy( iSaveBuf + totalLength, aThumbData.Ptr(), aThumbData.Length() );
		totalLength += aThumbData.Length();
		}	
	//
	// update data pointers
	//
	if( iSaveBufPos & 1 ) iSaveBufPos++;		// data must be in even address
	TInt doffset = iSaveBufPos;
	c = iDataEntry.Count();
	for( i=0; i<c; i++ )
		{
		iSaveBufPos = iDataEntry[ i ];
		TUint32 ptr = Get32( iSaveBuf + iSaveBufPos );
		ptr += doffset;
		Put32( ptr );
		}

	delete[] iSaveDataBuf;
	iSaveDataBuf = NULL;
	iDataEntry.Reset();
	
	TPtr8 buffer( iSaveBuf, totalLength );
	buffer.SetLength( totalLength );
	return buffer;
	}



TUint32 CExifParser::Get8()
	{
	return iData[ iPosition++ ];

	}



TUint32 CExifParser::Get16()
	{
	TUint32 value;
	if( iIntelByteOrder )
		{
		value = 256 * iData[ iPosition + 1 ] + iData[ iPosition ];
		}
	else
		{
		value = 256 * iData[ iPosition ] + iData[ iPosition + 1 ];
		}
	iPosition += 2;
	return value;	
	}



TUint32 CExifParser::Get32()
	{
	TUint32 value;
	if( iIntelByteOrder )
		{
		value = iData[ iPosition + 3 ];
		value <<= 8;
		value |= iData[ iPosition + 2 ];
		value <<= 8;
		value |= iData[ iPosition + 1 ];
		value <<= 8;
		value |= iData[ iPosition + 0 ];
		}
	else
		{
		value = iData[ iPosition + 0 ];
		value <<= 8;
		value |= iData[ iPosition + 1 ];
		value <<= 8;
		value |= iData[ iPosition + 2 ];
		value <<= 8;
		value |= iData[ iPosition + 3 ];
		}
	iPosition += 4;
	return value;	
	}



TUint32 CExifParser::Get8( const TUint8* aData )
	{
	return *aData;
	}



TUint32 CExifParser::Get16( const TUint8* aData )
	{
	TUint32 value;
	if( iIntelByteOrder )
		{
		value = 256 * aData[ 1 ] + aData[ 0 ];
		}
	else
		{
		value = 256 * aData[ 0 ] + aData[ 1 ];
		}
	return value;	
	}



TUint32 CExifParser::Get32( const TUint8* aData )
	{
	TUint32 value;
	if( iIntelByteOrder )
		{
		value = aData[ 3 ];
		value <<= 8;
		value |= aData[ 2 ];
		value <<= 8;
		value |= aData[ 1 ];
		value <<= 8;
		value |= aData[ 0 ];
		}
	else
		{
		value = aData[ 0 ];
		value <<= 8;
		value |= aData[ 1 ];
		value <<= 8;
		value |= aData[ 2 ];
		value <<= 8;
		value |= aData[ 3 ];
		}
	return value;	
	}



void CExifParser::ParseIfdL( TUint8 aIfd )
	{
	TUint32 tag = Get16();
	TUint32 type = Get16();
	TUint32 count = Get32();
	TUint32 offset = Get32();
	TUint32 revOffset = ( ( offset & 0xff ) << 24 ) + ( ( offset & 0xff00 ) << 8 ) + ( ( offset & 0xff0000 ) >> 8 ) + ( ( offset & 0xff000000 ) >> 24 );

	RDebug::Print( _L("Pos %d, Tag %x, type %d, count %d, offset %d, ifd %d"), iPosition-14, tag, type, count, offset, aIfd );
	
	const TUint8* valData = NULL;

	TInt bytes = KTypeLength[ type ] * count;

	if( bytes > 4 )
		{
		valData = &iData[ offset ];
		}
	else
		{
		if( iIntelByteOrder )
			{
			valData = (const TUint8*)(&offset);
			}
		else
			{
			// offset already reversed
			// must use reversed offset
			valData = (const TUint8*)(&revOffset);
			}
		}


	TExifEntry* entry = new( ELeave )TExifEntry;
	CleanupStack::PushL(entry);
	entry->iIfd = aIfd;
	entry->iTag = tag;
	entry->iType = type;
	entry->iCount = count;
	
	TUint32 value = 0;
		
	entry->iData = new( ELeave )TUint8[ bytes ];
	if( iIntelByteOrder )
		{
		Mem::Copy( entry->iData, valData, bytes );
		switch( type )
			{
			case EByte:
				{
				value = Get8( valData );
				break;
				}
			case EShort:
				{
				value = Get16( valData );
				break;
				}
			case ELong:
			case ESLong:
				{
				value = Get32( valData );
				break;
				}
			default:
				break;
			}
		}
	else
		{
		
		switch( type )
			{
			case EByte:
				{
				value = Get8( valData );
				entry->iData[ 0 ] = value;
				break;
				}
			case EAscii:
				{
				Mem::Copy( entry->iData, valData, bytes );
				//RDebug::Print( str );
				break;
				}
			case EShort:
				{
				entry->iData[ 1 ] = Get8( valData );
				entry->iData[ 0 ] = Get8( valData+1 );
				value = Get16( valData );
				break;
				}
			case ELong:
			case ESLong:
				{
				entry->iData[ 3 ] = Get8( valData );
				entry->iData[ 2 ] = Get8( valData+1 );
				entry->iData[ 1 ] = Get8( valData+2 );
				entry->iData[ 0 ] = Get8( valData+3 );
				value = Get32( valData );
				break;
				}
			case ERational:
			case ESRational:
				{
				entry->iData[ 3 ] = Get8( valData );
				entry->iData[ 2 ] = Get8( valData+1 );
				entry->iData[ 1 ] = Get8( valData+2 );
				entry->iData[ 0 ] = Get8( valData+3 );

				entry->iData[ 7 ] = Get8( valData+4 );
				entry->iData[ 6 ] = Get8( valData+5 );
				entry->iData[ 5 ] = Get8( valData+6 );
				entry->iData[ 4 ] = Get8( valData+7 );
				break;
				}
			case EUndefined:
			default:
				{
				Mem::Copy( entry->iData, valData, bytes );
				break;
				}
			}
		}
	//RDebug::Print( _L("value=%d"), value );
	
	iEntry.Append( entry );
	CleanupStack::Pop();
	

	if( tag == 0x927c )
		{
		//RDebug::Print( _L(" --MakerNote") );
		}

	if( tag == 0x8769 )
		{
		iParseStack.Append( TParseStack( value, ESubIfd ) );
		//RDebug::Print( _L(" --SubIfd %d, %d"), value, iPosition );
		}

	if( tag == 0xa005 )
		{
		iParseStack.Append( TParseStack( value, EInteroperability ) );
		//RDebug::Print( _L(" --Interoperability, %d, %d"), value, iPosition );
		}

	if( aIfd == EIfd1 )
		{
		if( tag == 0x0201 )
			{
			iThumbData = ((TUint8*)iData.Ptr()) + value;
			}
		if( tag == 0x0202 )
			{
			iThumbLen = value;
			}
		}
	}



TInt CExifParser::FindTag( TInt aIfd, TUint16 aTag )
	{
	TInt i;
	TInt c = iEntry.Count();
	for( i=0; i<c; i++ )
		{
		if( iEntry[ i ]->iIfd == aIfd )
			{
			if( iEntry[ i ]->iTag == aTag ) return i;
			}
		}
	return -1;
	}



void CExifParser::StoreEntry( TInt aIndex )
	{
	TExifEntry* e = iEntry[ aIndex ];

	TUint32 count = e->iCount;
	TUint32 data = 0;
	TInt bytes = KTypeLength[ e->iType ] * count;
	TUint8* d = e->iData;

	bool realData = false;

	if( bytes <= 4 )
		{
		// store data to offset
		TInt i;
		TInt shift = 0;
		for( i=0; i<bytes; i++ )
			{
			data |= ( d[ i ] << shift );
			shift += 8;
			}
		}
	else
		{
		// more than 4 bytes, store in data section
		data = StoreData( d, bytes );
		realData = true;
		}

	Put16( e->iTag );
	Put16( e->iType );
	Put32( count );
	if( realData )
		{
		iDataEntry.Append( iSaveBufPos );
		}
	Put32( data );
	}



void CExifParser::Put8( TUint8 aData )
	{
	iSaveBuf[ iSaveBufPos++ ] = aData;
	}
	


void CExifParser::Put16( TUint16 aData )
	{
	Put8( aData & 255 );
	Put8( aData / 256 );
	/*
	if( iIntelByteOrder )
		{
		Put8( aData & 255 );
		Put8( aData / 256 );
		}
	else
		{
		Put8( aData / 256 );
		Put8( aData & 255 );
		}
	*/
	}
	


void CExifParser::Put32( TUint32 aData )
	{
	Put16( aData & 65535 );
	Put16( aData / 65536 );
	/*
	if( iIntelByteOrder )
		{
		Put16( aData & 65535 );
		Put16( aData / 65536 );
		}
	else
		{
		Put16( aData / 65536 );
		Put16( aData & 65535 );
		}	
	*/
	}


TUint32 CExifParser::StoreData( TUint8* aData, TInt aCount )
	{
	if( iSaveDataBufPos & 1 ) iSaveDataBufPos++;	// must be even offset
	TUint32 pos = iSaveDataBufPos;
	TInt i;
	for( i=0; i<aCount; i++ )
		{
		iSaveDataBuf[ iSaveDataBufPos++ ] = aData[ i ];
		}
	return pos;
	}


TInt CExifParser::IfdCount( TInt aIfd )
	{
	TInt count = 0;
	TInt c = iEntry.Count();
	TInt i;
	for( i=0; i<c; i++ )
		{
		TExifEntry* e = iEntry[ i ];
		if( e->iIfd == aIfd )
			{
			count++;
			}
		}
	return count;
	}



void CExifParser::AddTagL( TInt aIfd, TUint16 aTag, TInt32 aValue )
	{
	TUint32 uvalue = *( TUint32* )( &aValue );

	TUint8* data = new( ELeave )TUint8[ 4 ];
	CleanupStack::PushL(data);
	if( iIntelByteOrder )
		{
		data[ 0 ] = uvalue & 255;
		data[ 1 ] = ( uvalue >> 8 ) & 255;
		data[ 2 ] = ( uvalue >> 16 ) & 255;
		data[ 3 ] = ( uvalue >> 24 ) & 255;
		}
	else
		{
		data[ 3 ] = uvalue & 255;
		data[ 2 ] = ( uvalue >> 8 ) & 255;
		data[ 1 ] = ( uvalue >> 16 ) & 255;
		data[ 0 ] = ( uvalue >> 24 ) & 255;
		}
	AddTagL( aIfd, aTag, ESLong, data, 4 );
	CleanupStack::Pop();
	}



void CExifParser::AddTagL( TInt aIfd, TUint16 aTag, TUint32 aValue )
	{
	TUint8* data = new( ELeave )TUint8[ 4 ];
	CleanupStack::PushL(data);
	if( iIntelByteOrder )
		{
		data[ 0 ] = aValue & 255;
		data[ 1 ] = ( aValue >> 8 ) & 255;
		data[ 2 ] = ( aValue >> 16 ) & 255;
		data[ 3 ] = ( aValue >> 24 ) & 255;
		}
	else
		{
		data[ 3 ] = aValue & 255;
		data[ 2 ] = ( aValue >> 8 ) & 255;
		data[ 1 ] = ( aValue >> 16 ) & 255;
		data[ 0 ] = ( aValue >> 24 ) & 255;
		}
	AddTagL( aIfd, aTag, ELong, data, 4 );
	CleanupStack::Pop();
	}



void CExifParser::AddTagL( TInt aIfd, TUint16 aTag, TUint16 aValue )
	{
	TUint8* data = new( ELeave )TUint8[ 2 ];
	CleanupStack::PushL(data);
	if( iIntelByteOrder )
		{
		data[ 0 ] = aValue & 255;
		data[ 1 ] = aValue / 256;
		}
	else
		{
		data[ 1 ] = aValue & 255;
		data[ 0 ] = aValue / 256;
		}
	AddTagL( aIfd, aTag, EShort, data, 2 );
	CleanupStack::Pop();
	}



void CExifParser::AddTagL( TInt aIfd, TUint16 aTag, TUint8 aValue )
	{
	TUint8* data = new( ELeave )TUint8[ 1 ];
	CleanupStack::PushL(data);
	*data = aValue;
	AddTagL( aIfd, aTag, EByte, data, 1 );
	CleanupStack::Pop();
	}



void CExifParser::AddTagL( TInt aIfd, TUint16 aTag, TPtrC8 aData )
	{
	TUint8* data = new( ELeave )TUint8[ aData.Length() ];
	CleanupStack::PushL(data);
	Mem::Copy( data, aData.Ptr(), aData.Length() );
	AddTagL( aIfd, aTag, EUndefined, data, aData.Length() );
	CleanupStack::Pop();
	}



void CExifParser::AddTagL( TInt aIfd, TUint16 aTag, const TUint8* aValue )
	{
	TInt l = User::StringLength( aValue ) + 1;
	TUint8* data = new( ELeave )TUint8[ l ];
	CleanupStack::PushL(data);
	Mem::Copy( data, aValue, l );
	AddTagL( aIfd, aTag, EAscii, data, l );
	CleanupStack::Pop();
	}



void CExifParser::AddTagL( TInt aIfd, TUint16 aTag, TUint32 aNominator, TUint32 aDenominator )
	{
	TUint8* data = new( ELeave )TUint8[ 8 ];
	CleanupStack::PushL(data);
	if( iIntelByteOrder )
		{
		data[ 0 ] = aNominator & 255;
		data[ 1 ] = ( aNominator >> 8 ) & 255;
		data[ 2 ] = ( aNominator >> 16 ) & 255;
		data[ 3 ] = ( aNominator >> 24 ) & 255;
		data[ 4 ] = aDenominator & 255;
		data[ 5 ] = ( aDenominator >> 8 ) & 255;
		data[ 6 ] = ( aDenominator >> 16 ) & 255;
		data[ 7 ] = ( aDenominator >> 24 ) & 255;
		}
	else
		{
		data[ 3 ] = aNominator & 255;
		data[ 2 ] = ( aNominator >> 8 ) & 255;
		data[ 1 ] = ( aNominator >> 16 ) & 255;
		data[ 0 ] = ( aNominator >> 24 ) & 255;
		data[ 7 ] = aDenominator & 255;
		data[ 6 ] = ( aDenominator >> 8 ) & 255;
		data[ 5 ] = ( aDenominator >> 16 ) & 255;
		data[ 4 ] = ( aDenominator >> 24 ) & 255;
		}
	AddTagL( aIfd, aTag, ERational, data, 8 );
	
	CleanupStack::Pop();
	}



void CExifParser::AddTagL( TInt aIfd, TUint16 aTag, TInt aType, TUint8* aData, TInt aDataLen )
	{
	TExifEntry* e = new( ELeave )TExifEntry;
	e->iIfd = aIfd;
	e->iTag = aTag;
	e->iType = aType;
	e->iData = aData;
	e->iCount = aDataLen / KTypeLength[ aType ];
	iEntry.Append( e );
	}


