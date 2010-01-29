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



#ifndef PLUGINBASEDEFS_H
#define PLUGINBASEDEFS_H

#include <e32base.h>

/*	
*	TYPEDEFS
*/

/*	TPluginType
*
*   Plugin type for plug-in framework.
*
*/
enum TPluginType
{
	//	Invalid
	KPluginTypeInvalid,
	//	Image Editor: image editing plug-in
	KPluginTypeImageEditor,
	//	Maximum value, invalid
	KPluginTypeMax
};

/*	
*	PLUG-IN CAPABILITY ID'S.
*/

/// Base offset for general capabilities
const TInt KPluginCapabilityBase = 0;

/// Base offset for plugin-specific capabilities
const TInt KPluginCapabilityCustomOffset = 3000;

/// Plugin type
const TInt KPluginCapabilityPluginType = KPluginCapabilityBase;

/// Plugin API major version
const TInt KPluginCapabilityAPIMajorVersion = KPluginCapabilityBase + 1;

/// Plugin API minor version
const TInt KPluginCapabilityAPIMinorVersion = KPluginCapabilityBase + 2;

/// Plugin API build version
const TInt KPluginCapabilityAPIBuildVersion = KPluginCapabilityBase + 3;

/// Plugin UID 
const TInt KPluginCapabilityUID = KPluginCapabilityBase + 4;

/*	
*	QUERY RETURN VALUES
*/

/// No errors
const TInt TPropRetErrNone = 0;

/// Queried property not supprted 
const TInt TPropRetErrNotSupported = -1;


#endif
