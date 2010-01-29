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


#include <e32def.h>

#ifndef __IMAGEEDITORUI_PAN__
#define __IMAGEEDITORUI_PAN__


/// Image Editor panics
enum TImageEditorPanicIds
{
	/// Trying to access control which is null
	EImageEditorPanicControlIsNull,

	/// Menu not accessibe when trying to invoke TryDisplayMenuBarL
	EImageEditorPanicMenuNotAccessible,

	/// Cannot access the status pane of the view
	EImageEditorPanicStatusPaneNotAccessible,

	/// Cannot access the cba of the view
	EImageEditorPanicCbaNotAccessible,
    
    /// When zoom text count doesn't match with zoom state count
	EImageEditorPanicZoomTextCountDoesntMatch,
    
	/// When everything goes Beverly Hills...
	EImageEditorPanicUnexpectedCommand = 90210
};

#endif __IMAGEEDITORUI_PAN__

// End of File

