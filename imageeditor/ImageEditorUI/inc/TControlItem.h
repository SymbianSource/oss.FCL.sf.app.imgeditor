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


#ifndef TCONTROLITEM_H
#define TCONTROLITEM_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CCoeControl;
class CWindowGc;

// CLASS DECLARATION
/**
*  TIVControlItem class.
*
*  @lib ImageViewer.dll
*/
NONSHARABLE_CLASS( TControlItem )
    {
    public: // Methods used from parent control

        void SetParent( CCoeControl* aParent );
        void SetItemRect( const TRect& aRect );
        void DrawItem( CWindowGc& aGc ) const;

    protected: // Methods used from derived sub item

        void DrawNow();

    private: // Methods implemented in sub item

        virtual void Draw( CWindowGc& aGc ) const = 0;
        virtual void SizeChanged() = 0;

    protected: // Data

        // Parent control (reference)
        CCoeControl* iParent;

        // Sub item rect
        TRect iRect;
    };

#endif // TCONTROLITEM_H

// End of File

