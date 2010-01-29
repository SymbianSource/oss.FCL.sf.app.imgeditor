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
* Resolution utility class for Image Editor.
*
*/


#include <eikappui.h>
#include <AknUtils.h> 

#include "ResolutionUtil.h"
#include "imageeditordebugutils.h"
//#include "ImageEditorUids.hrh"

// CONSTANTS
const TInt KStandardScreenWidth = 176;
const TInt KStandardScreenHeight = 208;

const TInt KSquareScreenWidth = 208;
const TInt KSquareScreenHeight = 208;

const TInt KQVGAScreenWidth = 240;
const TInt KQVGAScreenHeight = 320;

const TInt KDoubleScreenWidth = 352;
const TInt KDoubleScreenHeight = 416;

const TInt KHVGAScreenWidth = 320;
const TInt KHVGAScreenHeight = 480;

const TInt KVGAScreenWidth = 480;
const TInt KVGAScreenHeight = 640;

const TInt KQHDScreenWidth = 360;
const TInt KQHDScreenHeight = 640;

#define UID_RESOLUTION_UTIL  0x101FFA91

// CCoeStatic-derived class is accessible only inside the 
// thread, so the UID does not need to be globally unique.
const TUid KUidResolutionUtil = {UID_RESOLUTION_UTIL};

_LIT( KComponentName, "CResolutionUtil" );


//=============================================================================
CResolutionUtil::CResolutionUtil( const TUid& aUid )
: CCoeStatic( aUid ),
  iScreenRect(0,0,0,0),
  iClientRect(0,0,0,0),
  iLandscape(EFalse),
  iFullScreen(EFalse)
    {

    }

//=============================================================================
EXPORT_C CResolutionUtil::~CResolutionUtil()
{
    LOG(KImageEditorLogFile, "CResolutionUtil::~CResolutionUtil()");
}

//=============================================================================
EXPORT_C CResolutionUtil* CResolutionUtil::Self()
{
    CResolutionUtil* self = static_cast<CResolutionUtil *>( CCoeEnv::Static (KUidResolutionUtil) );
    __ASSERT_ALWAYS( self, User::Panic(KComponentName,KErrNotFound) );
    return self;
}

//=============================================================================
EXPORT_C CResolutionUtil* CResolutionUtil::InitializeL()
{
    LOG(KImageEditorLogFile, "CResolutionUtil::InitializeL()");

    // First check if this already exists
    CResolutionUtil* self = static_cast<CResolutionUtil *>( CCoeEnv::Static (KUidResolutionUtil) );
    
    // If not, create new instance
    if ( !self )
    {
        self = new (ELeave) CResolutionUtil( KUidResolutionUtil );
        LOG(KImageEditorLogFile, "CResolutionUtil::InitializeL(): created new instance");
    }

    return self;
}
    
