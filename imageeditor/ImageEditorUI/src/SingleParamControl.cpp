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


//  INCLUDES
#include <fbs.h>
#include <aknview.h>
#include <aknutils.h>
#include <AknIconUtils.h>
#include <ImageEditorUi.mbg>
#include <AknLayoutScalable_Avkon.cdl.h>
#include <AknLayoutScalable_Apps.cdl.h>
#include <eiklabel.h>

#ifdef RD_TACTILE_FEEDBACK 
#include <touchfeedback.h>
#endif /* RD_TACTILE_FEEDBACK  */

#include "ImageEditorUI.hrh"
#include "ImageEditorUiDefs.h"
#include "SingleParamControl.h"
#include "PreviewControlBase.h"
#include "SingleParamControlObserver.h"
#include "PluginInfo.h"
#include "CustomControlPanics.h"

// constants
const TReal KTouchSensitivity = 0.5;
const TInt KBorderPartsNum = 9;
const TInt KSliderPartsNum = 4;
const TInt KTouchSliderPartsNum = 7;
const TInt KSliderWidth = 20;
const TInt KScrollRepeatTimeout = 250000; // 0.25 seconds

//=============================================================================
EXPORT_C CSingleParamControl * CSingleParamControl::NewL (
		const TRect & aRect,
		CCoeControl * aParent,
		TBool aActionOnButtonRelease
)
	{
	CSingleParamControl * self = new (ELeave) CSingleParamControl;
	CleanupStack::PushL (self);
	self->ConstructL (aRect, aParent, aActionOnButtonRelease);
	CleanupStack::Pop (); // self
	return self;
	}

//=============================================================================
EXPORT_C CSingleParamControl::CSingleParamControl()
: iDragging( EFalse ),
iActionOnButtonRelease( EFalse ),
iTouchRect(0,0,0,0),
iMinimumValue(0),
iMaximumValue(1),
iStep(0),
iStepInPixels(0),
iNumberOfSteps(0),
iPosition(0),
iMarkerPressed( EFalse )
	{
	}

//=============================================================================
EXPORT_C void CSingleParamControl::ConstructL (
		const TRect & /*aRect*/,
		CCoeControl * aParent,
		TBool aActionOnButtonRelease
)
	{
	//	Set parent
	SetContainerWindowL (*aParent);

	TFileName iconFile (KImageEditorUiMifFile);

	// create popup and slider graphics
	for ( TInt i = 0; i < KBorderPartsNum; ++i )
		{
		CEikImage* image = new (ELeave) CEikImage;

		image->CreatePictureFromFileL(iconFile,
				EMbmImageeditoruiQgn_graf_popup_trans_center + 2*i,
				EMbmImageeditoruiQgn_graf_popup_trans_center_mask + 2*i);

		CleanupStack::PushL( image );
		image->SetContainerWindowL( *this );
		iBorders.AppendL( image );
		CleanupStack::Pop( image );
		}
	if( AknLayoutUtils::PenEnabled() )
		{
		for ( TInt i = 0; i < KTouchSliderPartsNum; ++i )
			{
			CEikImage* image = new (ELeave) CEikImage;

			image->CreatePictureFromFileL(iconFile,
					EMbmImageeditoruiQgn_graf_nslider_end_left + 2*i,
					EMbmImageeditoruiQgn_graf_nslider_end_left_mask + 2*i);

			CleanupStack::PushL( image );
			image->SetContainerWindowL( *this );
			iScrollBar.AppendL( image );
			CleanupStack::Pop( image );
			}
		}
	else
		{
		for ( TInt i = 0; i < KSliderPartsNum; ++i )
			{
			CEikImage* image = new (ELeave) CEikImage;

			image->CreatePictureFromFileL(iconFile,
					EMbmImageeditoruiQgn_graf_nslider_imed_end_left + 2*i,
					EMbmImageeditoruiQgn_graf_nslider_imed_end_left_mask + 2*i);

			CleanupStack::PushL( image );
			image->SetContainerWindowL( *this );
			iScrollBar.AppendL( image );
			CleanupStack::Pop( image );
			}
		}

	iText = new (ELeave) CEikLabel;
	iText->SetContainerWindowL( *this );
	iActionOnButtonRelease = aActionOnButtonRelease;

#ifdef RD_TACTILE_FEEDBACK 
    iTouchFeedBack = MTouchFeedback::Instance();
#endif /* RD_TACTILE_FEEDBACK  */
	
	EnableDragEvents();

	//	Activate control
	ActivateL();
	}

