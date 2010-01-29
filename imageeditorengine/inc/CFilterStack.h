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


#ifndef __CFILTERSTACK_H__
#define __CFILTERSTACK_H__

#include <e32base.h>
#include "MImageFilter.h"


class TFilter
	{
	public:
		inline ~TFilter()
			{
			delete iFilter;
			iLibrary.Close();
			}
		MImageFilter* iFilter;
		RLibrary iLibrary;
	};

class CFilterStack
	: public CBase
	{
	public:
		static CFilterStack* NewL();
		~CFilterStack();

	private:
		CFilterStack();
		void ConstructL();

	public:
		MImageFilter* Filter( TInt aPosition );

		void AddFilterL( const TDesC& aName );
		void AddFilterL( const TDesC& aName, TInt aPosition );
		void AddFilterL( const TFilter * aFilter );
		void AddFilterL( const TFilter * aFilter, const TInt aPosition );

		void RemoveFilter( TInt aPosition );
		void RemoveFilter ( TInt aPosition, TFilter *& aFilter );

		TInt FunctionL( TInt aPosition, const TDesC& aCmd );
		TInt NumFilters();

	private:
		void InitStack();

	private:
		RPointerArray< TFilter >iStack;
	};	


#endif
