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
* Effect plugin selection dialog that shows icons of available
* plugins in a grid.
*
*/


// INCLUDES
#include <fbs.h> 
#include <avkon.hrh>
#include <eikspane.h>
#include <aknutils.h>
#include <avkon.hrh>
#include <AknBidiTextUtils.h>
#include <BidiText.h> 
#include <AknIconUtils.h>
#include <AknInfoPopupNoteController.h>
#include <AknLayoutScalable_Avkon.cdl.h>
#include <AknLayoutScalable_Apps.cdl.h>
#include <eiklabel.h>
#include <ImageEditorUi.mbg>
#include <layoutmetadata.cdl.h>

#ifdef RD_TACTILE_FEEDBACK 
#include <touchfeedback.h>
#endif /* RD_TACTILE_FEEDBACK  */

#include "PluginGrid.h"
#include "PluginInfo.h"
#include "ResolutionUtil.h"
#include <ImageEditorUI.rsg>
#include "ImageEditorUI.hrh"
#include "ImageEditorUIDefs.h" 
#include "DrawUtils.h"

// CONSTANTS
const TInt KDefaultCurrentItem = 7;
const TInt KPortraitNumberOfRows = 5;
const TInt KPortraitNumberOfColumns = 3;
const TInt KLandscapeNumberOfRows = 3;
const TInt KLandscapeNumberOfColumns = 5;
const TInt KBorderPartsNum = 9;
const TInt KTooltipDelayBeforeShow = 200;

//=============================================================================
CPluginGrid::CPluginGrid (CFbsBitmap** aBitmap, 
                          const RPluginArray* aItems)
: iBitmap( aBitmap ), 
  iPluginItems( aItems ), 
  iCurrentItem( KDefaultCurrentItem ),
  iNumberOfRows( KPortraitNumberOfRows ),
  iNumberOfColumns( KPortraitNumberOfColumns ),
  iCellSize(0,0),
  iTouchGridRect(0,0,0,0),
  iHighlightPos(0,0),
  iParentRect(0,0,0,0)
    {
    }

//=============================================================================
CPluginGrid::~CPluginGrid ()
    {
    iBorders.ResetAndDestroy();
    iHighlight.ResetAndDestroy();
    iIcons.ResetAndDestroy();
    delete iText;

    iBitmap = NULL;
    iPluginItems = NULL;
    }

//=============================================================================
void CPluginGrid::ConstructL ()
    {
    TFileName iconFile (KImageEditorUiMifFile);

	// create popup graphics
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

	// create highlight graphics
    for ( TInt i = 0; i < KBorderPartsNum; ++i )
        {
        CEikImage* image = new (ELeave) CEikImage;

		image->CreatePictureFromFileL(iconFile,
										EMbmImageeditoruiQgn_graf_popup_trans_center + 2*i,
										EMbmImageeditoruiQgn_graf_popup_trans_center_mask + 2*i);
//										EMbmImageeditoruiQsn_fr_imed_grid_center + 2*i,
//										EMbmImageeditoruiQsn_fr_imed_grid_center_mask + 2*i);

        CleanupStack::PushL( image );
        image->SetContainerWindowL( *this );
        iHighlight.AppendL( image );
        CleanupStack::Pop( image );
        }

	// title text
	iText = new (ELeave) CEikLabel;
	iText->SetContainerWindowL( *this );
    HBufC* heading = CEikonEnv::Static()->AllocReadResourceLC( R_HEADING_APPLY_EFFECT );
	iText->SetTextL( heading->Des() );
	CleanupStack::PopAndDestroy( heading );

	// plugin graphics
    for (TInt i = 0; i < iPluginItems->Count(); ++i)
        {
        CPluginInfo * pgn_info = (*iPluginItems)[i];

        CEikImage* image = new (ELeave) CEikImage;

		image->SetPicture(pgn_info->Icon(), pgn_info->Mask());
		image->SetPictureOwnedExternally(ETrue);

        CleanupStack::PushL( image );
        image->SetContainerWindowL( *this );
        iIcons.AppendL( image );
        CleanupStack::Pop( image );
        }

    ActivateL();
    }