//=============================================================================
EXPORT_C CSingleParamControl::~CSingleParamControl()
	{
	iBorders.ResetAndDestroy();
	iScrollBar.ResetAndDestroy();
	if (iIcon)
		{
		delete iIcon;
		}
	delete iText;

	iParObserver = NULL;
	iItem = NULL;
	iEditorView = NULL;
	}

//=============================================================================
EXPORT_C void CSingleParamControl::SetView (CAknView * aView)
	{
	iEditorView = aView;
	}

EXPORT_C void CSingleParamControl::SetIcon(CEikImage* aIcon)
	{
	iIcon = aIcon;
	}

EXPORT_C void CSingleParamControl::SetCaption(const TDesC& aText)
	{
	iText->SetTextL( aText );
	}

//=============================================================================
EXPORT_C void CSingleParamControl::SetSelectedUiItemL (CPluginInfo * aItem)
	{
	iItem = aItem;
	}

//=============================================================================
EXPORT_C TKeyResponse CSingleParamControl::OfferKeyEventL (
		const TKeyEvent & aKeyEvent,
		TEventCode aType
)
	{
	if ( Busy() )
		{
		return EKeyWasConsumed;
		}
	else
		{
		if (EEventKey == aType)
			{
			switch (aKeyEvent.iCode)
				{

				case EKeyRightArrow:
					{
					if ( iPosition == iMaximumValue )
						{
						return EKeyWasConsumed;
						}
					MoveSlider(1);
					if( iActionOnButtonRelease )
						{
						iEditorView->HandleCommandL (EImageEditorCmdRender);
						DrawDeferred();
						}				
					return EKeyWasConsumed;
					}
				case EKeyLeftArrow:
					{
					if ( iPosition == iMinimumValue )
						{
						return EKeyWasConsumed;
						}
					MoveSlider(-1);
					if( iActionOnButtonRelease )
						{
						iEditorView->HandleCommandL (EImageEditorCmdRender);
						DrawDeferred();
						}	
					return EKeyWasConsumed;
					}

				case EKeyOK:
				case EKeyEnter:
					{
					// Cancel plug-in if no changes made
					iEditorView->HandleCommandL (EImageEditorApplyPlugin);
					return EKeyWasConsumed;
					}

				case EKeyDownArrow:
				case EKeyUpArrow:
					{
					return EKeyWasConsumed;
					}

				default:
					{
					break;
					}
				}
			}
		return EKeyWasNotConsumed;
		}
	}

