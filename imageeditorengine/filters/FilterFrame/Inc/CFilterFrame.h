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


#ifndef __CFilterFrame_H__
#define __CFilterFrame_H__

#include <e32base.h>
#include "MImageFilter.h"

class CFbsBitmap;

class CFilterFrame
	: public CBase
	, public MImageFilter
	{
	public:
		IMPORT_C static TInt Create();
		virtual ~CFilterFrame();
	
	private:
		static CFilterFrame* NewL();
		CFilterFrame();
		void ConstructL();
	
	public: // MImageFilter
		virtual TRect Rect();
        virtual TReal Scale();
        virtual TSize ViewPortSize();
		virtual TBlock * GetBlockL ( const TRect & aRect );
		virtual void SetParent( MImageFilter* aParent );
		virtual void SetChild( MImageFilter* aChild );
		virtual TInt CmdL( const TDesC16& aCmd );
		virtual const char* Type();
		virtual void LoadFrameL();

	private: // Data

		/// Frame buffer
		TUint32 *		iData;
		/// File name
		TFileName		iFile;
		/// Frame index
		TInt			iFrameInd;
		/// Mask index
		TInt			iMaskInd;
		///	Frame size
		TSize			iSize;
		///	Frame scale
		TPoint			iScale;
		/// Frame row offset
		TInt			iWsFrame;
		/// Mask row offset
		TInt			iWsMask;
    
        TRect           iOrigRect;
        TPoint          iOffset;
    };

#endif