//=============================================================================
TInt CPluginGrid::GetSelectedItemIndex() const
    {
    return iCurrentItem;
    }


//=============================================================================
void CPluginGrid::SizeChanged()
    {
    
    TBool variant = Layout_Meta_Data::IsLandscapeOrientation();    
    if( variant )
        {
        iNumberOfRows = KLandscapeNumberOfRows;
        iNumberOfColumns =  KLandscapeNumberOfColumns;
        }
    else
        {
        iNumberOfRows = KPortraitNumberOfRows; 
        iNumberOfColumns = KPortraitNumberOfColumns;
        }  
        
    TAknLayoutRect layoutRect;
    
    if(variant) 
        {
        layoutRect.LayoutRect( Rect(), AknLayoutScalable_Apps::popup_imed_trans_window(4) );        
        }
    else
        {
        layoutRect.LayoutRect( Rect(), AknLayoutScalable_Apps::bg_tb_trans_pane_cp02(1) );
        }        
    iParentRect = layoutRect.Rect();

	// determine grid cell size
    layoutRect.LayoutRect( iParentRect, AknLayoutScalable_Apps::cell_imed_effect_pane(1,1,variant) );
	iCellSize = layoutRect.Rect().Size();

	TInt delta = 0;

	// determine caption layout
    layoutRect.LayoutRect( iParentRect, AknLayoutScalable_Apps::heading_imed_pane(variant) );
    TAknLayoutText layoutText;
    layoutText.LayoutText( layoutRect.Rect(), AknLayoutScalable_Apps::heading_imed_pane_t1(variant) );
    TRgb color = layoutText.Color();
	iText->SetFont(layoutText.Font());
	iText->OverrideColorL( EColorLabelText, color );
	iText->SetExtent(layoutText.TextRect().iTl, layoutText.TextRect().Size());
	if(AknLayoutUtils::LayoutMirrored())
		{
		iText->SetAlignment( EHRightVCenter );    
		}
	else
		{
		iText->SetAlignment( EHLeftVCenter );
		}

	// determine popup border layouts
    if( !AknLayoutUtils::LayoutMirrored() )
        {                  
        AknLayoutUtils::LayoutControl( iBorders[1], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g2().LayoutLine() );
        AknLayoutUtils::LayoutControl( iBorders[2], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g3().LayoutLine() );
        AknLayoutUtils::LayoutControl( iBorders[3], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g4().LayoutLine() );
        AknLayoutUtils::LayoutControl( iBorders[4], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g5().LayoutLine() );
        AknLayoutUtils::LayoutControl( iBorders[5], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g6().LayoutLine() );
        AknLayoutUtils::LayoutControl( iBorders[6], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g7().LayoutLine() );
        }
    else
        {
        AknLayoutUtils::LayoutControl( iBorders[1], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g3().LayoutLine() );
        AknLayoutUtils::LayoutControl( iBorders[2], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g2().LayoutLine() );
        AknLayoutUtils::LayoutControl( iBorders[3], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g5().LayoutLine() );
        AknLayoutUtils::LayoutControl( iBorders[4], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g4().LayoutLine() );
        AknLayoutUtils::LayoutControl( iBorders[5], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g7().LayoutLine() );
        AknLayoutUtils::LayoutControl( iBorders[6], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g6().LayoutLine() );
        }
    AknLayoutUtils::LayoutControl( iBorders[0], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g1().LayoutLine() );
    AknLayoutUtils::LayoutControl( iBorders[7], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g8().LayoutLine() );
    AknLayoutUtils::LayoutControl( iBorders[8], iParentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g9().LayoutLine() );
    // Images need to be resized separately, because the standard LayoutImage
    // tries to preserve the aspect ratio and we don't want that.
    //
    for ( TInt i = 0; i < iBorders.Count(); ++i )
        {
        AknIconUtils::SetSize(
            const_cast<CFbsBitmap*>( iBorders[i]->Bitmap() ),
            iBorders[i]->Size(), EAspectRatioNotPreserved );
        }

	// plugin graphics sizes
    layoutRect.LayoutRect( iParentRect, AknLayoutScalable_Apps::cell_imed_effect_pane_g1(variant) );
    TRect iconRect = layoutRect.Rect();
    TSize iconSize = iconRect.Size();
    for (TInt i = 0; i < iPluginItems->Count(); ++i)
        {
        CPluginInfo * pgn_info = (*iPluginItems)[i];

        // Set size for scalable icons - MUST BE CALLED BEFORE ICON IS USABLE
        if (pgn_info->Icon())
            {
            AknIconUtils::SetSize(pgn_info->Icon(), iconSize);
            }
        if (pgn_info->Mask())
            {
            AknIconUtils::SetSize(pgn_info->Mask(), iconSize);
            }
        }

	// touch area
    layoutRect.LayoutRect( iParentRect, AknLayoutScalable_Apps::listscroll_imed_pane (variant) );
	iTouchGridRect = layoutRect.Rect();
	delta = layoutRect.Rect().Width();
    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::cell_imed_effect_pane(0,0,variant) );            
	iTouchGridRect.SetWidth(layoutRect.Rect().Width() * iNumberOfColumns);
	iTouchGridRect.SetHeight(layoutRect.Rect().Height() * iNumberOfRows);
	delta = (delta - iTouchGridRect.Width()) / 2;
    if( AknLayoutUtils::LayoutMirrored() )
        {                  
		delta = -delta;
        }
	iTouchGridRect.iTl.iX += delta;

	// plugin graphics positions
    layoutRect.LayoutRect( iParentRect, AknLayoutScalable_Apps::listscroll_imed_pane (variant) );
    TAknLayoutRect iconLayoutRect;
                  
    for ( TInt i = 0; i <iNumberOfRows; ++i )
        {
	    for ( TInt j = 0; j <iNumberOfColumns; ++j )
	        {
		    iconLayoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::cell_imed_effect_pane(j, i,variant) );
		    iconLayoutRect.LayoutRect( iconLayoutRect.Rect(), AknLayoutScalable_Apps::cell_imed_effect_pane_g1(variant) );
			TPoint iconPos = iconLayoutRect.Rect().iTl;
			iconPos.iX += delta;

			iIcons[j + iNumberOfColumns * i]->SetSize(iconSize);
			iIcons[j + iNumberOfColumns * i]->SetPosition(iconPos);
	        }
        }

	// determine highlight layouts
    layoutRect.LayoutRect( iParentRect, AknLayoutScalable_Apps::listscroll_imed_pane (variant) );
    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::cell_imed_effect_pane(0,0,variant) );
	iHighlightPos = layoutRect.Rect().iTl;
    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::grid_highlight_pane_cp017(variant) );
	TRect highlightRect = layoutRect.Rect();
	highlightRect.iTl.iX += delta;
	highlightRect.iBr.iX += delta;
    if( !AknLayoutUtils::LayoutMirrored() )
        {                  
	    AknLayoutUtils::LayoutControl( iHighlight[1], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g2().LayoutLine() );
	    AknLayoutUtils::LayoutControl( iHighlight[2], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g3().LayoutLine() );
	    AknLayoutUtils::LayoutControl( iHighlight[3], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g4().LayoutLine() );
	    AknLayoutUtils::LayoutControl( iHighlight[4], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g5().LayoutLine() );
	    AknLayoutUtils::LayoutControl( iHighlight[5], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g6().LayoutLine() );
	    AknLayoutUtils::LayoutControl( iHighlight[6], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g7().LayoutLine() );
        }
    else
        {
	    AknLayoutUtils::LayoutControl( iHighlight[1], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g3().LayoutLine() );
	    AknLayoutUtils::LayoutControl( iHighlight[2], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g2().LayoutLine() );
	    AknLayoutUtils::LayoutControl( iHighlight[3], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g5().LayoutLine() );
	    AknLayoutUtils::LayoutControl( iHighlight[4], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g4().LayoutLine() );
	    AknLayoutUtils::LayoutControl( iHighlight[5], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g7().LayoutLine() );
	    AknLayoutUtils::LayoutControl( iHighlight[6], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g6().LayoutLine() );
        }
    AknLayoutUtils::LayoutControl( iHighlight[0], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g1().LayoutLine() );
    AknLayoutUtils::LayoutControl( iHighlight[7], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g8().LayoutLine() );
    AknLayoutUtils::LayoutControl( iHighlight[8], highlightRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g9().LayoutLine() );
    // Images need to be resized separately, because the standard LayoutImage
    // tries to preserve the aspect ratio and we don't want that.
    //
    for ( TInt i = 0; i < iHighlight.Count(); ++i )
        {
        AknIconUtils::SetSize(
            const_cast<CFbsBitmap*>( iHighlight[i]->Bitmap() ),
            iHighlight[i]->Size(), EAspectRatioNotPreserved );
        }

	iHighlightPos = highlightRect.iTl - iHighlightPos;
    }

