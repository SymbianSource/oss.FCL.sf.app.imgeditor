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


#include "BitField.h"

//=============================================================================
EXPORT_C TBitField::TBitField()
{
	Reset();
}

//=============================================================================
EXPORT_C TBitField::~TBitField ()
{

}

//=============================================================================
EXPORT_C TBitField::TBitField (const TBitField & rhs)
{
	Mem::Copy ( iData, rhs.iData, (KSize >> 3) + 1 );
}

//=============================================================================
EXPORT_C TBitField & TBitField::operator= (const TBitField & rhs)
{
	if ( this != &rhs )
	{
		Mem::Copy (iData, rhs.iData, (KSize >> 3) + 1);
	}
	return *this;
}

//=============================================================================
EXPORT_C void TBitField::Reset ()
{
	Mem::FillZ (iData, (KSize >> 3) + 1);
}

//=============================================================================
EXPORT_C void TBitField::SetBit (const TUint32 aIndex)
{
	if ( aIndex >= KSize)
	{
		User::Panic (KClassName, 21);
	}
	iData[aIndex >> 3] |= (0x1 << (aIndex & 0x7));
}

//=============================================================================
EXPORT_C TInt TBitField::GetBit (const TUint32 aIndex) const
{
	if ( aIndex >= KSize)
	{
		User::Panic (KClassName, 21);
	}
	return ((iData[aIndex >> 3]) >> (aIndex & 0x7) & 0x1);
}

// End of File
