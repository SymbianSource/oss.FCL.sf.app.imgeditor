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
* Color selection popup dialog that shows a color grid.
*
*/


// INCLUDES
#include <ImageEditorUI.rsg>
#include <avkon.hrh>
#include <AknLayoutScalable_Avkon.cdl.h>
#include <AknLayoutScalable_Apps.cdl.h>
#include <fbs.h>
#include <AknIconUtils.h> 
#include <AknUtils.h> 

#include <AknBidiTextUtils.h>
#include <BidiText.h> 

#ifdef RD_TACTILE_FEEDBACK 
#include <touchfeedback.h>
#endif /* RD_TACTILE_FEEDBACK  */

#include <ImageEditorUi.mbg>
#include <ImageEditorUI.rsg>

#include "ResolutionUtil.h"
#include "ColorSelectionPopup.h"
#include "ImageEditorUiDefs.h"

// CONSTANTS
const TInt KGridColumnCount = 7;
const TInt KGridRowCount = 5;
const TInt KInitialItemIndex = 0;

//=============================================================================
CColorSelectionPopup::CColorSelectionPopup()
    {
    
    }

//=============================================================================
CColorSelectionPopup::~CColorSelectionPopup()
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
    iRGBArray.Close();
    }

//=============================================================================
void CColorSelectionPopup::ConstructL (	CCoeControl* aParent )
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
                
    iCurrentItem = KInitialItemIndex;
        					
	//	Activate control
    ActivateL();
    
    }

