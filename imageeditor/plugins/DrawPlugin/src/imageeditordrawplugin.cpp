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
 * Freehand draw UI plugin that sends UI commands to 
 * corresponding filter plugin. (filterdraw.dll).
 *
 */
#include "ImageEditordrawPlugin.h"
#include "ImageEditordrawControl.h"
#include "ImageEditorPluginBaseDefs.h"
#include "DrawUtils.h"
#include "SystemParameters.h"
#include "drawpath.h"
#ifdef TEXT_INSERTION_IN_PORTRAIT_ONLY
#include <aknappui.h>
#endif
#include <aknquerydialog.h> 
//	CONSTANTS
_LIT (KPgnResourceFile, "draw.rsc");

// ---------------------------------------------------------------------------
// CreateImageEditorPlugin
// ---------------------------------------------------------------------------
//
EXPORT_C CImageEditorPluginBase * CreateImageEditorPluginL()
	{
	CImageEditorDrawPlugin * plugin = new (ELeave) CImageEditorDrawPlugin;
	CleanupStack::PushL(plugin);
	plugin->ConstructL();
	CleanupStack::Pop(); // plugin
	return plugin;
	}

// ---------------------------------------------------------------------------
// CImageEditorDrawPlugin
// ---------------------------------------------------------------------------
//
CImageEditorDrawPlugin::CImageEditorDrawPlugin() :
	iLandscapeEnabled(EFalse)
	{
	}

// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
inline void CImageEditorDrawPlugin::ConstructL()
	{
	CImageEditorPluginBase::ConstructL(KPgnResourcePath, KPgnResourceFile);
	iRgb = KRgbWhite;
	iSize = TSize(8, 8);
	}

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CImageEditorDrawPlugin::~CImageEditorDrawPlugin()
	{
	ReleasePlugin();
	iSysPars = NULL;
	iPaths.ResetAndDestroy();
	}

// ---------------------------------------------------------------------------
// SetProperty
// ---------------------------------------------------------------------------
//
TInt CImageEditorDrawPlugin::SetProperty(TInt aPropertyId,
		TDesC& aPropertyValue)
	{
	TInt err(KErrNone);
	TLex parser;

	//	Copy data
	switch (aPropertyId)
		{
		case KCapSystemParameters:
			{
			parser.Assign(aPropertyValue);
			TInt tempval = 0;
			parser.Val(tempval);
			iSysPars = (const CSystemParameters *) tempval;
			err = KErrNone;
			break;
			}
		default:
			{
			err = CImageEditorPluginBase::SetProperty(aPropertyId,
					aPropertyValue);
			break;
			}
		}
	return err;
	}

// ---------------------------------------------------------------------------
// GetProperty
// ---------------------------------------------------------------------------
//
TInt CImageEditorDrawPlugin::GetProperty(TInt aPropertyId, TDes& aPropertyValue)
	{
	//	Clean buffer
	aPropertyValue.Zero();

	//	Copy data
	switch (aPropertyId)
		{
		case KCapParamStruct:
			{
			// Control found
			if (iControl)
				{
				aPropertyValue.Copy(
						((CImageEditorDrawControl *) iControl)->GetParam());
				return KErrNone;
				}

			//	Control not found, return KErrNotReady
			else
				{
				return KErrNotReady;
				}
			}
		case KCapReadyToRender:
			{
			TBool readytorender = ETrue;
			if (iControl)
				{
				readytorender
						= ((CImageEditorDrawControl *) iControl)->IsReadyToRender();
				}
			aPropertyValue.AppendNum((TInt) readytorender);
			return KErrNone;
			}
		case KCapIsLandscapeEnabled:
			{
			aPropertyValue.AppendNum((TInt) iLandscapeEnabled);
			return KErrNone;
			}
		case KCapIsSlowPlugin:
			{
			aPropertyValue.AppendNum((TInt) ETrue);
			return KErrNone;
			}
		default:
			{
			return CImageEditorPluginBase::GetProperty(aPropertyId,
					aPropertyValue);
			}
		}
	}

