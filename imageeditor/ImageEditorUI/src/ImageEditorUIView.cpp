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
 * Image Editor's view class that handles all view related issues.
 *
 */
// INCLUDE FILES
#include <eikmenub.h>
#include <eikbtgpc.h>
#include <bautils.h>
#include <avkon.hrh>
#include <aknviewappui.h>
#include <aknconsts.h>
#include <akntitle.h> 
#include <aknnavi.h>
#include <aknnavide.h> 
#include <aknglobalnote.h> 
#include <sendui.h>
#include <aknpopup.h> 
#include <CMessageData.h>
#include <AknInfoPopupNoteController.h>
#include <Avkon.rsg>
#include <apgcli.h>
#include <apadef.h>
#include <apaid.h>
#include <StringLoader.h>
#include <pathinfo.h> 
#include <ImageEditorUI.rsg>
#include "ImageEditorUI.hrh"
#include "ImageEditorUIPanics.h"
#include "ImageEditorUIView.h"
#include "ImageEditorUIContainer.h" 
#include "ImageEditorUtils.h"
#include "DrawUtils.h" 
#include "PluginInfo.h" 
#include "ResolutionUtil.h"
#include "ImageEditorControlBase.h" 
#include "SingleParamControl.h" 
#include "imageeditordebugutils.h"
#include "plugingrid.h"	
#include "text.rsg"
#ifdef AIW_PRINT_PROVIDER_USED
#include <AiwServiceHandler.h>
#include <AiwMenu.h>
#include <AiwCommon.hrh>
#endif
// CONSTANTS
_LIT (KResourceDir, "\\resource\\apps\\");
_LIT (KResourceFile, "imageeditorui.rsc");
_LIT (KComponentName, "ImageEditorUI");

#define KMediaGalleryUID3           0x101F8599 

// MACRO DEFINITIONS
#ifdef LANDSCAPE_SUPPORT
#define LANDSCAPE_ARG(x) x
#else
#define LANDSCAPE_ARG(x)
#endif

//=============================================================================
EXPORT_C void CImageEditorUIView::ConstructL()
	{
	LOG(KImageEditorLogFile, "CImageEditorUIView::ConstructL()");

	//	Read resource	
	TFileName resourcefile;
	resourcefile.Append(KResourceDir);
	resourcefile.Append(KResourceFile);

	User::LeaveIfError(CompleteWithAppPath(resourcefile));
	// Implementation of RConeResourceLoader uses BaflUtils::NearestLanguageFile 
	// to search for a localised resource in proper search order
	iResLoader.OpenL(resourcefile);

	//	Construct base view
	BaseConstructL(R_IMAGEEDITORUI_VIEW);

	//  Get reference to status pane 
	CEikStatusPane * sp = AppUi()->StatusPane();

	//  Get reference to navi pane
	iNaviPane = (CAknNavigationControlContainer *) sp->ControlL(TUid::Uid(
			EEikStatusPaneUidNavi));

	//  Get reference to title pane
	iTitlePane = (CAknTitlePane *) sp->ControlL(TUid::Uid(
			EEikStatusPaneUidTitle));

	// Initialize soft key ID:s (needs to be changed if cba resource in 
	// R_IMAGEEDITORUI_VIEW changes)
	iSoftkey1 = EAknSoftkeyEmpty;
	iSoftkey2 = EAknSoftkeyEmpty;
	iMSK = EAknSoftkeyEmpty;

	iInPlugin = EFalse;

	iPopupController = CAknInfoPopupNoteController::NewL();

	// Read strings needed for zoom popup
	HBufC8 * res8 =	CEikonEnv::Static()->AllocReadResourceAsDes8LC(R_ZOOM_TEXTS);
	TResourceReader resource;
	//Set resource reader buffer
	resource.SetBuffer(res8);

	//Read zoom text count
	TInt16 arraycount = (TInt16) resource.ReadInt16();

	// There have to be as many zoom texts as there are zoom states
	__ASSERT_ALWAYS( arraycount == ENumOfZooms, User::Panic(KComponentName,
					EImageEditorPanicZoomTextCountDoesntMatch) );

	if (arraycount > 0)
		{
		//Construct a new descriptor array
		if (!iZoomTexts)
			{
			iZoomTexts = new (ELeave) CDesCArraySeg(16);
			}

		//Read text to array
		for (TInt i = 0; i < arraycount; ++i)
			{
			//Construct a flat descriptor array and read all the texts
			HBufC * parameter = resource.ReadHBufCL();
			CleanupStack::PushL(parameter);
			TPtrC ptr = parameter->Des();
			iZoomTexts->AppendL(ptr);
			CleanupStack::PopAndDestroy(); // parameter
			}
		}
	CleanupStack::PopAndDestroy(); // res8


#ifdef AIW_PRINT_PROVIDER_USED
	iServiceHandler = CAiwServiceHandler::NewL();
	// Attach menu services.
	iServiceHandler->AttachMenuL(R_IMAGEEDITORUI_MENUPANE,
			R_IMAGE_EDITOR_INTEREST);
#endif   
	LOG(KImageEditorLogFile, "CImageEditorUIView: Constructred succesfully");
	}

//=============================================================================
EXPORT_C CImageEditorUIView::CImageEditorUIView() :
	CAknView(), iResLoader(*iEikonEnv), iBusy(ETrue), iContainerInStack(EFalse)
	{
	}

