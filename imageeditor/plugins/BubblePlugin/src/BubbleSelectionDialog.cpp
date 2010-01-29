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
* Dialog for Bubble plugin.
*
*/


// INCLUDES
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
#include <aknslistboxbackgroundcontrolcontext.h>
#include <AknLayoutScalable_Apps.cdl.h>
#include <layoutmetadata.cdl.h>

#include "BubbleSelectionGrid.h"
#include "BubbleScaler.h"
#include "Callback.h"
#include "Bubble.hrh"
#include "BubbleSelectionDialog.h"
#include "ResolutionUtil.h"
#include "ImageEditorUI.hrh"
#include "iepb.h"
#include "ImageEditorError.h"
#include <Bubble.rsg>

// debug log
#include "imageeditordebugutils.h"

_LIT(KBubblePluginLogFile,"BubblePlugin.log");

// Constants 
const TInt KDefaultNumberOfBubbles = 51;

const TInt KStandardBubbleGridCellWidth = 40;
const TInt KStandardBubbleGridCellHeight = 30;

const TInt KQVGABubbleGridCellWidth = 56;
const TInt KQVGABubbleGridCellHeight = 46;

const TInt KDoubleBubbleGridCellWidth = 80;
const TInt KDoubleBubbleGridCellHeight = 60;

const TInt KVGABubbleGridCellWidth = 95;
const TInt KVGABubbleGridCellHeight = 60;

const TInt KQHDBubbleGridCellWidth = 80;
const TInt KQHDBubbleGridCellHeight = 60;

const TInt KBubbleGridCellGap = 0;

const TInt KScrollBarWidth = 3;

const TInt KProgressInterval = 5;

_LIT (KBubbleResourceFile, "Bubble.rsc");
_LIT (KBubbleFileWild, "*.mbm");
_LIT( KBubbleExternalizeFile, "c:\\private\\101FFA91\\BubbleStorage.ini");


//=============================================================================
CBubbleSelectionDialog::CBubbleSelectionDialog(TFileName& aBubbleFileName, TInt& aBubbleFileNameIndex)
 : CEikDialog()
 , iGrid(NULL)
 , iBubbleFileName(aBubbleFileName)
 , iBubbleFileNameIndex(aBubbleFileNameIndex)
 , iIconArray(NULL)
 , iTextArray(NULL)
 , iProgressDialog(NULL)
 , iBubbleScaler(NULL)
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
CBubbleSelectionDialog::~CBubbleSelectionDialog()
    {
    if (iCallback)
    	{
        iCallback->Cancel();
    	}
    delete iCallback;

    if (iBubbleScaler)
    	{
        iBubbleScaler->Cancel();
    	}
    delete iBubbleScaler;
    
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
    
    iBubbleList.Close();    
    delete iCurrentBitmap;
    delete iCurrentMask;
    delete iProgressDialog;
    
    delete iGrid;
    }


