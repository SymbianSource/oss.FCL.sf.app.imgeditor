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
* Dialog for clipart plugin.
*
*/


#include <AknsDrawUtils.h>
#include <eikmover.h> 
#include <aknview.h>
#include <aknlists.h> 
#include <barsread.h>
#include <akniconarray.h> 
#include <gulicon.h>
#include <bautils.h>
#include <coecobs.h>
#include <uikon.hrh>
#include <ConeResLoader.h> 
#include <BitmapTransforms.h>
#include <eikprogi.h>
#include <s32file.h>
#include <AknLayoutScalable_Apps.cdl.h>
#include <layoutmetadata.cdl.h>
#include <aknslistboxbackgroundcontrolcontext.h>

#include "ClipartSelectionGrid.h"
#include "ClipartScaler.h"
#include "Callback.h"
#include "Clipart.hrh"
#include "ClipartSelectionDialog.h"
#include "ResolutionUtil.h"
#include "ImageEditorUI.hrh"
#include "iepb.h"
#include "ImageEditorError.h"
#include <Clipart.rsg>

// debug log
//#include "ImageEditorClipartLogFile.h"
#include "imageeditordebugutils.h"

_LIT(KClipartPluginLogFile,"ClipartPlugin.log");

// Constants 
const TInt KDefaultNumberOfCliparts = 51;

const TInt KStandardClipartGridCellWidth = 40;
const TInt KStandardClipartGridCellHeight = 30;

const TInt KQVGAClipartGridCellWidth = 55;
const TInt KQVGAClipartGridCellHeight = 46;

const TInt KDoubleClipartGridCellWidth = 80;
const TInt KDoubleClipartGridCellHeight = 60;

const TInt KVGAClipartGridCellWidth = 95;
const TInt KVGAClipartGridCellHeight = 60;

const TInt KQHDClipartGridCellWidth = 75;
const TInt KQHDClipartGridCellHeight = 65;

const TInt KClipartGridCellGap = 0;

//const TInt KScrollBarWidth = 3;

const TInt KProgressInterval = 5;

_LIT (KClipartResourceFile, "clipart.rsc");
_LIT (KClipartFileWild, "*.mbm");
_LIT( KClipartExternalizeFile, "c:\\private\\101FFA91\\ClipartStorage.ini");


//=============================================================================
CClipartSelectionDialog::CClipartSelectionDialog(TFileName& aClipartFileName, TInt& aClipartFileNameIndex)
 : CEikDialog()
 , iGrid(NULL)
 , iClipartFileName(aClipartFileName)
 , iClipartFileNameIndex(aClipartFileNameIndex)
 , iIconArray(NULL)
 , iTextArray(NULL)
 , iProgressDialog(NULL)
 , iClipartScaler(NULL)
 , iCallback(NULL)
 , iCurrentBitmap(NULL)
 , iCurrentMask(NULL)
 , iCurrentIcon(0)
 , iState(EIdle)
 , iBusy(ETrue)
 , iDoubleClick(EFalse)
 , iClick(EFalse)
    {

    }
//=============================================================================
CClipartSelectionDialog::~CClipartSelectionDialog()
    {
    if (iCallback)
    	{
        iCallback->Cancel();
    	}
    delete iCallback;

    if (iClipartScaler)
    	{
        iClipartScaler->Cancel();
    	}
    delete iClipartScaler;
    
    if (iIconArray)
    	{
    	delete iIconArray;
    	iIconArray = NULL;
    	}
    if (iTextArray)
    	{
    	delete iTextArray;	
    	iTextArray = NULL;
	    }
    
    iClipartList.Close();    
    delete iCurrentBitmap;
    delete iCurrentMask;
    delete iProgressDialog;
    
    delete iGrid;
    }