//=============================================================================
void CColorSelectionPopup::CreateColorBitmapsL( TSize aSize )
    {
    // Color constants shown on the grid
    // R indicates the row index and C the column index
    const TRgb KColor_R1C1 (0, 0, 0);
    const TRgb KColor_R1C2 (65, 0, 175);
    const TRgb KColor_R1C3 (0, 30, 128);
    const TRgb KColor_R1C4 (0, 80, 100);
    const TRgb KColor_R1C5 (0, 65, 0);
    const TRgb KColor_R1C6 (85, 55, 0);
    const TRgb KColor_R1C7 (120, 0, 0);
    const TRgb KColor_R2C1 (75, 75, 75);
    const TRgb KColor_R2C2 (128, 0, 255);
    const TRgb KColor_R2C3 (0, 65, 175);
    const TRgb KColor_R2C4 (0, 145, 165);
    const TRgb KColor_R2C5 (0, 128, 0);
    const TRgb KColor_R2C6 (128, 65, 0);
    const TRgb KColor_R2C7 (128, 0, 65);
    const TRgb KColor_R3C1 (150, 150, 150);
    const TRgb KColor_R3C2 (255, 0, 128);
    const TRgb KColor_R3C3 (0, 50, 255);
    const TRgb KColor_R3C4 (0, 165, 255);
    const TRgb KColor_R3C5 (0, 255, 0);
    const TRgb KColor_R3C6 (255, 128, 0);
    const TRgb KColor_R3C7 (255, 0, 0);
    const TRgb KColor_R4C1 (225, 225, 225);
    const TRgb KColor_R4C2 (255, 0, 255);
    const TRgb KColor_R4C3 (50, 145, 255);
    const TRgb KColor_R4C4 (0, 255, 255);
    const TRgb KColor_R4C5 (128, 255, 0);
    const TRgb KColor_R4C6 (255, 255, 0);
    const TRgb KColor_R4C7 (255, 115, 115);
    const TRgb KColor_R5C1 (255, 255, 255);
    const TRgb KColor_R5C2 (255, 128, 255);
    const TRgb KColor_R5C3 (180, 210, 255);
    const TRgb KColor_R5C4 (128, 255, 255);
    const TRgb KColor_R5C5 (180, 255, 180);
    const TRgb KColor_R5C6 (255, 255, 128);
    const TRgb KColor_R5C7 (255, 128, 180);

    // Read rgb values into an array
    iRGBArray.Reset();
    iRGBArray.Append( KColor_R1C1 );
    iRGBArray.Append( KColor_R1C2 );
    iRGBArray.Append( KColor_R1C3 );
    iRGBArray.Append( KColor_R1C4 );
    iRGBArray.Append( KColor_R1C5 );
    iRGBArray.Append( KColor_R1C6 );
    iRGBArray.Append( KColor_R1C7 );
    iRGBArray.Append( KColor_R2C1 );
    iRGBArray.Append( KColor_R2C2 );
    iRGBArray.Append( KColor_R2C3 );
    iRGBArray.Append( KColor_R2C4 );
    iRGBArray.Append( KColor_R2C5 );
    iRGBArray.Append( KColor_R2C6 );
    iRGBArray.Append( KColor_R2C7 );
    iRGBArray.Append( KColor_R3C1 );
    iRGBArray.Append( KColor_R3C2 );
    iRGBArray.Append( KColor_R3C3 );
    iRGBArray.Append( KColor_R3C4 );
    iRGBArray.Append( KColor_R3C5 );
    iRGBArray.Append( KColor_R3C6 );
    iRGBArray.Append( KColor_R3C7 );
    iRGBArray.Append( KColor_R4C1 );
    iRGBArray.Append( KColor_R4C2 );
    iRGBArray.Append( KColor_R4C3 );
    iRGBArray.Append( KColor_R4C4 );
    iRGBArray.Append( KColor_R4C5 );
    iRGBArray.Append( KColor_R4C6 );
    iRGBArray.Append( KColor_R4C7 );
    iRGBArray.Append( KColor_R5C1 );
    iRGBArray.Append( KColor_R5C2 );
    iRGBArray.Append( KColor_R5C3 );
    iRGBArray.Append( KColor_R5C4 );
    iRGBArray.Append( KColor_R5C5 );
    iRGBArray.Append( KColor_R5C6 );
    iRGBArray.Append( KColor_R5C7 );    
    
    // Generate color bitmaps    
    iColorBitmapsArray.ResetAndDestroy();    
    for( TInt i = 0; i < iRGBArray.Count(); i++ )
        {
        CFbsBitmap* bitmap = new ( ELeave ) CFbsBitmap;
        CleanupStack::PushL( bitmap );
        User::LeaveIfError ( bitmap->Create ( aSize, EColor64K ) );

        //  Compute color for bitmap
        TRgb color( iRGBArray[i] );		
         
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
void CColorSelectionPopup::Draw( const TRect& /*aRect*/ ) const
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
            
            // a trick that sets the highligh more or less to correct position in 
            // right-to-left languages. 
            if ( !AknLayoutUtils::LayoutMirrored() )
                {
                gc.SetPenSize( TSize(2,2) );
                }
            else
                {
                gc.SetPenSize( TSize(1,1) );
                }    
                                 
            gc.DrawRect( iHighlightRectsArray[ iCurrentItem ] );
            
            gc.SetBrushStyle (CGraphicsContext::ENullBrush);            
            }
            
        // draw color bitmap    
        TRect currentRect = iGridRectsArray[index];
        gc.BitBlt(currentRect.iTl, iColorBitmapsArray[index]);
        }
                  
     // Draw heading  
     // Convert the plug-in name to visual representation
     // (in case would be right-to-left)
     const CFont* font = NULL;
     font = AknLayoutUtils::FontFromId( EAknLogicalFontPrimaryFont );
     TBuf<256> visual;            
     // Text to be converted
     HBufC * heading = CEikonEnv::Static()->AllocReadResourceLC ( R_SELECT_COLOUR );
     
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

     if( dir == TBidiText::ERightToLeft )
        {
        align = CGraphicsContext::ERight;
        }
        gc.SetPenColor(KRgbWhite);
        gc.DrawText(
            visual,
            iHeadingRect,
            baseline,
            align,
            0);
    }
    