//=============================================================================
EXPORT_C void CSingleParamControl::HandlePointerEventL(const TPointerEvent &aPointerEvent)
	{
	if( AknLayoutUtils::PenEnabled() )
		{
		if( iTouchRect.Contains( aPointerEvent.iPosition ) )
			{
			TRect markerRect = iScrollBar[KSliderPartsNum - 1]->Rect();
			markerRect.iTl.iX -= KSliderWidth / 2;
			markerRect.iBr.iX += KSliderWidth / 2;
			// marker pressed?
			if( markerRect.Contains( aPointerEvent.iPosition ) )
				{
				if( aPointerEvent.iType == TPointerEvent::EButton1Down )
					{
					iDragging = ETrue;
					iMarkerPressed = ETrue;
					
#ifdef RD_TACTILE_FEEDBACK
					if ( iTouchFeedBack )
						{
						iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
						RDebug::Printf( "ImageEditor::SingleParamControl: ETouchFeedback" );
						}
#endif /* RD_TACTILE_FEEDBACK  */
					
					DrawDeferred();
					return;
					}
				}

			// marker dragged?
			if (iDragging)
				{
				if( aPointerEvent.iType == TPointerEvent::EButton1Up
						|| aPointerEvent.iType == TPointerEvent::EDrag )
					{
					TInt markerCenter = iScrollBar[KSliderPartsNum - 1]->Rect().iTl.iX
					+ (iScrollBar[KSliderPartsNum - 1]->Rect().iBr.iX
							- iScrollBar[KSliderPartsNum - 1]->Rect().iTl.iX) / 2;

					if(aPointerEvent.iPosition.iX> iScrollBar[KSliderPartsNum - 1]->Rect().iBr.iX)
						{
						MoveSlider((((TReal) (aPointerEvent.iPosition.iX
														- markerCenter))
										/ iStepInPixels) + KTouchSensitivity);
						}
					else if(aPointerEvent.iPosition.iX < iScrollBar[KSliderPartsNum - 1]->Rect().iTl.iX)
						{
						MoveSlider((((TReal) (aPointerEvent.iPosition.iX
														- markerCenter))
										/ iStepInPixels) - KTouchSensitivity);
						}
					}
				if( aPointerEvent.iType == TPointerEvent::EButton1Up )
					{
					if( iActionOnButtonRelease )
						{
						iEditorView->HandleCommandL (EImageEditorCmdRender);
						}
					iDragging = EFalse;
					iMarkerPressed = EFalse;
					DrawDeferred();
					}
				if( iActionOnButtonRelease )
					{
					DrawDeferred();
					}
				return;
				}

			// normal moving
			if( aPointerEvent.iType == TPointerEvent::EButton1Down
					|| aPointerEvent.iType == TPointerEvent::EButtonRepeat )
				{
				iMarkerPressed = ETrue;
				if(aPointerEvent.iPosition.iX> iScrollBar[KSliderPartsNum - 1]->Rect().iBr.iX)
					{
					MoveSlider(1);
					if( iActionOnButtonRelease )
						{
						iEditorView->HandleCommandL (EImageEditorCmdRender);
						DrawDeferred();
						}	
					}
				else if(aPointerEvent.iPosition.iX < iScrollBar[KSliderPartsNum - 1]->Rect().iTl.iX)
					{
					MoveSlider(-1);
					if( iActionOnButtonRelease )
						{
						iEditorView->HandleCommandL (EImageEditorCmdRender);
						DrawDeferred();
						}
					}
				Window().RequestPointerRepeatEvent( KScrollRepeatTimeout, iTouchRect );

#ifdef RD_TACTILE_FEEDBACK
					if ( iTouchFeedBack &&
							((aPointerEvent.iPosition.iX < markerRect.iTl.iX) ||
							(aPointerEvent.iPosition.iX > markerRect.iBr.iX)) )
						{
						iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
						RDebug::Printf( "ImageEditor::SingleParamControl: ETouchFeedback" );
						}
#endif /* RD_TACTILE_FEEDBACK  */
					
				return;
				}
			}
		if (iDragging && iMarkerPressed)
			{
			iEditorView->HandleCommandL (EImageEditorCmdRender);
			}
		
		iDragging = EFalse;
		iMarkerPressed = EFalse;
		DrawDeferred();
		return;
		}
	}

//=============================================================================
EXPORT_C void CSingleParamControl::HandlePluginCommandL (const TInt aCommand)
	{
	//  If soft key 1 pressed
	if (aCommand == EAknSoftkeyOk)
		{
		// Cancel plug-in if no changes made
		iEditorView->HandleCommandL (EImageEditorApplyPlugin);
		}

	//  If soft key 2 pressed

	else if (aCommand == EAknSoftkeyCancel)
		{
		iEditorView->HandleCommandL (EImageEditorCancelPlugin);
		}

	//  Default functionality

	else
		{
		CImageEditorControlBase::HandlePluginCommandL (aCommand);
		}
	}

