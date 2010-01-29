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



#ifndef IMAGEEDITORPLUGINBASEDEFS_H
#define IMAGEEDITORPLUGINBASEDEFS_H

#include "pluginbasedefs.h"

/*	
*	IMAGE PROCESSING PLUG-IN CAPABILITY ID'S.
*/

/// UI type
const TInt KCapPluginUiType= KPluginCapabilityCustomOffset;

/// Filter type
const TInt KCapPluginFilterType= KPluginCapabilityCustomOffset + 1;

/// Filter scope
const TInt KCapPluginScope = KPluginCapabilityCustomOffset + 2;

/// Filter scope
const TInt KCapPluginDisplayOrder = KPluginCapabilityCustomOffset + 3;

/// Plug-in name
const TInt KCapPluginName = KPluginCapabilityCustomOffset + 4;

/// Icon file name
const TInt KCapIconName = KPluginCapabilityCustomOffset + 5;

/// Filter file name
const TInt KCapFilterName = KPluginCapabilityCustomOffset + 6;

/// Plug-in miscellaneous parameter name array
const TInt KCapPluginParamNames = KPluginCapabilityCustomOffset + 7;

/// Soft key 1 command ID
const TInt KCapPluginSk1Cmd = KPluginCapabilityCustomOffset + 8;

/// Soft key 1 text 
const TInt KCapPluginSk1Text = KPluginCapabilityCustomOffset + 9;

/// Soft key 2 command ID
const TInt KCapPluginSk2Cmd = KPluginCapabilityCustomOffset + 10;

/// Soft key 2 text
const TInt KCapPluginSk2Text = KPluginCapabilityCustomOffset + 11;

/// Middle softkey command ID
const TInt KCapPluginMSKCmd = KPluginCapabilityCustomOffset + 12;

/// Middle softkey text
const TInt KCapPluginMSKText = KPluginCapabilityCustomOffset + 13;

/// Menu items
const TInt KCapPluginMenuItems = KPluginCapabilityCustomOffset + 14;


/// Pointer to the parameter struct
const TInt KCapParamStruct = KPluginCapabilityCustomOffset + 100;

/// System parameter class
const TInt KCapSystemParameters = KPluginCapabilityCustomOffset + 101;

/// System parameter class
const TInt KCapGlobalZoomDisabled = KPluginCapabilityCustomOffset + 102;

/// Ready to render
const TInt KCapReadyToRender = KPluginCapabilityCustomOffset + 103;

/// Is landscape enabled plugin
const TInt KCapIsLandscapeEnabled = KPluginCapabilityCustomOffset + 104;

/// Is slow plugin
const TInt KCapIsSlowPlugin = KPluginCapabilityCustomOffset + 105;

/*	
*
*	API VERSION
*
*/
const TInt KImageEditorPluginAPIMajor = 0;
const TInt KImageEditorPluginAPIMinor = 8;
const TInt KImageEditorPluginAPIBuild = 0;

///*	
//*
//*	TYPE DEFINITIONS
//*
//*/
typedef TInt TPluginInt;

#endif
