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


// INCLUDE FILES
#include "TControlItem.h"
#include <coecntrl.h>

// -----------------------------------------------------------------------------
// TControlItem::
// -----------------------------------------------------------------------------
//
void TControlItem::SetParent( CCoeControl* aParent )
    {
    iParent = aParent;
    }

// -----------------------------------------------------------------------------
// TControlItem::
// -----------------------------------------------------------------------------
//
void TControlItem::SetItemRect( const TRect& aRect )
    {
    iRect = aRect;
    SizeChanged();
    }

// -----------------------------------------------------------------------------
// TControlItem::
// -----------------------------------------------------------------------------
//
void TControlItem::DrawItem( CWindowGc& aGc ) const
    {
    Draw( aGc );
    }
    
// -----------------------------------------------------------------------------
// TControlItem::
// -----------------------------------------------------------------------------
//
void TControlItem::DrawNow()
    {
    if( iParent ) iParent->DrawNow();
    }

// End of File