//=============================================================================
EXPORT_C TInt CSingleParamControl::GetSoftkeyIndexL()
	{
	return 0;
	}

//=============================================================================
EXPORT_C TPtrC CSingleParamControl::GetNaviPaneTextL (
		TBool& aLeftNaviPaneScrollButtonVisibile,
		TBool& aRightNaviPaneScrollButtonVisible )
	{
	aLeftNaviPaneScrollButtonVisibile = EFalse;
	aRightNaviPaneScrollButtonVisible = EFalse;

	// Don't show any text in navi pane
	// The whole if-branch should probably be removed for good
	TBool showNaviPaneText ( EFalse );

	if ( iItem && showNaviPaneText )
		{
		return iItem->PluginName()->Des();
		}
	else
		{
		return TPtrC();
		}
	}

//=============================================================================
EXPORT_C void CSingleParamControl::SetParObserver (MSingleParControlObserver * aObserver)
	{
	iParObserver = aObserver;
	}

//=============================================================================
EXPORT_C void CSingleParamControl::Draw (const TRect & aRect) const
	{
	TInt variety = 0;
	if( AknLayoutUtils::PenEnabled() )
		{
		variety = 1;
		}

	TAknLayoutRect layoutRect;
	layoutRect.LayoutRect( Rect(), AknLayoutScalable_Apps::popup_imed_adjust2_window(variety) );
	TRect parentRect = layoutRect.Rect();

	// centralize the slider
	TPoint topLeft = parentRect.iTl;
	topLeft.iX = (Rect().Width() - parentRect.Width()) / 2;
	parentRect.SetRect(topLeft, parentRect.Size());

	TAknWindowComponentLayout l = AknLayoutScalable_Apps::slider_imed_adjust_pane(variety);
	TAknWindowLineLayout lineLayout = l.LayoutLine();
	layoutRect.LayoutRect(parentRect, lineLayout);

	TRect markerRect = layoutRect.Rect();
	TUint sliderTabWidth = KSliderWidth;
	TUint sliderBitmapWidth = layoutRect.Rect().Width() - sliderTabWidth;
	if ( iMinimumValue < iMaximumValue )
		{
		if(AknLayoutUtils::LayoutMirrored())
			{
        	markerRect.iTl.iX += (iPosition - iMinimumValue)
			* sliderBitmapWidth
			/ (iMaximumValue - iMinimumValue);
			}
		else
			{
			markerRect.iTl.iX += (iPosition - iMinimumValue)
			* sliderBitmapWidth
			/ (iMaximumValue - iMinimumValue);
			}
		iScrollBar[KSliderPartsNum - 1]->SetPosition(markerRect.iTl);
		if( AknLayoutUtils::PenEnabled() )
			{
			iScrollBar[KTouchSliderPartsNum - 1]->SetPosition(markerRect.iTl);
			}
		}

	if (iPreview)
		{
		CPreviewControlBase::DrawPreviewImage (aRect);
		}
	}

//=============================================================================
EXPORT_C TInt CSingleParamControl::CountComponentControls() const
	{
	TInt count = iBorders.Count();
	count += iScrollBar.Count();

	if( AknLayoutUtils::PenEnabled() )
		{
		count--;
		}

	if (iIcon)
		{
		count++;
		}
	count++;

	return count;
	}

