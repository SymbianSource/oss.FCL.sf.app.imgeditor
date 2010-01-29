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


#include "JPTimer.h"
#include "MTimerCallBack.h"



CJPTimer* CJPTimer::NewL( MTimerCallBack* aCallBack )
	{
	CJPTimer* self = new( ELeave )CJPTimer( aCallBack );
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CJPTimer::CJPTimer( MTimerCallBack* aCallBack )
	: CTimer( EPriorityStandard )
	, iCallBack( aCallBack )
	{
	}



void CJPTimer::ConstructL()
	{
	CTimer::ConstructL();
	CActiveScheduler::Add( this );
	}



CJPTimer::~CJPTimer()
	{
	Cancel();
	}



void CJPTimer::RunL()
	{
	iCalling = false;
	iCallBack->TimerCallBack();
	}



void CJPTimer::Call( TInt aWait )
	{
	if( iCalling ) return;
	iCalling = true;
	After( aWait );
	}
