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
* Container class that handles/forwards keyevents and pointer
* events to other controls.
*
*/


// INCLUDE FILES
#include <fbs.h>
#include <eikenv.h>
#include <eikspane.h>
#include <aknsbasicbackgroundcontrolcontext.h> 
#include <aknsdrawutils.h> 

#include "ImageEditorUIContainer.h"
#include "ImageEditorControlBase.h"
#include "ImageEditorUI.hrh"

#include "WaitIndicator.h"

#ifdef RD_TACTILE_FEEDBACK 
#include <touchfeedback.h>
#endif /* RD_TACTILE_FEEDBACK  */

const TInt KTouchPanTotalMoveThreshold = 5;

//=============================================================================
void CImageEditorUIContainer::ConstructL(const TRect & aRect)
	{
	//	Create window
	CreateWindowL();

	iWaitIndicator = CWaitIndicator::NewL();
	iWaitIndicator->SetParent( this);

#ifdef RD_TACTILE_FEEDBACK 
	iTouchFeedBack = MTouchFeedback::Instance();
#endif /* RD_TACTILE_FEEDBACK  */

	//	Set window rectangle
	SetRect(aRect);

	//	Set busy
	SetBusy();

	SetCanDrawOutsideRect();
	iBgContext = CAknsBasicBackgroundControlContext::NewL(
			KAknsIIDQsnBgAreaMain, Rect(), EFalse);

	EnableDragEvents();

	//	Activate container control
	ActivateL();
	}

//judge if it is draw plug-in
EXPORT_C void CImageEditorUIContainer::IsDrawPlugin(TBool aFlag)
	{
	iIsDrawPlugin = aFlag;
	}

//=============================================================================
CImageEditorUIContainer::~CImageEditorUIContainer()
	{
	delete iWaitIndicator;
	delete iBgContext;
	iEditorView = NULL;
	iPreview = NULL;
	iControl = NULL;
	}

//=============================================================================
void CImageEditorUIContainer::SetImageL(CFbsBitmap * aBitmap)
	{
	iPreview = aBitmap;
	if (iControl)
		{
		((CImageEditorControlBase*)iControl)->SetImageL(iPreview);
		}
	DrawDeferred();
	}

//=============================================================================
void CImageEditorUIContainer::SetControl(CCoeControl * aControl)
	{
	iControl = aControl;
	if (iControl)
		{
		((CImageEditorControlBase *)iControl)->SetView(iEditorView);
		if(!iIsDrawPlugin)
			{
			iControl->SetRect(Rect() );
			}		
		}
	DrawDeferred();
	}

//=============================================================================
void CImageEditorUIContainer::SetView(CImageEditorUIView * aView)
	{
	if (iControl)
		{
		((CImageEditorControlBase *)iControl)->SetView(aView);
		}
	iEditorView = aView;
	}

//=============================================================================
TKeyResponse CImageEditorUIContainer::OfferKeyEventL(const TKeyEvent & aKeyEvent, TEventCode aType)
	{
	if (Busy() )
		{
		return EKeyWasConsumed;
		}
	else
		{
		TKeyResponse res = EKeyWasNotConsumed;
		if (iControl)
			{
			res = ((CCoeControl*)iControl)->OfferKeyEventL(aKeyEvent,aType);
			}
		return res;
		}
	}

//=============================================================================
void CImageEditorUIContainer::SizeChanged()
	{
	if (iBgContext)
		{
		iBgContext->SetRect(Rect() );
		}

	if (iControl)
		{
		iControl->SetRect(Rect() );
		}

	iWaitIndicator->SetItemRect(Rect() );
	}

//=============================================================================
void CImageEditorUIContainer::HandlePointerEventL(const TPointerEvent &aPointerEvent)
	{
	if (Busy() )
		{
		return;
		}
	else
		{
		if (iControl)
			{
			((CCoeControl*)iControl)->HandlePointerEventL(aPointerEvent);
			}
		else
			{
			switch (aPointerEvent.iType)
				{
				case TPointerEvent::EButton1Down:
					{					
#ifdef RD_TACTILE_FEEDBACK
						if ( iTouchFeedBack )
							{
							iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
							RDebug::Printf( "ImageEditor::ImageEditorUIContainer: ETouchFeedback" );
							}
#endif /* RD_TACTILE_FEEDBACK  */
						if ( !iEditorView->InZoomingState() )
						{
						iEditorView->HandleCommandL(EImageEditorMenuCmdApplyEffect);
						}
					else
						{
						iTappedPosition = aPointerEvent.iPosition;
						}
					break;
					}
				case TPointerEvent::EDrag:
					{
					iXDirChange = iTappedPosition.iX - aPointerEvent.iPosition.iX;
					iYDirChange = iTappedPosition.iY - aPointerEvent.iPosition.iY;

					// Compare total change in pixels (absolute value) to 
					// threshold value. This is to prevent calling engine's 
					// Pan-function in every minimal movement                                                         
					if ( (Abs(iXDirChange) + Abs(iYDirChange) )	> KTouchPanTotalMoveThreshold )
						{
						iTappedPosition = aPointerEvent.iPosition;
						iEditorView->HandleCommandL(EImageEditorCmdTouchPan);
						}
					break;
					}
				case TPointerEvent::EButton1Up:
					{
					iTappedPosition = aPointerEvent.iPosition;
					break;
					}

				default:
					{
					break;
					}
				}
			}
		}
	}