//=============================================================================
void CClipartSelectionDialog::ConstructL(MCoeControlObserver* aParent)
{
	
	SetObserver(aParent);
	
    iClipartScaler = new (ELeave) CClipartScaler(this);
    iClipartScaler->ConstructL();

    // Callback
    iCallback = CObCallback::NewL(this);

    // Create icon and text arrays
    iIconArray = new(ELeave) CAknIconArray(KDefaultNumberOfCliparts);
	iTextArray = new(ELeave) CDesC16ArrayFlat(KDefaultNumberOfCliparts);
      
    TInt screenMode = CResolutionUtil::Self()->GetScreenMode();

    switch (screenMode)
    {

    case CResolutionUtil::EDouble:
    case CResolutionUtil::EDoubleFullScreen:
    case CResolutionUtil::EDoubleLandscape:
    case CResolutionUtil::EDoubleLandscapeFullScreen:
        {
        iClipartGridCellWidth = KDoubleClipartGridCellWidth;
        iClipartGridCellHeight = KDoubleClipartGridCellHeight;
        break;
        }

    case CResolutionUtil::EQVGA:
    case CResolutionUtil::EQVGAFullScreen:
    case CResolutionUtil::EQVGALandscape:
    case CResolutionUtil::EQVGALandscapeFullScreen:
        {
        iClipartGridCellWidth = KQVGAClipartGridCellWidth;
        iClipartGridCellHeight = KQVGAClipartGridCellHeight;
        break;
        }
    
    case CResolutionUtil::EVGA:
    case CResolutionUtil::EVGAFullScreen:
    case CResolutionUtil::EVGALandscape:
    case CResolutionUtil::EVGALandscapeFullScreen:
        {
        iClipartGridCellWidth = KVGAClipartGridCellWidth;
        iClipartGridCellHeight = KVGAClipartGridCellHeight;
        break;
        }
        
    case CResolutionUtil::EQHD:
    case CResolutionUtil::EQHDFullScreen:
    case CResolutionUtil::EQHDLandscape:
    case CResolutionUtil::EQHDLandscapeFullScreen:
        {
        iClipartGridCellWidth = KQHDClipartGridCellWidth;
        iClipartGridCellHeight = KQHDClipartGridCellHeight;
        break;
        }
    default:
        {
        iClipartGridCellWidth = KStandardClipartGridCellWidth;
        iClipartGridCellHeight = KStandardClipartGridCellHeight;
        break;
        }
    }

    // Check that if externalize file exists
    RFs& fs = ControlEnv()->FsSession();
    BaflUtils::EnsurePathExistsL( fs, KClipartExternalizeFile() );
   
    if( BaflUtils::FileExists(fs, KClipartExternalizeFile()) )
    {
        FindClipartsL();
    
        // Internalize icons if externalize file exists
        TRAPD(err,
        InternalizeIconArrayL();
        );

        if (err)
            {
            // if error delete externalization file
            BaflUtils::DeleteFile(
                ControlEnv()->FsSession(),
                KClipartExternalizeFile()
                );

            User::Leave(err);
            }
 
        iCallback->DoCallback(0);
    }
    else
    {
    
        // Find Cliparts
        FindClipartsL();

        // Create progress note dialog
        iProgressDialog = new ( ELeave ) CAknProgressDialog( reinterpret_cast<CEikDialog**>(&iProgressDialog) );
        iProgressDialog->PrepareLC(R_CLIPART_PROGRESS_NOTE);
        iProgressDialog->GetProgressInfoL()->SetFinalValue( iClipartList.Count() );
        iProgressDialog->SetCallback ( (MProgressDialogCallback *) this);
        iProgressDialog->RunLD();

        // Load clipart bitmaps for the grid - asynchronous method
        LoadBitmapsL();

    }
    
    
}