//=============================================================================    
void CPluginGrid::Draw(const TRect& /*aRect*/) const
    {
    //  Get graphics context
    CWindowGc & gc = SystemGc();

	//	Draw preview image if found
	if ( (*iBitmap)->Handle() )
	    {
		gc.BitBlt (Rect().iTl, *iBitmap);
        }
    
    TBool variant = Layout_Meta_Data::IsLandscapeOrientation();  
    TAknLayoutRect layoutRect;
    for ( TInt i = 0; i < iNumberOfRows; ++i )
        {
	    for ( TInt j = 0; j < iNumberOfColumns; ++j )
	        {
	        if(j + iNumberOfColumns * i == iCurrentItem)
	        	{
			    layoutRect.LayoutRect( iParentRect, AknLayoutScalable_Apps::listscroll_imed_pane (variant) );
			    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::cell_imed_effect_pane(j, i,variant) );
				TPoint deltaPos = iHighlight[1]->Position() - (layoutRect.Rect().iTl + iHighlightPos);

			    for ( TInt ii = 0; ii < iHighlight.Count(); ++ii )
					{
			            iHighlight[ii]->SetPosition(iHighlight[ii]->Position() - deltaPos);
			        }
	        	}
	        }
        }
    }

//=============================================================================    
TKeyResponse CPluginGrid::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    TKeyResponse res = EKeyWasNotConsumed;

    if (aType == EEventKey) 
        {
        // If in landscape mode, do different mapping for the number keys.
        TUint keyCode = aKeyEvent.iCode;
        TBool landscape = CResolutionUtil::Self()->GetLandscape();
        if (landscape)
        	{
            switch (keyCode)
                {
                case 49: // numeric keypad '1'
                    keyCode = 55;
                    break;
                case 50: // numeric keypad '2'
                    keyCode = 52;
                    break;
                case 51: // numeric keypad '3'
                    keyCode = 49;
                    break;
                case 52: // numeric keypad '4'
                    keyCode = 56;
                    break;
                case 54: // numeric keypad '6'
                    keyCode = 50;
                    break;
                case 55: // numeric keypad '7'
                    keyCode = 57;
                    break;
                case 56: // numeric keypad '8'
                    keyCode = 54;
                    break;
                case 57: // numeric keypad '9'
                    keyCode = 51;
                    break;
                default:
                    break;
                }
        	}

        switch (keyCode)
            {
            case EKeyLeftArrow:
            case 52: // numeric keypad '4'
                {
			    if( AknLayoutUtils::LayoutMirrored() )
			        {                  
	                iCurrentItem++;
	                if (iCurrentItem >= iPluginItems->Count())
	                    {
	                    iCurrentItem = 0; 
	                    }
			        }
				else
			        {                  
	                if (iCurrentItem == 0)
	                    {
	                    iCurrentItem = iPluginItems->Count() - 1; 
	                    }
	                else
	                    {
	                    iCurrentItem--;
	                    }
			        }

                res = EKeyWasConsumed;
                break;
                }

            case EKeyRightArrow:
            case 54: // numeric keypad '6'
                {
			    if( AknLayoutUtils::LayoutMirrored() )
			        {                  
	                if (iCurrentItem == 0)
	                    {
	                    iCurrentItem = iPluginItems->Count() - 1; 
	                    }
	                else
	                    {
	                    iCurrentItem--;
	                    }
			        }
				else
			        {                  
	                iCurrentItem++;
	                if (iCurrentItem >= iPluginItems->Count())
	                    {
	                    iCurrentItem = 0; 
	                    }
			        }

                res = EKeyWasConsumed;
                break;
                }

            case EKeyDownArrow:
            case 56: // numeric keypad '8'
                {
                iCurrentItem += iNumberOfColumns;
                if (iCurrentItem >= iPluginItems->Count())
                    {
                    iCurrentItem = iCurrentItem - iPluginItems->Count();
                    }

                res = EKeyWasConsumed;
                break;
                }

            case EKeyUpArrow:
            case 50: // numeric keypad '2'
                {
                iCurrentItem -= iNumberOfColumns;
                if (iCurrentItem < 0)
                    {
                    iCurrentItem = iPluginItems->Count() + iCurrentItem;
                    }

                res = EKeyWasConsumed;
                break;
                }

            case 49: // numeric keypad '1'
                {
                // Move up and left
                iCurrentItem -= iNumberOfColumns;
			    if( AknLayoutUtils::LayoutMirrored() )
			    if( AknLayoutUtils::LayoutMirrored() )
			        {                  
	                iCurrentItem ++;
			        }
				else
			        {                  
	                iCurrentItem --;
			        }
                if (iCurrentItem < 0)
                    {
                    iCurrentItem = iPluginItems->Count() + iCurrentItem;
                    }
                res = EKeyWasConsumed;
                break;
                }

            case 51: // numeric keypad '3'
                {
                // Move up and right
                iCurrentItem -= iNumberOfColumns;
			    if( AknLayoutUtils::LayoutMirrored() )
			        {                  
	                iCurrentItem --;
			        }
				else
			        {                  
	                iCurrentItem ++;
			        }
                if (iCurrentItem < 0)
                    {
                    iCurrentItem = iPluginItems->Count() + iCurrentItem;
                    }
                res = EKeyWasConsumed;
                break;
                }

            case 55: // numeric keypad '7'
                {
                // Move down and left
                iCurrentItem += iNumberOfColumns;
			    if( AknLayoutUtils::LayoutMirrored() )
			        {                  
	                iCurrentItem++;
			        }
				else
			        {                  
	                iCurrentItem--;
			        }
                if (iCurrentItem >= iPluginItems->Count())
                    {
                    iCurrentItem = iCurrentItem - iPluginItems->Count();
                    }
                res = EKeyWasConsumed;
                break;
                }

            case 57: // numeric keypad '9'
                {
                // Move down and left
                iCurrentItem += iNumberOfColumns;
			    if( AknLayoutUtils::LayoutMirrored() )
			        {                  
	                iCurrentItem--;
			        }
				else
			        {                  
	                iCurrentItem++;
			        }
                if (iCurrentItem >= iPluginItems->Count())
                    {
                    iCurrentItem = iCurrentItem - iPluginItems->Count();
                    }
                res = EKeyWasConsumed;
                break;
                }

             default:
                {
                break;
                }

            }

        }

    return res;
    }
    
