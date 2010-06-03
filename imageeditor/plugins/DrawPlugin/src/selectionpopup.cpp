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
* Selection popup dialog.
*
*/


// INCLUDES

#include <avkon.hrh>
#include <AknLayoutScalable_Avkon.cdl.h>
#include <AknLayoutScalable_Apps.cdl.h>
#include <fbs.h>
#include <AknIconUtils.h> 
#include <AknUtils.h> 

#include <AknBidiTextUtils.h>
#include <BidiText.h> 

#include <ImageEditorUi.mbg>
#include <ImageEditorUI.rsg>
#include <draw.rsg>
#include <drawpath.h>

#include "ResolutionUtil.h"
#include "selectionpopup.h"
#include "ImageEditorUiDefs.h"

// CONSTANTS
const TInt KGridColumnCount = 5;
const TInt KGridRowCount = 1;
const TInt KPenSizeOffset = 153;


//=============================================================================
CSelectionPopup::CSelectionPopup(TRgb& aCurrentColor, const TRect& aImageRect)
    {    
    // Set values into an array
    iItemArray.Append( 2 ); //default: 4
    iItemArray.Append( 4 );
    iItemArray.Append( 8 );
    iItemArray.Append( 16 );
    iItemArray.Append( 32 );
    
    //set defalut color
    iCurrentColor = aCurrentColor;
    
    iImageRect = aImageRect;
    }

//=============================================================================
CSelectionPopup::~CSelectionPopup()
    {
    delete iPopupCenter;
    delete iPopupCenterMask;
    delete iPopupCornerTL;
    delete iPopupCornerTLMask;
    delete iPopupCornerTR;
    delete iPopupCornerTRMask;
    delete iPopupCornerBL;
    delete iPopupCornerBLMask;
    delete iPopupCornerBR;
    delete iPopupCornerBRMask;
    delete iPopupSideL;
    delete iPopupSideLMask;
    delete iPopupSideR;
    delete iPopupSideRMask;
    delete iPopupSideT;
    delete iPopupSideTMask;
    delete iPopupSideB;
    delete iPopupSideBMask;     
           
    iHighlightRectsArray.Close();
    iGridRectsArray.Close();
    iColorBitmapsArray.ResetAndDestroy();
    iItemArray.Close();
    }

//=============================================================================
void CSelectionPopup::ConstructL (	CCoeControl* aParent )
    {
	// Set parent
	SetContainerWindowL( *aParent );  

    TFileName iconFile( KImageEditorUiMifFile );
    
    // Create trans popup window graphics
    // Center
    AknIconUtils::CreateIconL( iPopupCenter, iPopupCenterMask, iconFile, 
            EMbmImageeditoruiQgn_graf_popup_trans_center, 
			EMbmImageeditoruiQgn_graf_popup_trans_center_mask );
	
	// Top-left corner
    AknIconUtils::CreateIconL( iPopupCornerTL, iPopupCornerTLMask, iconFile, 
            EMbmImageeditoruiQgn_graf_popup_trans_corner_tl, 
			EMbmImageeditoruiQgn_graf_popup_trans_corner_tl_mask );		
    
    // Top-right corner
    AknIconUtils::CreateIconL( iPopupCornerTR, iPopupCornerTRMask, iconFile, 
            EMbmImageeditoruiQgn_graf_popup_trans_corner_tr, 
			EMbmImageeditoruiQgn_graf_popup_trans_corner_tr_mask );
    
    // Bottom-left corner
    AknIconUtils::CreateIconL( iPopupCornerBL, iPopupCornerBLMask, iconFile, 
            EMbmImageeditoruiQgn_graf_popup_trans_corner_bl, 
			EMbmImageeditoruiQgn_graf_popup_trans_corner_bl_mask );
			
	// Bottom-right corner
    AknIconUtils::CreateIconL( iPopupCornerBR, iPopupCornerBRMask, iconFile, 
            EMbmImageeditoruiQgn_graf_popup_trans_corner_br, 
			EMbmImageeditoruiQgn_graf_popup_trans_corner_br_mask );
    
    // left side
    AknIconUtils::CreateIconL( iPopupSideL, iPopupSideLMask, iconFile, 
            EMbmImageeditoruiQgn_graf_popup_trans_side_l, 
			EMbmImageeditoruiQgn_graf_popup_trans_side_l_mask );
			
	// left side
    AknIconUtils::CreateIconL( iPopupSideR, iPopupSideRMask, iconFile, 
            EMbmImageeditoruiQgn_graf_popup_trans_side_r, 
			EMbmImageeditoruiQgn_graf_popup_trans_side_r_mask );
	
	// top side
    AknIconUtils::CreateIconL( iPopupSideT, iPopupSideTMask, iconFile, 
            EMbmImageeditoruiQgn_graf_popup_trans_side_t, 
			EMbmImageeditoruiQgn_graf_popup_trans_side_t_mask );
	
	// bottom side
    AknIconUtils::CreateIconL( iPopupSideB, iPopupSideBMask, iconFile, 
            EMbmImageeditoruiQgn_graf_popup_trans_side_b, 
			EMbmImageeditoruiQgn_graf_popup_trans_side_b_mask );						                                   
                   					
	//	Activate control
    ActivateL();
    
    }

