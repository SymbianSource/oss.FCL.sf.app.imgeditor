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


#ifndef __CJPTIMER_H__
#define __CJPTIMER_H__

#include <e32base.h>

class MTimerCallBack;

class CJPTimer : public CTimer
{

	public:
		
		static CJPTimer* NewL( MTimerCallBack* aCallBack );
		~CJPTimer();
	
	private:
		
		void ConstructL();
		CJPTimer( MTimerCallBack* aCallBack );

	public:

		void Call( TInt aWait );

	private: // CActive

		void RunL();

	private:

		MTimerCallBack * iCallBack;
		bool 			iCalling;
	};
#endif
