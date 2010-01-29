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


#ifndef __MJPEGSAVE_H__
#define __MJPEGSAVE_H__

#include <e32std.h>

class MJpegSave
	{
	public:
		virtual ~MJpegSave() {}

		/// Starts jpeg save
		/// @param aSize size of jpeg file
		/// @param aExif descriptor containing exif data
		/// @param aSaveBufferSize save buffer size in bytes
		/// @param aQuality quality factor 0..100
		virtual void StartSaveL( const TSize& aSize, TPtr8 aExif, TInt aSaveBufferSize, TInt aQuality ) = 0;

		/// Saves one macroblock
		/// at the moment macroblocks are 8x8 pixels 
		/// @param aBitmap 8x8 pixels RGB bitmap 32bits per pixel ( 0RGB )
		virtual void SaveBlock( const TBitmapHandle& aBitmap ) = 0;

		/// Finalizes the save to file
		virtual void FinalizeSave() = 0;

		/// Finalizes the save to buffer
		/// @return TPtrC8 save buffer descriptor
		virtual TPtrC8 Finalize() = 0;
	};
#endif