//=============================================================================
void CSelectionPopup::CreateColorBitmapsL( TSize /* aSize */ )
    {    
    //TODO: to set icon here
    // Generate color bitmaps    
    iColorBitmapsArray.Reset(); 
    
    TSize size(2,2);
    for( TInt i = iItemArray.Count(); i > 0 ; i-- )
        {
        CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
        CleanupStack::PushL( bitmap );
        User::LeaveIfError ( bitmap->Create ( size, EColor64K ) );
        size.iHeight*=2;
        size.iWidth*=2;
        //  Compute color for bitmap
        TRgb color( iCurrentColor );//iRGBArray[i] );		
         
    	//	Fill the bitmap with the correct color
    	TUint16 color_64k = ( TUint16 )
    		(0 | 
    		((( TUint8 )color.Red() >> 3) << 11) | 
    		((( TUint8 )color.Green() >> 2) << 5) | 
    		(( TUint8 )color.Blue() >> 3));

        bitmap->LockHeapLC();
    	TUint16 * pd = (TUint16 *)bitmap->DataAddress();
    	TSize size = bitmap->SizeInPixels();
    	TInt w = (bitmap->ScanLineLength(size.iWidth, EColor64K) >> 1) 
    	                                                * size.iHeight;
    	for( TInt k = w; k != 0; --k)
    	    {
    		*pd++ = color_64k;
    	    }
    	
    	iColorBitmapsArray.Append( bitmap );
    	
        CleanupStack::PopAndDestroy(); // LockHeapLC()
        CleanupStack::Pop(); // bitmap
                
        }    
    
    }
    
