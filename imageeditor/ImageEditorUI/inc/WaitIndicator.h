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


#ifndef CWAITINDICATOR_H
#define CWAITINDICATOR_H

//  INCLUDES
#include "TControlItem.h"
#include <e32base.h>

// FORWARD DECLARATIONS
class CWindowGc;
class CFbsBitmap;

// CLASS DECLARATION
/**
*  CWaitIndicator control class.
*
*
*  @lib ImageEditor.app/ImageEditor.exe
*  @since 2.6
*/
NONSHARABLE_CLASS( CWaitIndicator ) : private CTimer, public TControlItem
    {
    public: // Constructors and destructor

		static CWaitIndicator* NewL();
        ~CWaitIndicator();

    public: // New functions

        TBool IsVisible();
        void Show();
        void Hide();

    public:
    
        void SetRect( const TRect& aRect );
        void Draw( CWindowGc& aGc ) const;

    private: // From CTimer

        void RunL();

    private: // From TIVControlItem
    
        void Draw( const TRect& aRect ) const;
        void SizeChanged();

    private: // Constructors and destructor

		CWaitIndicator();
        void ConstructL();

    private: //data
        
		// Bitmaps and masks for wait note
		RPointerArray<CFbsBitmap> iBitmaps;

		// Current index of waitnote bitmap animation
		TInt iIndex;

    };
#endif  // CWAITINDICATOR_H

// End of File