//=============================================================================
EXPORT_C TInt CResolutionUtil::ScreenMode() const
    {

    TInt screenMode = EUnknown;
    TSize screenSize = iScreenRect.Size();

    if (!iLandscape)
        {
        switch (screenSize.iWidth)
            {
            // Standard
            case KStandardScreenWidth:
                {
                if (screenSize.iHeight == KStandardScreenHeight)
                    {
                    if (!iFullScreen)
                        {
                        screenMode = EStandard;
                        }                     
                    else
                        {
                        screenMode = EStandardFullScreen;
                        }
                    }
                break;
                }
            // Square
            case KSquareScreenWidth:
                {
                if (screenSize.iHeight == KSquareScreenHeight)
                    {
                    if (!iFullScreen)
                        {
                        screenMode = ESquare;
                        }                     
                    else
                        {
                        screenMode = ESquareFullScreen;
                        }
                    }
                break;
                }
            // QVGA
            case KQVGAScreenWidth:
                {
                if (screenSize.iHeight == KQVGAScreenHeight)
                    {
                    if (!iFullScreen)
                        {
                        screenMode = EQVGA;
                        }
                    else
                        {
                        screenMode = EQVGAFullScreen;
                        }
                    }
                break;
                }
            // Double
            case KDoubleScreenWidth:
                {
                if (screenSize.iHeight == KDoubleScreenHeight)
                    {
                    if (!iFullScreen)
                        {
                        screenMode = EDouble;
                        }
                    else
                        {
                        screenMode = EDoubleFullScreen;
                        }
                    }
                break;
                }
            // Unknown
            default:
                {
                screenMode = EUnknown;
                break;
                }
            }
        }
    else
        {
        // Landscape iHeight == normal iWidth
        switch (screenSize.iHeight)
            {
            // Standard, landscape
            case KStandardScreenWidth:
                {
                if (screenSize.iWidth == KStandardScreenHeight)
                    {
                    if (!iFullScreen)
                        {
                        screenMode = EStandardLandscape;
                        }                     
                    else
                        {
                        screenMode = EStandardLandscapeFullScreen;
                        }
                    }
                break;
                }
            // Square, rotated
            case KSquareScreenWidth:
                {
                if (screenSize.iWidth == KSquareScreenHeight)
                    {
                    if (!iFullScreen)
                        {
                        screenMode = ESquareRotated;
                        }                     
                    else
                        {
                        screenMode = ESquareRotatedFullScreen;
                        }
                    }
                break;
                }
            // QVGA, landscape
            case KQVGAScreenWidth:
                {
                if (screenSize.iWidth == KQVGAScreenHeight)
                    {
                    if (!iFullScreen)
                        {
                        screenMode = EQVGALandscape;
                        }
                    else
                        {
                        screenMode = EQVGALandscapeFullScreen;
                        }
                    }
                break;
                }
            // Double, landscape
            case KDoubleScreenWidth:
                {
                if (screenSize.iWidth == KDoubleScreenHeight)
                    {
                    if (!iFullScreen)
                        {
                        screenMode = EDoubleLandscape;
                        }
                    else
                        {
                        screenMode = EDoubleLandscapeFullScreen;
                        }
                    }
                break;
                }
            // Unknown
            default:
                {
                screenMode = EUnknown;
                break;
                }
            }

        }

    return screenMode;
    }

//=============================================================================
EXPORT_C TInt CResolutionUtil::GetScreenMode() const
    {
    TInt screenMode = EUnknown;
    TSize screenSize = iScreenRect.Size();
    
    // Standard (Portrait)
    if ( screenSize.iWidth == KStandardScreenWidth &&
         screenSize.iHeight == KStandardScreenHeight )
        {
        if (!iFullScreen)
            {
            screenMode = EStandard;
            }
        else
            {
            screenMode = EStandardFullScreen;
            }
        }
    // Square (Portrait)
    else if ( screenSize.iWidth == KSquareScreenWidth &&
              screenSize.iHeight == KSquareScreenHeight )
        {
        if (!iFullScreen)
            {
            screenMode = ESquare;
            }
        else
            {
            screenMode = ESquareFullScreen;
            }
        }
    // QVGA (Portrait)
    else if ( screenSize.iWidth == KQVGAScreenWidth &&
              screenSize.iHeight == KQVGAScreenHeight )
        {
        if (!iFullScreen)
            {
            screenMode = EQVGA;
            }
        else
            {
            screenMode = EQVGAFullScreen;
            }
        }
    // Double (Portrait)
    else if ( screenSize.iWidth == KDoubleScreenWidth &&
          screenSize.iHeight == KDoubleScreenHeight )
        {
        if (!iFullScreen)
            {
            screenMode = EDouble;
            }
        else
            {
            screenMode = EDoubleFullScreen;
            }
        }
    // HVGA (Portrait) 
    else if ( screenSize.iWidth == KHVGAScreenWidth &&
              screenSize.iHeight == KHVGAScreenHeight )
        {
        if (!iFullScreen)
            {
            screenMode = EHVGA;
            }
        else
            {
            screenMode = EHVGAFullScreen;
            }
        }
    // VGA (Portrait)
    else if ( screenSize.iWidth == KVGAScreenWidth &&
              screenSize.iHeight == KVGAScreenHeight )
        {
        if (!iFullScreen)
            {
            screenMode = EVGA;
            }
        else
            {
            screenMode = EVGAFullScreen;
            }
        }
    // QHD (Portrait)
    else if ( screenSize.iWidth == KQHDScreenWidth &&
              screenSize.iHeight == KQHDScreenHeight )
        {
        if (!iFullScreen)
            {
            screenMode = EQHD;
            }
        else
            {
            screenMode = EQHDFullScreen;
            }
        }    
    // Standard (Landscape)
    else if ( screenSize.iWidth == KStandardScreenHeight &&
         screenSize.iHeight == KStandardScreenWidth )
        {
        if (!iFullScreen)
            {
            screenMode = EStandardLandscape;
            }
        else
            {
            screenMode = EStandardLandscapeFullScreen;
            }
        }
    // Square (Landscape)
    else if ( screenSize.iWidth == KSquareScreenHeight &&
              screenSize.iHeight == KSquareScreenWidth )
        {
        if (!iFullScreen)
            {
            screenMode = ESquareRotated;
            }
        else
            {
            screenMode = ESquareRotatedFullScreen;
            }
        }
    // QVGA (Landscape)
    else if ( screenSize.iWidth == KQVGAScreenHeight &&
              screenSize.iHeight == KQVGAScreenWidth )
        {
        if (!iFullScreen)
            {
            screenMode = EQVGALandscape;
            }
        else
            {
            screenMode = EQVGALandscapeFullScreen;
            }
        }
    // Double (Landscape)
    else if ( screenSize.iWidth == KDoubleScreenHeight &&
          screenSize.iHeight == KDoubleScreenWidth )
        {
        if (!iFullScreen)
            {
            screenMode = EDoubleLandscape;
            }
        else
            {
            screenMode = EDoubleLandscapeFullScreen;
            }
        }
    // HVGA (Landscape)
    else if ( screenSize.iWidth == KHVGAScreenHeight &&
              screenSize.iHeight == KHVGAScreenWidth )
        {
        if (!iFullScreen)
            {
            screenMode = EHVGALandscape;
            }
        else
            {
            screenMode = EHVGALandscapeFullScreen;
            }
        }
    // VGA (Landscape)
    else if ( screenSize.iWidth == KVGAScreenHeight &&
              screenSize.iHeight == KVGAScreenWidth )
        {
        if (!iFullScreen)
            {
            screenMode = EVGALandscape;
            }
        else
            {
            screenMode = EVGALandscapeFullScreen;
            }
        }
    // QHD (Landscape)
    else if ( screenSize.iWidth == KQHDScreenHeight &&
              screenSize.iHeight == KQHDScreenWidth )
        {
        if (!iFullScreen)
            {
            screenMode = EQHDLandscape;
            }
        else
            {
            screenMode = EQHDLandscapeFullScreen;
            }
        }    
    // Unknown    
    else 
        {
        screenMode = EUnknown;
        }
         
    return screenMode;                         
    }
    