//=============================================================================
void CPluginGrid::HandlePointerEventL( const TPointerEvent &aPointerEvent )
    {	
    if( AknLayoutUtils::PenEnabled() )
		{
		switch( aPointerEvent.iType )
			{
			case TPointerEvent::EButton1Down:
			case TPointerEvent::EDrag:
				{
				TBool variant = Layout_Meta_Data::IsLandscapeOrientation(); 	
			    TAknLayoutRect layoutRect;
			    for ( TInt i = 0; i < iNumberOfRows; ++i )
			        {
				    for ( TInt j = 0; j < iNumberOfColumns; ++j )
				        {
					    layoutRect.LayoutRect( iParentRect, AknLayoutScalable_Apps::listscroll_imed_pane (variant) );
					    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::cell_imed_effect_pane(j, i, variant) );

						if( layoutRect.Rect().Contains( aPointerEvent.iPosition ) )
							{
							// set new highlighted item
							iCurrentItem = j + iNumberOfColumns * i;
							return;
							}
				        }
			        }

				break;
				}
			case TPointerEvent::EButton1Up:
				{
				break;
				}
			default:
				{
				break;	
				}	
			}
		}
    }

//=============================================================================    
TRect CPluginGrid::GridRect() const
    {
    TRect boundingRect( iTouchGridRect );
//    boundingRect.BoundingRect( iTitleboxBorderRect );
    return boundingRect;
    }