//=============================================================================    
void CColorSelectionPopup::SizeChanged()
    {
    
    TAknLayoutRect layoutRect;
    layoutRect.LayoutRect( Rect(), AknLayoutScalable_Apps::popup_imed_trans_window(0) );
    TRect parentRect = layoutRect.Rect();
    
    TAknLayoutRect iconLayout;
    if ( !AknLayoutUtils::LayoutMirrored() )
        {        
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
        }
    else
        {        
    	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g1());
    	iPopupCenterRect = iconLayout.Rect();
    	
    	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g3());
    	iPopupCornerTLRect = iconLayout.Rect();
    	
    	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g2());
    	iPopupCornerTRRect = iconLayout.Rect();
    	
    	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g5());
    	iPopupCornerBLRect = iconLayout.Rect();
    	
    	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g4());
    	iPopupCornerBRRect = iconLayout.Rect();
    	
    	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g7());
    	iPopupSideLRect = iconLayout.Rect();
    	
    	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g6());
    	iPopupSideRRect = iconLayout.Rect();
    	
    	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g8());
    	iPopupSideTRect = iconLayout.Rect();
    	
    	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Avkon::bg_tb_trans_pane_g9());
    	iPopupSideBRect = iconLayout.Rect();    
        }	
        		
	iconLayout.LayoutRect( parentRect, AknLayoutScalable_Apps::listscroll_imed_pane());
	TRect listscrollPaneRect = iconLayout.Rect();
	
	iconLayout.LayoutRect( listscrollPaneRect, AknLayoutScalable_Apps::grid_imed_colour_35_pane( 1 ));
	TRect gridPaneRect = iconLayout.Rect();
	
	// Calculate grid's cell rects
	iHighlightRectsArray.Reset();
	iGridRectsArray.Reset();
	for( TInt row = 0; row < KGridRowCount; row++ )
	    {
	    for( TInt column = 0; column < KGridColumnCount; column++ )
	        {
	        iconLayout.LayoutRect( gridPaneRect, 
	                               AknLayoutScalable_Apps::cell_imed_colour_pane(0 , column, row ));
            TRect cellRect = iconLayout.Rect();
            
	        iconLayout.LayoutRect( cellRect, 
	                               AknLayoutScalable_Apps::hgihlgiht_grid_pane_cp016(0));
	                               
	        TRect highlightRect = iconLayout.Rect();
	        iHighlightRectsArray.Append( highlightRect );
	        
	        iconLayout.LayoutRect( cellRect, 
	                               AknLayoutScalable_Apps::cell_imed_colour_pane_g1(0) );	       
	        
	        iGridRectsArray.Append( iconLayout.Rect() );
	        
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
TKeyResponse CColorSelectionPopup::OfferKeyEventL( const TKeyEvent& aKeyEvent,
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
                if ( !AknLayoutUtils::LayoutMirrored() )
                    {
                    if (iCurrentItem == 0)
                        {
                        iCurrentItem = iGridRectsArray.Count() - 1; 
                        }
                    else
                        {
                        iCurrentItem--;
                        }
                    }
                else
                    {
                    iCurrentItem++;
                    if (iCurrentItem >= iGridRectsArray.Count())
                        {
                        iCurrentItem = 0; 
                        }
                    }
                                                            
                res = EKeyWasConsumed;
                break;
                }

            case EKeyRightArrow:
            case 54: // numeric keypad '6'
                {
                if ( !AknLayoutUtils::LayoutMirrored() )
                    {
                    iCurrentItem++;
                    if (iCurrentItem >= iGridRectsArray.Count())
                        {
                        iCurrentItem = 0; 
                        }
                    }
                else
                    {
                    if (iCurrentItem == 0)
                        {
                        iCurrentItem = iGridRectsArray.Count() - 1; 
                        }
                    else
                        {
                        iCurrentItem--;
                        }
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
void CColorSelectionPopup::HandlePointerEventL( const TPointerEvent &aPointerEvent )
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
TRect CColorSelectionPopup::GridRect() const
    {    
    return iPopupCenterRect;
    }

//=============================================================================    
TBool CColorSelectionPopup::HighlightedItemPressed( TPoint aPosition ) const
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
TRgb CColorSelectionPopup::GetSelectedColor() const
    {
    return iRGBArray[iCurrentItem];
    }

//=============================================================================    
void CColorSelectionPopup::SetSelectedColor(TRgb aRgb)
    {
    for( TInt i = 0; i < iRGBArray.Count(); i++ )
        {
		if (iRGBArray[i] == aRgb)
			{
			iCurrentItem = i;
			break;
			}
        }
    }


//=============================================================================
// DIALOG
//=============================================================================
TInt CColorSelectionDialog::RunDlgLD( const CFbsBitmap* aBitmap, 
                                      const TRect& aRect, 
                                      TRgb& aRgb )
    {
    CColorSelectionDialog* dialog = 
        new (ELeave) CColorSelectionDialog ( aRgb );
    dialog->ConstructL( aBitmap, aRect);
    return dialog->ExecuteLD( R_IMAGE_EDITOR_COLOR_DIALOG );
    }

//=============================================================================
CColorSelectionDialog::CColorSelectionDialog( TRgb& aRgb )
: iCurrentRgb( aRgb )
    {
    }
    
//=============================================================================
CColorSelectionDialog::~CColorSelectionDialog()
    {
    delete iPopup;
    }

//=============================================================================

void CColorSelectionDialog::ConstructL( const CFbsBitmap* aBitmap,
                                        const TRect& aRect )
    {    
    iPopup = new ( ELeave ) CColorSelectionPopup();
    iPopup->ConstructL( this );
    
    SetRect(aRect);   
    
    // Not drawn correctly if calling SetRect()
    iPopup->SetSize(aRect.Size());
    iPopup->SetPosition(TPoint(0,0));
    iPopup->SetImageL( aBitmap );
    iPopup->SetSelectedColor( iCurrentRgb );

#ifdef RD_TACTILE_FEEDBACK 
    iTouchFeedBack = MTouchFeedback::Instance();    
#endif /* RD_TACTILE_FEEDBACK  */     
    }

//=============================================================================
TBool CColorSelectionDialog::OkToExitL( TInt aButtonId )
    {
    if ( aButtonId == EAknSoftkeyOk || aButtonId == EAknSoftkeySelect)
        {
        iCurrentRgb = iPopup->GetSelectedColor();
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }

//=============================================================================    
void CColorSelectionDialog::Draw(const TRect& /*aRect*/) const
    {
    }

//=============================================================================    
void CColorSelectionDialog::SizeChanged()
    {  
    iPopup->SetSize( Rect().Size() );
    }

//=============================================================================    
void CColorSelectionDialog::PreLayoutDynInitL() 
    { 
    }


//=============================================================================    
TInt CColorSelectionDialog::CountComponentControls() const
    {
    return 1;
    }

//=============================================================================    
CCoeControl* CColorSelectionDialog::ComponentControl(TInt /*aIndex*/) const
    {
    return iPopup;
    }

//=============================================================================    
TKeyResponse CColorSelectionDialog::OfferKeyEventL(const TKeyEvent& aKeyEvent,TEventCode aType)
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
void CColorSelectionDialog::HandlePointerEventL( 
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
#ifdef RD_TACTILE_FEEDBACK 
					if ( iTouchFeedBack )
	                	{
			            iTouchFeedBack->InstantFeedback( ETouchFeedbackBasic );
			            RDebug::Printf( "ImageEditor::CColorSelectionDialog: ETouchFeedback" );
	                	}
#endif /* RD_TACTILE_FEEDBACK  */			        	        
			        iPopup->HandlePointerEventL( aPointerEvent );			        
			        }    		        
				break;
				}
			case TPointerEvent::EDrag:
				{
			    if ( iPopup->GridRect().Contains( aPointerEvent.iPosition ) )
			        {
#ifdef RD_TACTILE_FEEDBACK 
					if ( iTouchFeedBack )
	                	{
			            iTouchFeedBack->InstantFeedback( ETouchFeedbackSensitive );
			            RDebug::Printf( "ImageEditor::CColorSelectionDialog: ETouchFeedbackSensitive" );
	                	}
#endif /* RD_TACTILE_FEEDBACK  */
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