//============================================================================= 
void  CClipartSelectionDialog::PreLayoutDynInitL () 
{
   
	CAknGrid* grid = (CAknGrid*)ControlOrNull(EClipartSelectionGrid);

    // Set icon array to grid
    grid->ItemDrawer()->FormattedCellData()->SetIconArray(iIconArray);
	iIconArray = NULL; // ownership moved

    grid->Model()->SetItemTextArray(iTextArray);
	iTextArray = NULL;  // ownership moved

	TRect clientRect;
	CResolutionUtil::Self()->GetClientRect(clientRect);
	
	grid->SetRect(clientRect);
    
    TInt xOffset( 0 );  
    TAknWindowLineLayout lineLayout;    
    TAknLayoutRect layoutRect;    
    TInt variety( 0 );
    TRect scrollPaneRect;
	if( Layout_Meta_Data::IsLandscapeOrientation() )
    	{
     	variety = 1;
    	}
    	
    if( AknLayoutUtils::LayoutMirrored() )
        {
	    // scroll_pane_cp029
	    lineLayout =
            AknLayoutScalable_Apps::scroll_pane_cp029( variety ).LayoutLine();
        layoutRect.LayoutRect( Rect(), lineLayout );
        scrollPaneRect = layoutRect.Rect();
        xOffset = scrollPaneRect.Width() + 5;
        }
    
    TInt rows = clientRect.Height() / iClipartGridCellHeight;
    TInt columns = (clientRect.Width() - scrollPaneRect.Width()) / iClipartGridCellWidth;
    
    AknListBoxLayouts::SetupGridPos( *grid, 
                                     xOffset, 
                                     0, 
                                     -1, 
                                     -1, 
                                     iClipartGridCellWidth, 
                                     iClipartGridCellHeight );
                
    //  Set grid layout
    grid->SetLayoutL (
        EFalse, 
        ETrue, 
        ETrue, 
        columns, 
        rows, 
        TSize(iClipartGridCellWidth, iClipartGridCellHeight)
        );
    
    grid->SetPrimaryScrollingType (CAknGridView::EScrollIncrementLineAndLoops);
    grid->SetSecondaryScrollingType (CAknGridView::EScrollIncrementLineAndLoops);

    // Set gaps between items
    ((CAknGridView*)grid->View())->
        SetSpacesBetweenItems(TSize(KClipartGridCellGap,KClipartGridCellGap));
    
    grid->SetCurrentDataIndex (0);		

	SetupGrid();
	
    // Set scroll bar
    grid->CreateScrollBarFrameL(ETrue, EFalse);
    grid->ScrollBarFrame()->
       SetScrollBarVisibilityL(CEikScrollBarFrame::EAuto, CEikScrollBarFrame::EAuto);

    // Inform list box about new icons
    grid->HandleItemAdditionL();
	
    grid->SetListBoxObserver(this);
    TBool iLandscape = CResolutionUtil::Self()->GetLandscape();
    if (iLandscape)
    {
    grid->ItemDrawer()->ColumnData()->SetSkinEnabledL(ETrue);
    CAknsListBoxBackgroundControlContext* context = static_cast<CAknsListBoxBackgroundControlContext*>(grid->ItemDrawer()->ColumnData()->SkinBackgroundContext());
    TRect bgRect = clientRect;
           
    TRect spRect; 
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EBatteryPane, spRect);
          
    bgRect.iTl.iY-=spRect.Height();
    context->SetRect(bgRect);
    context->SetBitmap( KAknsIIDQsnBgAreaMainFlat );
    
    } 
    // Reset busy flag
    SetBusy(EFalse);
	
}

//============================================================================= 
void CClipartSelectionDialog::SetSizeAndPosition( const TSize& aSize)
{
	CEikDialog::SetSizeAndPosition(aSize);

}

