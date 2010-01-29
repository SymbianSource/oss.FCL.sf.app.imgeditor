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


#ifndef __CFILTERTEXT_H__
#define __CFILTERTEXT_H__

#include <e32base.h>
#include "MImageFilter.h"

class CFont;

class CFilterText
	: public CBase
	, public MImageFilter
	{
	public:
		IMPORT_C static TInt Create();
		virtual ~CFilterText();
	
	private:
		static CFilterText* NewL();
		CFilterText();
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

	private: // new methods

        void CreateBitmapL ( const TSize & aSize );
		void CreateTextL ( const TDesC16 & aText );


	private: // Data

		TUint32*	iData;
		TRect		iRect;
		TSize		iTextSize;
		TSize		iSize;
		TUint32		iColor;
		const CFont *	iFont;		
	
		TPoint		iPosition;
		TPoint		iCorrectPosition;
		TInt		iZoom;
		TInt		iAngle;
		TPoint		iScale;




	};

#endif
