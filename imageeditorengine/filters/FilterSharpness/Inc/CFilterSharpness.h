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


#ifndef __CFilterSharpness_H__
#define __CFilterSharpness_H__

#include <e32base.h>
#include "MImageFilter.h"

class CFilterSharpness
	: public CBase
	, public MImageFilter
	{
	public:
		IMPORT_C static TInt Create();
		virtual ~CFilterSharpness();
	
	private:
		static CFilterSharpness* NewL();
		CFilterSharpness();
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

		MImageFilter* iParent;
		MImageFilter* iChild;

		TInt		iSharpness;

		TInt		iN;
		TInt		iN2;
		TInt		iHN;
		TBool		iSharpen;

        //  Row caches
		TUint8 *	ipRCR;
		TUint8 *	ipRCG;
		TUint8 *	ipRCB;

        //  Column caches
		TUint8 *	ipCCR;
		TUint8 *	ipCCG;
		TUint8 *	ipCCB;
		
        //  Division LUT
        TUint8 *    iDivLUT;
	};

#endif
