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


#ifndef __CFILTERICLSOURCE_H__
#define __CFILTERICLSOURCE_H__

#include <e32base.h>
#include "MImageFilter.h"

class CFbsBitmap;
class CImageDecoder;

//  Loader observer
//=============================================================================
class TBlockEntry
{
public:
	inline ~TBlockEntry()
	{
	    delete[] iData;
	}
public:
	TRect       iRect;
	TUint32 *   iData;
};


//  Loader observer
//=============================================================================
class MLoaderObserver
{
public:
    virtual void OperationReady (const TInt aError) = 0;
};


//  CActive derived loader class
//=============================================================================
class CImageLoader : public CActive
{

public:
    CImageLoader (MLoaderObserver * aObserver);
    virtual ~CImageLoader();
    void LoadBitmapL (TDesC & aFileName, CFbsBitmap * iBitmap);

protected:
    virtual void RunL();
    virtual void DoCancel();

private:
    CFbsBitmap *        iBitmap;
    CImageDecoder *     iDecoder;
    MLoaderObserver *   iObserver;
};


//  ICL source filter
//=============================================================================
class CFilterIclSource
	: public CBase
	, public MImageFilter
    , public MLoaderObserver
	{

public:

        IMPORT_C static TInt Create();
		virtual ~CFilterIclSource();

public: // MImageFilter
		virtual TRect Rect();
        virtual TReal Scale();
        virtual TSize ViewPortSize();
	    virtual TBlock * GetBlockL ( const TRect & aRect );
		virtual void SetParent( MImageFilter* aParent );
		virtual void SetChild( MImageFilter* aChild );
		virtual TInt CmdL( const TDesC16& aCmd );
		virtual const char* Type();

public: // MLoaderObserver
        virtual void OperationReady (const TInt aError);

protected:        


private:
		static CFilterIclSource* NewL();
		CFilterIclSource();
		void ConstructL();
        void LoadRectL();

private: // Data

        enum TOutputState
        {
            EDirect,
            EBuffer,
        } iOutputState;

        TRect           iRect;
        TRect           iScaledRect;
        CFbsBitmap *    iBitmap;
        TInt            iWStep;
        CImageLoader *  iLoader;

        //  destination size
        TSize           iDestSize;
        //  destination buffer
        TUint32 *       iDestBuffer;
        //  relative scale
        TReal           iRelScale;

        TBlockEntry **  iBlockBuffer;
        TSize			iSize;
		TSize           iSizeInBlocks;
		TUint32         iLastBlock;
        TInt *          iIndexMap;
        TInt            iOldestBlock;
        TInt            iBlockBufferSize;
        TFileName		iFileName;
};


#endif
