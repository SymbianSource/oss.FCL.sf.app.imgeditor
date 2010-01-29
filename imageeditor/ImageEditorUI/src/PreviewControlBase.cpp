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



#include "PreviewControlBase.h"

//=============================================================================
EXPORT_C CPreviewControlBase * CPreviewControlBase::NewL (
	const TRect &		aRect,
	CCoeControl	*		aParent
	)
{
    CPreviewControlBase * self = new (ELeave) CPreviewControlBase;
    CleanupStack::PushL (self);
    self->ConstructL (aRect, aParent);
    CleanupStack::Pop(); // self
    return self;
}

//=============================================================================
EXPORT_C void CPreviewControlBase::ConstructL (
	const TRect &		aRect,
	CCoeControl	*		aParent
	)
{
    //  Set container
	SetContainerWindowL (*aParent);
    
	//	Set extent
    SetRect (aRect);

    //  Activate
    ActivateL();
}

//=============================================================================
EXPORT_C CPreviewControlBase::~CPreviewControlBase()
{
    iPreview = NULL;
}

//=============================================================================
EXPORT_C void CPreviewControlBase::SetImageL (CFbsBitmap * aBitmap)
{
    iPreview = aBitmap;
}

//=============================================================================
EXPORT_C void CPreviewControlBase::SetImageL (const CFbsBitmap * aBitmap)
{
    iPreview = aBitmap;
}

//=============================================================================
EXPORT_C void CPreviewControlBase::DrawPreviewImage (const TRect & aRect) const
{
   //  Get graphics context
    CWindowGc & gc = SystemGc();

    // Set graphics context parameters
	gc.SetPenStyle (CGraphicsContext::ESolidPen);
	gc.SetPenColor (KRgbBlack);
    gc.SetBrushStyle (CGraphicsContext::ESolidBrush);
	gc.SetBrushColor (KRgbBlack);

	//	Draw preview image if found
	if ( iPreview && iPreview->Handle() )
	{
		gc.BitBlt (TPoint(0,0), iPreview);
	}
    else
    {
        gc.DrawRect (aRect);
    }
}

//=============================================================================
EXPORT_C void CPreviewControlBase::Draw (const TRect & aRect) const
{
    DrawPreviewImage(aRect);
}

// End of File
