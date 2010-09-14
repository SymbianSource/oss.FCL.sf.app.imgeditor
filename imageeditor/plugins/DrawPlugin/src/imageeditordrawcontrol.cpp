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
 * Image Editor plugin control class.
 *
 */
//  INCLUDES
#include "imageeditordrawcontrol.h"
#include "draw.hrh"
#include "ImageEditorUI.hrh"
#include "ImageEditorPluginBase.hrh"
#include "PluginInfo.h"
#include "JpTimer.h"
#include "ImageEditorUids.hrh"
#include "ImageEditorUiDefs.h"
#include "SystemParameters.h"
#include "drawpath.h"
#include "selectionpopup.h"
#include "iepb.h" 
// debug log
#include "imageeditordebugutils.h"
#include <fbs.h>
#include <badesca.h>
#include <gdi.h>
#include <eikenv.h>
#include <bitdev.h>
#include <aknview.h>
#include <aknutils.h>
#include <ImageEditorUI.mbg>
#include <AknInfoPopupNoteController.h> 
#include <draw.rsg> 
#include <bautils.h> 
#include <ConeResLoader.h> 
#include <gulalign.h>
#include <csxhelp/sie.hlp.hrh>
#include <Icons_doodle.mbg>
#include "CImageLabel.h"
#include <aknnotewrappers.h>
// Log file
_LIT(KDrawPluginLogFile,"drawplugin.log");

//  CONSTANTS
const TInt KWait = 1;
const TInt KMainTextIndex = 0;

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------

CImageEditorDrawControl* CImageEditorDrawControl::NewL(const TRect& aRect,
		CCoeControl* aParent, RPointerArray<CDrawPath>& aPaths, TRgb& aRgb,
		TSize& aSize, TBool &aDisplayTool)
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::NewL()");

	CImageEditorDrawControl * self = new (ELeave) CImageEditorDrawControl(
			aPaths, aRgb, aSize, aDisplayTool);
	CleanupStack::PushL(self);
	self->ConstructL(aRect, aParent);
	CleanupStack::Pop(); // self
	return self;
	}

// ---------------------------------------------------------------------------
// CImageEditorDrawControl
// ---------------------------------------------------------------------------
CImageEditorDrawControl::CImageEditorDrawControl(
		RPointerArray<CDrawPath>& aPaths, TRgb& aRgb, TSize& aSize,
		TBool& aDisplayTool) :
	iTickCount(0), iX(0), iY(0), iNaviStepMultiplier(
			KDefaultSmallNavigationStepMultiplier), iDisplayingOkOptionsMenu(
			EFalse), iReadyToRender(EFalse), iResLoader(*ControlEnv()), iPaths(
			aPaths), iRgb(aRgb), iSize(aSize), iDisplayTool(aDisplayTool)

	{
	}

// ---------------------------------------------------------------------------
// ~CImageEditorDrawControl()
// ---------------------------------------------------------------------------

CImageEditorDrawControl::~CImageEditorDrawControl()
	{
		LOG(KDrawPluginLogFile,
				"CImageEditorDrawControl::~CImageEditorDrawControl()");
	delete iCustomComponet;
	delete iTimer;
	delete iIndicator;
	delete iIndicatorMask;
	delete iPopupController;
	delete iTooltipResize;
	delete iRollAo;
	delete iNaviPaneText;
	delete iGc;
	delete iBitmapDev;
	delete iAnimationAO;

	iUndoPaths.ResetAndDestroy();
	iResLoader.Close();

	for (int i = 0; i < iTotalComponent; i++)
		{
		CImageLabel* &temp = iLabels[i];
		if (temp != NULL)
			{
			delete temp;
			temp = NULL;
			}
		}
	iLabels.ResetAndDestroy();

	}

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
void CImageEditorDrawControl::ConstructL(const TRect& aRect,
		CCoeControl* aParent)
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::ConstructL()");

	CreateWindowL();
	EnableDragEvents();
	
	iStatuspaneHeight = aRect.iTl.iY;
	iUndo = EFalse;
	iRedo = EFalse;
	iCanRedo = EFalse;
	//  Create resource utility
	TFileName resFile;
	// resource file name
	_LIT(KResourceFile, "draw.rsc");
	resFile.Append(KPgnResourcePath);
	resFile.Append(KResourceFile);

	User::LeaveIfError(CompleteWithAppPath(resFile));
	// Implementation of RConeResourceLoader uses BaflUtils::NearestLanguageFile 
	// to search for a localised resource in proper search order
	iResLoader.OpenL(resFile);

	//	Create timer for fast key repeat
	iTimer = CJPTimer::NewL(this);

	iPopupController = CAknInfoPopupNoteController::NewL();
	iTooltipResize = ControlEnv()->AllocReadResourceL(R_TOOLTIP_TEXT_RESIZE);

	SetExtentToWholeScreen();
	
	iCustomComponetClick = EFalse;
	TRect rect = Rect();
	//judge the current screen status.
	if (rect.Width() < rect.Height())
		{
		InitializeMainMenu(EMainMenuVertical);
		}
	else if (rect.Width() > rect.Height())
		{
		InitializeMainMenu(EMainMenuHorizontal);
		}

	TRect Trect(TPoint(0, 0), TPoint(50, 50));
	iCustomComponet = CCustomerComponet::NewL(Trect, this);
	iCustomComponet->SetExtent(TPoint(40, 100), TSize(50, 50));
	iCustomComponet->SetHandlePointerObserver(this);
	iLastPointEventType = EFalse;
	//	Activate control
	ActivateL();

	}