//=============================================================================    
TBool CPluginGrid::HighlightedItemPressed( TPoint aPosition ) const
    {	
    TAknLayoutRect layoutRect;
    TBool variant = Layout_Meta_Data::IsLandscapeOrientation(); 
    for ( TInt i = 0; i < iNumberOfRows; ++i )
        {
	    for ( TInt j = 0; j < iNumberOfColumns; ++j )
	        {
	        if(j + iNumberOfColumns * i == iCurrentItem)
	        	{
			    layoutRect.LayoutRect( iParentRect, AknLayoutScalable_Apps::listscroll_imed_pane (variant) );
			    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::cell_imed_effect_pane(j, i, variant) );
	        	}
	        }
        }
                    
    if (layoutRect.Rect().Contains( aPosition ))  
        {
        return ETrue;
        }
    else 
        {
        return EFalse;
        }       
    }

//=============================================================================
void CPluginGrid::ShowTooltip ( CAknInfoPopupNoteController* aPopupController,
                                CCoeControl* aCallingControl )
    {
    TAknLayoutRect layoutRect;
    TBool variant = Layout_Meta_Data::IsLandscapeOrientation();
    
    TBool layoutFound = EFalse; 
    for ( TInt i = 0; i < iNumberOfRows; ++i )
        {
	    for ( TInt j = 0; j < iNumberOfColumns; ++j )
	        {
	        if(j + iNumberOfColumns * i == iCurrentItem)
	        	{
			    layoutRect.LayoutRect( iParentRect, AknLayoutScalable_Apps::listscroll_imed_pane (variant) );
			    layoutRect.LayoutRect( layoutRect.Rect(), AknLayoutScalable_Apps::cell_imed_effect_pane(j, i, variant) );
			    layoutFound = ETrue;
	        	}
	        }
        }
    
    if( layoutFound )    
        {
        CPluginInfo * pgn_info = ( *iPluginItems )[ iCurrentItem ];
    
        // change default time to shorter one
        aPopupController->SetTimeDelayBeforeShow( KTooltipDelayBeforeShow );
            
        SDrawUtils::ShowToolTip( aPopupController,
                                 aCallingControl,
                                 layoutRect.Rect(),
                                 pgn_info->PluginName()->Des() );    
        }
    
    }

