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
* Container for draw path.
*
*/

#include "drawpath.h"

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CDrawPath* CDrawPath::NewL()
	{
	CDrawPath* self = new( ELeave ) CDrawPath;
	TInt arrayGranularity(8);
	CleanupStack::PushL( self );
	self->iItemArray = new( ELeave ) 
		CArrayFixFlat<TPoint> ( arrayGranularity );
	CleanupStack::Pop( self );
	return self;
	}
	
// ---------------------------------------------------------------------------
// ~CDrawPath
// ---------------------------------------------------------------------------
//
CDrawPath::~CDrawPath()
	{
	delete iItemArray;
	}

// ---------------------------------------------------------------------------
// AddItemL
// ---------------------------------------------------------------------------
//
void CDrawPath::AddItemL(TPoint aItem)
	{
	ASSERT( iItemArray );
	iItemArray->AppendL( aItem );
	}

// ---------------------------------------------------------------------------
// CDrawPath
// ---------------------------------------------------------------------------
//
CDrawPath::CDrawPath():iColor(KRgbWhite)
    {
    }