//=============================================================================
EXPORT_C CImageEditorUIView::~CImageEditorUIView()
	{
	LOG(KImageEditorLogFile, "CImageEditorUIView::~CImageEditorUIView()");

	iResLoader.Close();

#ifdef AIW_PRINT_PROVIDER_USED
	delete iServiceHandler;
#endif

	if (iNaviPane && iNaviDecorator)
		{
		iNaviPane->Pop(iNaviDecorator);
		delete iNaviDecorator;
		iNaviDecorator = 0;
		}
	iNaviDecorator = 0;

	if (iNaviPane && iPreviousNaviDecorator)
		{
		iNaviPane->Pop(iPreviousNaviDecorator);
		delete iPreviousNaviDecorator;
		iPreviousNaviDecorator = 0;
		}
	iPreviousNaviDecorator = 0;

	RemoveContainerFromStack();
	if (iContainer)
		{
		delete iContainer;
		iContainer = 0;
		}

	iArray.Reset();

	iNaviPane = NULL;
	iTitlePane = NULL;
	iControl = NULL;
	iPreview = NULL;
	iSendAppUi = NULL;

	if (iPopupController)
		{
		delete iPopupController;
		}

	if (iZoomTexts)
		{
		iZoomTexts->Reset();
		delete iZoomTexts;
		}

	LOG(KImageEditorLogFile, "CImageEditorUIView: View deleted");
	}

//=============================================================================
EXPORT_C TUid CImageEditorUIView::Id() const
	{
	return KViewId;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::UpdateLayoutL(TInt LANDSCAPE_ARG(aScreenMode)) const
	{
	CEikStatusPane* sp = StatusPane();
	__ASSERT_ALWAYS( sp, User::Panic(KComponentName, EImageEditorPanicStatusPaneNotAccessible) );

#ifdef LANDSCAPE_SUPPORT

	TPixelsAndRotation rotation;
	iEikonEnv->ScreenDevice()->GetScreenModeSizeAndRotation(aScreenMode,rotation);

	LOGFMT2(KImageEditorLogFile, "CImageEditorUIView: View mode: ScreenMode: %d, Rotation: %d", aScreenMode, rotation.iRotation);

	switch (rotation.iRotation)
		{
		case CFbsBitGc::EGraphicsOrientationRotated270: // view mode
			{
			LOG(KImageEditorLogFile, "CImageEditorUIView: EGraphicsOrientationRotated270");

#ifndef SWAP_SOFTKEY_POSITIONS		
			sp->SwitchLayoutL(R_AVKON_STACON_PANE_LAYOUT_USUAL_SOFTKEYS_RIGHT);
#else								
			sp->SwitchLayoutL(R_AVKON_STACON_PANE_LAYOUT_USUAL_SOFTKEYS_LEFT);
#endif
			break;
			}

		case CFbsBitGc::EGraphicsOrientationRotated90: // camcorder mode
			{
			LOG(KImageEditorLogFile, "CImageEditorUIView: EGraphicsOrientationRotated90");

#ifndef SWAP_SOFTKEY_POSITIONS		
			sp->SwitchLayoutL(R_AVKON_STACON_PANE_LAYOUT_USUAL_SOFTKEYS_LEFT);
#else						
			sp->SwitchLayoutL(R_AVKON_STACON_PANE_LAYOUT_USUAL_SOFTKEYS_RIGHT);
#endif
			break;
			}

		case CFbsBitGc::EGraphicsOrientationNormal:
		default:
			{
			if (CResolutionUtil::Self()->GetLandscape())
				{
				sp->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL /*R_AVKON_WIDESCREEN_PANE_LAYOUT_USUAL_FLAT*/);
				}
			else
				{
				sp->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
				}
			break;
			}
		}

#endif  

	if (iFullScreen)
		{
		//  Hide status pane
		sp->MakeVisible(EFalse);
		}
	else
		{
		//  Make status pane visible
		sp->MakeVisible(ETrue);

#ifdef LANDSCAPE_SUPPORT

		// And now the CBA...
		TRect rect = ClientRect();

		CEikButtonGroupContainer* cba = Cba();
		__ASSERT_ALWAYS( cba, User::Panic(KComponentName, EImageEditorPanicCbaNotAccessible) );

		MEikButtonGroup* bgrp = cba->ButtonGroup();
		STATIC_CAST( CEikCba*, bgrp)->SetBoundingRect(rect);
		cba->HandleResourceChange(KEikDynamicLayoutVariantSwitch);

#endif
		}
	}

