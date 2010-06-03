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


#ifndef T_DRAWPATH_H
#define T_DRAWPATH_H


#include <e32cmn.h>
#include <e32std.h>
#include <gdi.h>
 
/**
 *  Container for draw path
 *
 *  @lib internal(filterdraw.dll)
 *  @since S60 5.0
 */
class RDrawPath : public RArray<TPoint>
    {
public:

    RDrawPath();
	TBuf<20> iBuf;
    
public:
    /**
     * Color.
     *
     * @since S60 5.0
     * @return Path color
     */
	TRgb Color(){return iColor;};
    
    /**
     * SetColor. (defalt KRgbRed)
     *
     * @since S60 5.0
     * @param aRgb Path color
     */	
	void SetColor(TRgb aRgb){iColor=aRgb;};
	
	/**
     * Size.
     *
     * @since S60 5.0
     * @return Path size
     */
	TSize Size(){return iSize;};
    
    /**
     * SetSize.
     *
     * @since S60 5.0
     * @param aSize Path size
     */	
	void SetSize(TSize aSize){iSize=aSize;};

private: // data

    /**
     * Path color 
     */

    TRgb iColor;
	TSize iSize;
    };

#endif // T_DRAWPATH_H
