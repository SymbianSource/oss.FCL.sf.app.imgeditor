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


/// INCLUDES
#include <resize.mbg>
#include <resize.rsg>
#include <aknutils.h> 
#include <aknglobalnote.h>
#include <ConeResLoader.h> 
#include <stringloader.h>
#include "SingleParamControl.h"
#include "SystemParameters.h"
#include "ResizeControl.h"
#include "ImageEditorResizePlugin.h"
#include "PluginInfo.h"

/// CONSTANTS
_LIT (KPgnResourceFile, "resize.rsc");
_LIT (KResourceDir, "\\resource\\apps\\");

const TInt KResizeHSTitleIndex = 0;
const TInt KMinDimension = 25;
const TInt KMaxCaptionLength = 50;

//=============================================================================
CResizeControl * CResizeControl::NewL (const TRect&	aRect, CCoeControl*	aParent)
{
	CResizeControl * self = new (ELeave) CResizeControl;
	CleanupStack::PushL (self);
	self->ConstructL (aRect, aParent);
	CleanupStack::Pop ();   // self
	return self;
}

//=============================================================================
CResizeControl::CResizeControl() 
{
	// EMPTY
}

//=============================================================================
CResizeControl::~CResizeControl()
{
	delete iControl;
    iControl = NULL;
    iParamArray.Reset();
//    iEditorView = NULL;
    iSysPars = NULL;
    iPluginInfo = NULL;
    iParameters = NULL;
    delete iBufBitmap;
}

//=============================================================================
void CResizeControl::ConstructL (
    const TRect &   aRect, 
    CCoeControl *   aParent
    )
{
	//	Set parent
	SetContainerWindowL (*aParent);


	iControl = CSingleParamControl::NewL (aRect, aParent);
    ((CSingleParamControl *)iControl)->SetParObserver ( (MSingleParControlObserver*)this );
//	aPluginControl = iControl;

	//	Activate control
    ActivateL();
}

//=============================================================================
void CResizeControl::SetView (CAknView * aView)
{
	((CSingleParamControl *)iControl)->SetView(aView);
//    iEditorView = aView;
}

//=============================================================================
void CResizeControl::SetSelectedUiItemL (CPluginInfo * aItem)
{
	iPluginInfo = aItem;
}

//=============================================================================
void CResizeControl::PrepareL ()
{

}

//=============================================================================
TKeyResponse CResizeControl::OfferKeyEventL (
    const TKeyEvent &       aKeyEvent, 
    TEventCode              aType
    )
{
    TKeyResponse response = EKeyWasNotConsumed;

    if ( Busy() )
	{
        response = EKeyWasConsumed;
    }
    else if (aType != EEventKey  )
    {
        switch (aKeyEvent.iScanCode)
		{   
		    // Just consume these keys (No EEventKey type event delivered for 
		    // these keys so this must be done here)
		    case EStdKeyRightShift:
		    case EStdKeyLeftShift:
			{
				response = EKeyWasConsumed;
                break;
			}
		}
    }
    else if (EEventKey == aType)
	{
        switch (aKeyEvent.iCode)
        {
            // Just consume the keys
		    case 0x31: // 1
		    case 0x32: // 2
		    case 0x33: // 3
            case 0x37: // 7
		    case 0x38: // 8
		    case 0x39: // 9
			case EKeyDownArrow:
			case EStdKeyDecVolume:
			case 0x30:			
			case EKeyUpArrow:
			case EStdKeyIncVolume:
            case 0x35:
            case 0x2a: // *
		    case 0x23: // #
//			case EKeyLeftArrow:
//			case EKeyRightArrow:
			{
				response = EKeyWasConsumed;
                break;
			}
		    case 0x34: // 4
			{
				TKeyEvent kevent;
				TEventCode ecode = EEventKey;
				kevent.iCode = EKeyLeftArrow;
				iControl->OfferKeyEventL(kevent, ecode);

				response = EKeyWasConsumed;
                break;
			}
            case 0x36: // 6 
			{
				TKeyEvent kevent;
				TEventCode ecode = EEventKey;
				kevent.iCode = EKeyRightArrow;
				iControl->OfferKeyEventL(kevent, ecode);

				response = EKeyWasConsumed;
                break;
			}
            default:
            {
                response = iControl->OfferKeyEventL(aKeyEvent, aType);
            }
        }
    }
	return response;
}

//=============================================================================
void CResizeControl::HandlePointerEventL( const TPointerEvent &aPointerEvent )
{
    return iControl->HandlePointerEventL( aPointerEvent );
}