// ---------------------------------------------------------------------------
// CImageEditorDrawControl::InitializeMainMenu£¨£©
// initialize tool bar component.
// ---------------------------------------------------------------------------
void CImageEditorDrawControl::InitializeMainMenu(
		TMainMenuLayout aMainMenuLayout)
	{
	iMainMenuLayout = aMainMenuLayout;
	_LIT(KSvgPath, "\\resource\\apps\\icons_doodle.mif");
	TFileName svgFile(KSvgPath);
	User::LeaveIfError(CompleteWithAppPath(svgFile));
	iTotalComponent = 7;
	TRect clipRect;
	if (iMainMenuLayout == EMainMenuVertical)
		{
		iLoopMenuRect = TRect(TPoint(0, Rect().iBr.iY - 87), TSize(360, 87));

		for (int i = 0; i < iTotalComponent; i++)
			{
			if (iDisplayTool)
				{
				TRect
						Rect1(TPoint(i * 72, iLoopMenuRect.iTl.iY), TSize(72,
								87));
				clipRect = Rect1;
				}

			else
				{
				TRect Rect2(TPoint(i * 72 - iLoopMenuRect.Width() - 72 * 2,
						iLoopMenuRect.iTl.iY), TSize(72, 87));
				clipRect = Rect2;
				}

			CImageLabel* label = CImageLabel::NewL(this, clipRect,
					CImageLabel::ELabelVertical);
			label->SetHandlePointerObserver(this);
			iLabels.Append(label);
			}
		}
	else if (iMainMenuLayout ==EMainMenuHorizontal )
		{
		iLoopMenuRect = TRect(TPoint(Rect().iBr.iX - 87, 0), TSize(87, 360));

		for (int i = 0; i < iTotalComponent; i++)
			{
			if (iDisplayTool)
				{
				TRect
						Rect1(TPoint(iLoopMenuRect.iTl.iX, i * 72), TSize(87,
								72));
				clipRect = Rect1;
				}
			else
				{
				TRect Rect2(TPoint(iLoopMenuRect.iTl.iX, i * 72
						- iLoopMenuRect.Height() - 72 * 2), TSize(87, 72));
				clipRect = Rect2;
				}

			CImageLabel* label = CImageLabel::NewL(this, clipRect,
					CImageLabel::ELabelHorizontal);
			label->SetHandlePointerObserver(this);
			iLabels.Append(label);
			}
		}

	iLabels[0]->SetImage(svgFile, EMbmIcons_doodlePen);
	iLabels[0]->SetTooltip(_L("Pensize"));

	iLabels[1]->SetImage(svgFile, EMbmIcons_doodlePalette_colored);
	iLabels[1]->SetTooltip(_L("Palette"));

	iLabels[2]->SetImage(svgFile, EMbmIcons_doodleUndo);
	iLabels[2]->SetTooltip(_L("Undo"));

	iLabels[3]->SetImage(svgFile, EMbmIcons_doodleRedo);
	iLabels[3]->SetTooltip(_L("Redo"));
	
	iLabels[4]->SetImage(svgFile, EMbmIcons_doodleSave1);
	iLabels[4]->SetTooltip(_L("Save"));

	iLabels[5]->SetImage(svgFile, EMbmIcons_doodleInfo);
	iLabels[5]->SetTooltip(_L("About"));

	iLabels[6]->SetImage(svgFile, EMbmIcons_doodleSave2);
	iLabels[6]->SetTooltip(_L("Null"));

	iAnimationAO = CAnimationAO::NewL();
	iAnimationAO->SetObserver(this);
	iRollAo = CRollAO::NewL();
	iRollAo->SetObserver(this);

	iButtonDownPoint = TPoint(0, 0);
	iDragStartPoint = TPoint(0, 0);
	iDragPrePoint = TPoint(0, 0);
	iDragUpPoint = TPoint(0, 0);

	iPointStartInLoopRect = EFalse;
	iDragIsStart = EFalse;
	iDragOffset = 0;

	}

// ---------------------------------------------------------------------------
// SetView
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::SetView(CAknView* aView)
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::SetView()");
	ASSERT(aView);
	iEditorView = aView;
	}

// ---------------------------------------------------------------------------
// SetSelectedUiItemL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::SetSelectedUiItemL(CPluginInfo* aItem)
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::SetSelectedUiItemL()");
	ASSERT(aItem);
	iItem = aItem;
	delete iNaviPaneText;
	iNaviPaneText = NULL;
	iNaviPaneText = (iItem->Parameters()[KMainTextIndex]).AllocL();
	iEditorView->HandleCommandL(EImageEditorCmdRender);
	}

// ---------------------------------------------------------------------------
// OfferKeyEventL
// ---------------------------------------------------------------------------
//
TKeyResponse CImageEditorDrawControl::OfferKeyEventL(
		const TKeyEvent& aKeyEvent, TEventCode aType)
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::OfferKeyEventL()");

	TKeyResponse response = EKeyWasNotConsumed;

	//  If busy, do not handle anything
	if (Busy())
		{
		response = EKeyWasConsumed;
		}

	// In main state handle the OK Options menu
	else if (aKeyEvent.iCode == EKeyOK)
		{
		iDisplayingOkOptionsMenu = ETrue;
		iEditorView->HandleCommandL(EImageEditorTryDisplayMenuBar);
		response = EKeyWasConsumed;
		}

	//  We handle only event keys
	else if (EEventKey == aType)
		{

		switch (aKeyEvent.iCode)
			{

			case EKeyDownArrow:
			case EKeyUpArrow:
			case EKeyRightArrow:
			case EKeyLeftArrow:
				{
				SetPositionOnImage(ComputeIndicatorPosition());
				StoreParameters(EFalse, EFalse);
				response = EKeyWasConsumed;
				break;
				}

			case EKeyOK:
				{
				break;
				}

			case 0x30: // 0		    		    
			case 0x32: // 2
			case 0x34: // 4
			case 0x35: // 5
			case 0x36: // 6
			case 0x38: // 8
			case EStdKeyIncVolume: // zoom in key
			case EStdKeyDecVolume: // zoom out key
				{
				StorePosAndScaleRelScreen();
				break;
				}

			default:
				{
				break;
				}
			}
		}

	//	Key pressed down, mark pressed key
	else if (aType == EEventKeyDown)
		{
		switch (aKeyEvent.iScanCode)
			{
			case EStdKeyUpArrow:
				{
				iKeyCode = 1;
				response = EKeyWasConsumed;
				break;
				}
			case EStdKeyDownArrow:
				{
				iKeyCode = 2;
				response = EKeyWasConsumed;
				break;
				}
			case EStdKeyLeftArrow:
				{
				iKeyCode = 3;
				response = EKeyWasConsumed;
				break;
				}
			case EStdKeyRightArrow:
				{
				iKeyCode = 4;
				response = EKeyWasConsumed;
				break;
				}
			}

		if (iKeyCode != 0)
			{
			iNaviStepMultiplier = KDefaultSmallNavigationStepMultiplier;
			iTickCount = 0;
			iTimer->Call(KWait);
			}
		}

	//	Key released, mark all keys to zero
	else if (aType == EEventKeyUp)
		{
		switch (aKeyEvent.iScanCode)
			{
			case EStdKeyUpArrow:
			case EStdKeyDownArrow:
			case EStdKeyLeftArrow:
			case EStdKeyRightArrow:
				{
				iKeyCode = 0;
				response = EKeyWasConsumed;
				ShowTooltip();
				break;
				}
			}
		}

	return response;
	}

