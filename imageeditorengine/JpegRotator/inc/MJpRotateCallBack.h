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


#ifndef __MJPROTATECALLBACK_H__
#define __MJPROTATECALLBACK_H__

class MJpRotateCallBack
	{
	public:
		virtual void JpRotateStatus( TInt aCount, TInt aTotal ) = 0;
	};

#endif