//=============================================================================
void CBubbleSelectionDialog::ConstructL(MCoeControlObserver* aParent)
{
	
	SetObserver(aParent);
	
    iBubbleScaler = new (ELeave) CBubbleScaler(this);
    iBubbleScaler->ConstructL();

    // Callback
    iCallback = CObCallback::NewL(this);

    // Create icon and text arrays
    iIconArray = new(ELeave) CAknIconArray(KDefaultNumberOfBubbles);
	iTextArray = new(ELeave) CDesC16ArrayFlat(KDefaultNumberOfBubbles);
      
    TInt screenMode = CResolutionUtil::Self()->GetScreenMode();

    switch (screenMode)
    {

    case CResolutionUtil::EDouble:
    case CResolutionUtil::EDoubleFullScreen:
    case CResolutionUtil::EDoubleLandscape:
    case CResolutionUtil::EDoubleLandscapeFullScreen:
        {
        iBubbleGridCellWidth = KDoubleBubbleGridCellWidth;
        iBubbleGridCellHeight = KDoubleBubbleGridCellHeight;
        break;
        }

    case CResolutionUtil::EQVGA:
    case CResolutionUtil::EQVGAFullScreen:
    case CResolutionUtil::EQVGALandscape:
    case CResolutionUtil::EQVGALandscapeFullScreen:
        {
        iBubbleGridCellWidth = KQVGABubbleGridCellWidth;
        iBubbleGridCellHeight = KQVGABubbleGridCellHeight;
        break;
        }
    
    case CResolutionUtil::EVGA:
    case CResolutionUtil::EVGAFullScreen:
    case CResolutionUtil::EVGALandscape:
    case CResolutionUtil::EVGALandscapeFullScreen:
        {
        iBubbleGridCellWidth = KVGABubbleGridCellWidth;
        iBubbleGridCellHeight = KVGABubbleGridCellHeight;
        break;
        }
        
    case CResolutionUtil::EQHD:
    case CResolutionUtil::EQHDFullScreen:
    case CResolutionUtil::EQHDLandscape:
    case CResolutionUtil::EQHDLandscapeFullScreen:
        {
        iBubbleGridCellWidth = KQHDBubbleGridCellWidth;
        iBubbleGridCellHeight = KQHDBubbleGridCellHeight;
        break;
        }
    default:
        {
        iBubbleGridCellWidth = KStandardBubbleGridCellWidth;
        iBubbleGridCellHeight = KStandardBubbleGridCellHeight;
        break;
        }
    }

    // Check that if externalize file exists
    RFs& fs = ControlEnv()->FsSession();
    BaflUtils::EnsurePathExistsL( fs, KBubbleExternalizeFile() );
   
    if( BaflUtils::FileExists(fs, KBubbleExternalizeFile()) )
    {
        FindBubblesL();
    
        // Internalize icons if externalize file exists
        TRAPD(err,
        InternalizeIconArrayL();
        );

        if (err)
            {
            // if error delete externalization file
            BaflUtils::DeleteFile(
                ControlEnv()->FsSession(),
                KBubbleExternalizeFile()
                );

            User::Leave(err);
            }
 
        iCallback->DoCallback(0);
    }
    else
    {
    
        // Find Bubbles
        FindBubblesL();

        // Create progress note dialog
        iProgressDialog = new ( ELeave ) CAknProgressDialog( reinterpret_cast<CEikDialog**>(&iProgressDialog) );
        iProgressDialog->PrepareLC(R_BUBBLE_PROGRESS_NOTE);
        iProgressDialog->GetProgressInfoL()->SetFinalValue( iBubbleList.Count() );
        iProgressDialog->SetCallback ( (MProgressDialogCallback *) this);
        iProgressDialog->RunLD();

        // Load Bubble bitmaps for the grid - asynchronous method
        LoadBitmapsL();

    }
    
    
}

//============================================================================= 
void  CBubbleSelectionDialog::PreLayoutDynInitL () 
{
   
	CAknGrid* grid = (CAknGrid*)ControlOrNull(EBubbleSelectionGrid);

    // Set icon array to grid
    grid->ItemDrawer()->FormattedCellData()->SetIconArray(iIconArray);
	iIconArray = NULL; // ownership moved

    grid->Model()->SetItemTextArray(iTextArray);
	iTextArray = NULL;  // ownership moved

	TRect clientRect;
	CResolutionUtil::Self()->GetClientRect(clientRect);

	TInt rows = clientRect.Height() / iBubbleGridCellHeight;
    TInt columns = (clientRect.Width() - KScrollBarWidth) / iBubbleGridCellWidth;
	
	grid->SetRect(clientRect);

    //  Set grid layout
    grid->SetLayoutL (
        EFalse, 
        ETrue, 
        ETrue, 
        columns, 
        rows, 
        TSize(iBubbleGridCellWidth, iBubbleGridCellHeight)
        );
    
    grid->SetPrimaryScrollingType (CAknGridView::EScrollIncrementLineAndLoops);
    grid->SetSecondaryScrollingType (CAknGridView::EScrollIncrementLineAndLoops);

    // Set gaps between items
    ((CAknGridView*)grid->View())->
        SetSpacesBetweenItems(TSize(KBubbleGridCellGap,KBubbleGridCellGap));
    
    grid->SetCurrentDataIndex (0);		

	SetupGrid();
	
    // Set scroll bar
    grid->CreateScrollBarFrameL(ETrue, EFalse);
    // Hide scroll bar as there are only so few bubbles at the moment available
    grid->ScrollBarFrame()->
       SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff);

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
void CBubbleSelectionDialog::SetSizeAndPosition( const TSize& aSize)
{
	CEikDialog::SetSizeAndPosition(aSize);

}