// ---------------------------------------------------------------------------
// SizeChanged
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::SizeChanged()
	{
	if (iSizeChangeFlag)
		{
		SetToolBarStatus();
		}
	else
		{
		iSizeChangeFlag = ETrue;
		}
	if (iDragIsStart)
		{
		if (iMainMenuLayout == EMainMenuVertical)
			{
			for (TInt i = 0; i < iTotalComponent; i++)
				{
				if (iLabels[0]->Rect().iTl.iX <= 0)
					{
					TInt x = iLabels[i]->Rect().iTl.iX;
					iLabels[i]->SetPosition(TPoint(x + iDragOffset,
							iLoopMenuRect.iTl.iY));
					iLabels[i]->ResetControl();
					}
				}
			}
		else if (iMainMenuLayout ==EMainMenuHorizontal )
			{
			for (TInt i = 0; i < iTotalComponent; i++)
				{
				if (iLabels[0]->Rect().iTl.iY <= 0)
					{
					TInt y = iLabels[i]->Rect().iTl.iY;
					iLabels[i]->SetPosition(TPoint(iLoopMenuRect.iTl.iX, y
							+ iDragOffset));
					iLabels[i]->ResetControl();
					}
				}
			}
		DrawNow();
		}
	}
// ---------------------------------------------------------------------------
// CImageEditorDrawControl::SetToolBarStatus()
// Set and get current status : display or hidden.
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::SetToolBarStatus()
	{
	if (iFlagDragToolBar)
		{
		iDisplayTool = ETrue;
		iFlagDragToolBar = EFalse;
		}
	if (iMainMenuLayout == EMainMenuVertical)
		{
		TInt leftX = iLabels[0]->Rect().iTl.iX;
		TInt rightX = iLabels[iTotalComponent - 1]->Rect().iBr.iX;
		if (leftX == 0)
			iDisplayTool = ETrue;
		else if (rightX == 0)
			iDisplayTool = EFalse;
		}
	else if (iMainMenuLayout ==EMainMenuHorizontal )
		{
		TInt topY = iLabels[0]->Rect().iTl.iY;
		TInt bottomY = iLabels[iTotalComponent - 1]->Rect().iBr.iY;
		if (topY == 0)
			iDisplayTool = ETrue;
		else if (bottomY == 0)
			iDisplayTool = EFalse;

		}

	}
// ---------------------------------------------------------------------------
// GetParam
// ---------------------------------------------------------------------------
//
TDesC& CImageEditorDrawControl::GetParam()
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::GetParam()");
		LOGDES(KDrawPluginLogFile, iParam);
	return iParam;
	}

// ---------------------------------------------------------------------------
// SetSystemParameters
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::SetSystemParameters(
		const CSystemParameters* aPars)
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::SetSystemParameters()");
	ASSERT(aPars);
	iSysPars = aPars;
	}

// ---------------------------------------------------------------------------
// HandlePluginCommandL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::HandlePluginCommandL(const TInt aCommand)
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::HandlePluginCommandL()");

	switch (aCommand)
		{
		case EImageEditorFocusLost:
			{
			if (iTimer)
				{
				iTimer->Cancel();
				}
			break;
			}
		case EPgnSoftkeyIdOk:
			{
			iPopupController->HideInfoPopupNote();
			break;
			}
		case EPgnSoftkeyIdCancel:
			{
			iPopupController->HideInfoPopupNote();
			iEditorView->HandleCommandL(EImageEditorCancelPlugin);
			break;
			}
		case EPgnSoftkeyIdDone:
			// FALLTROUHG
		case EDrawPgnMenuCmdDone:
			{
			iPopupController->HideInfoPopupNote();
			StoreParameters(EFalse, ETrue);
			iEditorView->HandleCommandL(EImageEditorCmdRender);//results Draw call
			iEditorView->HandleCommandL(EImageEditorApplyPlugin);
			break;
			}
		case EDrawPgnMenuCmdResize:
			{
			StoreParameters(ETrue, ETrue);
			iEditorView->HandleCommandL(EImageEditorCmdRender);//results Draw call
			SelectSizeL();
			break;
			}
		case EDrawPgnMenuCmdColor:
			{
			StoreParameters(ETrue, ETrue);
			iEditorView->HandleCommandL(EImageEditorCmdRender);//results Draw call            
			SDrawUtils::LaunchColorSelectionPopupL(iPreview, Rect(), iRgb);
			break;
			}
		case EDrawPgnMenuCmdCancel:
			{
			iReadyToRender = EFalse;
			iEditorView->HandleCommandL(EImageEditorCancelPlugin);
			break;
			}

		case EImageEditorGlobalZoomChanged:
			// FALLTROUGHT
		case EImageEditorGlobalPanChanged:
			{
			RestorePosAndScaleRelScreen();
			StoreTempParams();
			iEditorView->HandleCommandL(EImageEditorCmdRender);
			break;
			}

		case EImageEditorPreScreenModeChange:
			{
			StorePosAndScaleRelImage();
			break;
			}

		case EImageEditorPostScreenModeChange:
			{
			RestorePosAndScaleRelImage();
			ClipPosition();
			StoreTempParams();
			iEditorView->HandleCommandL(EImageEditorCmdRender);
			break;
			}
		default:
			{
			break;
			}
		}
	}

// ---------------------------------------------------------------------------
// GetSoftkeyIndexL
// ---------------------------------------------------------------------------
//
TInt CImageEditorDrawControl::GetSoftkeyIndexL()
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::GetSoftkeyIndexL()");
	// : Check needed states
	TInt state(2);
	return state;
	}