//=============================================================================    
TInt CPluginGrid::CountComponentControls() const
    {
    TInt count = iBorders.Count();
    count += iHighlight.Count();
    count += iIcons.Count();
	count++;

    return count;
    }

//=============================================================================    
CCoeControl* CPluginGrid::ComponentControl(TInt aIndex) const
    {
    if ( aIndex < iBorders.Count() )
        {
        return iBorders[aIndex];
        }
    if ( aIndex < iBorders.Count() + iHighlight.Count())
        {
        return iHighlight[aIndex - iBorders.Count()];
        }
    if ( aIndex < iBorders.Count() + iHighlight.Count() + iIcons.Count())
        {
        return iIcons[aIndex - iBorders.Count() - iHighlight.Count()];
        }

    return iText;
    }

//=============================================================================
TInt CPluginSelectionDialog::RunDlgLD(CFbsBitmap** aBitmap, 
                                       TInt& aSelectedItem, 
                                       const RPluginArray* aItems,
                                       const TRect& aRect)
    {
    CPluginSelectionDialog* dialog = 
        new (ELeave) CPluginSelectionDialog (aBitmap, aSelectedItem, aItems);
    dialog->ConstructL(aRect);
    return dialog->ExecuteLD(R_IMAGE_EDITOR_PLUGIN_DIALOG);
    }

