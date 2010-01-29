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


#ifndef __CEXIFPARSER_H__
#define __CEXIFPARSER_H__

#include <e32base.h>



class TExifEntry
	{
	public:
		inline TExifEntry()
			{
			iIfd = 0;
			iTag = 0;
			iType = 0;
			iCount = 0;
			iData = NULL;
			}

		inline ~TExifEntry()
			{
			delete iData;
			}
	public:
		TUint8 iIfd;
		TUint16 iTag;
		TUint8 iType;
		TUint32 iCount;
		TUint8* iData;
	};


class TParseStack
	{
	public:
		inline TParseStack( TUint32 aPosition, TUint8 aIfd )
			: iPosition( aPosition )
			, iIfd( aIfd )
			{
			}
	public:
		TUint32 iPosition;
		TUint8 iIfd;
	};



class CExifParser
	: public CBase
	{
	public:
		enum EIfd
			{
			EIfd0 = 0,
			EIfd1,
			ESubIfd,
			EInteroperability
			};

	public:
		static CExifParser* NewL();
		static CExifParser* NewLC();
		~CExifParser();

	private:
		CExifParser();
		void ConstructL();

	public:
		void ParseL( const TPtrC8& aData );
		TBool TagExist( TInt aIfd, TUint16 aTag );
		void DeleteTag( TInt aIfd, TUint16 aTag );
		TUint32 TagValue( TInt aIfd, TUint16 aTag );
		
		void AddTagL( TInt aIfd, TUint16 aTag, TInt32 aValue );
		void AddTagL( TInt aIfd, TUint16 aTag, TUint32 aValue );
		void AddTagL( TInt aIfd, TUint16 aTag, TUint16 aValue );
		void AddTagL( TInt aIfd, TUint16 aTag, TUint8 aValue );
		void AddTagL( TInt aIfd, TUint16 aTag, TPtrC8 aData );
		void AddTagL( TInt aIfd, TUint16 aTag, const TUint8* aValue );
		void AddTagL( TInt aIfd, TUint16 aTag, TUint32 aNominator, TUint32 aDenominator );

		TPtrC8 ThumbData();
		TPtr8 SaveL( const TPtrC8& aThumbData );


	private: // new methods
		TUint32 Get8();
		TUint32 Get16();
		TUint32 Get32();
		TUint32 Get8( const TUint8* aData );
		TUint32 Get16( const TUint8* aData );
		TUint32 Get32( const TUint8* aData );
		void ParseIfdL( TUint8 aIfd );
		TInt FindTag( TInt aIfd, TUint16 aTag );
		void StoreEntry( TInt aIndex );

		void Put8( TUint8 aData );
		void Put16( TUint16 aData );
		void Put32( TUint32 aData );

		TUint32 StoreData( TUint8* aData, TInt aCount );

		TInt IfdCount( TInt aType );

		void AddTagL( TInt aIfd, TUint16 aTag, TInt aType, TUint8* aData, TInt aDataLen );



	private:
		TPtrC8	iData;
		TInt	iPosition;
		TBool	iIntelByteOrder;

		RArray< TParseStack >iParseStack;
		RPointerArray< TExifEntry >iEntry;

		TUint8*	iSaveBuf;				// used to store tags
		TInt	iSaveBufPos;

		TUint8*	iSaveDataBuf;			// used to store tag data
		TInt	iSaveDataBufPos;

		RArray< TUint32 >iDataEntry;	// list of tag data pointers

		TUint8*	iThumbData;
		TInt	iThumbLen;
	};

#endif
