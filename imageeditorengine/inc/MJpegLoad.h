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


#ifndef __MJPEGLOAD_H__
#define __MJPEGLOAD_H__

#include <e32base.h>
#include "TBitmapHandle.h"


class TJpegData
	{
	public:
		TSize iSize;
		TSize iBlockSize;
		TSize iSizeInBlocks;
	};



/// Jpeg scale factor, used by SetScale()
/// scales are 1:1, 1:2, 1:4 and 1:8
enum TJpegScale
	{
	EScale1 = 0,
	EScale2 = 1,
	EScale4 = 2,
	EScale8 = 3
	};


/// Jpeg loader interface
class MJpegLoad
	{
	public:
		virtual ~MJpegLoad() {}

		/// Opens jpeg file for decoding
		/// @param aFile full filename of jpeg
		virtual void OpenL( const TFileName& aFile ) = 0;

		/// Opens jpeg file for decoding
		/// @param aData descriptor of jpeg file data
		virtual void OpenL( const TPtr8& aData ) = 0;

		/// Sets load scale
		/// @param aScale scale factor to use
		virtual void SetScale( TJpegScale aScale ) = 0;
		
		/// Loads image or partial image
		/// for normal image load, use Info().iSize
		/// @param aRect rectangle in pixels to load. 
		///			Returns real decoded size which is 
		///			macroblock size dependent
		/// @return TBitmapHandle loaded image.
		virtual TBitmapHandle LoadImageL( TRect& aRect ) = 0;
		
		/// Scans the jpeg file for random access load
		/// must be called before LoadBlock()
		virtual void ScanRandomL() = 0;

		/// Decodes one macroblock from jpeg
		/// @param aBlock x&y coordinates of macroblock to decode
		/// @return block bitmap in 32bit RGB ( 0RGB ) format
		virtual TBitmapHandle LoadBlockL( const TPoint& aBlock ) = 0;

		/// Gives info about jpeg
		/// Can only be called after OpenL
		/// return jpeg information struct
		virtual const TJpegData& Info() = 0;
		
		/// Gives Exif data chunk if any
		virtual TPtrC8 ExifData() = 0;


	};

#endif
