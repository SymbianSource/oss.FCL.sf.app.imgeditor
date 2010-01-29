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
#include "WaitIndicator.h"
#include <ImageEditorUI.mbg>

#include <fbs.h>
#include <w32std.h>

#include <AknIconUtils.h>
#include <data_caging_path_literals.hrh>
#include <aknlayoutscalable_apps.cdl.h>

// -----------------------------------------------------------------------------
// CWaitIndicator::CWaitIndicator()
// C++ constructor can NOT contain any code, that might leave.
// -----------------------------------------------------------------------------
//
CWaitIndicator::CWaitIndicator()
:CTimer( EPriorityStandard )
    {
    CActiveScheduler::Add( this );
	}

// -----------------------------------------------------------------------------
// CWaitIndicator::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWaitIndicator* CWaitIndicator::NewL()
	{
    CWaitIndicator* self = new( ELeave ) CWaitIndicator();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
	}

// -----------------------------------------------------------------------------
// CWaitIndicator::ConstructL
// Symbian two phased constructor may leave
// -----------------------------------------------------------------------------
//
void CWaitIndicator::ConstructL()
    {
    CTimer::ConstructL();

    // Bitmap file name and drive letter.
    // Path is generated using PathInfo.
    _LIT( KMbmDrive, "z:" );
    _LIT( KMbmFileName, "ImageEditorUI.mbm" );

    // Waitnote bitmap count
    const TInt KWaitnoteBitmapCount = 10;

	TFileName mbmPath;
	mbmPath.Copy( KMbmDrive );
	mbmPath.Append( KDC_APP_BITMAP_DIR );
	mbmPath.Append( KMbmFileName );

	for ( TInt i = 0; i < KWaitnoteBitmapCount; ++i )
		{
		CFbsBitmap* bitmap;
		CFbsBitmap* mask;

		// *2 for bitmap and mask
		AknIconUtils::CreateIconL( bitmap, mask, mbmPath,
						EMbmImageeditoruiQgn_graf_ring_wait_01 + i * 2,
						EMbmImageeditoruiQgn_graf_ring_wait_01_mask + i * 2 );

        TInt err = iBitmaps.Append( bitmap );
		if( err )
		    {
		    delete bitmap;
		    delete mask;
		    }
        else
            {
            err = iBitmaps.Append( mask );
            if( err )
                {
		        delete mask;
                }            
            }
	    User::LeaveIfError( err );
		}
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CWaitIndicator::~CWaitIndicator()
    {
    Cancel();
    iBitmaps.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CWaitIndicator::
// -----------------------------------------------------------------------------
//
TBool CWaitIndicator::IsVisible()
    {
    return IsActive();
    }

// -----------------------------------------------------------------------------
// CWaitIndicator::
// -----------------------------------------------------------------------------
//
void CWaitIndicator::Show()
    {
	// From LAF
	TTimeIntervalMicroSeconds32 KWaitNoteDelay = TTimeIntervalMicroSeconds32( 1000*100 );
    
    if( !IsActive() )
        {
        // Start timer if not already
        After( KWaitNoteDelay );
        }
    }

// -----------------------------------------------------------------------------
// CWaitIndicator::
// -----------------------------------------------------------------------------
//
void CWaitIndicator::Hide()
    {
    // Stop waitnote
    Cancel();

    // Request draw event
    DrawNow();
    }

// ----------------------------------------------------------------------------
// CWaitIndicator::RunL
// ----------------------------------------------------------------------------
//
void CWaitIndicator::RunL()
	{
	// From LAF
	TTimeIntervalMicroSeconds32 KWaitNoteInterval = TTimeIntervalMicroSeconds32( 1000*100 );

    // Waitnote bitmap count
    const TInt KWaitnoteBitmapCount = 10;

    // Start timer again
    After( KWaitNoteInterval );

	// Draw next frame
    ++iIndex;
    if ( iIndex >= KWaitnoteBitmapCount )
        {
        iIndex = 0;
        }
    DrawNow();
	}

// -----------------------------------------------------------------------------
// CWaitIndicator::Draw
// -----------------------------------------------------------------------------
//
void CWaitIndicator::Draw( CWindowGc& aGc ) const
    {
    if( IsActive() )
        {
        // *2 because masks are in same array
        CFbsBitmap* bitmap = iBitmaps[iIndex * 2];
        CFbsBitmap* mask = iBitmaps[iIndex * 2 + 1];

        TSize bitmapSize( bitmap->SizeInPixels() );
        TSize rectSize( iRect.Size() );
        TPoint topLeft( iRect.iTl );

        // Center the image
        if( bitmapSize.iWidth < rectSize.iWidth )
            {
            topLeft.iX = ( rectSize.iWidth - bitmapSize.iWidth ) / 2;
            }
        if( bitmapSize.iHeight < rectSize.iHeight )
            {
            topLeft.iY = ( rectSize.iHeight - bitmapSize.iHeight ) / 2;
            }

        // Draw wait indicator
        aGc.BitBltMasked( topLeft, bitmap, TRect( bitmapSize ), mask, EFalse );
        }
    }
    
// -----------------------------------------------------------------------------
// CWaitIndicator::
// -----------------------------------------------------------------------------
//
void CWaitIndicator::SizeChanged()
    {
    // Waitnote bitmap count
    const TInt KWaitnoteBitmapCount = 10;

    // From LAF
	// Variety 0
	TAknWindowLineLayout layout = AknLayoutScalable_Apps::main_image2_pane_g1( 0 );
    TSize waitNoteSize( layout.iW, layout.iH );
	for ( TInt i = 0; i < KWaitnoteBitmapCount; ++i )
		{
        AknIconUtils::SetSize( iBitmaps[i * 2], waitNoteSize );
		}
    }
    
// End of File
