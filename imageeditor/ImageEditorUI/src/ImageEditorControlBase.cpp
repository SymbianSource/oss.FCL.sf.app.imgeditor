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
* A base class for Image Editor controls.
*
*/



#include "ImageEditorControlBase.h"
#include "ImageEditorUiPanics.h"

//=============================================================================
EXPORT_C CImageEditorControlBase::CImageEditorControlBase () : CCoeControl()
{
    // EMPTY
}

//=============================================================================
EXPORT_C CImageEditorControlBase::~CImageEditorControlBase ()
{
    // EMPTY  
}

//=============================================================================
EXPORT_C void CImageEditorControlBase::ConstructL (
	const TRect &		/*aRect*/,
	CCoeControl	*		/*aParent*/
	)
{
    // EMPTY
}

//=============================================================================
EXPORT_C void CImageEditorControlBase::SetImageL (CFbsBitmap * /*aBitmap*/)
{
    // EMPTY
}

//=============================================================================
EXPORT_C void CImageEditorControlBase::SetImageL (const CFbsBitmap * /*aBitmap*/)
{
    // EMPTY
}

//=============================================================================
EXPORT_C void CImageEditorControlBase::SetView (CAknView * /*aView*/)
{
    // EMPTY
}

//=============================================================================
EXPORT_C void CImageEditorControlBase::SetSelectedUiItemL (CPluginInfo * /*aItem*/)
{
    // EMPTY
}

//=============================================================================
EXPORT_C void CImageEditorControlBase::PrepareL ()
{
    // EMPTY
}

//=============================================================================
EXPORT_C TKeyResponse CImageEditorControlBase::OfferKeyEventL (
        const TKeyEvent &   /*aKeyEvent*/,
        TEventCode          /*aType*/
        )
{
    return EKeyWasNotConsumed;
}

//=============================================================================
EXPORT_C TInt CImageEditorControlBase::CountComponentControls() const
{
    return 0;
}

//=============================================================================
EXPORT_C CCoeControl * CImageEditorControlBase::ComponentControl (TInt /*aIndex*/) const
{
    return 0;
}

//=============================================================================
EXPORT_C void CImageEditorControlBase::SetBusy()
{
    iBusy = ETrue;
}

//=============================================================================
EXPORT_C void CImageEditorControlBase::ResetBusy()
{
    iBusy = EFalse;
}

//=============================================================================
EXPORT_C TBool CImageEditorControlBase::Busy() const
{
    return iBusy;
}

//=============================================================================
EXPORT_C void CImageEditorControlBase::HandlePluginCommandL (const TInt /*aCommand*/)
{
    // EMPTY
}

//=============================================================================
EXPORT_C TInt CImageEditorControlBase::GetSoftkeyIndexL ()
{
    return -1;
}

//=============================================================================
EXPORT_C TInt CImageEditorControlBase::GetContextMenuResourceId()
{
    return 0;
}

//=============================================================================
EXPORT_C TPtrC CImageEditorControlBase::GetNaviPaneTextL (
    TBool& aLeftNaviPaneScrollButtonVisibile, 
    TBool& aRightNaviPaneScrollButtonVisible )
{
    aLeftNaviPaneScrollButtonVisibile = EFalse;
    aRightNaviPaneScrollButtonVisible = EFalse;
    return TPtrC();
}

//=============================================================================
EXPORT_C TBitField CImageEditorControlBase::GetDimmedMenuItems ()
{
	// return all-zero bitfield
    return TBitField();
}

//=============================================================================
EXPORT_C void CImageEditorControlBase::SizeChanged()
{
    // EMPTY
}

//=============================================================================
EXPORT_C void CImageEditorControlBase::HandlePointerEventL( const TPointerEvent& /*aPointerEvent*/ )
{
    // EMPTY
}

//=============================================================================
EXPORT_C void CImageEditorControlBase::Draw (const TRect & /*aRect*/) const
{
    // EMPTY
}

// End of File
