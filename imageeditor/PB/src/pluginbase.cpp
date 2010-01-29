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



#include "pluginbasedefs.h"
#include "pluginbase.h"


//=============================================================================
EXPORT_C CPluginBase * CPluginBase::NewL ()
{
	CPluginBase * self = new (ELeave) CPluginBase;
	CleanupStack::PushL (self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
}

//=============================================================================
EXPORT_C CPluginBase::~CPluginBase ()
{
    iPluginBaseInternalRepresentation = NULL;
}

//=============================================================================
EXPORT_C TInt CPluginBase::SetProperty (
	TInt		/*aPropertyId*/, 
	TDesC &		/*aPropertyValue*/
	)
{
	return TPropRetErrNotSupported;
}

//=============================================================================
EXPORT_C TInt CPluginBase::GetProperty (
	TInt		/*aPropertyId*/, 
	TDes &		/*aPropertyValue*/
	) 

{
	return TPropRetErrNotSupported;
}

//=============================================================================
EXPORT_C CPluginBase::CPluginBase ()
{

}

//=============================================================================
EXPORT_C void CPluginBase::ConstructL ()
{

}

//=============================================================================
EXPORT_C void CPluginBase::ReservedBaseMethod1 ()
{

}

//=============================================================================
EXPORT_C void CPluginBase::ReservedBaseMethod2 ()
{

}

// End of File