// ---------------------------------------------------------------------------
// GetContextMenuResourceId
// ---------------------------------------------------------------------------
//
TInt CImageEditorDrawControl::GetContextMenuResourceId()
	{
	return R_TEXT_CONTEXT_MENUBAR;
	}

// ---------------------------------------------------------------------------
// GetDimmedMenuItems
// ---------------------------------------------------------------------------
//
TBitField CImageEditorDrawControl::GetDimmedMenuItems()
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::GetDimmedMenuItems()");

	TBitField dimmedMenuItems;
	TInt count = iItem->MenuItems().Count();

	if (iDisplayingOkOptionsMenu)
		{
		// Dim the command EImageEditorCancelPlugin
		for (TInt i = 0; i < count; i++)
			{
			// get the menu item id
			TInt menuItem = iItem->MenuItems().At(i).iCommandId;
			if (menuItem == EDrawPgnMenuCmdCancel)
				{
				dimmedMenuItems.SetBit(i);
				}
			}
		iDisplayingOkOptionsMenu = EFalse;
		}

	for (TInt i = 0; i < count; i++)
		{
		// get the menu item id
		TInt menuItem = iItem->MenuItems().At(i).iCommandId;
		if (menuItem == EDrawPgnMenuCmdMax)
			{
			dimmedMenuItems.SetBit(i);
			}
		}
	return dimmedMenuItems;
	}

// ---------------------------------------------------------------------------
// GetNaviPaneTextL
// ---------------------------------------------------------------------------
//
TPtrC CImageEditorDrawControl::GetNaviPaneTextL(
		TBool& aLeftNaviPaneScrollButtonVisibile,
		TBool& aRightNaviPaneScrollButtonVisible)
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::GetNaviPaneTextL()");

	aLeftNaviPaneScrollButtonVisibile = EFalse;
	aRightNaviPaneScrollButtonVisible = EFalse;
	return *iNaviPaneText;
	}

// ---------------------------------------------------------------------------
// Draw
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::Draw(const TRect& aRect) const
	{
	CWindowGc & gc = SystemGc();
	gc.SetBrushColor(KRgbBlack);
	gc.Clear(aRect);

	TRect ImageRect(iSysPars->VisibleImageRectPrev());
	CPreviewControlBase::DrawPreviewImage(aRect);
	gc.SetClippingRect(TRect(TPoint(ImageRect.iTl.iX, ImageRect.iTl.iY
			+ iStatuspaneHeight), ImageRect.Size()));

	if (iPaths.Count() > 0)
		{
		//        ASSERT( iSysPars ); 
		TInt count = iPaths.Count();
		for (TInt i(0); i < iPaths.Count(); ++i)
			{
			CDrawPath* path = iPaths[i];
			CArrayFix<TPoint>* pathPoints = path->ItemArray();
			gc.SetPenStyle(CGraphicsContext::ESolidPen);
			gc.SetPenColor(path->Color());
			gc.SetPenSize(ScaledLineSize(path->Size()));
			gc.DrawPolyLine(pathPoints);
			}
		}
	}

// ---------------------------------------------------------------------------
// NaviDown
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::NaviDown()
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::NaviDown()");
	}

// ---------------------------------------------------------------------------
// NaviUp
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::NaviUp()
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::NaviUp()");
	}

// ---------------------------------------------------------------------------
// NaviRight
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::NaviRight()
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::NaviRight()");
	}

// ---------------------------------------------------------------------------
// NaviLeft
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::NaviLeft()
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::NaviLeft()");
	}

// ---------------------------------------------------------------------------
// SelectSizeL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::SelectSizeL()
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::SelectSizeL()");
	// Symmetric line width / height
	TInt size(iSize.iWidth);
	CSelectionDialog::RunDlgLD(iPreview, Rect(), size, iRgb,
			iSysPars->VisibleImageRectPrev());
	iSize = TSize(size, size);
	}

// ---------------------------------------------------------------------------
// StoreTempParams
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::StoreTempParams()
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::StoreTempParams()");
	}

// ---------------------------------------------------------------------------
// RestoreTempParams
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::RestoreTempParams()
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::RestoreTempParams()");
	}

// ---------------------------------------------------------------------------
// GetHelpContext
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::GetHelpContext(TCoeHelpContext& aContext) const
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::GetHelpContext()");

	aContext.iMajor = TUid::Uid(UID_IMAGE_EDITOR);
	aContext.iContext = KSIE_HLP_EDIT_DRAW;
	}

// ---------------------------------------------------------------------------
// PrepareL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::PrepareL()
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::PrepareL()");

	//	Get current view port
	TRect rect = iSysPars->VisibleImageRectPrev();

	iReadyToRender = ETrue;

	iEditorView->HandleCommandL(EImageEditorCmdRender);
	}

// ---------------------------------------------------------------------------
// LoadIndicatorL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::LoadIndicatorL(TInt aBitmapInd, TInt aMaskInd)
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::LoadIndicatorL()");

	//  Delete old indicator
	delete iIndicator;
	iIndicator = 0;
	delete iIndicatorMask;
	iIndicatorMask = 0;

	//  Load new indicator
	SDrawUtils::GetIndicatorBitmapL(iIndicator, iIndicatorMask, aBitmapInd,
			aMaskInd);
	}

// ---------------------------------------------------------------------------
// ComputeIndicatorPosition
// ---------------------------------------------------------------------------
//
TPoint CImageEditorDrawControl::ComputeIndicatorPosition() const
	{
	TRect vprect = iSysPars->VisibleImageRect();
	TRect vpprect = iSysPars->VisibleImageRectPrev();

	TInt x(0);
	TInt y(0);
	// check if there is no indicator
	if (!iIndicator)
		{
		return TPoint(x, y);
		}

	y = ((iY - vprect.iTl.iY) * vpprect.Height()) / vprect.Height();
	y += vpprect.iTl.iY;

	x = ((iX - vprect.iTl.iX) * vpprect.Width()) / vprect.Width();
	x += vpprect.iTl.iX;

	return TPoint(x, y);
	}

// ---------------------------------------------------------------------------
// IsReadyToRender
// ---------------------------------------------------------------------------
//
TBool CImageEditorDrawControl::IsReadyToRender() const
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::IsReadyToRender()");
	return iReadyToRender;
	}