//=============================================================================
CPluginSelectionDialog::CPluginSelectionDialog(CFbsBitmap** aBitmap, 
                                               TInt& aSelectedItem, 
                                               const RPluginArray* aItems)
 : iBitmap(aBitmap), iItems(aItems), iSelectedItem(aSelectedItem)
    {

    }
//=============================================================================
CPluginSelectionDialog::~CPluginSelectionDialog()
    {
    delete iGrid;
    iBitmap = NULL;
    iItems = NULL;
    delete iPopupController;
    iPopupController = NULL;
    }

//=============================================================================

void CPluginSelectionDialog::ConstructL(const TRect& aRect)
    {
    SetRect(aRect);

    iGrid = new (ELeave) CPluginGrid(iBitmap, iItems);
    iGrid->ConstructL();

    iGrid->SetSize(aRect.Size());
    iGrid->SetPosition(TPoint(0,0));

    iPopupController = CAknInfoPopupNoteController::NewL(); 
    
#ifdef RD_TACTILE_FEEDBACK 
    iTouchFeedBack = MTouchFeedback::Instance();
#endif /* RD_TACTILE_FEEDBACK  */
    
    }

//=============================================================================
TBool CPluginSelectionDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyOk || aButtonId == EAknSoftkeySelect)
        {
        if (iGrid)
            {
            iSelectedItem = iGrid->GetSelectedItemIndex();
            }
        return ETrue;
        }
    else
        {
        iSelectedItem = KErrCancel;
        return EFalse;
        }
    }

//=============================================================================    
void CPluginSelectionDialog::Draw(const TRect& /*aRect*/) const
    {
    }

//=============================================================================    
void CPluginSelectionDialog::SizeChanged()
    {
    if (iGrid)
	    {
	    iGrid->SetRect( Rect() );
	    iGrid->SizeChanged();
	    
		if ( iPopupController )
		    {
		    // Shows the first tooltip when effect grid is entered
		    iGrid->ShowTooltip( iPopupController, this );
		    }		
	    }  
    }

//=============================================================================    
TInt CPluginSelectionDialog::CountComponentControls() const
    {
    return 1;
    }

//=============================================================================    
CCoeControl* CPluginSelectionDialog::ComponentControl(TInt /*aIndex*/) const
    {
    return iGrid;
    }