//=============================================================================
TInt CResizeControl::CountComponentControls() const
{
	TInt count(0);
	if (iControl)
	{
		++count;
	}
    return count;
}

//=============================================================================
CCoeControl * CResizeControl::ComponentControl (TInt aIndex) const
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
void CResizeControl::HandlePluginCommandL (const TInt aCommand)
	{
	return ((CSingleParamControl *)iControl)->HandlePluginCommandL(aCommand);

/*
	switch(aCommand)
		{
		case EPgnSoftkeyIdCancel:
			{
			iEditorView->HandleCommandL (EImageEditorCancelPlugin);
			break;
			}
		case EPgnSoftkeyIdOk:
			{
				if ( iControl->Position() != iInitPosition )
				{
					((CImageEditorUIView*)iEditorView)->
						ShowConfirmationNoteL(iPluginInfo->Parameters()[KResolutionChangedTextIndex] );
					iEditorView->HandleCommandL (EImageEditorApplyPlugin);
				
				}
				else
				{
					iEditorView->HandleCommandL (EImageEditorCancelPlugin);
				}
            }
		}
*/
	}


//=============================================================================
TInt CResizeControl::GetSoftkeyIndexL ()
{
    return 0;
}

/*
//=============================================================================
TPtrC CResizeControl::GetNaviPaneTextL (
    TBool& aLeftNaviPaneScrollButtonVisibile, 
    TBool& aRightNaviPaneScrollButtonVisible )
{
    aLeftNaviPaneScrollButtonVisibile = EFalse;
    aRightNaviPaneScrollButtonVisible = EFalse;
	
    // .loc file parameter format strings
    _LIT(KParameter1, "%0N");
    _LIT(KParameter2, "%1N");

    // Getting string from resources
    TPtrC ptr = iPluginInfo->Parameters()[KStatusPaneTextIndex]; 
 
    // Finding format patterns
    TInt pos1 = ptr.Find(KParameter1);
    TInt pos2 = ptr.Find(KParameter2);
    
    if(pos1 == KErrNotFound || pos2 == KErrNotFound)
        {
        User::Leave(KSIEEInternal);
        }

    // Clear navi pane 
    iNaviPaneText.Zero();

    // Add new text and replace format strings with new resolution
    iNaviPaneText.Append(ptr.Left(pos1));
    iNaviPaneText.AppendNum (iSize.iWidth);
    iNaviPaneText.Append (ptr.Mid(pos1 + 3, pos2 - (pos1 + 3)));
    iNaviPaneText.AppendNum (iSize.iHeight);
	iNaviPaneText.Append (ptr.Mid(pos2 + 3));

    return iNaviPaneText;
	}
*/

//=============================================================================
TBitField CResizeControl::GetDimmedMenuItems ()
{
    return TBitField();
}

//=============================================================================
void CResizeControl::SetSystemParameters (CSystemParameters * aSysPars, CImageEditorResizePlugin* aParent)
{
    iSysPars = aSysPars;
	ComputeAndSetSliderParameters(aParent);
}

//=============================================================================
void CResizeControl::SizeChanged()
{
	iControl->SetRect(Rect());
}

//=============================================================================
void CResizeControl::Draw (const TRect & aRect) const
{
    // Fill the background with black
    CWindowGc & gc = SystemGc();
	gc.SetPenStyle (CGraphicsContext::ESolidPen);
	gc.SetPenColor (KRgbBlack);
	gc.SetBrushStyle (CGraphicsContext::ESolidBrush);
	gc.SetBrushColor (KRgbBlack);
    
    if ( iPreview && iPreview->Handle() )
    {

		if ( ((CSingleParamControl *)iControl)->Position() == iInitPosition )
		{
			gc.BitBlt ( TPoint(0,0), iPreview );
		}
		else
		{
			gc.Clear (aRect);

			TReal ratio = (TReal)iSize.iHeight / (TReal)iOrigSize.iHeight;
			TInt w = (TInt)(ratio * (TReal)Rect().Width());
			TInt h = (TInt)(ratio * (TReal)Rect().Height());

			TRect target;
			target.iTl.iX = ((Rect().Width() - w) >> 1);
			target.iTl.iY = ((Rect().Height() - h) >> 1);
			target.iBr.iX = ((Rect().Width() + w) >> 1) + 1;
			target.iBr.iY = ((Rect().Height() + h) >> 1) + 1;
			TRect rect ( iPreview->SizeInPixels() );
			//rect.iBr.iX++;
			//rect.iBr.iY++;

			gc.DrawBitmap (target, iPreview, rect);
		}
    }
}


