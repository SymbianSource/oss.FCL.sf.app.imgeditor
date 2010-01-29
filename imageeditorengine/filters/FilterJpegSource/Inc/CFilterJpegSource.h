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


#ifndef __CFILTERJPEGSOURCE_H__
#define __CFILTERJPEGSOURCE_H__

#include <e32base.h>
#include "MImageFilter.h"
#include "TBitmapHandle.h"


class MJpegLoad;

class TBlockEntry
    {
	public:
		inline ~TBlockEntry()
			{
			delete iBitmap.iData;
			}
	public:
		TRect iRect;
		TBitmapHandle iBitmap;
	};



class CFilterJpegSource
	: public CBase
	, public MImageFilter
	{
	public:
		IMPORT_C static TInt Create();
		virtual ~CFilterJpegSource();
	
	private:
		static CFilterJpegSource* NewL();
		CFilterJpegSource();
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
        
        void LoadRectL();

	private: // Data

        enum TOutputState
        {
            EDirect,
            EBuffer,
        } iOutputState;

        /// decoder instance
		MJpegLoad *     iJpeg;
        //  file name
        TFileName		iFileName;
        //  macroblock size
        TSize           iBlockSize;
        //  image size in macroblocks
        TSize           iSizeInBlocks;
        //  image rectangle
        TRect           iRect;
        //  scaled visible image rectangle
        TRect           iScaledRect;
        //  image size
        TSize           iFullSize;

        //  destination size
        TSize           iDestSize;
        //  destination buffer
        TUint32 *       iDestBuffer;
        //  relative scale
        TReal           iRelScale;

		TBlockEntry **  iBlockBuffer;
		TInt            iXAnd;
		TInt            iYAnd;
		TUint32         iLastBlock;
        TInt *          iIndexMap;
        TInt            iOldestBlock;

	};

#endif