// ---------------------------------------------------------------------------
// InitPluginL
// ---------------------------------------------------------------------------
//
TInt CImageEditorDrawPlugin::InitPluginL(const TRect& aRect,
		CCoeControl* aParent, CCoeControl*& aPluginControl)
	{
	//  Delete previous control
	ReleasePlugin();

	//	Get pointer to the parameter descriptor array
	TBuf<256> readbuf;
	TLex parser;
	User::LeaveIfError(CImageEditorPluginBase::GetProperty(
			KCapPluginParamNames, readbuf));
	parser.Assign(readbuf);
	TInt tempval = 0;
	parser.Val(tempval);
	//CDesCArray * pars = (CDesCArray *)tempval;

#ifdef TEXT_INSERTION_IN_PORTRAIT_ONLY
	// Text input is always insterted in portrait mode.
	// Store the original orientation before forcing to portrait.
	CAknAppUiBase* appUi = static_cast<CAknAppUiBase *>(
			CEikonEnv::Static()->EikAppUi() );
	CAknAppUiBase::TAppUiOrientation originalOrientation =
	appUi->Orientation();
	appUi->SetOrientationL(CAknAppUiBase::EAppUiOrientationPortrait);
#endif

	CalculatePositionOnView();

	iControl = CImageEditorDrawControl::NewL(aRect, aParent, iPaths, iRgb, iSize,iDiplayToolPluginParam);
	aPluginControl = iControl;

	//  Set system parameters
	((CImageEditorDrawControl*) iControl)->SetSystemParameters(iSysPars);

	iVisibleImageRectPrevFirst = iSysPars->VisibleImageRectPrev();

	iLandscapeEnabled = ETrue;

#ifdef TEXT_INSERTION_IN_PORTRAIT_ONLY
	// Set always back to original orientation, even if leaves
	CAknAppUiBase::TAppUiOrientation orientation = appUi->Orientation();

	if (orientation != originalOrientation)
		{
		appUi->SetOrientationL(originalOrientation);

		// Send screen device change event to validate screen
		TWsEvent event;

		RWsSession& rws = CEikonEnv::Static()->WsSession();
		event.SetType( EEventScreenDeviceChanged );
		event.SetTimeNow();
		event.SetHandle( rws.WsHandle() );

		User::LeaveIfError( rws.SendEventToAllWindowGroups(event) );
		}
#else
#ifdef LANDSCAPE_ONLY

	appUi->SetOrientationL(CAknAppUiBase::EAppUiOrientationLandscape);

	// Send screen device change event to validate screen
	TWsEvent event;

	RWsSession& rws = CEikonEnv::Static()->WsSession();
	event.SetType( EEventScreenDeviceChanged );
	event.SetTimeNow();
	event.SetHandle( rws.WsHandle() );

	User::LeaveIfError( rws.SendEventToAllWindowGroups(event) );

#endif
#endif

	return KErrNone;
	}

// ---------------------------------------------------------------------------
// ProcessImageL
// ---------------------------------------------------------------------------
//
void CImageEditorDrawPlugin::ProcessImageL(CEditorImage * /*aImage*/)
	{
	}

// ---------------------------------------------------------------------------
// ReleasePlugin
// ---------------------------------------------------------------------------
//
void CImageEditorDrawPlugin::ReleasePlugin()
	{
	delete iControl;
	iControl = NULL;
	}
// ---------------------------------------------------------------------------
// CalculatePositionOnView
// ---------------------------------------------------------------------------
//
void CImageEditorDrawPlugin::CalculatePositionOnView()
	{
	if (iPaths.Count() > 0)
		{
		// new image rect on preview
		TRect rectPrevLast = iSysPars->VisibleImageRectPrev(); 
		//real image rect
		TRect rectReal = iSysPars->VisibleImageRect();  
		//the old position of old image rect
		TInt xPositionPrevFirst = iVisibleImageRectPrevFirst.iTl.iX; 
		TInt yPositionPrevFirst = iVisibleImageRectPrevFirst.iTl.iY;
		//the old width and height of old image rect
		TInt xPosFactorDivider = iVisibleImageRectPrevFirst.Width();
		TInt yPosFactorDivider = iVisibleImageRectPrevFirst.Height();

		for (TInt i(0); i < iPaths.Count(); ++i)
			{
			CDrawPath* path = iPaths[i];
			CArrayFix<TPoint>* pathPoints = path->ItemArray();
			if (pathPoints && (pathPoints->Count() > 0))
				{
				for (TInt i(0); i < pathPoints->Count(); ++i)
					{
					//the old absolute point on screen 
					TPoint & point = (*pathPoints)[i];
					//the relative position on old image rect
					TInt xPositionCompareFirst = point.iX - xPositionPrevFirst;
					TInt yPositionCompareFirst = point.iY - yPositionPrevFirst;
					//the relative position on new image rect
					TReal xPositionCompareLast = TReal(rectPrevLast.Width()
							* xPositionCompareFirst)
							/ iVisibleImageRectPrevFirst.Width();
					TReal yPositionCompareLast = TReal(rectPrevLast.Height()
							* yPositionCompareFirst)
							/ iVisibleImageRectPrevFirst.Height();
					//the new absolute position on screen
					TInt xPositionOnView = rectPrevLast.iTl.iX
							+ xPositionCompareLast;
					TInt yPositionOnView = rectPrevLast.iTl.iY
							+ yPositionCompareLast;
					point.SetXY(xPositionOnView, yPositionOnView);
					}
				}
			}
		}
	}

// End of File