//=============================================================================
TBool CClipartSelectionDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyOk || aButtonId == EAknSoftkeySelect)
        {
        ClipartFilenameL(iClipartFileName, iClipartFileNameIndex);
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

//=============================================================================

TBool CClipartSelectionDialog::CallbackMethodL ( TInt /*aParam*/ )
{
	LOG(KClipartPluginLogFile, "CClipartSelectionDialog::CallbackMethodL()");

    // Report that the grid is ready
    ReportEventL(MCoeControlObserver::EEventStateChanged);

    return EFalse;
}

//=============================================================================
TKeyResponse CClipartSelectionDialog::OfferKeyEventL (
    const TKeyEvent &   aKeyEvent,
    TEventCode          aType
    )
{
	TKeyResponse result = EKeyWasNotConsumed;
    if (
        ( aKeyEvent.iCode == EKeyEnter )
       )
        {
        TryExitL( EAknSoftkeyOk );
        result = EKeyWasConsumed;
        }
    else
        {
        result = CEikDialog::OfferKeyEventL( aKeyEvent, aType );
        }
    return result;

}

//=============================================================================
void CClipartSelectionDialog::FindClipartsL()
{
	LOG(KClipartPluginLogFile, "CClipartSelectionGrid::FindClipartsL()");

    //	Read resource
	TFileName resourcefile;
	resourcefile.Append(KPgnResourcePath);
	resourcefile.Append(KClipartResourceFile);
    User::LeaveIfError( CompleteWithAppPath( resourcefile ) );

    //  Get reference to application file session
    RFs & fs = ControlEnv()->FsSession();
    
    // Implementation of RConeResourceLoader uses BaflUtils::NearestLanguageFile 
    // to search for a localised resource in proper search order
    RConeResourceLoader resLoader( *ControlEnv() );
    resLoader.OpenL ( resourcefile );
    CleanupClosePushL(resLoader);

    CDesCArrayFlat* array = 
        ControlEnv()->ReadDesCArrayResourceL(R_CLIPART_DIRS);
    CleanupStack::PushL(array);

    TBuf<256> pathList;
    _LIT(KPathSeparator, ";");

    for (TInt i = 0; i < array->Count(); ++i)
        {
        pathList.Append((*array)[i]);
        pathList.Append(KPathSeparator);
        }
    CleanupStack::PopAndDestroy(2); // resLoader, array

	//  Create a file finder
    TFindFile fileFinder (fs); 
    CDir * fileList = NULL; 

    //  Find files by wild card and directory
    TInt err = fileFinder.FindWildByPath (
		KClipartFileWild, 
		&pathList, 
		fileList
		);

	LOGFMT(KClipartPluginLogFile, "CClipartSelectionGrid::FindClipartsL: found %d cliparts", fileList->Count());

    // Trace leave
    if (err)
        {
		LOGFMT(KClipartPluginLogFile, "CClipartSelectionGrid::FindClipartsL: FindWildByPath returned %d. Leaving...", err);
        User::LeaveIfError(KSIEEInternal);
        }

	//	Go through files
    while (err == KErrNone)
    {
        CleanupStack::PushL (fileList);

		//	Check all plug-in candidates
        for (TInt i = 0; i < fileList->Count(); ++i) 
        {

            //	Create a full file name for a MBM file
            TParse fullentry;
            fullentry.Set (
				(*fileList)[i].iName,&
                fileFinder.File(), 
				0
				); 
            TPtrC fullname (fullentry.FullName());

            //  Check the amount of frames 
            TInt err_bmp = KErrNone;
            TInt j = 0;
            
            while (err_bmp == KErrNone)
            {
                // Try to load next bitmap. 
                // KErrNotFound if no more bitmaps
                CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
                CleanupStack::PushL (bitmap);
                err_bmp = bitmap->Load (fullname, j);

                CFbsBitmap* bitmapMask = new (ELeave) CFbsBitmap;
                CleanupStack::PushL (bitmapMask);
                TInt err_mask = bitmapMask->Load (fullname, j + 1);
                
                if (err_bmp == KErrNone && err_mask != KErrNone)
                    {
                    User::Leave(KSIEEInternal);
                    }

                if (err_bmp == KErrNone)
                {
 
                    // Add filename-index pair to the clipart list
                    TClipartListItem item;
                    item.iFilename = fullname;
                    item.iIndex = j;
                    iClipartList.Append(item);

                }
                CleanupStack::PopAndDestroy(2); // bitmap, mask

                j += 2;
            }
        }	
    
		CleanupStack::PopAndDestroy(); /// fileList

		//	Try once again
        err = fileFinder.FindWild (fileList); 

    }

}


//=============================================================================
void CClipartSelectionDialog::LoadBitmapsL()
    {
    LOG(KClipartPluginLogFile, "CClipartSelectionGrid::LoadBitmapsL()");

    // Create and load new one
    iCurrentBitmap = new (ELeave) CFbsBitmap;
    User::LeaveIfError(iCurrentBitmap->Load(
        iClipartList[iCurrentIcon].iFilename, 
        iClipartList[iCurrentIcon].iIndex));

    // Create and load new one
    iCurrentMask = new (ELeave) CFbsBitmap;
    User::LeaveIfError(iCurrentMask->Load(
        iClipartList[iCurrentIcon].iFilename, 
        iClipartList[iCurrentIcon].iIndex + 1));

    // Scale bitmap to grid size
    iState = EScalingBitmap;
    iClipartScaler->ScaleBitmapL(
        iCurrentBitmap,
        TSize(iClipartGridCellWidth, iClipartGridCellHeight));
    }

//=============================================================================
void CClipartSelectionDialog::AddIconL()
    {
	LOG(KClipartPluginLogFile, "CClipartSelectionGrid::AddIconL()");
    // Create icon
    CGulIcon* icon = NULL;
    icon = CGulIcon::NewLC();
    icon->SetBitmapsOwnedExternally (EFalse);

    // Set bitmap to icon and append icon to icon array
    icon->SetBitmap (iCurrentBitmap);
    iCurrentBitmap = NULL;
    icon->SetMask(iCurrentMask);
    iCurrentMask = NULL;
    iIconArray->AppendL(icon);

    CleanupStack::Pop(); // icon

    //  Create grid item text
    TBuf<8> buf;
    buf.AppendNum (iCurrentIcon);
    buf.Append(_L("\t1"));
    iTextArray->AppendL (buf);
    
    // Increase current icon index
    iCurrentIcon++;

    // Load next icon
    // one step begins.
    if (iProgressDialog && (iCurrentIcon % KProgressInterval) == 0)
        {
        iProgressDialog->GetProgressInfoL()->IncrementAndDraw (KProgressInterval);
        }
    if (iCurrentIcon < iClipartList.Count())
        {
   
        LoadBitmapsL();
        }
    else
        {

        // Externalize loaded icons
        TRAPD(err,
            ExternalizeIconArrayL();
        );
        if (err)
            {
            // if error delete externalization file
            BaflUtils::DeleteFile(
                ControlEnv()->FsSession(),
                KClipartExternalizeFile()
                );
            }
        // dismiss progress dialog
        if (iProgressDialog)
            {
            iProgressDialog->ProcessFinishedL();
            }

        SetBusy(EFalse);

        // Report that the grid is ready
        ReportEventL(MCoeControlObserver::EEventStateChanged);
        }
    }


//=============================================================================
void CClipartSelectionDialog::ClipartScalerOperationReadyL(TInt aError)
    {
    if (aError == KErrNone)
        {
        
        switch (iState)
            {
            case EScalingBitmap:
                {
                // Scale the mask bitmap
                iState = EScalingMask;
                iClipartScaler->ScaleBitmapL(
                    iCurrentMask,
                    TSize(iClipartGridCellWidth, iClipartGridCellHeight));
                break;
                }
            case EScalingMask:
                {
                iState = EIdle;
                // Add icon to grid
                AddIconL();
                break;
                }
            default:
                {
				break;
                }
            }
        }
    else
        {
        User::Leave(KSIEEInternal);
        }
    }

//=============================================================================
void CClipartSelectionDialog::DialogDismissedL( TInt aButtonId )
{
	LOGFMT(KClipartPluginLogFile, "CClipartSelectionGrid::DialogDismissedL(%d)", aButtonId);

	CAknGrid* grid = (CAknGrid*)ControlOrNull(EClipartSelectionGrid);

    if( aButtonId == EAknSoftkeyCancel )
    {
        // Cancel the on-going image decoding
        iClipartScaler->Cancel();

//		grid->HandleItemAdditionL();

        // Report cancel request
        ReportEventL(MCoeControlObserver::EEventRequestCancel);
    }

}

//=============================================================================
void CClipartSelectionDialog::ClipartFilenameL(TDes& aFilename, TInt& aIndex)
    {
	CAknGrid* grid = (CAknGrid*)ControlOrNull(EClipartSelectionGrid);

    // Get MBM file and icon index of the selected clip-art
    aFilename = iClipartList[grid->CurrentItemIndex()].iFilename;
    aIndex = iClipartList[grid->CurrentItemIndex()].iIndex;
    }

//=============================================================================
void CClipartSelectionDialog::SetupGrid ()
{

	CAknGrid* grid = (CAknGrid*)ControlOrNull(EClipartSelectionGrid);

	LOG(KClipartPluginLogFile, "CClipartSelectionGrid::SetupGrid()");

	// Setup text foreground and background colors to default
	AknListBoxLayouts::SetupStandardGrid (*grid);
    
	// Get local copies of data we will need	
	CFormattedCellListBoxItemDrawer * itemDrawer = grid->ItemDrawer();
	    
    TInt cell_w = grid->ColumnWidth(); 
	TInt cell_h = grid->ItemHeight();
	
	// Set up graphics sub-cells
	AknListBoxLayouts::SetupGridFormGfxCell (
		*grid,                  // the grid
		itemDrawer,				// the grid's drawer
		0,						// index of the graphic within item strings
		0,						// left position
		0,						// top position
		0,						// right - unused
		0,						// bottom - unused
		cell_w, 				// width of graphic
		cell_h,					// height of graphic
		TPoint (0, 0),			// start position
		TPoint (cell_w, cell_h)	// end position
		);

}

//=============================================================================
void CClipartSelectionDialog::InternalizeIconArrayL()
{
	LOG(KClipartPluginLogFile, "CClipartSelectionGrid::InternalizeIconArrayL()");

    //	Internalize clipart icons
    RFileReadStream stream;

    User::LeaveIfError (
        stream.Open (
        ControlEnv()->FsSession(),
        KClipartExternalizeFile(),
        EFileRead | EFileShareReadersOnly
        ));
    stream.PushL();

    CGulIcon* icon = NULL;
    CFbsBitmap* bitmap = NULL;

    TInt count = stream.ReadInt32L();
    for ( TInt i=0; i < count; i++ )
    {                
        icon = CGulIcon::NewLC();
        icon->SetBitmapsOwnedExternally (EFalse);
    
        // Text index
        TInt len = stream.ReadInt32L();
        HBufC* iconText = HBufC::NewLC(len);
        TPtr iconTextPtr = iconText->Des();
        stream >> iconTextPtr;

        iTextArray->AppendL(iconTextPtr);

        CleanupStack::PopAndDestroy(); // iconText

        // Bitmap Data

        bitmap = new(ELeave) CFbsBitmap;
        CleanupStack::PushL(bitmap);

        stream >> *bitmap;

        // Save data
        //TBuf<20> name;
        //name.Format(_L("bitmap%d.mbm"), i);
        //bitmap->Save(name);

        // Set bitmap to icon - Ownership is transfered
        icon->SetBitmap (bitmap);

        CleanupStack::Pop();

        bitmap = new(ELeave) CFbsBitmap;
        CleanupStack::PushL(bitmap);

        stream >> *bitmap;

        //TBuf<20> name2;
        //name2.Format(_L("mask%d.mbm"), i);
        //bitmap->Save(name2);

        // Set bitmap to icon - Ownership transfers
        icon->SetMask(bitmap);

        CleanupStack::Pop();
        bitmap = NULL;

        // Set icon to icon array
        iIconArray->AppendL(icon);
    
        CleanupStack::Pop(); // icon
        icon = NULL;

    } 

    CleanupStack::PopAndDestroy(); // stream

}


//=============================================================================
void CClipartSelectionDialog::ExternalizeIconArrayL()
{
	LOG(KClipartPluginLogFile, "CClipartSelectionGrid::ExternalizeIconArrayL()");

    RFileWriteStream stream;

    User::LeaveIfError (
        stream.Replace (
        ControlEnv()->FsSession(),
        KClipartExternalizeFile(),
        EFileWrite
        ));
    stream.PushL();

    // Externalize the contents of clipart
    stream.WriteInt32L( iTextArray->Count() );

    for ( TInt i=0; i< iTextArray->Count(); i++ )
    {
        // Text index 
        TPtrC text = (*iTextArray)[i];
        stream.WriteInt32L(text.Length());
        stream << text;

        // Bitmap 
        CFbsBitmap* bitmap = (*iIconArray)[i]->Bitmap();
        stream << *bitmap;
        bitmap = NULL;

        // Mask
        bitmap = (*iIconArray)[i]->Mask();
        stream << *bitmap;
        bitmap = NULL;
        
    }

    // Write extra before commiting 
    stream.WriteInt32L( 0 );

    stream.CommitL();

    CleanupStack::PopAndDestroy(); // stream

}

//=============================================================================
void CClipartSelectionDialog::SetBusy(TBool aBusy)
{
    iBusy = aBusy;
}

//=============================================================================
SEikControlInfo CClipartSelectionDialog::CreateCustomControlL(TInt aControlType)
{

	SEikControlInfo controlInfo;
	controlInfo.iControl = NULL;
	controlInfo.iTrailerTextId = 0;
	controlInfo.iFlags = 0;

	switch (aControlType)
    {
 	
 		case EClipartCustomControlType:
		{
			controlInfo.iControl = new(ELeave) CClipartSelectionGrid;
			break;
		}
        
        default:
		{
			break;
		}
        
    }

	return controlInfo;
} 

void CClipartSelectionDialog::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
{
	if ( aEventType == EEventItemDoubleClicked )
	{
		iDoubleClick = ETrue;
	}
	// also single click is enough for selecting item
	else if ( aEventType == EEventItemClicked )
    {
        iClick = ETrue;
    }
}

void CClipartSelectionDialog::HandleDialogPageEventL(TInt /*aEventId*/)
{
    if( iDoubleClick || iClick )
    {
        TryExitL( EAknSoftkeyOk );
    }
}

void CClipartSelectionDialog::HandleResourceChange(TInt aType)
{
    CEikDialog::HandleResourceChange(aType);
    if( aType == KEikDynamicLayoutVariantSwitch )
    {
        CAknGrid* grid = (CAknGrid*)ControlOrNull(EClipartSelectionGrid);
        if ( grid )
        {
        	TRect clientRect = Rect(); 
	        grid->SetRect(clientRect);
            TInt xOffset( 0 );  
            TAknWindowLineLayout lineLayout;    
            TAknLayoutRect layoutRect; 
            TInt variety( 0 );
            TRect scrollPaneRect;
	        if( Layout_Meta_Data::IsLandscapeOrientation() )
    	    {
     	    variety = 1;
    	    }
    	
            if( AknLayoutUtils::LayoutMirrored() )
            {
	        // scroll_pane_cp029
	        lineLayout =
            AknLayoutScalable_Apps::scroll_pane_cp029( variety ).LayoutLine();
            layoutRect.LayoutRect( Rect(), lineLayout );
            scrollPaneRect = layoutRect.Rect();
            xOffset = scrollPaneRect.Width() + 5;
            }
            TInt rows = clientRect.Height() / iClipartGridCellHeight;
            TInt columns = (clientRect.Width() - scrollPaneRect.Width()) / iClipartGridCellWidth;
          
            //  Set grid layout
           
            grid->SetLayoutL (
            EFalse, 
            ETrue, 
            ETrue, 
            columns, 
            rows, 
            TSize(iClipartGridCellWidth, iClipartGridCellHeight)
            );
                          
        }
    }
}

// End of File