//=============================================================================
EXPORT_C CCoeControl* CSingleParamControl::ComponentControl(TInt aIndex) const
	{
	if ( aIndex < iBorders.Count() )
		{
		return iBorders[aIndex];
		}

	TInt touchSelected = 0;
	if( AknLayoutUtils::PenEnabled() )
		{
		touchSelected++;
		}

	if ( aIndex < iBorders.Count() + iScrollBar.Count() - touchSelected )
		{
		if ( iMarkerPressed && (aIndex - iBorders.Count() == 3) )
			{
			return iScrollBar[aIndex - iBorders.Count() + 3];
			}
		return iScrollBar[aIndex - iBorders.Count()];
		}
	if ( aIndex == iBorders.Count() + iScrollBar.Count() - touchSelected )
		{
		return iText;
		}
	return iIcon;
	}

//=============================================================================
EXPORT_C void CSingleParamControl::SizeChanged()
	{
	CountImageSizesAndPositions();
	}

//=============================================================================
EXPORT_C void CSingleParamControl::SetSliderMinimumAndMaximum(TInt aMin, TInt aMax)
	{
	iMinimumValue = aMin;
	iMaximumValue = aMax;
	}

//=============================================================================
EXPORT_C void CSingleParamControl::SetSliderPosition(TInt aPosition)
	{
	__ASSERT_ALWAYS( aPosition >= iMinimumValue, Panic(EHorizontalSliderPanicIndexUnderflow) );
	__ASSERT_ALWAYS( aPosition <= iMaximumValue, Panic(EHorizontalSliderPanicIndexOverflow) );

	iPosition = aPosition;
	}

//=============================================================================
EXPORT_C void CSingleParamControl::SetSliderStep(TUint aStep)
	{
	iStep = aStep;

	TInt variety = 0;
	if( AknLayoutUtils::PenEnabled() )
		{
		variety = 1;
		}

	TAknLayoutRect layoutRect;
	layoutRect.LayoutRect( Rect(), AknLayoutScalable_Apps::popup_imed_adjust2_window(variety) );
	TRect parentRect = layoutRect.Rect();
	TAknWindowComponentLayout l = AknLayoutScalable_Apps::slider_imed_adjust_pane(variety);
	TAknWindowLineLayout lineLayout = l.LayoutLine();
	layoutRect.LayoutRect(parentRect, lineLayout);

	TUint sliderTabWidth = iScrollBar[KSliderPartsNum - 1]->Size().iWidth;
	TUint sliderBitmapWidth = layoutRect.Rect().Width() - sliderTabWidth;

	iStepInPixels = sliderBitmapWidth;
	if ( iMinimumValue < iMaximumValue )
		{
		iStepInPixels = iStep * sliderBitmapWidth
		/ (iMaximumValue - iMinimumValue);
		}
	}

//=============================================================================
EXPORT_C void CSingleParamControl::SetSliderStepAmount(TUint8 aAmount)
	{
		{
		iNumberOfSteps = aAmount;

		if(aAmount == 0)
			{
			iStep = 0;
			}
		else
			{
			iStep = (iMaximumValue - iMinimumValue) / aAmount;
			}
		}
	}

//=============================================================================
EXPORT_C TInt CSingleParamControl::Position() const
	{
	return iPosition;
	}