// ---------------------------------------------------------------------------
// StoreParameters
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::StoreParameters(TBool aLastItem, TBool aDone)
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::StoreParameters()");
	RDebug::Print(_L("CImageEditorDrawControl::StoreParameters color[%d]"),
	iRgb.Internal());

	iParam.Copy(_L("x "));
	iParam.AppendNum(iX);
	iParam.Append(_L(" y "));
	iParam.AppendNum(iY);
	if (aLastItem)
		{
		iParam.Append(_L(" size "));
		iParam.AppendNum(iSize.iHeight);
		iParam.Append(_L(" color "));
		iParam.AppendNum(iRgb.Value());
		iParam.Append(_L(" lastItem "));
		}

	if (aDone)
		{
		iParam.Zero();
		iParam.Append(_L("done"));
		}
	if (iUndo)
		{
		iParam.Zero();
		iParam.Append(_L("Undo"));
		}
	if (iRedo)
		{
		iParam.Zero();
		iParam.Append(_L("redone"));
		}
	}

// ---------------------------------------------------------------------------
// TimerCallBack
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::TimerCallBack()
	{
		LOG(KDrawPluginLogFile, "CImageEditorDrawControl::TimerCallBack()");

	if (iTickCount > KDefaultFastKeyTimerMultiplyThresholdInTicks)
		{
		iNaviStepMultiplier = KDefaultBigNavigationStepMultiplier;
		}
	else
		{
		iTickCount++;
		}

	if (iKeyCode)
		{

		switch (iKeyCode)
			{
			case 1:
				{
				NaviUp();
				break;
				}
			case 2:
				{
				NaviDown();
				break;
				}
			case 3:
				{
				NaviLeft();
				break;
				}
			case 4:
				{
				NaviRight();
				break;
				}
			default:
				break;
			}
			//StoreParameters();
			TRAP_IGNORE(iEditorView->HandleCommandL(EImageEditorCmdRender));
		iTimer->Call(KWait);
		}
	}

// ---------------------------------------------------------------------------
// StorePosAndScaleRelScreen
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::StorePosAndScaleRelScreen()
	{
		LOG(KDrawPluginLogFile,
				"CImageEditorClipartControl::StorePosAndScaleRelScreen()");
	iParam.Copy(_L("nop"));
	}

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::RestorePosAndScaleRelScreen()
	{
		LOG(KDrawPluginLogFile,
				"CImageEditorClipartControl::RestorePosAndScaleRelScreen()");
	}

// ---------------------------------------------------------------------------
// StorePosAndScaleRelImage
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::StorePosAndScaleRelImage()
	{
	TReal relscale = iSysPars->RelScale();
		LOGFMT(KDrawPluginLogFile,
				"CImageEditorClipartControl::StorePosAndScaleRelImage():%g",
				relscale);
	}

// ---------------------------------------------------------------------------
// RestorePosAndScaleRelImage
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::RestorePosAndScaleRelImage()
	{
	TReal relscale = iSysPars->RelScale();
		LOGFMT(KDrawPluginLogFile,
				"CImageEditorClipartControl::RestorePosAndScaleRelImage():%g",
				relscale);
	}

// ---------------------------------------------------------------------------
// ClipPosition
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::ClipPosition()
	{
	if (iX < iSysPars->VisibleImageRect().iTl.iX)
		{
		iX = iSysPars->VisibleImageRect().iTl.iX;
		}
	else if (iX > iSysPars->VisibleImageRect().iBr.iX)
		{
		iX = iSysPars->VisibleImageRect().iBr.iX;
		}

	if (iY < iSysPars->VisibleImageRect().iTl.iY)
		{
		iY = iSysPars->VisibleImageRect().iTl.iY;
		}
	else if (iY > iSysPars->VisibleImageRect().iBr.iY)
		{
		iY = iSysPars->VisibleImageRect().iBr.iY;
		}
	}

// ---------------------------------------------------------------------------
// HandlePointerEventL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::HandlePointerEventL(
		const TPointerEvent &aPointerEvent)
	{
	SetToolBarStatus();
	RDebug::Print(_L("CImageEditorDrawControl::HandlePointerEventL pen[%d]"),
	AknLayoutUtils::PenEnabled());
	if (iRollflag)
		return;
	if (AknLayoutUtils::PenEnabled() && !Busy())
		{
		RDebug::Print(_L("CImageEditorDrawControl::iType [%d]"),
		aPointerEvent.iType);
		TBool lastItem(EFalse);
		TPoint point = aPointerEvent.iPosition;

		switch (aPointerEvent.iType)
			{
			case TPointerEvent::EButton1Down:
				{

				iButtonDownPoint = point;
				iLastPointEventType = EFalse;
				CCoeControl::HandlePointerEventL(aPointerEvent);
                 //Handle Drag tool bar event.
				if (iLoopMenuRect.Contains(point) && !iCustomComponetClick
						&& iDisplayTool)

					{
					iPointStartInLoopRect = ETrue;
					if (iDragIsStart)
						{
						iAnimationAO->Cancel();
						iDragIsStart = EFalse;
						}

					}
				//Doodle event.
				else if ((!iLoopMenuRect.Contains(point)
						&& !iCustomComponetClick && iDisplayTool)
						|| (!iDisplayTool && !iCustomComponetClick))
					{
					iPointStartInLoopRect = EFalse;
					//System dispose
					iReadyToRender = EFalse;
					CDrawPath* lastPath = CDrawPath::NewL();
					CleanupStack::PushL(lastPath);
					lastPath->SetColor(iRgb);
					lastPath->SetSize(iSize);
					SetPositionOnImage(aPointerEvent.iPosition);
					lastPath->AddItemL(aPointerEvent.iPosition);
					iPaths.AppendL(lastPath);
					CleanupStack::Pop(lastPath);
					iCanRedo = EFalse;
					iUndoPaths.ResetAndDestroy();
					}
				}
				break;
			case TPointerEvent::EButton1Up:
				{
				//		System dispose
				iReadyToRender = ETrue;
				iCustomComponetClick = EFalse;
				lastItem = ETrue;
				CCoeControl::HandlePointerEventL(aPointerEvent);
				HandleMainMenuButtonUp(point);
				DrawNow();

				}
				break;
			case TPointerEvent::EDrag:
				{
				CCoeControl::HandlePointerEventL(aPointerEvent);
			    //Mark last event use for custompoent.	
				iLastPointEventType = ETrue;

				if ((iDisplayTool && iPaths.Count() && !iCustomComponetClick
						&& !iPointStartInLoopRect) || (!iDisplayTool
						&& iPaths.Count() && !iCustomComponetClick))
					{
					iCanRedo = EFalse;
					iUndoPaths.ResetAndDestroy();
					CDrawPath* path = iPaths[iPaths.Count() - 1];
					// store current position for next round
					iPointerPosition = aPointerEvent.iPosition;
					SetPositionOnImage(aPointerEvent.iPosition);
					path->AddItemL(aPointerEvent.iPosition);
					}
				//Drag customcomponent event
				if (iCustomComponetClick)
					{
					TPoint con(aPointerEvent.iPosition.iX - 25,
							aPointerEvent.iPosition.iY - 25);
					iCustomComponet->SetExtent(con, TSize(50, 50));
					DrawNow();
					}
                //Drag tool bar event
				if (iDisplayTool && iPointStartInLoopRect)

					{
					iFlagDragToolBar = ETrue;
					HandleMainMenuButtonDrag(point);
					}
				}
				break;
			default:

				break;

			}

		//System dispose
		if ((!iPointStartInLoopRect && iDisplayTool) || !iDisplayTool)
			{
			if (!(iCustomComponet->Rect().Contains(aPointerEvent.iPosition)))
				{
				StoreParameters(lastItem, EFalse);
				iEditorView->HandleCommandL(EImageEditorCmdRender);//results Draw call
				}
			}
		}

	}