//=============================================================================    
TKeyResponse CPluginSelectionDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
    {
    // Selection key or numeric keypad '5' close the dialog
    TKeyResponse res = EKeyWasNotConsumed;
    if (aType == EEventKey && 
        (aKeyEvent.iCode == EKeyOK || aKeyEvent.iCode == 53 || aKeyEvent.iScanCode == EStdKeyEnter) )
        {
        TryExitL (EAknSoftkeyOk);
        res = EKeyWasConsumed;
        }
    else if (aType == EEventKey && aKeyEvent.iCode == EKeyNo || aKeyEvent.iCode == EKeyEscape) // Do not exit if dialog active
		{
		TryExitL (EAknSoftkeyCancel);
		res = EKeyWasNotConsumed;
        }
#ifdef FULLSCREEN_AVAILABLE        
    else if (aType == EEventKey && aKeyEvent.iCode == 48 ) // 0             
      {
      // Switch normal screen / full screen
      if (CResolutionUtil::Self()->GetFullScreen())
          {
          iEikonEnv->EikAppUi()->HandleCommandL(EImageEditorMenuCmdNormalScreen);
	      SetRect(iEikonEnv->EikAppUi()->ClientRect());
	      SizeChanged();
	      DrawNow();
          }
      else
	      {      
	      iEikonEnv->EikAppUi()->HandleCommandL(EImageEditorMenuCmdFullScreen);			  
	      SetExtentToWholeScreen();
	      SizeChanged();
	      DrawNow();
	      }              
      res = EKeyWasConsumed;              
      }
#endif // FULLSCREEN_AVAILABLE      						 
    else 
        {
        res = iGrid->OfferKeyEventL(aKeyEvent, aType);
        if (res == EKeyWasConsumed)
            {
            DrawDeferred();
            iGrid->ShowTooltip( iPopupController, this );
            }

        }
        
    return res;
    }

//=============================================================================
void CPluginSelectionDialog::HandlePointerEventL( 
                                           const TPointerEvent &aPointerEvent )
    {
    if( AknLayoutUtils::PenEnabled() )
	    {
        TBool draw( EFalse );
        switch( aPointerEvent.iType )
			{
			case TPointerEvent::EButton1Down:
				{
			    // forward event only if grid area was pressed    
			    if ( iGrid->GridRect().Contains( aPointerEvent.iPosition ) )
			        {
			        iPopupController->HideInfoPopupNote();
#ifdef RD_TACTILE_FEEDBACK 
					if ( iTouchFeedBack )
	                	{
			            iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
			            RDebug::Printf( "ImageEditor::CPluginSelectionDialog: ETouchFeedback" );
	                	}
#endif /* RD_TACTILE_FEEDBACK  */
			        iGrid->HandlePointerEventL( aPointerEvent );			        
			        draw = ETrue;
			        }    
		        else
		            {
		            // Cancel if tapped outside of the grid
		            TryExitL( EAknSoftkeyCancel );
		            }
				break;
				}
			case TPointerEvent::EDrag:
				{
			    if ( iGrid->GridRect().Contains( aPointerEvent.iPosition ) )
			        {
			        TBool highlightedBefore = 
			            iGrid->HighlightedItemPressed( aPointerEvent.iPosition );
			        
			        iGrid->HandlePointerEventL( aPointerEvent );
			        
			        TBool highlightedAfter = 
			            iGrid->HighlightedItemPressed( aPointerEvent.iPosition );
			        
			        // if these two values differ from each other, it indicates
			        // that focus has changed in the grid to a new item    
			        if ( highlightedBefore != highlightedAfter )
			            {
#ifdef RD_TACTILE_FEEDBACK 
                        if ( iTouchFeedBack )
                            {
                            iTouchFeedBack->InstantFeedback( ETouchFeedbackSensitive );
                            RDebug::Printf( "ImageEditor::CPluginSelectionDialog: ETouchFeedbackSensitive" );
                            }
#endif /* RD_TACTILE_FEEDBACK  */
			        
			            draw = ETrue;			                
			            }			        			        
			        }
				break;		
				}
			case TPointerEvent::EButton1Up:
				{
				// already highlighted item is pressed
			    if ( iGrid->HighlightedItemPressed( aPointerEvent.iPosition ) )
			        {
			        TryExitL (EAknSoftkeyOk);
			        }
				break;
				}
			default:
				{
				break;	
				}	
			}
			
	    if ( draw )
	        {
		    DrawDeferred();
		    iGrid->ShowTooltip( iPopupController, this );
	        }	
        }
    }
    
// End of File
