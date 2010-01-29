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


#ifndef __MIMAGEFILTER_H__
#define __MIMAGEFILTER_H__

#include <e32base.h>

class TBlock
{
public:

    TBlock() {};

    TBlock (const TRect & aRect) 
    {
        iRect = aRect;
        iWidth = aRect.iBr.iX - aRect.iTl.iX;
        iHeight = aRect.iBr.iY - aRect.iTl.iY;
        iDataLength = iWidth * iHeight;
        iData = new (ELeave) TUint32 [iDataLength];
        Mem::FillZ(iData, iDataLength * sizeof(TUint32));
    };
    ~TBlock() {delete[] iData;};
public:
    TRect       iRect;
    TInt        iWidth;
    TInt        iHeight;
    TInt        iDataLength;
    TUint32 *   iData;

protected:

};

class MImageFilter
	{
	public:
		virtual ~MImageFilter() {}
        virtual TRect Rect() = 0;
        virtual TReal Scale() = 0;
        virtual TSize ViewPortSize() = 0;
		virtual TBlock * GetBlockL ( const TRect & aRect ) = 0;
        virtual void SetParent( MImageFilter* aParent ) = 0;
		virtual void SetChild( MImageFilter* aChild ) = 0;
		virtual TInt CmdL( const TDesC16& aCmd ) = 0;
		virtual const char* Type() = 0;
		MImageFilter* iParent;
		MImageFilter* iChild;
	};

#endif