//=============================================================================
EXPORT_C void CResolutionUtil::GetClientRect(TRect& aRect) const
    {
    aRect = iClientRect; 
    }

//=============================================================================
EXPORT_C void CResolutionUtil::SetClientRect(const TRect& aRect)
    {
    iClientRect = aRect; 
    }

//=============================================================================
EXPORT_C void CResolutionUtil::GetScreenRect(TRect& aRect) const
    {
    aRect = iScreenRect; 
    }

//=============================================================================
EXPORT_C void CResolutionUtil::SetScreenRect(const TRect& aRect) 
    {
    iScreenRect = aRect; 
    }

//=============================================================================
EXPORT_C void CResolutionUtil::SetFullScreen(TBool aFullScreen)
    {
    iFullScreen = aFullScreen; 
    }

//=============================================================================
EXPORT_C TBool CResolutionUtil::GetFullScreen() const
    {
    return iFullScreen; 
    }

//=============================================================================
EXPORT_C void CResolutionUtil::SetLandscape(TBool aLandscape)
    {
    iLandscape = aLandscape; 
    }

//=============================================================================
EXPORT_C TBool CResolutionUtil::GetLandscape() const
    {
    return iLandscape; 
    }

//=============================================================================
EXPORT_C void CResolutionUtil::UpdateScreenMode()
    {
    LOG(KImageEditorLogFile, "CResolutionUtil::UpdateScreenMode()");

    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, iScreenRect);
    //iScreenRect.SetRect(0,0,352,416);

    if (iFullScreen)
        {
        iClientRect = iScreenRect;
        }
    else 
        {
        AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, iClientRect);
        }

    LOGFMT2(KImageEditorLogFile, "CResolutionUtil: Screen dimensions: W:%d, H:%d", iScreenRect.Width(), iScreenRect.Height());
    LOGFMT4(KImageEditorLogFile, "  ClientRect: iTl.iX:%d, iTl.iY:%d, iBr.iX:%d, iBr.iY:%d", iClientRect.iTl.iX, iClientRect.iTl.iY, iClientRect.iBr.iX, iClientRect.iBr.iY);
    LOGFMT2(KImageEditorLogFile, "  Full screen: %d, Landscape mode: %d", iFullScreen, iLandscape);
    }

// End of File
