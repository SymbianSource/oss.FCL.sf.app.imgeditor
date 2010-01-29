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
#include "ImageEditorDocument.h"
#include "ImageEditorAppUi.h"


//=============================================================================
CImageEditorDocument::CImageEditorDocument (CEikApplication & aApp) :
CAknDocument (aApp)    
{

}

//=============================================================================
CImageEditorDocument::~CImageEditorDocument()
{
    LOG(KImageEditorLogFile, "CImageEditorDocument::~CImageEditorDocument()");
}

//=============================================================================
void CImageEditorDocument::ConstructL()
{
	LOG(KImageEditorLogFile, "CImageEditorDocument: Document created");
}

//=============================================================================
CImageEditorDocument * CImageEditorDocument::NewL (CEikApplication & aApp)
{
    CImageEditorDocument * self = new (ELeave) CImageEditorDocument (aApp);
    CleanupStack::PushL (self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}
    
//=============================================================================
CEikAppUi * CImageEditorDocument::CreateAppUiL()
{
	return new (ELeave) CImageEditorAppUi;
}


//=============================================================================
void CImageEditorDocument::OpenFileL(CFileStore*& /*aFileStore*/, RFile& aFile)
{
    LOG(KImageEditorLogFile, "CImageEditorDocument::OpenFileL()");

	CleanupClosePushL(aFile);

    // File handle must be closed. Open File Service won't do it. 
    // Using cleanup stack, because iAppUi->OpenFileL() may leave.
	((CImageEditorAppUi*)iAppUi)->OpenFileL(aFile);

	CleanupStack::PopAndDestroy();    
}
