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


#include "EditorImage.h"

//=============================================================================
CEditorImage * CEditorImage::NewL ()
{
	CEditorImage * self = CEditorImage::NewLC ();
	CleanupStack::Pop(); // self;
	return self;
}

//=============================================================================
CEditorImage * CEditorImage::NewLC ()
{
	CEditorImage * self = new (ELeave) CEditorImage;
	CleanupStack::PushL (self);
	self->ConstructL ();
	return self;
}

//=============================================================================
CEditorImage::~CEditorImage ()
{
    iPreview = 0;
    iFull = 0;
}

//=============================================================================
CFbsBitmap *& CEditorImage::GetPreviewData()
{
	return iPreview;
}

//=============================================================================
CFbsBitmap *& CEditorImage::GetFullData()
{
	return iFull;
}

//=============================================================================
TFileName & CEditorImage::GetImageName()
{
	return iFileName;
}

//=============================================================================
void CEditorImage::LockHeapLC() const
{
    if ( iPreview )
    {
        iPreview->LockHeapLC();
    }
}

//=============================================================================
void CEditorImage::ReleaseHeap() const
{
    if (iPreview)
        {
        iPreview->UnlockHeap();
        }
}

//=============================================================================
CEditorImage::CEditorImage () 
{

}

//=============================================================================
void CEditorImage::ConstructL ()
{
	iFileName.Zero();	
}

