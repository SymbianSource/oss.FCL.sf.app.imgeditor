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


#ifndef CFILTERROTATE_H
#define CFILTERROTATE_H

#include <e32base.h>
#include "MImageFilter.h"

class CFilterRotate
	: public CBase
	, public MImageFilter
	{
	public:
		IMPORT_C static TInt Create();
		virtual ~CFilterRotate();
	
	private:
		static CFilterRotate* NewL();
		CFilterRotate();
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

		enum TRotation
		{
			ERotationMin = 0,	// invalid
			ERotation0,
			ERotation90,
			ERotation180,
			ERotation270,
			ERotationMax		// invalid
		} iRotation;

		TSize iVpSize;
	};

#endif // CFILTERROTATE_H