//=============================================================================
void CResizeControl::ComputeAndSetSliderParameters (CImageEditorResizePlugin* aParent) 
{
    //  Store image size
    TInt width = 
        (iSysPars->ViewPortRect().iBr.iX - 
        iSysPars->ViewPortRect().iTl.iX);

    TInt height = 
        (iSysPars->ViewPortRect().iBr.iY - 
        iSysPars->ViewPortRect().iTl.iY);

    iOrigSize = TSize (
        (TInt)(iSysPars->Scale() * width + 0.5), 
        (TInt)(iSysPars->Scale() * height + 0.5)
        );

	ComputeImageSizes();

    //  Set slider properties
    TInt count = iParamArray.Count();
    if ( count > 0 )
    {
		((CSingleParamControl *)iControl)->SetSliderMinimumAndMaximum(0, iParamArray.Count() - 1);
    }
    else
    {
		((CSingleParamControl *)iControl)->SetSliderMinimumAndMaximum(0, 0);
    }
    ((CSingleParamControl *)iControl)->SetSliderStep(1);

    iInitPosition = 0;
    width = (TInt)(iSysPars->Scale() * width + 0.5);
    height = (TInt)(iSysPars->Scale() * height + 0.5);
    for (TInt i = 0; i < iParamArray.Count(); ++i)
    {
        if ( (width == iParamArray[i]) || ((height == iParamArray[i])) )
        {
			((CSingleParamControl *)iControl)->SetSliderPosition(i);
			iInitPosition = i;
        }
    }

    ComputeSizeAndScale();

    TFileName iconFile (KResourceDir);
	TBuf<256> readbuf;
	User::LeaveIfError ( aParent->GetProperty (KCapIconName, readbuf) );
	iconFile.Append(readbuf);
    CEikImage* icon = new (ELeave) CEikImage;
	icon->CreatePictureFromFileL(iconFile,
								 EMbmResizeQgn_indi_imed_downple_super);
								 
	((CSingleParamControl *)iControl)->SetIcon(icon);
	
	// Get caption from plugin properties
	User::LeaveIfError ( aParent->GetProperty (KCapPluginParamNames, readbuf) );
	TLex parser;
	parser.Assign (readbuf);
    TInt tempval = 0;
	parser.Val ( tempval );
    iParameters = (CDesCArray *)tempval;
    
    // Format the resolutions
    TBuf< KMaxCaptionLength > caption_temp;
    TBuf< KMaxCaptionLength > caption;
    StringLoader::Format( caption_temp, (*iParameters)[KResizeHSTitleIndex], 0, iSize.iWidth );
    StringLoader::Format( caption, caption_temp, 1, iSize.iHeight );
    ((CSingleParamControl *)iControl)->SetCaption( caption );
}


//=============================================================================
void CResizeControl::ComputeSizeAndScale() 
{
	//	Compute current image size and scale
	TReal ar = (TReal)iOrigSize.iWidth / (TReal)iOrigSize.iHeight;
    if ( iOrigSize.iHeight < iOrigSize.iWidth )
	{
		iSize.iHeight = iParamArray[ ((CSingleParamControl *)iControl)->Position() ];
		iSize.iWidth = (TReal)(ar * iSize.iHeight) + 0.5; 
	}
	else
	{
		iSize.iWidth = iParamArray[ ((CSingleParamControl *)iControl)->Position() ];		
		iSize.iHeight = (TReal)(iSize.iWidth / ar) + 0.5; 

	}

    iSysPars->Scale() = 
        (TReal)iSize.iHeight / 
        (TReal)((iSysPars->ViewPortRect().iBr.iY - 
        iSysPars->ViewPortRect().iTl.iY));
}

