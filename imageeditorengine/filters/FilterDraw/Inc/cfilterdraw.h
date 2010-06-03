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
* Filter for Draw UI plugin.
*
*/


#ifndef __CFILTER_DRAW_H_
#define __CFILTER_DRAW_H_

#include <e32base.h>
#include "MImageFilter.h"
#include "drawpath.h"

/**
 *  CFilterDraw
 *
 *  @lib internal(filterdraw.dll)
 *  @since S60 5.0
 */
class CFilterDraw: public CBase, public MImageFilter
	{
	public: // C'tor & destructor
		IMPORT_C static TInt Create();
		~CFilterDraw();
	
	private: // called from Create()
		static CFilterDraw* NewL();
		CFilterDraw();
	
	public: // From MImageFilter
		TRect Rect();
        TReal Scale();
        TSize ViewPortSize();
		TBlock* GetBlockL ( const TRect& aRect );
		void SetParent( MImageFilter* aParent );
		void SetChild( MImageFilter* aChild );
		TInt CmdL( const TDesC16& aCmd );
		const char* Type();
		void LoadFrameL();
	
	private: // implementation		
	    void RDrawPath2PointArray( const RDrawPath& aPath,
    		CArrayFix<TPoint>*& aArrayPtr ) const;
	    void RealToViewedFactories(TReal& aWidth, TReal& aHeight);
	
	private: // Data
	    // Data is ready to be rendered
	    TBool iReadyToRender;
	    //If it can redo
	    TBool iCanRedo;
		/// Data buffer
	    static	TUint32* iData;
		// Drawed lines
		RArray<RDrawPath> iPaths;	
		// Bitmap size 
		TSize iBitmapSize;	
		//Undo Path
		RArray<RDrawPath> iUndoPaths;
		CFbsBitmap* iBitmap;
		CFbsBitmap* iMask;
    };

#endif // __CFILTER_DRAW_H_