//=============================================================================
TInt CImageEditorUIContainer::CountComponentControls() const
	{
	TInt count = 0;
	if (iControl)
		{
		++count;
		}
	return count;
	}

//=============================================================================
CCoeControl * CImageEditorUIContainer::ComponentControl(TInt aIndex) const
	{
	switch (aIndex)
		{
		case 0:
			{
			return iControl;
			}
		default:
			{
			return 0;
			}
		}
	}

//=============================================================================
void CImageEditorUIContainer::SetBusy()
	{
	if (iControl)
		{
		((CImageEditorControlBase *)iControl)->SetBusy();
		}
	iBusy = ETrue;
	}

//=============================================================================
void CImageEditorUIContainer::ResetBusy()
	{
	if (iControl)
		{
		((CImageEditorControlBase *)iControl)->ResetBusy();
		}
	iBusy = EFalse;
	}

//=============================================================================
TBool CImageEditorUIContainer::Busy() const
	{
	return iBusy;
	}

//=============================================================================
void CImageEditorUIContainer::SetFullScreen()
	{
	iFullScreen = ETrue;
	}

//=============================================================================
void CImageEditorUIContainer::ResetFullScreen()
	{
	iFullScreen = EFalse;
	}

//=============================================================================
void CImageEditorUIContainer::Draw(const TRect & /*aRect*/) const
	{
	// Get graphics context
	CWindowGc & gc = SystemGc();

	// If there is a plug-in control, it is responsible
	// for drawing the preview image (to avoid flicker)
	if (!iControl)
		{
		gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
		gc.SetBrushColor(KRgbWhite);
		gc.SetPenStyle(CGraphicsContext::ESolidPen);
		gc.SetPenColor(KRgbWhite);

		if (iPreview && iPreview->Handle() )
			{
			gc.DrawBitmap(Rect(), iPreview);
			}
		else
			{
			// Draw skin background
			MAknsSkinInstance* skin = AknsUtils::SkinInstance();
			MAknsControlContext* cc = AknsDrawUtils::ControlContext( this);
			AknsDrawUtils::DrawBackground(skin, cc, this, gc, TPoint(0, 0),
					Rect(), KAknsDrawParamDefault);
			}
		}
	iWaitIndicator->DrawItem(gc);
	}

//=============================================================================
void CImageEditorUIContainer::HandleControlEventL(CCoeControl * /*aControl*/,
		TCoeEvent /*aEventType*/)
	{

	}

//=============================================================================
void CImageEditorUIContainer::GetHelpContext(TCoeHelpContext& aContext) const
	{
	if (iControl)
		{
		iControl->GetHelpContext(aContext);
		}
	}

//=============================================================================
void CImageEditorUIContainer::GetDragDirections(TInt& xMovement, TInt& yMovement)
	{
	xMovement = iXDirChange;
	yMovement = iYDirChange;
	}

//=============================================================================
void CImageEditorUIContainer::HandleNaviDecoratorEventL(TInt aEventID)
	{
	if (aEventID == EAknNaviDecoratorEventRightTabArrow)
		{
		// A right arrow key event is simulated
		TKeyEvent key;
		key.iRepeats = 0;
		key.iCode = EKeyRightArrow;
		key.iModifiers = 0;
		CEikonEnv::Static()->SimulateKeyEventL(key, EEventKey);
		}
	else
		if (aEventID == EAknNaviDecoratorEventLeftTabArrow)
			{
			// A left arrow key event is simulated
			TKeyEvent key;
			key.iRepeats = 0;
			key.iCode = EKeyLeftArrow;
			key.iModifiers = 0;
			CEikonEnv::Static()->SimulateKeyEventL(key, EEventKey);
			}
	}

//=============================================================================
TTypeUid::Ptr CImageEditorUIContainer::MopSupplyObject(TTypeUid aId)
	{
	if (aId.iUid == MAknsControlContext::ETypeId && iBgContext)
		{
		return MAknsControlContext::SupplyMopObject(aId, iBgContext);
		}
	return CCoeControl::MopSupplyObject(aId);
	}

//=============================================================================
void CImageEditorUIContainer::HideWaitNote()
	{
	iWaitIndicator->Hide();
	}

//=============================================================================
void CImageEditorUIContainer::ShowWaitNote()
	{
	iWaitIndicator->Show();
	}

// End of File