//=============================================================================
TBool CBubbleSelectionDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyOk || aButtonId == EAknSoftkeySelect)
        {
        BubbleFilenameL(iBubbleFileName, iBubbleFileNameIndex);
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

//=============================================================================

TBool CBubbleSelectionDialog::CallbackMethodL ( TInt /*aParam*/ )
{
	LOG(KBubblePluginLogFile, "CBubbleSelectionDialog::CallbackMethodL()");

    // Report that the grid is ready
    ReportEventL(MCoeControlObserver::EEventStateChanged);

    return EFalse;
}

//=============================================================================
TKeyResponse CBubbleSelectionDialog::OfferKeyEventL (
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
void CBubbleSelectionDialog::FindBubblesL()
{
	LOG(KBubblePluginLogFile, "CBubbleSelectionGrid::FindBubblesL()");

    //	Read resource
	TFileName resourcefile;
	resourcefile.Append(KPgnResourcePath);
	resourcefile.Append(KBubbleResourceFile);
    User::LeaveIfError( CompleteWithAppPath( resourcefile ) );

    //  Get reference to application file session
    RFs & fs = ControlEnv()->FsSession();
    
    // Implementation of RConeResourceLoader uses BaflUtils::NearestLanguageFile 
    // to search for a localised resource in proper search order
    RConeResourceLoader resLoader( *ControlEnv() );
    resLoader.OpenL ( resourcefile );
    CleanupClosePushL(resLoader);

    CDesCArrayFlat* array = 
        ControlEnv()->ReadDesCArrayResourceL(R_BUBBLE_DIRS);
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
		KBubbleFileWild, 
		&pathList, 
		fileList
		);

	LOGFMT(KBubblePluginLogFile, "CBubbleSelectionGrid::FindBubblesL: found %d Bubbles", fileList->Count());

    // Trace leave
    if (err)
        {
		LOGFMT(KBubblePluginLogFile, "CBubbleSelectionGrid::FindBubblesL: FindWildByPath returned %d. Leaving...", err);
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
 
                    // Add filename-index pair to the Bubble list
                    TBubbleListItem item;
                    item.iFilename = fullname;
                    item.iIndex = j;
                    iBubbleList.Append(item);

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
void CBubbleSelectionDialog::LoadBitmapsL()
    {
    LOG(KBubblePluginLogFile, "CBubbleSelectionGrid::LoadBitmapsL()");

    // Create and load new one
    iCurrentBitmap = new (ELeave) CFbsBitmap;
    User::LeaveIfError(iCurrentBitmap->Load(
        iBubbleList[iCurrentIcon].iFilename, 
        iBubbleList[iCurrentIcon].iIndex));

    // Create and load new one
    iCurrentMask = new (ELeave) CFbsBitmap;
    User::LeaveIfError(iCurrentMask->Load(
        iBubbleList[iCurrentIcon].iFilename, 
        iBubbleList[iCurrentIcon].iIndex + 1));

    // Scale bitmap to grid size
    iState = EScalingBitmap;
    iBubbleScaler->ScaleBitmapL(
        iCurrentBitmap,
        TSize(iBubbleGridCellWidth, iBubbleGridCellHeight));
    }

//=============================================================================
void CBubbleSelectionDialog::AddIconL()
    {
	LOG(KBubblePluginLogFile, "CBubbleSelectionGrid::AddIconL()");
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
    if (iCurrentIcon < iBubbleList.Count())
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
                KBubbleExternalizeFile()
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
void CBubbleSelectionDialog::BubbleScalerOperationReadyL(TInt aError)
    {
    if (aError == KErrNone)
        {
        
        switch (iState)
            {
            case EScalingBitmap:
                {
                // Scale the mask bitmap
                iState = EScalingMask;
                iBubbleScaler->ScaleBitmapL(
                    iCurrentMask,
                    TSize(iBubbleGridCellWidth, iBubbleGridCellHeight));
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
void CBubbleSelectionDialog::DialogDismissedL( TInt aButtonId )
{
	LOGFMT(KBubblePluginLogFile, "CBubbleSelectionGrid::DialogDismissedL(%d)", aButtonId);

	CAknGrid* grid = (CAknGrid*)ControlOrNull(EBubbleSelectionGrid);

    if( aButtonId == EAknSoftkeyCancel )
    {
        // Cancel the on-going image decoding
        iBubbleScaler->Cancel();

//		grid->HandleItemAdditionL();

        // Report cancel request
        ReportEventL(MCoeControlObserver::EEventRequestCancel);
    }

}

//=============================================================================
void CBubbleSelectionDialog::BubbleFilenameL(TDes& aFilename, TInt& aIndex)
    {
	CAknGrid* grid = (CAknGrid*)ControlOrNull(EBubbleSelectionGrid);

    // Get MBM file and icon index of the selected clip-art
    aFilename = iBubbleList[grid->CurrentItemIndex()].iFilename;
    aIndex = iBubbleList[grid->CurrentItemIndex()].iIndex;
    }

//=============================================================================
void CBubbleSelectionDialog::SetupGrid ()
{

	CAknGrid* grid = (CAknGrid*)ControlOrNull(EBubbleSelectionGrid);

	LOG(KBubblePluginLogFile, "CBubbleSelectionGrid::SetupGrid()");

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
void CBubbleSelectionDialog::InternalizeIconArrayL()
{
	LOG(KBubblePluginLogFile, "CBubbleSelectionGrid::InternalizeIconArrayL()");

    //	Internalize Bubble icons
    RFileReadStream stream;

    User::LeaveIfError (
        stream.Open (
        ControlEnv()->FsSession(),
        KBubbleExternalizeFile(),
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
void CBubbleSelectionDialog::ExternalizeIconArrayL()
{
	LOG(KBubblePluginLogFile, "CBubbleSelectionGrid::ExternalizeIconArrayL()");

    RFileWriteStream stream;

    User::LeaveIfError (
        stream.Replace (
        ControlEnv()->FsSession(),
        KBubbleExternalizeFile(),
        EFileWrite
        ));
    stream.PushL();

    // Externalize the contents of Bubble
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
void CBubbleSelectionDialog::SetBusy(TBool aBusy)
{
    iBusy = aBusy;
}

//=============================================================================
SEikControlInfo CBubbleSelectionDialog::CreateCustomControlL(TInt aControlType)
{

	SEikControlInfo controlInfo;
	controlInfo.iControl = NULL;
	controlInfo.iTrailerTextId = 0;
	controlInfo.iFlags = 0;

	switch (aControlType)
    {
 	
 		case EBubbleCustomControlType:
		{
			controlInfo.iControl = new(ELeave) CBubbleSelectionGrid;
			break;
		}
        
        default:
		{
			break;
		}
        
    }

	return controlInfo;
} 

void CBubbleSelectionDialog::HandleListBoxEventL(CEikListBox* /*aListBox*/, TListBoxEvent aEventType)
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

void CBubbleSelectionDialog::HandleDialogPageEventL(TInt /*aEventId*/)
{
    if( iDoubleClick || iClick )
    {
        TryExitL( EAknSoftkeyOk );
    }
}

//-------------------------- Handle resource change
void CBubbleSelectionDialog::HandleResourceChange(TInt aType)
{
    CEikDialog::HandleResourceChange(aType);
    if( aType == KEikDynamicLayoutVariantSwitch )
    {
        CAknGrid* grid = (CAknGrid*)ControlOrNull(EBubbleSelectionGrid);
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
					lineLayout = AknLayoutScalable_Apps::scroll_pane_cp029( variety ).LayoutLine();
					layoutRect.LayoutRect( Rect(), lineLayout );
					scrollPaneRect = layoutRect.Rect();
					xOffset = scrollPaneRect.Width() + 5;
            	}
            TInt rows = clientRect.Height() / iBubbleGridCellHeight;
            TInt columns = (clientRect.Width() - scrollPaneRect.Width()) / iBubbleGridCellWidth;
          
            //  Set grid layout
           
            grid->SetLayoutL (
            EFalse, 
            ETrue, 
            ETrue, 
            columns, 
            rows, 
            TSize(iBubbleGridCellWidth, iBubbleGridCellHeight)
            );
                          
        }
    }
}

// End of File

