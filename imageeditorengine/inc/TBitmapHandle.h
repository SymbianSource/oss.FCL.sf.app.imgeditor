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


#ifndef __TBITMAPHANDLE_H__
#define __TBITMAPHANDLE_H__

#include <e32std.h>

enum EBmpType 
	{
	EUnknownColor = -1,
	E4KColor = 0,
	E256Gray,
	E256Palette,
	E64KColor,
	E16MColor,
	E64Bit
	};

#define DEFAULTCOLOR E4KColor

/// \brief Bitmap handle. 
///
/// All bitmaps should be described with this type.
/// Used by MGc and MSystem interfaces
/// MGc uses E4KColor, E256Gray and E256Palette bitmaps
class TBitmapHandle
	{
	public:
		/// Default constructor
		/// fills in default values for bitmap
		inline TBitmapHandle() 
			{ 
			iType = DEFAULTCOLOR; 
			iData = NULL; 
			iSize = TSize( 0, 0 );
			iDrawRect = TRect( TPoint( 0,0 ), iSize );
			}
			
		/// Constructor
		/// @param aType bitmap color format
		/// @param aData bitmap pixel data
		/// @param aSize bitmap size
		/// @param aDrawRect rectangle inside bitmap to draw from / to
		inline TBitmapHandle( EBmpType aType, TAny* aData, const TSize& aSize, const TRect& aDrawRect ) 
			{ 
			iType = aType; 
			iData = aData; 
			iSize = aSize; 
			iDrawRect = aDrawRect;
			}
		inline ~TBitmapHandle() 
			{ 
			//delete iData; 
			}

	public:
		/// Bitmap color format
		/// Can be
		/// E4KColor
		/// E256Gray
		/// E256Palette
		/// E64KColor
		/// E16MColor
		/// E64Bit
		EBmpType iType;
		
		/// Bitmap pixel data
		TAny* iData;
		
		/// Bitmap size
		TSize iSize;
		
		/// Bitmap draw area
		/// MGc uses this area to draw from / to bitmap
		TRect iDrawRect;
		
	};

#endif //__TBITMAPHANDLE_H__
