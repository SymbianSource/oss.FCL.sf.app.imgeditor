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


#ifndef __CFILTERJPEGTARGET_H__
#define __CFILTERJPEGTARGET_H__

#include <e32base.h>
#include "MImageFilter.h"
#include "TBitmapHandle.h"

class MJpegSave;

class CFilterJpegTarget
	: public CBase
	, public MImageFilter
	{
	public:
		IMPORT_C static TInt Create();
		virtual ~CFilterJpegTarget();
	
	private:
		static CFilterJpegTarget* NewL();
		CFilterJpegTarget();
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

	private: // Data


		TRect iRect;
		TSize iSize;

		MJpegSave* iJpegSave;

		TBitmapHandle iBm;
		TInt iBlockWidth;
		TInt iBlockHeight;
		TInt iBlockY;
		TFileName iFileName;

		RFs iFs;
		RFile iFile;

	};

#endif