// ---------------------------------------------------------------------------
// CImageEditorDrawControl::HandleMainMenuButtonUp()
//
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::HandleMainMenuButtonUp(TPoint aPoint)
	{
	if (iMainMenuLayout == EMainMenuVertical)
		{
		if (iDragIsStart)
			{
			//Drag event is end (Except activating animation effect).
			iDragIsStart = EFalse;

			if (Abs(iDragPrePoint.iX - aPoint.iX) > 1)
				{
				//Animation effect is activated
				iDragIsStart = ETrue;
				iStartSpeed = aPoint.iX - iDragPrePoint.iX;
				iAnimationAO->StartL(0);
				}
			}
		}
	else if (iMainMenuLayout ==EMainMenuHorizontal )
		{
		if (iDragIsStart)
			{
			//Drag event is end (Except activating animation effect).
			iDragIsStart = EFalse;

			if (Abs(iDragPrePoint.iY - aPoint.iY) > 1)
				{
				//Animation effect is activated
				iDragIsStart = ETrue;
				iStartSpeed = aPoint.iY - iDragPrePoint.iY;
				iAnimationAO->StartL(0);
				}
			}
		}
	}

// ---------------------------------------------------------------------------
// HandleMainMenuButtonDrag
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::HandleMainMenuButtonDrag(TPoint aPoint)
	{
	if (iMainMenuLayout == EMainMenuVertical)
		{
		if (iPointStartInLoopRect && !iDragIsStart && Abs(aPoint.iX
				- iButtonDownPoint.iX) >= DRAG_LENGTH)
			{
			iDragIsStart = ETrue;
			iDragStartPoint = aPoint;

			for (TInt i = 0; i < iTotalComponent; i++)
				{
				if (iLabels[i]->Rect().Contains(iButtonDownPoint))
					{
					iLabels[i]->LabelIsMoving();
					}
				}
			}

		if (iDragIsStart)
			{
			iDragPrePoint = iDragUpPoint;
			iDragUpPoint = aPoint;
			iDragOffset = aPoint.iX - iDragStartPoint.iX;
			iDragStartPoint = aPoint;

			CalculateEffectiveOffset();
			SizeChanged();
			}
		}
	else if (iMainMenuLayout ==EMainMenuHorizontal )
		{
		if (iPointStartInLoopRect && !iDragIsStart && Abs(aPoint.iY
				- iButtonDownPoint.iY) >= DRAG_LENGTH)
			{
			iDragIsStart = ETrue;
			iDragStartPoint = aPoint;

			for (TInt i = 0; i < iTotalComponent; i++)
				{
				if (iLabels[i]->Rect().Contains(iButtonDownPoint))
					{
					iLabels[i]->LabelIsMoving();
					}
				}
			}

		if (iDragIsStart)
			{
			iDragPrePoint = iDragUpPoint;
			iDragUpPoint = aPoint;
			iDragOffset = aPoint.iY - iDragStartPoint.iY;
			iDragStartPoint = aPoint;

			CalculateEffectiveOffset();
			SizeChanged();
			}
		}
	}

// ---------------------------------------------------------------------------
// UndoDrawPath
// ---------------------------------------------------------------------------
//   
void CImageEditorDrawControl::UndoDrawPath()
	{
	if (iPaths.Count())
		{
		iUndoPaths.Append(iPaths[iPaths.Count() - 1]);
		iPaths.Remove(iPaths.Count() - 1);
		iCanRedo = ETrue;
		}
	}

// ---------------------------------------------------------------------------
// RedoDrawPath
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::RedoDrawPath()
	{
	if (iUndoPaths.Count())
		{
		iPaths.Append(iUndoPaths[iUndoPaths.Count() - 1]);
		iUndoPaths.Remove(iUndoPaths.Count() - 1);
		}
	}