//=============================================================================
void CSelectionPopup::Draw( const TRect& aRect ) const
    {
    const TRgb KHighlightCenterColor (229, 229, 229);
    
    CWindowGc& gc = SystemGc();
     
    // Change brush to null to enable transparent drawing
    gc.SetBrushStyle (CGraphicsContext::ENullBrush);
                
    // draw center of the borders
    gc.BitBltMasked( iPopupCenterRect.iTl, iPopupCenter, 
                     iPopupCenterRect.Size(), iPopupCenterMask, EFalse );    
    // draw Top left corner
	gc.BitBltMasked( iPopupCornerTLRect.iTl, iPopupCornerTL, 
	                 iPopupCornerTLRect.Size(), iPopupCornerTLMask, EFalse );
	// draw Top right corner		
	gc.BitBltMasked( iPopupCornerTRRect.iTl, iPopupCornerTR, 
	                 iPopupCornerTRRect.Size(), iPopupCornerTRMask, EFalse );			
    // draw Bottom left corner
	gc.BitBltMasked( iPopupCornerBLRect.iTl, iPopupCornerBL, 
	                 iPopupCornerBLRect.Size(), iPopupCornerBLMask, EFalse );
	// draw Bottom right corner		
	gc.BitBltMasked( iPopupCornerBRRect.iTl, iPopupCornerBR, 
	                 iPopupCornerBRRect.Size(), iPopupCornerBRMask, EFalse );
    // draw Left side
	gc.BitBltMasked( iPopupSideLRect.iTl, iPopupSideL, 
	                 iPopupSideLRect.Size(), iPopupSideLMask, EFalse );
	// draw Right side		
    gc.BitBltMasked( iPopupSideRRect.iTl, iPopupSideR, 
                     iPopupSideRRect.Size(), iPopupSideRMask, EFalse );			
    // draw Top side
    gc.BitBltMasked( iPopupSideTRect.iTl, iPopupSideT, 
                     iPopupSideTRect.Size(), iPopupSideTMask, EFalse );			
    // draw Bottom side
    gc.BitBltMasked( iPopupSideBRect.iTl, iPopupSideB, 
                     iPopupSideBRect.Size(), iPopupSideBMask, EFalse );			
    
    // Draw color items and selection indicator
    for( TInt index = 0; index < iGridRectsArray.Count(); index++ )
        {
        // Draw selection indicator for current item
        if( index == iCurrentItem )
            {
            gc.SetBrushStyle( CGraphicsContext::ESolidBrush );
            gc.SetBrushColor( KHighlightCenterColor );
            gc.SetPenColor( KRgbBlack );
            gc.SetPenSize( TSize(2,2) );     
            gc.DrawRect( iHighlightRectsArray[ iCurrentItem ] );

            gc.SetBrushStyle (CGraphicsContext::ENullBrush);            
            }
            
        // draw color bitmap    
        TRect currentRect = iGridRectsArray[index];
        currentRect.iTl.iX = currentRect.iTl.iX + (currentRect.iBr.iX - currentRect.iTl.iX)/2 - iItemArray[index]/2;
        currentRect.iTl.iY = currentRect.iTl.iY + (currentRect.iBr.iY - currentRect.iTl.iY)/2 - iItemArray[index]/2;
        gc.BitBlt(currentRect.iTl, iColorBitmapsArray[index]);
        }
                  
     // Draw heading  
     // Convert the plug-in name to visual representation
     // (in case would be right-to-left)
     const CFont* font = NULL;
     font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimaryFont );
     TBuf<256> visual;            
     // Text to be converted
     HBufC * heading = ControlEnv()->AllocReadResourceLC ( R_TITLE_LINE_WEIGHT );
     
     AknBidiTextUtils::ConvertToVisualAndClip( 
         heading->Des(), 
         visual,
         *font,
         iHeadingRect.Width(),
         iHeadingRect.Width()
         );
        
     CleanupStack::PopAndDestroy( heading );
        
     gc.UseFont( font );
     gc.SetBrushStyle ( CGraphicsContext::ENullBrush );

     TInt baseline = iHeadingRect.Height() / 2 + font->AscentInPixels() / 2; 

     // Check the directionality of the current language and set the alignment
     // according the directionality
     TBidiText::TDirectionality dir = TBidiText::ScriptDirectionality(User::Language());
     CGraphicsContext::TTextAlign align = CGraphicsContext::ELeft;

     if (dir == TBidiText::ERightToLeft)
		{
		align = CGraphicsContext::ERight;
		}
	gc.SetPenColor(KRgbWhite);
	gc.DrawText(visual, iHeadingRect, baseline, align, 0);

    }
    
