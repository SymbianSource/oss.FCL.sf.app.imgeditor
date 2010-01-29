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



// INCLUDE FILES
#include    "ImageEditorApp.h"
#include    "ImageEditorDocument.h"
#include	"ImageEditorUids.hrh"

#ifdef ENABLE_DEBUGLOG
#include	"editorversion.h"
#endif

#include <eikstart.h>


//=============================================================================
LOCAL_C CApaApplication* NewApplication()
{
    return new CImageEditorApp;
}

//=============================================================================
GLDEF_C TInt E32Main()
{
    return EikStart::RunApplication(NewApplication);
}

//=============================================================================
TUid CImageEditorApp::AppDllUid() const
{
    return TUid::Uid(UID_IMAGE_EDITOR);
}

//=============================================================================
CDictionaryStore * CImageEditorApp::OpenIniFileLC (RFs & aFs) const
{
    return CEikApplication::OpenIniFileLC (aFs);
}
   
//=============================================================================
CApaDocument * CImageEditorApp::CreateDocumentL()
{
    // create log file if the logging is enabled
	LOG_INIT(KImageEditorLogFile);
	LOGFMT3(KImageEditorLogFile, "Image Editor version: MAJOR=%d, MINOR=%d, BUILD=%d", my_version_major, my_version_minor, my_version_build);

    return CImageEditorDocument::NewL (*this);
}

// End of File