// ---------------------------------------------------------------------------
// HandlePointerNotify
// ---------------------------------------------------------------------------
//
void CImageEditorDrawControl::HandlePointerNotify(CCoeControl* aComponent)
	{
	//pen size
	if (iLabels[0] == dynamic_cast<CImageLabel*> (aComponent))
		{
		MakeVisibleLable(EFalse);
		SelectSizeL();
		MakeVisibleLable(ETrue);
		}
	//pen color
	else if (iLabels[1] == dynamic_cast<CImageLabel*> (aComponent))
		{
		MakeVisibleLable(EFalse);
		SDrawUtils::LaunchColorSelectionPopupL(iPreview, Rect(), iRgb);
		MakeVisibleLable(ETrue);
		}
	//undo
	else if (iLabels[2] == dynamic_cast<CImageLabel*> (aComponent))
		{
		UndoDrawPath();
		iUndo = ETrue;
		StoreParameters(EFalse, EFalse);
		iEditorView->HandleCommandL(EImageEditorCmdRender);
		iUndo = EFalse;
		}
	//save and quit
	else if (iLabels[3] == dynamic_cast<CImageLabel*> (aComponent))
		{
		iRedo = ETrue;
		RedoDrawPath();
		StoreParameters(EFalse, EFalse);
		iEditorView->HandleCommandL(EImageEditorCmdRender);
		iRedo = EFalse;
		}
	else if (iLabels[4] == dynamic_cast<CImageLabel*> (aComponent))
		{
		iPopupController->HideInfoPopupNote();
		StoreParameters(EFalse, ETrue);
		iEditorView->HandleCommandL(EImageEditorApplyPlugin);
		}
	else if (iLabels[5] == dynamic_cast<CImageLabel*> (aComponent))
		{
        iEditorView->HandleCommandL(EImageEditorMenuCmdHelp);
		}
	else if (iLabels[6] == dynamic_cast<CImageLabel*> (aComponent))
		{

		}
	else if (iCustomComponet == dynamic_cast<CCustomerComponet*> (aComponent))
		{
		if (iCustomComponet->GetButtonType())
			{
			iCustomComponetClick = ETrue;
			}

		if ((!iCustomComponet->GetButtonType()) && (!iLastPointEventType))
			{
			StartTheAnimation();

			}
		}

	}

// ---------------------------------------------------------------------------
// SetPositionOnImage
// ---------------------------------------------------------------------------
//    
void CImageEditorDrawControl::SetPositionOnImage(TPoint aPointedPosition)
	{
	ASSERT(iSysPars);
	// Get system parameters
	TRect visibleImageRect(iSysPars->VisibleImageRect());
	TRect visibleImageRectPrev(iSysPars->VisibleImageRectPrev());

	TInt xPosFactorDivider(visibleImageRectPrev.Width());
	TInt yPosFactorDivider(visibleImageRectPrev.Height());

		LOGFMT2(KDrawPluginLogFile, "CImageEditorDrawControl::vir w:%d h:%d",
				visibleImageRect.Width(), visibleImageRect.Height());
		LOGFMT2(KDrawPluginLogFile, "CImageEditorDrawControl::virp w:%d h:%d",
				visibleImageRectPrev.Width(), visibleImageRectPrev.Height());

	// Dividing by zero will cause panic -> check
	if (xPosFactorDivider == 0 || yPosFactorDivider == 0)
		{
		//	Set default position
		iX = (visibleImageRect.iTl.iX + visibleImageRect.iBr.iX) / 2;
		iY = (visibleImageRect.iTl.iY + visibleImageRect.iBr.iY) / 2;
		}
	else
		{
		// Calculate relative position on the screen
		TReal xPositionFactor(TReal(aPointedPosition.iX
				- visibleImageRectPrev.iTl.iX) / xPosFactorDivider);
		//
		//		TReal yPositionFactor(TReal(aPointedPosition.iY - iStatuspaneHeight
		//				- visibleImageRectPrev.iTl.iY) / yPosFactorDivider);
		TReal yPositionFactor(TReal(aPointedPosition.iY
				- visibleImageRectPrev.iTl.iY) / yPosFactorDivider);
			LOGFMT2(KDrawPluginLogFile, "CImageEditorDrawControl::pfx:%g pfy:%g",
					xPositionFactor, yPositionFactor);
		// Calculate position on visible image		

		iX = visibleImageRect.iTl.iX + visibleImageRect.Width()
				* xPositionFactor + 0.5;

		iY = visibleImageRect.iTl.iY + visibleImageRect.Height()
				* yPositionFactor + 0.5;
		}

		LOGFMT4(KDrawPluginLogFile,
				"CImageEditorDrawControl::Draw x:%d iX:%d y:%d iY:%d",
				aPointedPosition.iX, iX, aPointedPosition.iY, iY);
	// Check that not out of bounds    
	//ClipPosition();          
	}

// ---------------------------------------------------------------------------
// GetPositionOnView
// ---------------------------------------------------------------------------
// 
TPoint CImageEditorDrawControl::GetPositionOnView()
	{
	ASSERT(iSysPars);
	// Get system parameters 
	TRect visibleImageRect(iSysPars->VisibleImageRect());
	TRect visibleImageRectPrev(iSysPars->VisibleImageRectPrev());

	TInt xPosFactorDivider(visibleImageRectPrev.Width());
	TInt yPosFactorDivider(visibleImageRectPrev.Height());

	// Calculate relative position on the screen
	TReal xPositionFactor(TReal(iX + visibleImageRectPrev.iTl.iX)
			/ xPosFactorDivider);

	TReal yPositionFactor(TReal(iY + visibleImageRectPrev.iTl.iY)
			/ yPosFactorDivider);

		LOGFMT2(KDrawPluginLogFile,
				"CImageEditorDrawControl::GetPositionOnView xf:%g yf:%g",
				xPositionFactor, yPositionFactor);

		LOGFMT2(KDrawPluginLogFile,
				"CImageEditorDrawControl::GetPositionOnView xf:%d yf:%d",
				xPosFactorDivider, yPosFactorDivider);

		LOGFMT2(KDrawPluginLogFile,
				"CImageEditorDrawControl::GetPositionOnView w:%d h:%d", iX, iY);

	TInt xPosition(iX + visibleImageRectPrev.iTl.iX);
	TInt yPosition(iY + visibleImageRectPrev.iTl.iY);

		LOGFMT3(KDrawPluginLogFile,
				"CImageEditorDrawControl::GetPositionOnView w:%d h:%d S:%g",
				xPosition, yPosition, iSysPars->RelScale());

	return TPoint(xPosition, yPosition);
	}

// ---------------------------------------------------------------------------
// ShowTooltip
// ---------------------------------------------------------------------------
//    
void CImageEditorDrawControl::ShowTooltip()
	{
	iPopupController->HideInfoPopupNote();

	TPoint iconPosition = ComputeIndicatorPosition();
	TRect iconRect(iconPosition.iX, iconPosition.iY, iconPosition.iX,
			iconPosition.iY);
	}