//=============================================================================
void CResizeControl::ComputeImageSizes()
{

	const TSize KQSXGASize(2592, 1944);
	const TSize KQXGASize(2048, 1536);
	const TSize KUXGASize(1600, 1200);
	const TSize KSXGASize(1280, 960);
	const TSize KSVGASize(800, 600);
	const TSize KVGASize(640, 480);
	const TSize KQVGASize(320, 240);

#ifdef RESIZE_CIF_SIZES_SUPPORTED
	const TSize KCIFSize(352, 288);
	const TSize KQCIFSize(176, 144);
	const TSize KSubQCIFSize(128, 96);
#endif

#ifdef RESIZE_CIF_SIZES_SUPPORTED
	PopulateParamArray(KSubQCIFSize);
	PopulateParamArray(KQCIFSize);
#endif

	PopulateParamArray(KQVGASize);

#ifdef RESIZE_CIF_SIZES_SUPPORTED
	PopulateParamArray(KCIFSize);
#endif 

	PopulateParamArray(KVGASize);
	PopulateParamArray(KSVGASize);
	PopulateParamArray(KSXGASize);
	PopulateParamArray(KUXGASize);
	PopulateParamArray(KQSXGASize);

	// Landscape
	if (iOrigSize.iHeight < iOrigSize.iWidth)
	{
		iParamArray.Append(iOrigSize.iHeight);
	}
	else
	{
		iParamArray.Append(iOrigSize.iWidth);
	}
}

//=============================================================================
void CResizeControl::PopulateParamArray(const TSize& aTargetSize )
{
	
	TReal origRatio = 0.0;
	if (iOrigSize.iWidth < iOrigSize.iHeight)
	{
		origRatio = (TReal)iOrigSize.iHeight / iOrigSize.iWidth;	
	}
	else
	{
		origRatio = (TReal)iOrigSize.iWidth / iOrigSize.iHeight;
		
	}
		
	TReal targetRatio = (TReal)aTargetSize.iWidth / aTargetSize.iHeight;
		
	if (aTargetSize.iWidth < iOrigSize.iWidth && aTargetSize.iHeight < iOrigSize.iHeight)
	{
	
		// Landscape
		if (iOrigSize.iHeight < iOrigSize.iWidth)
		{
			if (origRatio > targetRatio)
			{
				TInt h = (TReal)(aTargetSize.iWidth * iOrigSize.iHeight) / iOrigSize.iWidth + 0.5;	
				
				if (h > KMinDimension)
				{
					iParamArray.Append(h);							
				}


			}
			else
			{
				iParamArray.Append(aTargetSize.iHeight);
				
			}
		}
		// Portrait
		else
		{
			if (origRatio > targetRatio)
			{
				TInt w = (TReal)(aTargetSize.iWidth * iOrigSize.iWidth) / iOrigSize.iHeight + 0.5;	
				
				if (w > KMinDimension)
				{
					iParamArray.Append(w);		
				}
			}
			else
			{
				iParamArray.Append(aTargetSize.iHeight);
				
			}
		}
	}
}

//=============================================================================
void CResizeControl::ParamOperation (const TParamOperation aOperation)
{
    switch (aOperation)
    {
        case EParamOperationSubtract:
        case EParamOperationAdd:
        {
			ComputeSizeAndScale();
			
    	    // Format the resolutions to caption text
            TBuf< KMaxCaptionLength > caption_temp;
            TBuf< KMaxCaptionLength > caption;
            StringLoader::Format( caption_temp, (*iParameters)[KResizeHSTitleIndex], 0, iSize.iWidth );
            StringLoader::Format( caption, caption_temp, 1, iSize.iHeight );
            ((CSingleParamControl *)iControl)->SetCaption( caption );

    	    break;
        }
        case EParamOperationDefault:
        {
    	    break;
        }
        default:
        {
    	    break;
        }
    }
}

TReal CResizeControl::GetParam () const
{
    return 0;
}

void CResizeControl::ShowPopupNote ()
	{
	if (iOrigSize != iSize)
		{
	    //	Read resource
		TFileName resourcefile;
		resourcefile.Append(KPgnResourcePath);
		resourcefile.Append(KPgnResourceFile);
	    User::LeaveIfError( CompleteWithAppPath( resourcefile ) );

	    //  Get reference to application file session
	    RFs & fs = CEikonEnv::Static()->FsSession();
	    
	    // Implementation of RConeResourceLoader uses BaflUtils::NearestLanguageFile 
	    // to search for a localised resource in proper search order
	    RConeResourceLoader resLoader( *CEikonEnv::Static() );
	    resLoader.OpenL ( resourcefile );
	    CleanupClosePushL(resLoader);

    	TBuf<256> readbuf;
    	CEikonEnv::Static()->ReadResourceAsDes16L(readbuf, R_SIE_RESIZE_RESOLUTION_CHANGED);
	    CleanupStack::PopAndDestroy(); // resLoader


	    CAknGlobalNote * globalnote = CAknGlobalNote::NewLC();
	    globalnote->ShowNoteL (EAknGlobalConfirmationNote, readbuf); 
	    CleanupStack::PopAndDestroy(); // globalnote
		}
	}


// End of File
