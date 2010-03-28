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


#ifndef __CFILTERBUBBLE_H__
#define __CFILTERBUBBLE_H__

#include <e32base.h>
#include "MImageFilter.h"
#include <fbs.h>

class CFilterBubble
	: public CBase
	, public MImageFilter
	{
	public:
		IMPORT_C static TInt Create();
		virtual ~CFilterBubble();
	
	private:
		static CFilterBubble* NewL();
		CFilterBubble();
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

	private: // New methods
		void LoadImageL();

	private: // Data

		TUint32 *	iData;
		TSize		iSize;
		TFileName	iFileName;
		TInt		iBubble;
		TInt		iMask;

		TPoint		iPosition;
		TPoint		iCorrectPosition;
		TInt		iZoom;
		TInt		iAngle;
		TPoint		iScale;

	};

#endif