// ---------------------------------------------------------------------------
// ScaledLineSize
// ---------------------------------------------------------------------------
//    
TSize CImageEditorDrawControl::ScaledLineSize(TSize aOriginalSize) const
	{
	TInt sizew(aOriginalSize.iWidth * iSysPars->RelScale() + 0.5);
	TInt sizeh(aOriginalSize.iHeight * iSysPars->RelScale() + 0.5);
	if (sizew < 1)
		{
		sizew++;
		}
	if (sizeh < 1)
		{
		sizeh++;
		}

		LOGFMT3(KDrawPluginLogFile,
				"CImageEditorDrawControl::ScaledLineSize w:%d h:%d S:%g", sizew,
				sizeh, iSysPars->RelScale());
	return TSize(sizew, sizeh);
	}

// ---------------------------------------------------------------------------
// CountComponentControls
// ---------------------------------------------------------------------------
//  
TInt CImageEditorDrawControl::CountComponentControls() const
	{
	return iTotalComponent + 1;
	}

// ---------------------------------------------------------------------------
// ComponentControl
// ---------------------------------------------------------------------------
//  
CCoeControl* CImageEditorDrawControl::ComponentControl(TInt aIndex) const
	{
	switch (aIndex)
		{
		case 7:
			return iCustomComponet;
		case 0:
			return iLabels[0];
		case 1:
			return iLabels[1];
		case 2:
			return iLabels[2];
		case 3:
			return iLabels[3];
		case 4:
			return iLabels[4];
		case 5:
			return iLabels[5];
		case 6:
			return iLabels[6];
		default:
			return NULL;
		}
	return NULL;
	}

// ---------------------------------------------------------------------------
// Notify
// ---------------------------------------------------------------------------
//  
void CImageEditorDrawControl::Notify()
	{
	if (iStartSpeed > 0)
		{
		iStartSpeed -= 2;
		if (iStartSpeed <= 0)
			{
			iStartSpeed = 0;
			iDragIsStart = EFalse;
			iAnimationAO->Cancel();
			}
		}
	else if (iStartSpeed < 0)
		{
		iStartSpeed += 2;
		if (iStartSpeed >= 0)
			{
			iStartSpeed = 0;
			iDragIsStart = EFalse;
			iAnimationAO->Cancel();
			}
		}
	else
		{
		iDragIsStart = EFalse;
		iAnimationAO->Cancel();
		}

	iDragOffset += iStartSpeed;
	CalculateEffectiveOffset();
	SizeChanged();
	}

// ---------------------------------------------------------------------------
// Roll 
// ---------------------------------------------------------------------------
//  
void CImageEditorDrawControl::Roll()
	{
	SetToolBarStatus();

	TInt offset = 0;
	iRollflag = ETrue;
	if (iDisplayTool)
		iRollDirection = EFalse;
	else
		iRollDirection = ETrue;

	if (iRollDirection)
		{
		offset = 4;
		}
	else
		{
		offset = -4;
		}

	if (iMainMenuLayout == EMainMenuVertical)
		{
		for (TInt i = 0; i < iLabels.Count(); i++)
			{
			TInt x = iLabels[i]->Rect().iTl.iX;
			iLabels[i]->SetPosition(TPoint(x + offset, iLoopMenuRect.iTl.iY));
			iLabels[i]->ResetControl();
			}
		if (iLabels[0]->Rect().iTl.iX == 0
				|| iLabels[iLabels.Count() - 1]->Rect().iBr.iX == 0)
			iRollflag = EFalse;
		}
	else if (iMainMenuLayout == EMainMenuHorizontal)
		{
		for (TInt i = 0; i < iLabels.Count(); i++)
			{
			TInt y = iLabels[i]->Rect().iTl.iY;
			iLabels[i]->SetPosition(TPoint(iLoopMenuRect.iTl.iX, y + offset));
			iLabels[i]->ResetControl();
			}
		if (iLabels[0]->Rect().iTl.iY == 0
				|| iLabels[iLabels.Count() - 1]->Rect().iBr.iY == 0)
			iRollflag = EFalse;
		}

	DrawNow();

	if (!iRollflag)
		{
		iRollAo->Cancel();
		}
	}

// ---------------------------------------------------------------------------
// CalculateEffectiveOffset
// ---------------------------------------------------------------------------
//  
void CImageEditorDrawControl::CalculateEffectiveOffset()
	{
	if (iMainMenuLayout == EMainMenuVertical)
		{
		TInt leftX = iLabels[0]->Rect().iTl.iX;
		TInt rightX = iLabels[iTotalComponent - 1]->Rect().iBr.iX;
		TInt offset = iDragOffset;
		if (leftX + offset > 0)
			{
			iDragOffset = Abs(leftX);
			}

		if (rightX + offset < 360)
			{
			iDragOffset = -(rightX - 360);
			}
		}
	else if (iMainMenuLayout ==EMainMenuHorizontal )
		{
		TInt topY = iLabels[0]->Rect().iTl.iY;
		TInt bottomY = iLabels[iTotalComponent - 1]->Rect().iBr.iY;
		TInt offset = iDragOffset;
		if (topY + offset > 0)
			{
			iDragOffset = Abs(topY);
			}

		if (bottomY + offset < 360)
			{
			iDragOffset = -(bottomY - 360);
			}
		}
	}

// ---------------------------------------------------------------------------
// MakeVisibleLable
// ---------------------------------------------------------------------------
//  
void CImageEditorDrawControl::MakeVisibleLable(TBool aFlag)
	{
	if (aFlag)
		{
		for (int i = 0; i < iTotalComponent; i++)
			{
			iLabels[i]->MakeVisible(ETrue);
			}
		}
	else
		{
		for (int i = 0; i < iTotalComponent; i++)
			{
			iLabels[i]->MakeVisible(EFalse);
			}
		}
	}

// ---------------------------------------------------------------------------
// StartTheAnimation
// ---------------------------------------------------------------------------
//  
void CImageEditorDrawControl::StartTheAnimation()
	{
	iRollAo->StartL(0);
	}

// End of File