//=============================================================================
EXPORT_C TBool CImageEditorUIView::ViewScreenModeCompatible(TInt /*aScreenMode*/)
	{
	// Supports any screen size
	return ETrue;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::HandleCommandL(TInt aCommand)
	{

	// If focus is gained or lost
	if ((aCommand == EImageEditorFocusGained) || (aCommand
			== EImageEditorFocusLost))
		{
		if (iContainer && iControl)
			{
			((CImageEditorControlBase *) iControl)->HandlePluginCommandL(aCommand);
			}
		}
	//  If soft key 1 pressed
	else if (iInPlugin && (aCommand == iSoftkey1) && (aCommand
			!= EAknSoftkeyOptions))
		{
		if (iContainer && iContainer->Busy())
			{
			return;
			}
		else
			{
			((CImageEditorControlBase *) iControl)->HandlePluginCommandL(aCommand);
			}
		}

	//  If soft key 2 pressed
	else if (iInPlugin && (aCommand == iSoftkey2))
		{
		if (iContainer && iContainer->Busy())
			{
			return;
			}
		else
			{
			((CImageEditorControlBase *) iControl)->HandlePluginCommandL(
					aCommand);
			}
		}

	//  If msk pressed
	else if (iInPlugin && (aCommand == iMSK))
		{
		if (iContainer && iContainer->Busy())
			{
			return;
			}
		else
			{
			((CImageEditorControlBase *) iControl)->HandlePluginCommandL(aCommand);
			}
		}

	//  Menu plug-in item selected
	else if (iInPlugin && MenuItemPressedL(aCommand))
		{
		((CImageEditorControlBase *) iControl)->HandlePluginCommandL(aCommand);
		}

	else
		{
		switch (aCommand)
			{
			case EImageEditorPreGlobalZoomChange:
			case EImageEditorGlobalZoomChanged:
			case EImageEditorGlobalCropChanged:
			case EImageEditorPreGlobalPanChange:
			case EImageEditorGlobalPanChanged:
			case EImageEditorGlobalRotationChanged:
			case EImageEditorPreScreenModeChange:
			case EImageEditorPostScreenModeChange:
				{
				if (iInPlugin)
					{
					((CImageEditorControlBase *) iControl)->HandlePluginCommandL(aCommand);
					}
				return;
				}
			case EAknSoftkeyBack:
				{
				if (iContainer && iContainer->Busy())
					{
					return;
					}
				else
					{
					AppUi()->HandleCommandL(EImageEditorSoftkeyCmdBack);
					break;
					}
				}
			case EImageEditorUpdateSoftkeys:
				{
				if (iInPlugin)
					{
					UpdateSoftkeysL();
					}
				break;
				}
			case EImageEditorUpdateNavipane:
				{
				if (iInPlugin)
					{
					UpdateNaviPaneL();
					}
				break;
				}
			case EImageEditorTryDisplayMenuBar:
				{
				CEikMenuBar* menu = MenuBar();
				__ASSERT_ALWAYS( menu, User::Panic(KComponentName, EImageEditorPanicMenuNotAccessible) );
				menu->TryDisplayMenuBarL();
				break;
				}
			case EImageEditorDoPrint:
				{
#ifdef AIW_PRINT_PROVIDER_USED

				CAiwGenericParamList& in = iServiceHandler->InParamListL();

				TFileName filename(iImageFileName);
				TAiwVariant variant(filename);
				TAiwGenericParam param(EGenericParamFile, variant);

				in.AppendL(param);

				iServiceHandler->ExecuteMenuCmdL(iSelectedPrintMenuCmd, in,
						iServiceHandler->OutParamListL());
#endif       
				break;
				}
			default:
				{
				// Print and save
				if (aCommand >= EImageEditorMenuAiwPrintBase && aCommand
						<= EImageEditorMenuAiwPrintLast)
					{
					iSelectedPrintMenuCmd = aCommand;
					AppUi()->HandleCommandL(EImageEditorSaveAndPrint);
					}
				else
					{
					AppUi()->HandleCommandL(aCommand);
					}
				break;
				}
			}
		}
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::HandleClientRectChange()
	{
	LOG(KImageEditorLogFile, "CImageEditorUIView: Client rect changed");

	if (iContainer)
		{
		iContainer->SetRect(ClientRect());
		}
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetImageL(CFbsBitmap * aBitmap)
	{
	iPreview = aBitmap;
	if (iContainer)
		{
		iContainer->SetImageL(aBitmap);
		}
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetImageFileName(const TDesC& aFileName)
	{
	iImageFileName.Copy(aFileName);
	}

//=============================================================================
EXPORT_C CCoeControl * CImageEditorUIView::GetContainer() const
	{
	return iContainer;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::ActivatePluginL(CCoeControl * aControl)
	{
	LOG(KImageEditorLogFile, "CImageEditorUIView: Activating plugin");

	// Activate the plugin based on UI type
	switch (iArray[iIndex]->PluginUiType())
		{
		case 1: // EPluginUiTypeNone
			{
			// For non-UI plug-ins active main view and render
			iInPlugin = EFalse;
			ActivateMainViewL();
			HandleCommandL(EImageEditorCmdRender);
			break;

			}
		case 2: // EPluginUiTypeSingleParam
		case 3: // EPluginUiTypeMultiParam
		case 4: // EPluginUiTypeCustomized
			{
			iInPlugin = ETrue;

			//  Set control
			iControl = aControl;
			if (iContainer)
				{
				iContainer->SetControl(iControl);
				}

			//  Set plug-in item
			if (iControl)
				{
				LOG(KImageEditorLogFile, "CImageEditorUIView: Activating plugin: calling SetSelectedUiItemL");

				((CImageEditorControlBase *) iControl)->SetSelectedUiItemL(	iArray[iIndex]);
				}

			//  Prepare the plug-in item
			if (iControl)
				{
				LOG(KImageEditorLogFile, "CImageEditorUIView: Activating plugin: calling ");

				((CImageEditorControlBase *) iControl)->PrepareL();
				}

			//  Update soft keys
			UpdateSoftkeysL();

			//  Update navigation pane
			UpdateNaviPaneL();

			break;
			}
		default:
			{
			iInPlugin = ETrue;

			//  Set control
			iControl = aControl;
			if (iContainer)
				{
				iContainer->SetControl(iControl);
				}
			break;
			}
		}
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::ActivateMainViewL()
	{

	LOG(KImageEditorLogFile, "CImageEditorUIView: Activating main view");

	iInPlugin = EFalse;

	//  Reset buttons
	iSoftkey1 = EAknSoftkeyOptions;
	iSoftkey2 = EAknSoftkeyBack;
	iMSK = EImageEditorMenuCmdApplyEffect;

	if (InZoomingState())
		{
		if (iZoomMode == (ENumOfZooms - 1))
			{
			SetCustomButtonsL(R_IMAGEEDITORUI_SK_OPTIONS_CANCEL);
			}
		else
			{
			SetCustomButtonsL(R_IMAGEEDITORUI_SK_OPTIONS_CANCEL_ZOOMIN);
			}
		}
	else
		{
		SetCustomButtonsL(R_IMAGEEDITORUI_SK_OPTIONS_BACK_APPLY);
		}

	//  Reset navi pane
	ClearNaviPaneTextL();

	//  Set control in container to NULL
	iControl = 0;
	if (iContainer)
		{
		iContainer->SetControl(iControl);
		}

	}

//=============================================================================
EXPORT_C CPluginInfo * CImageEditorUIView::GetSelectedPluginInfoL()
	{
	TInt selectedItem;
	TInt dialog_ok = CPluginSelectionDialog::RunDlgLD(&iPreview, selectedItem,
			&iArray, ClientRect());

	if (dialog_ok && selectedItem != KErrCancel)
		{
		iIndex = selectedItem;
		return iArray[selectedItem];
		}
	else
		{
		if (InZoomingState())
			{
			if (iZoomMode == (ENumOfZooms - 1))
				{
				SetCustomButtonsL(R_IMAGEEDITORUI_SK_OPTIONS_CANCEL);
				}
			else
				{
				SetCustomButtonsL(R_IMAGEEDITORUI_SK_OPTIONS_CANCEL_ZOOMIN);
				}
			}
		else
			{
			SetCustomButtonsL(R_IMAGEEDITORUI_SK_OPTIONS_BACK_APPLY);
			}
		return NULL;
		}
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::AddPluginUiItemL(
		const CPluginInfo * aPluginInfo)
	{
	TLinearOrder<CPluginInfo> order(CPluginInfo::ComparePluginOrder);
	iArray.InsertInOrder(aPluginInfo, order);
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetCustomButtonsL(TInt aResourceID) const
	{
	LOG(KImageEditorLogFile, "CImageEditorUIView: Setting buttons");

	CEikButtonGroupContainer* cba = Cba();
	__ASSERT_ALWAYS( cba, User::Panic(KComponentName, EImageEditorPanicCbaNotAccessible) );

	cba->SetCommandSetL(aResourceID);
	cba->DrawDeferred();
	}

//=============================================================================
EXPORT_C TInt CImageEditorUIView::LaunchSaveChangesQueryL() const
	{
	//	Create dialog prompt
	HBufC * prompt = iEikonEnv->AllocReadResourceLC(
			R_LIST_QUERY_SAVE_CONFIRMATION);

	//	Execute query dialog
	TInt ret = SDrawUtils::LaunchQueryDialogL(*prompt);

	CleanupStack::PopAndDestroy(); // prompt

	return ret;
	}

//=============================================================================
EXPORT_C TInt CImageEditorUIView::LaunchExitWithoutSavingQueryL() const
	{
	//	Create dialog prompt
	HBufC * prompt = iEikonEnv->AllocReadResourceLC(
			R_LIST_QUERY_EXIT_WITHOUT_SAVING_CONFIRMATION);

	//	Execute query dialog
	TInt ret = SDrawUtils::LaunchQueryDialogL(*prompt);

	CleanupStack::PopAndDestroy(); // prompt

	return ret;
	}

//=============================================================================
EXPORT_C TInt CImageEditorUIView::LaunchCancelTextInputQueryL() const
	{

	// Create dialog prompt
	// At the moment query is not in use and corresponding string is removed
	// from the loc file. To enable this, a new string has to be defined. 
	HBufC * prompt = iEikonEnv->AllocReadResourceLC(
			R_LIST_QUERY_CANCEL_TEXT_INPUT);

	//	Execute query dialog
	TInt ret = SDrawUtils::LaunchQueryDialogOkOnlyL(*prompt);

	CleanupStack::PopAndDestroy(); // save_conf, prompt

	return ret;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::LaunchSendQueryL(
		const TSendingCapabilities& aCaps, TFileName& aFileName) const
	{
	if (iSendAppUi)
		{
		CMessageData* msgData = CMessageData::NewLC();
		msgData->AppendAttachmentL(aFileName);

		iSendAppUi->ShowQueryAndSendL(msgData, aCaps);

		CleanupStack::PopAndDestroy(msgData);
		}
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::LaunchSendQueryL(
		const TSendingCapabilities& aCaps, RFile& aFileHandle) const
	{
	if (iSendAppUi)
		{
		CMessageData* msgData = CMessageData::NewLC();
		msgData->AppendAttachmentHandleL(aFileHandle);

		iSendAppUi->ShowQueryAndSendL(msgData, aCaps);

		CleanupStack::PopAndDestroy(msgData);
		}
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::LaunchSaveWaitDialogL(
		CAknProgressDialog** aSelfPtr, TInt aFinalValue,
		MProgressDialogCallback* aCallback) const
	{
	//	Create dialog prompt
	HBufC * prompt;

	TApaAppCaption caption;
	TRAPD( err, ResolveCaptionNameL( caption ) );

	// If something goes wrong, show basic "Saving" note
	if (err)
		{
		prompt = iEikonEnv->AllocReadResourceLC(R_WAIT_NOTE_SAVING);
		}
	else
		{
		prompt = StringLoader::LoadLC(R_WAIT_NOTE_SAVING_TO, caption);
		}

	//	Launch wait dialog with Cancel softkey
	SDrawUtils::LaunchProgressNoteL(aSelfPtr, R_PROGRESS_DIALOG, *prompt,
			aFinalValue, aCallback);

	CleanupStack::PopAndDestroy(); // prompt
	}

//=============================================================================
void CImageEditorUIView::ResolveCaptionNameL(TApaAppCaption& aCaption) const
	{
	RApaLsSession appArcSession;
	CleanupClosePushL(appArcSession);
	User::LeaveIfError(appArcSession.Connect());

	// Get Media Gallery caption
	TApaAppInfo appInfo;
	User::LeaveIfError(appArcSession.GetAppInfo(appInfo, TUid::Uid(
			KMediaGalleryUID3)));

	aCaption = appInfo.iCaption;

	CleanupStack::PopAndDestroy(&appArcSession);
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::LaunchLoadWaitDialogL(
		CAknWaitDialog** aSelfPtr, MProgressDialogCallback* aCallback) const
	{
	//	Create dialog prompt
	HBufC * prompt = iEikonEnv->AllocReadResourceLC(R_WAIT_NOTE_LOADING);

	//	Launch wait dialog with empty softkeys
	SDrawUtils::LaunchWaitNoteL(aSelfPtr, R_WAIT_DIALOG, *prompt, aCallback);

	CleanupStack::PopAndDestroy(); // prompt
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::LaunchWaitDialog() const
	{
	// Show empty set of cba buttons while animation on the screen
	SetCustomButtonsL(R_AVKON_SOFTKEYS_EMPTY);
	iContainer->ShowWaitNote();
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::HideWaitDialog() const
	{
	iContainer->HideWaitNote();
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::ShowInformationNoteL(const TDesC& aNoteText) const
	{

	CAknGlobalNote * globalnote = CAknGlobalNote::NewLC();
	globalnote->ShowNoteL(EAknGlobalInformationNote, aNoteText);

	CleanupStack::PopAndDestroy(); // globalnote

	}

//=============================================================================
EXPORT_C void CImageEditorUIView::ShowConfirmationNoteL(const TDesC& aNoteText) const
	{

	CAknGlobalNote * globalnote = CAknGlobalNote::NewLC();
	globalnote->ShowNoteL(EAknGlobalConfirmationNote, aNoteText);

	CleanupStack::PopAndDestroy(); // globalnote

	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetNaviPaneTextL(const TDesC & aText,
		TBool aLeftNaviPaneScrollButtonVisibile,
		TBool aRightNaviPaneScrollButtonVisible)
	{
	LOG(KImageEditorLogFile, "CImageEditorUIView: Setting navi pane text");

	if (iNaviPane)
		{
		//  Create a new decorator
		CAknNavigationDecorator* newNaviDecorator =
				iNaviPane->CreateNavigationLabelL(aText);
		CleanupStack::PushL(newNaviDecorator);

		// Set the navi scroll indicators for the new decorator
		newNaviDecorator->SetScrollButtonDimmed(
				CAknNavigationDecorator::ELeftButton,
				!aLeftNaviPaneScrollButtonVisibile);
		newNaviDecorator->SetScrollButtonDimmed(
				CAknNavigationDecorator::ERightButton,
				!aRightNaviPaneScrollButtonVisible);
		if (aLeftNaviPaneScrollButtonVisibile
				|| aRightNaviPaneScrollButtonVisible)
			{
			newNaviDecorator->MakeScrollButtonVisible(ETrue);
			}

		// Set new decorator on top of Navi Pane's decorator stack 
		// (topmost decorator is the one to show)
		iNaviPane->PushL(*newNaviDecorator);

		// remove old "previous decorator" from decorator stack and delete it
		if (iPreviousNaviDecorator)
			{
			iNaviPane->Pop(iPreviousNaviDecorator);
			delete iPreviousNaviDecorator;
			iPreviousNaviDecorator = NULL;
			}

		// update decorator pointers to new ones    
		iPreviousNaviDecorator = iNaviDecorator;
		iNaviDecorator = newNaviDecorator;

		// set observer for the currently shown decorator
		(iNaviPane->Top())->SetNaviDecoratorObserver(iContainer);

		CleanupStack::Pop(newNaviDecorator);
		}

	}

//=============================================================================
EXPORT_C void CImageEditorUIView::ClearNaviPaneTextL()
	{
	LOG(KImageEditorLogFile, "CImageEditorUIView: Clearing navi pane text");
	if (iNaviPane)
		{
		if (iNaviDecorator)
			{
			iNaviPane->Pop(iNaviDecorator);
			delete iNaviDecorator;
			iNaviDecorator = 0;
			}

		if (iPreviousNaviDecorator)
			{
			iNaviPane->Pop(iPreviousNaviDecorator);
			delete iPreviousNaviDecorator;
			iPreviousNaviDecorator = 0;
			}
		HBufC * title = iEikonEnv->AllocReadResourceLC(R_MAIN_VIEW_TITLE);
		iNaviDecorator = iNaviPane->CreateNavigationLabelL(title->Des());
		iPreviousNaviDecorator
				= iNaviPane->CreateNavigationLabelL(title->Des());

		iNaviPane->PushL(*iPreviousNaviDecorator);
		iNaviPane->PushL(*iNaviDecorator);
		iNaviPane->DrawDeferred();
		iNaviDecorator->DrawDeferred();
		CleanupStack::PopAndDestroy(); // title
		}
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetTitlePaneTextL(const TDesC & aText)
	{
	iTitlePane->SetTextL(aText);
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::ClearTitlePaneTextL()
	{
	iTitlePane->SetTextL(KNullDesC);
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetSoftKey1L(const TDesC & aText,
		const TInt aCommand)
	{
	CEikButtonGroupContainer * cba = Cba();
	// TInt replace_position = cba->PositionById (iSoftkey1);
	TInt replace_position(CEikButtonGroupContainer::ELeftSoftkeyPosition);
	cba->SetCommandL(replace_position, aCommand, aText);
	cba->DrawDeferred();
	iSoftkey1 = aCommand;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetSoftKey2L(const TDesC & aText,
		const TInt aCommand)
	{
	CEikButtonGroupContainer * cba = Cba();
	//TInt replace_position = cba->PositionById (iSoftkey2); 
	TInt replace_position(CEikButtonGroupContainer::ERightSoftkeyPosition);
	cba->SetCommandL(replace_position, aCommand, aText);
	cba->DrawDeferred();
	iSoftkey2 = aCommand;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetMiddleSoftKeyL(const TDesC & aText,
		const TInt aCommand)
	{
	CEikButtonGroupContainer * cba = Cba();

	if (aCommand == EAknSoftkeyContextOptions)
		{
		TInt
				contextMenuId =
						((CImageEditorControlBase *) iControl)->GetContextMenuResourceId();
		if (contextMenuId)
			{
		//if in doodle don't show select dialog.
			if (contextMenuId != R_TEXT_CONTEXT_MENUBAR)
				{
				MenuBar()->SetContextMenuTitleResourceId(contextMenuId);
				}
			}
		}

	TInt replace_position(CEikButtonGroupContainer::EMiddleSoftkeyPosition);
	cba->SetCommandL(replace_position, aCommand, aText);
	cba->DrawDeferred();
	iMSK = aCommand;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetBusy()
	{

	if (iContainer)
		{
		iContainer->SetBusy();
		}

	iBusy = ETrue;
	RemoveContainerFromStack();

	}

//=============================================================================
EXPORT_C void CImageEditorUIView::ResetBusy()
	{

	AddContainerToStack();
	iBusy = EFalse;

	if (iContainer)
		{
		iContainer->ResetBusy();
		}
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetUndoFlag(const TBool aUndo)
	{
	iCanUndo = aUndo;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetSaveFlag(const TBool aSave)
	{
	iCanSave = aSave;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetFullScreen()
	{
	if (iContainer)
		{
		iContainer->SetFullScreen();
		}
	iFullScreen = ETrue;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::ResetFullScreen()
	{
	if (iContainer)
		{
		iContainer->ResetFullScreen();
		}
	iFullScreen = EFalse;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetZoomModeL(const TZoomMode& aMode)
	{
	iZoomMode = aMode;

	HandleZoomModeChangeL();
	}

//=============================================================================
void CImageEditorUIView::HandleZoomModeChangeL()
	{

	if (!iInPlugin && iZoomMode != EZoomNormal)
		{
		if (iZoomMode != (ENumOfZooms - 1))
			{
			SetCustomButtonsL(R_IMAGEEDITORUI_SK_OPTIONS_CANCEL_ZOOMIN);
			}
		else
			{
			SetCustomButtonsL(R_IMAGEEDITORUI_SK_OPTIONS_CANCEL);
			}
		}
	else if (!iInPlugin && iZoomMode == EZoomNormal)
		{

		SetCustomButtonsL(R_IMAGEEDITORUI_SK_OPTIONS_BACK_APPLY);
		}

	// hide previous tooltip
	iPopupController->HideInfoPopupNote();

	// show tooltip (also if in plugin)
	if (iZoomMode != EZoomNormal)
		{

		SDrawUtils::ShowToolTip(iPopupController, (CCoeControl*) iContainer,
				TPoint(iContainer->Rect().iTl.iX, iContainer->Rect().iTl.iY),
				EHRightVTop, iZoomTexts->MdcaPoint(iZoomMode));
		}

	}

//=============================================================================
EXPORT_C void CImageEditorUIView::DynInitMenuPaneL(TInt aResourceId,
		CEikMenuPane * aMenuPane)
	{
	// This is called by the framework, aMenuPane should never be null pointer...
	__ASSERT_ALWAYS( aMenuPane, User::Panic(KComponentName, EImageEditorPanicMenuNotAccessible) );

#ifdef AIW_PRINT_PROVIDER_USED
	if (iServiceHandler->HandleSubmenuL(*aMenuPane))
		{
		// Return if AIW submenu 
		return;
		}
#endif    

	if (aResourceId != R_IMAGEEDITORUI_MENUPANE)
		{
		return;
		}

	// In busy mode
	if (iBusy)
		{
		DimAllL(aResourceId, *aMenuPane);
		}
	// In idle mode
	else
		{
		//  In plug-in mode
		if (iInPlugin)
			{
			DimAllL(aResourceId, *aMenuPane);
			InsertPluginItemsL(*aMenuPane);

#ifdef FULLSCREEN_AVAILABLE            
			// After plugin specific items, add menu commands that are common
			// for all plugins' options menus
			// In normal screen mode
			if ( !iFullScreen )
				{
				aMenuPane->SetItemDimmed( EImageEditorMenuCmdFullScreen, EFalse );
				}

			// In full screen mode

			else
				{
				aMenuPane->SetItemDimmed( EImageEditorMenuCmdNormalScreen, EFalse );
				}
#endif // FULLSCREEN_AVAILABLE        
			aMenuPane->SetItemDimmed(EImageEditorMenuCmdHelp, EFalse);
			aMenuPane->SetItemDimmed(EImageEditorMenuCmdExit, EFalse);
			}
		else if (InZoomingState())
			{
			aMenuPane->SetItemDimmed(EImageEditorMenuCmdUndo, ETrue);
#ifdef FULLSCREEN_AVAILABLE
			aMenuPane->SetItemDimmed( EImageEditorMenuCmdFullScreen, ETrue );
			aMenuPane->SetItemDimmed( EImageEditorMenuCmdNormalScreen, ETrue );
#endif // FULLSCREEN_AVAILABLE  
			if (!iCanSave || IsMemoryInCriticalLevel())
				{
				aMenuPane->SetItemDimmed(EImageEditorMenuCmdSave, ETrue);
				}

			TInt position;
			if (aMenuPane->MenuItemExists(EImageEditorMenuCmdSend, position))
				{
				aMenuPane->SetItemDimmed(EImageEditorMenuCmdSend, ETrue);
				}
			if (aMenuPane->MenuItemExists(EImageEditorAiwCriteriaPrint,position))
				{
				aMenuPane->SetItemDimmed(EImageEditorAiwCriteriaPrint, ETrue);
				}

			if (iZoomMode == EZoomIn3)
				{
				aMenuPane->SetItemDimmed(EImageEditorMenuCmdZoomIn, ETrue);
				}
			// Set "Apply Effect" item to be after "Zoom Out" in zooming 
			// state's options menu
			if (aMenuPane->MenuItemExists(EImageEditorMenuCmdApplyEffect,position))
				{
				CEikMenuPaneItem::SData data = aMenuPane->ItemData(
						EImageEditorMenuCmdApplyEffect);
				aMenuPane->DeleteMenuItem(EImageEditorMenuCmdApplyEffect);
				aMenuPane->AddMenuItemL(data, EImageEditorMenuCmdZoomOut);
				}
			}
		//  In main view mode
		else
			{

#ifdef AIW_PRINT_PROVIDER_USED

#else
			aMenuPane->SetItemDimmed( EImageEditorAiwCriteriaPrint, ETrue );
#endif   

			//  Display CSendUi menu item
			if (iSendAppUi)
				{
				iSendAppUi->AddSendMenuItemL(*aMenuPane,
						SIE_SENDUI_MENU_ITEM_INDEX, EImageEditorMenuCmdSend,
						TSendingCapabilities(0, 0,
								TSendingCapabilities::ESupportsAttachments));
				}

			//  Nothing to undo
			if (!iCanUndo)
				{
				aMenuPane->SetItemDimmed(EImageEditorMenuCmdUndo, ETrue);
				}
#ifdef FULLSCREEN_AVAILABLE
			// In full screen mode
			if ( iFullScreen )
				{
				aMenuPane->SetItemDimmed( EImageEditorMenuCmdFullScreen, ETrue );
				}

			// In normal screen mode

			else
				{
				aMenuPane->SetItemDimmed( EImageEditorMenuCmdNormalScreen, ETrue );
				}
#endif // FULLSCREEN_AVAILABLE
			if (!iCanSave || IsMemoryInCriticalLevel())
				{
				aMenuPane->SetItemDimmed(EImageEditorMenuCmdSave, ETrue);
				}

			if (iZoomMode == EZoomNormal)
				{
				aMenuPane->SetItemDimmed(EImageEditorMenuCmdZoomOut, ETrue);
				aMenuPane->SetItemDimmed(EImageEditorMenuCmdFitToScreen, ETrue);
				}

#ifdef AIW_PRINT_PROVIDER_USED
			if (iServiceHandler->IsAiwMenu(aResourceId))
				{
				CAiwGenericParamList& in = iServiceHandler->InParamListL();

				TFileName filename(iImageFileName);
				TAiwVariant variant(filename);
				TAiwGenericParam param(EGenericParamFile, variant);

				in.AppendL(param);

				iServiceHandler->InitializeMenuPaneL(*aMenuPane, aResourceId,
						EImageEditorMenuAiwPrintBase, in);
				}
#endif

			}
		}
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::SetSendAppUi(CSendUi* aSendAppUi)
	{
	iSendAppUi = aSendAppUi;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::HandleStatusPaneSizeChange()
	{

	}

//=============================================================================
void CImageEditorUIView::DimAllL(TInt /*aResourceId*/, CEikMenuPane & aMenuPane) const
	{
	LOG(KImageEditorLogFile, "CImageEditorUIView::DimAllL");

	aMenuPane.SetItemDimmed(EImageEditorMenuCmdApplyEffect, ETrue);
	aMenuPane.SetItemDimmed(EImageEditorMenuCmdExit, ETrue);
	aMenuPane.SetItemDimmed(EImageEditorMenuCmdUndo, ETrue);
#ifdef FULLSCREEN_AVAILABLE    
	aMenuPane.SetItemDimmed (EImageEditorMenuCmdFullScreen, ETrue);
	aMenuPane.SetItemDimmed (EImageEditorMenuCmdNormalScreen, ETrue);
#endif // FULLSCREEN_AVAILABLE    
	aMenuPane.SetItemDimmed(EImageEditorMenuCmdZoomIn, ETrue);
	aMenuPane.SetItemDimmed(EImageEditorMenuCmdZoomOut, ETrue);
	aMenuPane.SetItemDimmed(EImageEditorMenuCmdSave, ETrue);
	aMenuPane.SetItemDimmed(EImageEditorAiwCriteriaPrint, ETrue);
	aMenuPane.SetItemDimmed(EImageEditorMenuCmdHelp, ETrue);
	aMenuPane.SetItemDimmed(EImageEditorMenuCmdFitToScreen, ETrue);
	}

//=============================================================================
void CImageEditorUIView::UpdateSoftkeysL()
	{
	__ASSERT_ALWAYS( iControl, User::Panic(KComponentName, EImageEditorPanicControlIsNull) );

	//  Get plug-in info
	CPluginInfo * pgn_info = iArray[iIndex];

	//  Get soft key pair index 
	TInt sk_index = ((CImageEditorControlBase *) iControl)->GetSoftkeyIndexL();

	//  Set soft key 1
	SetSoftKey1L(pgn_info->Sk1Texts()[sk_index], pgn_info->Sk1Cmds()[sk_index]);

	//  Set soft key 2
	SetSoftKey2L(pgn_info->Sk2Texts()[sk_index], pgn_info->Sk2Cmds()[sk_index]);

	//  Set middle soft key
	SetMiddleSoftKeyL(pgn_info->MSKTexts()[sk_index],
			pgn_info->MSKCmds()[sk_index]);
	}

//=============================================================================
void CImageEditorUIView::UpdateNaviPaneL()
	{
	__ASSERT_ALWAYS( iControl, User::Panic(KComponentName, EImageEditorPanicControlIsNull) );

	TBool leftNaviPaneScrollButtonVisibile;
	TBool rightNaviPaneScrollButtonVisible;
	TPtrC naviText = ((CImageEditorControlBase *) iControl)->GetNaviPaneTextL(
			leftNaviPaneScrollButtonVisibile, rightNaviPaneScrollButtonVisible);

	SetNaviPaneTextL(naviText, leftNaviPaneScrollButtonVisibile,
			rightNaviPaneScrollButtonVisible);
	}

//=============================================================================
void CImageEditorUIView::InsertPluginItemsL(CEikMenuPane & aMenuPane)
	{
	__ASSERT_ALWAYS( iControl, User::Panic(KComponentName, EImageEditorPanicControlIsNull) );

	// Insert the plug-in commands
	CMenuItemArray & menu_items = iArray[iIndex]->MenuItems();
	TBitField dimmed =
			((CImageEditorControlBase *) iControl)->GetDimmedMenuItems();
	for (TInt i = 0; i < menu_items.Count(); ++i)
		{
		// Check the visibility for each menu item.
		if (!dimmed.GetBit(i))
			{
			aMenuPane.InsertMenuItemL(menu_items[i], i);
			}
		}
	}

//=============================================================================
TBool CImageEditorUIView::MenuItemPressedL(TInt aCommand)
	{
	CMenuItemArray & menu_items = iArray[iIndex]->MenuItems();
	for (TInt i = 0; i < menu_items.Count(); ++i)
		{
		if (menu_items[i].iCommandId == aCommand)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::DoActivateL(
		const TVwsViewId & /*aPrevViewId*/, TUid /*aCustomMessageId*/,
		const TDesC8 & /*aCustomMessage*/
)
	{
	LOG(KImageEditorLogFile, "CImageEditorUIView: Activating view");
	//  Construct container
	iContainer = new (ELeave) CImageEditorUIContainer;
	iContainer->SetMopParent(this);
	iContainer->ConstructL(ClientRect());
	iContainer->SetView(this);
	HandleCommandL(EImageEditorCmdViewReady);

	}

//=============================================================================
EXPORT_C void CImageEditorUIView::DoDeactivate()
	{
	LOG(KImageEditorLogFile, "CImageEditorUIView: Deactivating view");

	RemoveContainerFromStack();

	if (iContainer)
		{
		delete iContainer;
		iContainer = 0;
		}
	}

//=============================================================================
void CImageEditorUIView::AddContainerToStack()
	{
	if (!iContainerInStack && iContainer)
		{
		TRAPD(err, AppUi()->AddToViewStackL (*this, iContainer); );
		if (err == KErrNone)
			{
			iContainerInStack = ETrue;
			}
		}
	}

//=============================================================================
void CImageEditorUIView::RemoveContainerFromStack()
	{
	if (iContainerInStack)
		{
		AppUi()->RemoveFromViewStack(*this, iContainer);
		iContainerInStack = EFalse;
		}
	}

//=============================================================================
EXPORT_C TBool CImageEditorUIView::InZoomingState()
	{
	if (iZoomMode == EZoomIn1 || iZoomMode == EZoomIn2 || iZoomMode == EZoomIn3)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

//=============================================================================
EXPORT_C TInt CImageEditorUIView::LaunchSaveImageQueryL() const
	{
	//	Create dialog heading and options
	HBufC * heading = CEikonEnv::Static()->AllocReadResourceLC(
			R_SIE_LIST_QUERY_HEADING_SAVE);
	HBufC * option1 = CEikonEnv::Static()->AllocReadResourceLC(
			R_SIE_LIST_QUERY_SAVE_NEW);
	HBufC * option2 = CEikonEnv::Static()->AllocReadResourceLC(
			R_SIE_LIST_QUERY_SAVE_REPLACE);

	//	Query dialog texts
	CDesCArray * options = new (ELeave) CDesCArraySeg(2);
	CleanupStack::PushL(options);
	options->AppendL(option1->Des());
	options->AppendL(option2->Des());

	//	Execute query dialog
	TInt ret = SDrawUtils::LaunchListQueryDialogL(options, *heading);

	options->Reset();

	CleanupStack::PopAndDestroy(4); //options, option2, option1, heading

	return ret;
	}

//=============================================================================
EXPORT_C void CImageEditorUIView::GetTouchPanDirections(TInt& xMovement,
		TInt& yMovement)
	{

	xMovement = 0;
	yMovement = 0;
	iContainer->GetDragDirections(xMovement, yMovement);
	}

EXPORT_C TBool CImageEditorUIView::IsMemoryInCriticalLevel()
	{
	TEntry entry;
	RFs & fs = iEikonEnv->FsSession();
	fs.Entry(iImageFileName, entry);
	TFileName driveAndPath;
	driveAndPath.Copy(PathInfo::PhoneMemoryRootPath());
	driveAndPath.Append(PathInfo::ImagesPath());
	return !ImageEditorUtils::ImageFitsToDriveL(fs, iImageFileName,
			driveAndPath);
	}

// End of File