//=============================================================================    
void CSelectionPopup::SizeChanged()
    {
    
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( Rect(), AknLayoutScalable_Apps::popup_imed_trans_window(0) );
    TRect parentRect = layoutRect.Rect();
    
    if(parentRect.iTl.iX > parentRect.iTl.iY)
    	{
    	 parentRect.SetRect(parentRect.iTl.iX-KPenSizeOffset,parentRect.iTl.iY,parentRect.iBr.iX-KPenSizeOffset,parentRect.iBr.iY);      
    	}
   
	TAknLayoutRect iconLayout;
	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g1());
	iPopupCenterRect = iconLayout.Rect();
	
	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g2());
	iPopupCornerTLRect = iconLayout.Rect();
	
	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g3());
	iPopupCornerTRRect = iconLayout.Rect();
	
	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g4());
	iPopupCornerBLRect = iconLayout.Rect();
	
	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g5());
	iPopupCornerBRRect = iconLayout.Rect();
	
	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g6());
	iPopupSideLRect = iconLayout.Rect();
	
	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g7());
	iPopupSideRRect = iconLayout.Rect();
	
	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g8());
	iPopupSideTRect = iconLayout.Rect();
	
	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g9());
	iPopupSideBRect = iconLayout.Rect();
		
	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Apps::listscroll_imed_pane());
	TRect listscrollPaneRect = iconLayout.Rect();
	
	iconLayout.LayoutRect( listscrollPaneRect, AknLayoutScalable_Apps::grid_imed_colour_35_pane( 1 ));
	TRect gridPaneRect = iconLayout.Rect();
	
	iHighlightRectsArray.Reset();
	iGridRectsArray.Reset();
	for( TInt row = 0; row < KGridRowCount; row++ )
	    {
	    for( TInt column = 0; column < KGridColumnCount; column++ )
	        {
	        iconLayout.LayoutRect( gridPaneRect, 
	                               AknLayoutScalable_Apps::cell_imed_colour_pane(0 , column, row ));
	        TRect highlightRect = iconLayout.Rect();
	        iHighlightRectsArray.Append( highlightRect );
	        
	        iconLayout.LayoutRect( highlightRect, 
	                               AknLayoutScalable_Apps::cell_imed_colour_pane_g1(0) );	       
	        
	        iGridRectsArray.Append(iconLayout.Rect());
	        }
	    }	

    CreateColorBitmapsL( iconLayout.Rect().Size() );
    
    //Heading rect
	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Apps::heading_imed_pane());
	TRect heading = iconLayout.Rect();
	
    TAknLayoutText layoutText;
    layoutText.LayoutText( heading, AknLayoutScalable_Apps::heading_imed_pane_t1() );
    iHeadingRect = layoutText.TextRect();
    iLayoutTextHeading = layoutText;            		
    
    // Resize graphics to correct size
	AknIconUtils::SetSize( iPopupCenter, iPopupCenterRect.Size(), EAspectRatioNotPreserved);
	AknIconUtils::SetSize( iPopupCornerTL, iPopupCornerTLRect.Size(), EAspectRatioNotPreserved);
	AknIconUtils::SetSize( iPopupCornerTR, iPopupCornerTRRect.Size(), EAspectRatioNotPreserved);
	AknIconUtils::SetSize( iPopupCornerBL, iPopupCornerBLRect.Size(), EAspectRatioNotPreserved);
	AknIconUtils::SetSize( iPopupCornerBR, iPopupCornerBRRect.Size(), EAspectRatioNotPreserved);
	AknIconUtils::SetSize( iPopupSideL, iPopupSideLRect.Size(), EAspectRatioNotPreserved);
	AknIconUtils::SetSize( iPopupSideR, iPopupSideRRect.Size(), EAspectRatioNotPreserved);
    AknIconUtils::SetSize( iPopupSideT, iPopupSideTRect.Size(), EAspectRatioNotPreserved);
    AknIconUtils::SetSize( iPopupSideB, iPopupSideBRect.Size(), EAspectRatioNotPreserved);
	
    }