//=============================================================================
EXPORT_C void CSingleParamControl::CountImageSizesAndPositions()
	{
	TInt variety = 0;
	if( AknLayoutUtils::PenEnabled() )
		{
		variety = 1;
		}

	TAknLayoutRect layoutRect;
	layoutRect.LayoutRect( Rect(), AknLayoutScalable_Apps::popup_imed_adjust2_window(variety) );
	TRect parentRect = layoutRect.Rect();

	// centralize the slider
	TPoint topLeft = parentRect.iTl;
	topLeft.iX = (Rect().Width() - parentRect.Width()) / 2;
	parentRect.SetRect(topLeft, parentRect.Size());

	// determine popup border layouts
	if(AknLayoutUtils::LayoutMirrored())
		{
		AknLayoutUtils::LayoutControl( iBorders[1], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g3().LayoutLine() );
		AknLayoutUtils::LayoutControl( iBorders[2], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g2().LayoutLine() );
		AknLayoutUtils::LayoutControl( iBorders[3], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g5().LayoutLine() );
		AknLayoutUtils::LayoutControl( iBorders[4], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g4().LayoutLine() );
		AknLayoutUtils::LayoutControl( iBorders[5], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g7().LayoutLine() );
		AknLayoutUtils::LayoutControl( iBorders[6], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g6().LayoutLine() );
		}
	else
		{
		AknLayoutUtils::LayoutControl( iBorders[1], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g2().LayoutLine() );
		AknLayoutUtils::LayoutControl( iBorders[2], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g3().LayoutLine() );
		AknLayoutUtils::LayoutControl( iBorders[3], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g4().LayoutLine() );
		AknLayoutUtils::LayoutControl( iBorders[4], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g5().LayoutLine() );
		AknLayoutUtils::LayoutControl( iBorders[5], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g6().LayoutLine() );
		AknLayoutUtils::LayoutControl( iBorders[6], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g7().LayoutLine() );
		}
	AknLayoutUtils::LayoutControl( iBorders[0], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g1().LayoutLine() );
	AknLayoutUtils::LayoutControl( iBorders[7], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g8().LayoutLine() );
	AknLayoutUtils::LayoutControl( iBorders[8], parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g9().LayoutLine() );

	// Images need to be resized separately, because the standard LayoutImage
	// tries to preserve the aspect ratio and we don't want that.
	//
	for ( TInt i = 0; i < iBorders.Count(); ++i )
		{
		AknIconUtils::SetSize(
				const_cast<CFbsBitmap*>( iBorders[i]->Bitmap() ),
				iBorders[i]->Size(), EAspectRatioNotPreserved );
		}

	// determine icon layout
	if (iIcon)
		{
		AknLayoutUtils::LayoutControl( iIcon, parentRect, AknLayoutScalable_Apps::popup_imed_adjust2_window_g1(variety).LayoutLine() );
		AknIconUtils::SetSize(
				const_cast<CFbsBitmap*>( iIcon->Bitmap() ),
				iIcon->Size(), EAspectRatioNotPreserved );
		}

	// determine caption layout
	TAknLayoutText layoutText;
	layoutText.LayoutText( parentRect, AknLayoutScalable_Apps::popup_imed_adjust2_window_t1(variety) );
	TRgb color = layoutText.Color();
	iText->SetFont(layoutText.Font());
	iText->OverrideColorL( EColorLabelText, color );
	iText->SetExtent(layoutText.TextRect().iTl, layoutText.TextRect().Size());
	if(AknLayoutUtils::LayoutMirrored())
		{
		iText->SetAlignment( EHRightVTop );
		}
	else
		{
		iText->SetAlignment( EHLeftVTop );
		}

	// determine scrollbar layouts
	if( AknLayoutUtils::PenEnabled() )
		{
		if(AknLayoutUtils::LayoutMirrored())
			{
			AknLayoutUtils::LayoutControl( iScrollBar[4], parentRect, AknLayoutScalable_Apps::popup_imed_adjust2_window_g3(0).LayoutLine() );
			AknLayoutUtils::LayoutControl( iScrollBar[5], parentRect, AknLayoutScalable_Apps::popup_imed_adjust2_window_g2(0).LayoutLine() );
			}
		else
			{
			AknLayoutUtils::LayoutControl( iScrollBar[4], parentRect, AknLayoutScalable_Apps::popup_imed_adjust2_window_g2(0).LayoutLine() );
			AknLayoutUtils::LayoutControl( iScrollBar[5], parentRect, AknLayoutScalable_Apps::popup_imed_adjust2_window_g3(0).LayoutLine() );
			}
		}
	TAknWindowComponentLayout l = AknLayoutScalable_Apps::slider_imed_adjust_pane(variety);
	TAknWindowLineLayout lineLayout = l.LayoutLine();
	layoutRect.LayoutRect(parentRect, lineLayout);
	if(AknLayoutUtils::LayoutMirrored())
		{
		AknLayoutUtils::LayoutControl( iScrollBar[0], layoutRect.Rect(), AknLayoutScalable_Apps::slider_imed_adjust_pane_g2(variety).LayoutLine() );
		AknLayoutUtils::LayoutControl( iScrollBar[1], layoutRect.Rect(), AknLayoutScalable_Apps::slider_imed_adjust_pane_g1(variety).LayoutLine() );
		}
	else
		{
		AknLayoutUtils::LayoutControl( iScrollBar[0], layoutRect.Rect(), AknLayoutScalable_Apps::slider_imed_adjust_pane_g1(variety).LayoutLine() );
		AknLayoutUtils::LayoutControl( iScrollBar[1], layoutRect.Rect(), AknLayoutScalable_Apps::slider_imed_adjust_pane_g2(variety).LayoutLine() );
		}
	AknLayoutUtils::LayoutControl( iScrollBar[2], layoutRect.Rect(), AknLayoutScalable_Apps::slider_imed_adjust_pane_g3(variety).LayoutLine() );
	AknLayoutUtils::LayoutControl( iScrollBar[3], layoutRect.Rect(), AknLayoutScalable_Apps::slider_imed_adjust_pane_g4(variety).LayoutLine() );
	iScrollBar[3]->SetSize(TSize(KSliderWidth, iScrollBar[3]->Size().iHeight));
	if( AknLayoutUtils::PenEnabled() )
		{
		AknLayoutUtils::LayoutControl( iScrollBar[6], layoutRect.Rect(), AknLayoutScalable_Apps::slider_imed_adjust_pane_g4(variety).LayoutLine() );
		iScrollBar[6]->SetSize(TSize(KSliderWidth, iScrollBar[6]->Size().iHeight));
		}
	for ( TInt i = 0; i < iScrollBar.Count(); ++i )
		{
		AknIconUtils::SetSize(
				const_cast<CFbsBitmap*>( iScrollBar[i]->Bitmap() ),
				iScrollBar[i]->Size(), EAspectRatioNotPreserved );
		}

	// set touch area
	if( AknLayoutUtils::PenEnabled() )
		{
		iTouchRect.iTl.iX = iScrollBar[4]->Rect().iTl.iX;
		iTouchRect.iTl.iY = iScrollBar[0]->Rect().iTl.iY;
		iTouchRect.iBr.iX = iScrollBar[5]->Rect().iBr.iX;
		iTouchRect.iBr.iY = iScrollBar[1]->Rect().iBr.iY;
		}

	// slider tab width
	TUint sliderTabWidth = KSliderWidth;
	// slider bitmap is actually a bit longer but this resolves the problem
	// where the tab is drawn outside of the slider when in maximum position
	TUint sliderBitmapWidth = layoutRect.Rect().Width() - sliderTabWidth;
	iStepInPixels = sliderBitmapWidth;
	if ( iMinimumValue < iMaximumValue )
		{
		iStepInPixels = iStep * sliderBitmapWidth
		/ (iMaximumValue - iMinimumValue);
		}
	}

EXPORT_C void CSingleParamControl::MoveSlider(TInt aSteps)
	{
	iPosition += iStep * aSteps;

	if(iPosition> iMaximumValue)
		{
		iPosition = iMaximumValue;
		}
	if(iPosition < iMinimumValue)
		{
		iPosition = iMinimumValue;
		}

	if(aSteps < 0)
		{
		for ( TInt i = 0; i> aSteps; i-- )
			{
			iParObserver->ParamOperation(MSingleParControlObserver::EParamOperationSubtract);
			}
		}
	else
		{
		for ( TInt i = 0; i < aSteps; i++ )
			{
			iParObserver->ParamOperation(MSingleParControlObserver::EParamOperationAdd);
			}
		}
	
	if( !iActionOnButtonRelease )
		{
		iEditorView->HandleCommandL (EImageEditorCmdRender);
		}
	}

// End of File


