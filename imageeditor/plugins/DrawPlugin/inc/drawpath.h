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
 *  Container for draw path (i.e.TDrawItems)
 *
 *
 *  @lib internal(filterdraw.dll)
 *  @since S60 5.0
 */
NONSHARABLE_CLASS( CDrawPath ) : public CBase
    {
public:

	static CDrawPath* NewL();
	~CDrawPath();

public:
    /**
     * AddItemL.
     *
     * @since S60 5.0
     * Add new Item to array
     */
	void AddItemL(TPoint aItem);
	
    /**
     * ItemArray.
     *
     * @since S60 5.0
     * @return Pointer to item array.
     * ownership not transferred.
     */
	CArrayFix<TPoint>* ItemArray() const {return iItemArray;};
		
    /**
     * Color.
     *
     * @since S60 5.0
     * @return Path color
     */
	TRgb Color() const {return iColor;};
    
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
	TSize Size() const {return iSize;};
    
    /**
     * SetSize.
     *
     * @since S60 5.0
     * @param aSize Path size
     */	
	void SetSize(TSize aSize){iSize=aSize;};

private: // C'tor

    CDrawPath();
       
private: // data

    /**
     * Path color 
     */
    TRgb iColor;
    /**
     * Path Size
     */
	TSize iSize;
    /**
     * Array of pahts
     */	
	CArrayFix<TPoint>* iItemArray;
    };

#endif // T_DRAWPATH_H