//=============================================================================      
TKeyResponse CSelectionPopup::OfferKeyEventL( const TKeyEvent& aKeyEvent,
                                                   TEventCode aType )
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
                if (iCurrentItem == 0)
                    {
                    iCurrentItem = iGridRectsArray.Count() - 1; 
                    }
                else
                    {
                    iCurrentItem--;
                    }

                res = EKeyWasConsumed;
                break;
                }

            case EKeyRightArrow:
            case 54: // numeric keypad '6'
                {
                iCurrentItem++;
                if (iCurrentItem >= iGridRectsArray.Count())
                    {
                    iCurrentItem = 0; 
                    }

                res = EKeyWasConsumed;
                break;
                }

            case EKeyDownArrow:
            case 56: // numeric keypad '8'
                {
                iCurrentItem += KGridColumnCount;
                if (iCurrentItem >= iGridRectsArray.Count())
                    {
                    iCurrentItem = iCurrentItem - iGridRectsArray.Count();
                    }

                res = EKeyWasConsumed;
                break;
                }

            case EKeyUpArrow:
            case 50: // numeric keypad '2'
                {
                iCurrentItem -= KGridColumnCount;
                if (iCurrentItem < 0)
                    {
                    iCurrentItem = iGridRectsArray.Count() + iCurrentItem;
                    }

                res = EKeyWasConsumed;
                break;
                }

            case 49: // numeric keypad '1'
                {
                // Move up and left
                iCurrentItem -= KGridColumnCount;
                iCurrentItem --;
                if (iCurrentItem < 0)
                    {
                    iCurrentItem = iGridRectsArray.Count() + iCurrentItem;
                    }
                res = EKeyWasConsumed;
                break;
                }

            case 51: // numeric keypad '3'
                {
                // Move up and right
                iCurrentItem -= KGridColumnCount;
                iCurrentItem ++;
                if (iCurrentItem < 0)
                    {
                    iCurrentItem = iGridRectsArray.Count() + iCurrentItem;
                    }
                res = EKeyWasConsumed;
                break;
                }

            case 55: // numeric keypad '7'
                {
                // Move down and left
                iCurrentItem += KGridColumnCount;
                iCurrentItem--;
                if (iCurrentItem >= iGridRectsArray.Count())
                    {
                    iCurrentItem = iCurrentItem - iGridRectsArray.Count();
                    }
                res = EKeyWasConsumed;
                break;
                }

            case 57: // numeric keypad '9'
                {
                // Move down and left
                iCurrentItem += KGridColumnCount;
                iCurrentItem++;
                if (iCurrentItem >= iGridRectsArray.Count())
                    {
                    iCurrentItem = iCurrentItem - iGridRectsArray.Count();
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
void CSelectionPopup::HandlePointerEventL( const TPointerEvent &aPointerEvent )
    {
    if( AknLayoutUtils::PenEnabled() )
		{
		switch( aPointerEvent.iType )
			{
			case TPointerEvent::EButton1Down:
			case TPointerEvent::EDrag:
				{	
				// calculate which one of the grid items was pressed by
				// by comparing item rects and pressed position						
				for (TInt i = 0; i < iHighlightRectsArray.Count(); ++i)
                    {
                    TRect iconRect( iHighlightRectsArray[i] );
                    
                    if( iconRect.Contains( aPointerEvent.iPosition ) )
                        {
                        // set new highlighted item
                        iCurrentItem = i;
                        break;
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
TRect CSelectionPopup::GridRect() const
    {    
    return iPopupCenterRect;
    }

//=============================================================================    
TBool CSelectionPopup::HighlightedItemPressed( TPoint aPosition ) const
    {	    
	TRect iconRect = iHighlightRectsArray[ iCurrentItem ];		
	                   
    if (iconRect.Contains( aPosition ))  
        {
        return ETrue;
        }
    else 
        {
        return EFalse;
        }       
    }

//=============================================================================    
void CSelectionPopup::SetSelectedValue( TInt aSelected )
    {
    ASSERT( aSelected >= 0 );
    iCurrentItem = iItemArray.Find( aSelected );
    }

//=============================================================================    
TInt CSelectionPopup::GetSelectedValue() const
    {
    ASSERT( iCurrentItem >= 0 && iCurrentItem < iItemArray.Count() );
    return iItemArray[iCurrentItem];
    }


//=============================================================================
// DIALOG
//=============================================================================
TInt CSelectionDialog::RunDlgLD( const CFbsBitmap* aBitmap,
                                      const TRect& aRect, 
                                      TInt& aSelection,
                                      TRgb& aCurrentColor,
                                      const TRect& aImageRect)
    {
    CSelectionDialog* dialog = 
        new (ELeave) CSelectionDialog ( aSelection);
    dialog->ConstructL( aBitmap, aRect, aCurrentColor, aImageRect);
    return dialog->ExecuteLD( R_IMAGE_EDITOR_COLOR_DIALOG );
    }

//=============================================================================
CSelectionDialog::CSelectionDialog( TInt& aSelection)
: iCurrentValue( aSelection )
    {
    }
    
//=============================================================================
CSelectionDialog::~CSelectionDialog()
    {
    delete iPopup;
    }

//=============================================================================

void CSelectionDialog::ConstructL( const CFbsBitmap* aBitmap,	
                                        const TRect& aRect,
                                        TRgb& aCurrentColor,
                                        const TRect& aImageRect)
    {    
    iPopup = new ( ELeave ) CSelectionPopup(aCurrentColor, aImageRect);
    iPopup->SetSelectedValue(iCurrentValue);
    iPopup->ConstructL(this);
    
    SetRect(aRect);   
    
    // Not drawn correctly if calling SetRect()
    iPopup->SetSize(aRect.Size());
    iPopup->SetPosition(TPoint(0,0));
    iPopup->SetImageL( aBitmap ); // set the image which we are editing
    }

//=============================================================================
TBool CSelectionDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyOk || aButtonId == EAknSoftkeySelect)
        {
        iCurrentValue = iPopup->GetSelectedValue();
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

//=============================================================================    
void CSelectionDialog::Draw(const TRect& /*aRect*/) const
    {
    }

//=============================================================================    
void CSelectionDialog::SizeChanged()
    {  
    }

//=============================================================================    
void CSelectionDialog::PreLayoutDynInitL() 
    { 
    }


//=============================================================================    
TInt CSelectionDialog::CountComponentControls() const
    {
    return 1;
    }

//=============================================================================    
CCoeControl* CSelectionDialog::ComponentControl(TInt /*aIndex*/) const
    {
    return iPopup;
    }

//=============================================================================    
TKeyResponse CSelectionDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
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
        res = iPopup->OfferKeyEventL(aKeyEvent, aType);
        if (res == EKeyWasConsumed)
            {
            DrawDeferred();
            }
        }
         
    return res;
    }

//=============================================================================
void CSelectionDialog::HandlePointerEventL( 
                                           const TPointerEvent &aPointerEvent )
    {    
    if( AknLayoutUtils::PenEnabled() )
	    {
        switch( aPointerEvent.iType )
			{
			case TPointerEvent::EButton1Down:
				{				
			    // forward event only if grid area was pressed    
			    if ( iPopup->GridRect().Contains( aPointerEvent.iPosition ) )
			        {			        
			        iPopup->HandlePointerEventL( aPointerEvent );
			        }    		        
				break;
				}
			case TPointerEvent::EDrag:
				{
			    if ( iPopup->GridRect().Contains( aPointerEvent.iPosition ) )
			        {
			        iPopup->HandlePointerEventL( aPointerEvent );
			        }
				break;		
				}
			case TPointerEvent::EButton1Up:
				{
				// already highlighted item is pressed
			    if ( iPopup->HighlightedItemPressed( aPointerEvent.iPosition ) )
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
        }
    }
    
// End of file 

