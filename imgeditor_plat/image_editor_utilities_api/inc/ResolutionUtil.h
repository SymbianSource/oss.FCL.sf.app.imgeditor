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
* CResolutionUtil provides functions to query current screen dimensions.
*
*/



#ifndef RESOLUTIONUTIL_H
#define RESOLUTIONUTIL_H

#include <e32base.h>
#include <coemain.h>

/**
 *  The class provides functions to query current screen dimensions.
 *  The class is used through CCoeEnv::Static()
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ImageEditorUtils.lib
 *  @since S60 v5.0
 */
class CResolutionUtil : public CCoeStatic
    {

public:

    /**  Screen modes */
    enum TScreenModes
        {
        EUnknown,
        EStandard,
        EStandardFullScreen,
        EStandardLandscape,
        EStandardLandscapeFullScreen,
        ESquare,
        ESquareFullScreen,
        ESquareRotated,
        ESquareRotatedFullScreen,
        EQVGA,
        EQVGAFullScreen,
        EQVGALandscape,
        EQVGALandscapeFullScreen,
        EDouble,
        EDoubleFullScreen,
        EDoubleLandscape,
        EDoubleLandscapeFullScreen,
        EHVGA,
        EHVGAFullScreen,
        EHVGALandscape,
        EHVGALandscapeFullScreen,
        EVGA,
        EVGAFullScreen,
        EVGALandscape,
        EVGALandscapeFullScreen,
        EQHD,
        EQHDFullScreen,
        EQHDLandscape,
        EQHDLandscapeFullScreen
        };

    /**
     * Returns the singleton instance of this CCoeStatic.
     * If the instance does not exist, panics the tread.
     *
     * @since S60 v5.0
     * @param -
     * @return the singleton instance of CResolutionUtil
     */
     IMPORT_C static CResolutionUtil* Self();

    /**
     * Returns the singleton instance of this CCoeStatic.
     * If the instance does not exist, a new one is created.
     *
     * @since S60 v5.0
     * @param -
     * @return the singleton instance of CResolutionUtil
     */
     IMPORT_C static CResolutionUtil* InitializeL();

    /**
     * Destructor.
     */
     IMPORT_C virtual ~CResolutionUtil();

    /**
     * Returns the screen mode.
     *
     * @since S60 v5.0
     * @param -
     * @return screen mode, see TScreenModes
     */
     IMPORT_C TInt ScreenMode() const;
    
    /**
     * Returns the screen mode. Return the real screen mode i.e. Image Editor
     * flagging has no effect on return value.
     *
     * @since S60 v5.0
     * @param -
     * @return screen mode, see TScreenModes
     */
     IMPORT_C TInt GetScreenMode() const;
     
    /**
     * Returns the screen rect for the current screen mode.
     *
     * @since S60 v5.0
     * @param aRect screen rect for specified screen mode
     * @return -
     */
     IMPORT_C void GetScreenRect(TRect& aRect) const;

    /**
     * Sets the screen rect for the current screen mode.
     *
     * @since S60 v5.0
     * @param aRect screen rect for specified screen mode
     * @return -
     */
     IMPORT_C void SetScreenRect(const TRect& aRect);

   /**
     * Gets the client rect for the current screen mode.
     *
     * @since S60 v5.0
     * @param aRect client rect for specified screen mode
     * @return -
     */
     IMPORT_C void GetClientRect(TRect& aRect) const; 

   /**
     * Sets the client rect for the current screen mode.
     *
     * @since S60 v5.0
     * @param aRect client rect for specified screen mode
     * @return -
     */
     IMPORT_C void SetClientRect(const TRect& aRect);

   /**
     * Determines the current screen size and updates rect.
     *
     * @since S60 v5.0
     * @param -
     * @return -
     */
     IMPORT_C void UpdateScreenMode();

   /**
     * Sets Full Screen mode to true or false.
     *
     * @since S60 v5.0
     * @param aFullScreen ETrue if full screen, otherwise EFalse
     * @return -
     */
     IMPORT_C void SetFullScreen(TBool aFullScreen);

   /**
     * Gets the Full Screen state.
     *
     * @since S60 v5.0
     * @param aFullScreen ETrue if full screen, otherwise EFalse
     * @return -
     */
     IMPORT_C TBool GetFullScreen() const;

  /**
     * Sets the landscape mode to true or false.
     *
     * @since S60 v5.0
     * @param aLandscape ETrue if landscape, otherwise EFalse
     * @return -
     */
     IMPORT_C void SetLandscape(TBool aLandscape);

  /**
     * Gets the landscape mode to true or false.
     *
     * @since S60 v5.0
     * @param aLandscape ETrue if landscape, otherwise EFalse
     * @return -
     */
     IMPORT_C TBool GetLandscape() const;
     
private:

	CResolutionUtil( const TUid& aUid );

private: 

    /**
     * Screen rect
     */
    TRect iScreenRect;
    
    /**
     * Client rect
     */    
    TRect iClientRect;
    
    /**
     * Landscape mode
     */        
    TBool iLandscape;
    
    /**
     * Full Screen mode
     */        
    TBool iFullScreen;
    
    };

#endif // RESUTIL_H
