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
 * Draw filter for Draw UI plugin.
 *
 */

#include <fbs.h>
#include <bitdev.h>
#include <e32math.h>
#include "cfilterdraw.h"

const TInt KDrawBitmapWidth(480);
const TInt KDrawBitmapHeight(640);
TUint32* CFilterDraw::iData = NULL;
// ---------------------------------------------------------------------------
// Create
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CFilterDraw::Create()
	{
	CFilterDraw* ptr = NULL;
	TRAP_IGNORE( ptr = NewL() );
	return (TInt) ((MImageFilter*) ptr);
	}

// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CFilterDraw* CFilterDraw::NewL()
	{
	return new (ELeave) CFilterDraw();
	}

// ---------------------------------------------------------------------------
// ~CFilterDraw
// ---------------------------------------------------------------------------
//
CFilterDraw::~CFilterDraw()
	{
	if(iData)
	{
	delete[] iData;
	iData = NULL;
	}
	iReadyToRender = EFalse;
	// Close all paths
	for (TInt i(0); i < iPaths.Count(); i++)
		{
		iPaths[i].Close();
		}
	iPaths.Close();
	
	for (TInt j(0); j < iUndoPaths.Count(); j++)
		{
		iUndoPaths[j].Close();
		}
	iUndoPaths.Close();
	
	if(iMask)
		{
		delete iMask;
		iMask =NULL;
		}
	if(iBitmap)
		{
		delete iBitmap;
		iBitmap =NULL;
		}
	}

// ---------------------------------------------------------------------------
// CFilterDraw
// ---------------------------------------------------------------------------
//
CFilterDraw::CFilterDraw() :
	 iBitmapSize(KDrawBitmapWidth, KDrawBitmapHeight), iCanRedo(
			EFalse)
	{
	}

// ---------------------------------------------------------------------------
// Rect
// ---------------------------------------------------------------------------
//
TRect CFilterDraw::Rect()
	{
	ASSERT(iChild);
	return iChild->Rect();
	}

// ---------------------------------------------------------------------------
// Scale
// ---------------------------------------------------------------------------
//
TReal CFilterDraw::Scale()
	{
	ASSERT(iChild);
	return iChild->Scale();
	}

// ---------------------------------------------------------------------------
// ViewPortSize
// ---------------------------------------------------------------------------
//
TSize CFilterDraw::ViewPortSize()
	{
	ASSERT(iChild);
	return iChild->ViewPortSize();
	}

// ---------------------------------------------------------------------------
// GetBlockL
// ---------------------------------------------------------------------------
//
TBlock* CFilterDraw::GetBlockL(const TRect& aRect)
	{
	ASSERT(iChild);
	TBlock* pB = iChild->GetBlockL(aRect);
	if (!pB)
		return NULL;
	TUint32* pD = pB->iData;

	if (iData && iReadyToRender)
		{
		// Factor between real and viewed image
		TReal realToViewedFactorWidth;
		TReal realToViewedFactorHeight;
		RealToViewedFactories(realToViewedFactorWidth, realToViewedFactorHeight);
		const TInt width(iBitmapSize.iWidth); // Data bitmap width

		for (TInt y(pB->iRect.iTl.iY); y < pB->iRect.iBr.iY; ++y)
			{
			TInt realY(y / Scale() + 0.5);
			TInt viewY(realY / realToViewedFactorHeight + 0.5);
			for (TInt x(pB->iRect.iTl.iX); x < pB->iRect.iBr.iX; ++x)
				{
				TInt realX(x / Scale() + 0.5);
				TInt viewX(realX / realToViewedFactorWidth + 0.5);

				if( viewY >= KDrawBitmapHeight)
					{
					viewY--;
					}
				if(iData[width * viewY + viewX])
					{
					TUint32 color = iData[width * viewY + viewX];
					if ((color & 0xff000000))
						{
						*pD = color;
						}
					}
				*pD++;
				}
			}
		}
	
	return pB;
	}

// ---------------------------------------------------------------------------
// SetParent
// ---------------------------------------------------------------------------
//
void CFilterDraw::SetParent(MImageFilter* aParent)
	{
	ASSERT(aParent);
	iParent = aParent;
	}

// ---------------------------------------------------------------------------
// SetChild
// ---------------------------------------------------------------------------
//
void CFilterDraw::SetChild(MImageFilter* aChild)
	{
	ASSERT(aChild);
	iChild = aChild;
	}

// ---------------------------------------------------------------------------
// CmdL
// ---------------------------------------------------------------------------
//
TInt CFilterDraw::CmdL(const TDesC16& aCmd)
	{
	TInt bbb(0);
	bbb = iPaths.Count();
	ASSERT(iChild);
	TLex lex(aCmd);
	TPoint position(0, 0);
	// Factor between real and viewed image
	TReal realToViewedFactorWidth;
	TReal realToViewedFactorHeight;
	RealToViewedFactories(realToViewedFactorWidth, realToViewedFactorHeight);

	//	Handle parameters
	while (!lex.Eos())
		{
		RDebug::RawPrint(aCmd);
		TPtrC token = lex.NextToken();
		TInt pathCount(0);
		pathCount = iPaths.Count();

		if (token.Compare(_L("x")) == 0)
			{
			iCanRedo = EFalse;
			for (TInt j(0); j < iUndoPaths.Count(); j++)
				{
				iUndoPaths[j].Close();
				}
			iUndoPaths.Close();
			iReadyToRender = EFalse;
			TReal relscale = Scale();
			TInt param = 0;
			lex.Inc();
			lex.Val(param);
			// Coordinates on data bitmap
			position.iX = (TReal(param) / relscale) / realToViewedFactorWidth + 0.5;
			}
		else if (token.Compare(_L("y")) == 0)
			{
			TReal relscale = Scale();
			TInt param = 0;
			lex.Inc();
			lex.Val(param);
			position.iY = (TReal(param) / relscale) / realToViewedFactorHeight + 0.5;
			RDebug::Print(
					_L("CFilterDraw::CmdL x:%d y:%d Scale:%g Rw:%d Rh:%d Vpw:%d Vph:%d Rtvw:%g Rtvh:%g"),
					position.iX, position.iY, relscale, Rect().Size().iWidth,
					Rect().Size().iHeight, ViewPortSize().iWidth,
					ViewPortSize().iHeight, realToViewedFactorWidth,
					realToViewedFactorHeight);

			if (!iPaths.Count())
				{
				RDrawPath newPath;
				User::LeaveIfError(newPath.Append(position));
				User::LeaveIfError(iPaths.Append(newPath));
				}
			else
				{
				ASSERT(iPaths.Count());
				RDrawPath& lastPath = iPaths[iPaths.Count() - 1];
				User::LeaveIfError(lastPath.Append(position));
				}
			}
		else if (token.Compare(_L("color")) == 0)
			{
			TUint32 color(0);
			lex.Inc();
			lex.Val(color, EDecimal);
			TRgb rgb(color);
			TUint32 colorValue = (rgb.Red() << 16) + (rgb.Green() << 8)	+ rgb.Blue();
			ASSERT( iPaths.Count() );
			RDrawPath& lastPath = iPaths[iPaths.Count() - 1];
			lastPath.SetColor(TRgb(colorValue));
			}
		else if (token.Compare(_L("size")) == 0)
			{
			TInt size(0);
			lex.Inc();
			lex.Val(size);
			// Scale line size to match bitmap scale
			TInt sizew( (TReal(size) / realToViewedFactorWidth) + 0.5 );
			TInt sizeh( (TReal(size) / realToViewedFactorHeight) + 0.5 );

			if (!sizew)
				{
				sizew++;
				}
			if (!sizeh)
				{
				sizeh++;
				}

			if (iPaths.Count())
				{
				RDrawPath& lastPath = iPaths[iPaths.Count() - 1];
				lastPath.SetSize(TSize(sizew, sizeh));
				}
			}
		else if (token.Compare(_L("lastItem")) == 0)
			{
			ASSERT(iPaths.Count());

			RDrawPath& lastPath = iPaths[iPaths.Count() - 1];
			RDebug::Print(
					_L("CFilterDraw::CmdL lastItem count:%d size:%d r:%d g:%d b:%d"),
					iPaths.Count(), lastPath.Size().iHeight,
					lastPath.Color().Red(), lastPath.Color().Green(),
					lastPath.Color().Blue());

			RDrawPath newPath;
			User::LeaveIfError(iPaths.Append(newPath));
			}
		else if (token.Compare(_L("done")) == 0)
			{
			LoadFrameL();
			iReadyToRender = ETrue;
			}
		//Undo functionality
		else if (token.Compare(_L("Undo")) == 0)
			{
			TInt count(0);
			count = iPaths.Count();
			count = iUndoPaths.Count();
			
			if (iPaths.Count() > 1)
				{
				TInt count = iPaths.Count();
				iUndoPaths.Append(iPaths[iPaths.Count() - 2]);
				iPaths.Remove(iPaths.Count() - 2);
				iCanRedo = ETrue;
				}
			count = iPaths.Count();
			count = iUndoPaths.Count();
			}
		//Redo functionality
		else if (token.Compare(_L("redone")) == 0)
			{
			TInt count(0);
			count = iPaths.Count();
			count = iUndoPaths.Count();

			if (iCanRedo)
				{
				if (iUndoPaths.Count())
					{
					if (iPaths.Count() >= 1)
						{
						iPaths.Insert(iUndoPaths[iUndoPaths.Count() - 1],iPaths.Count() - 1);
						iUndoPaths.Remove(iUndoPaths.Count() - 1);
						}
					}
				}
			count = iPaths.Count();
			count = iUndoPaths.Count();
			}
		}
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// Type
// ---------------------------------------------------------------------------
//
const char* CFilterDraw::Type()
	{
	return "frame";
	}

// ---------------------------------------------------------------------------
// LoadFrameL
// ---------------------------------------------------------------------------
//
void CFilterDraw::LoadFrameL()
	{
	RDebug::Print(_L("CFilterDraw::LoadFrameL w:%d h:%d"), iBitmapSize.iWidth,
			iBitmapSize.iHeight);
	
	TInt xxx(0);
	xxx = iPaths.Count();
	//  Create a bitmap big enough to hold the drawed lines
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(bitmap);
	User::LeaveIfError(bitmap->Create(iBitmapSize, EColor16MA));

	// create mask
	CFbsBitmap* mask = new (ELeave) CFbsBitmap();
	CleanupStack::PushL(mask);
	User::LeaveIfError(mask->Create(iBitmapSize, EColor16MA));

	CFbsBitmapDevice* maskDevice = CFbsBitmapDevice::NewL(mask);
	CleanupStack::PushL(maskDevice);

	CFbsBitGc * maskContext(NULL);
	User::LeaveIfError(maskDevice->CreateContext(maskContext));
	CleanupStack::PushL(maskContext);
	maskContext->SetPenStyle(CGraphicsContext::ESolidPen);
	maskContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
	maskContext->SetBrushColor(KRgbBlack);

	//  Create bitmap device and context
	CFbsBitmapDevice * bitmapDevice = CFbsBitmapDevice::NewL(bitmap);
	CleanupStack::PushL(bitmapDevice);

	//	Create bitmap graphics context
	CFbsBitGc * bitmapContext(NULL);
	User::LeaveIfError(bitmapDevice->CreateContext(bitmapContext));
	CleanupStack::PushL(bitmapContext);

	TDisplayMode dmode = bitmap->DisplayMode();

	for (TInt pathNumber(0); pathNumber < iPaths.Count(); pathNumber++)
		{
		RDrawPath path = iPaths[pathNumber];
		bitmapContext->SetPenStyle(CGraphicsContext::ESolidPen);
		bitmapContext->SetBrushStyle(CGraphicsContext::ESolidBrush);
		bitmapContext->SetPenColor(path.Color());
		bitmapContext->SetPenSize(path.Size());
		maskContext->SetPenSize(path.Size());

		RDebug::Print(_L("CFilterDraw::LoadFrameL ps:%dx%d S:%g"),
				path.Size().iWidth, path.Size().iHeight, Scale());

		CArrayFix<TPoint>* pointArray = NULL;
		RDrawPath2PointArray(path, pointArray);
		bitmapContext->DrawPolyLine(pointArray);
		maskContext->DrawPolyLine(pointArray);

		delete pointArray;
		}

	//	Create memory buffer to hold rendered image data
    
	if (!iData)
		{		
		iData = new (ELeave) TUint32[iBitmapSize.iWidth * iBitmapSize.iHeight];
		Mem::FillZ(iData, iBitmapSize.iWidth * iBitmapSize.iHeight
								* sizeof(TUint32));
		}
	
	TBitmapUtil bm(bitmap);
	bm.Begin(TPoint(0, 0));
	TBitmapUtil maskbm(mask);
	maskbm.Begin(TPoint(0, 0));
	TRgb rgb(0);
	
	// Find drawed lines from bitmap    
	for (TInt y(0); y < iBitmapSize.iHeight - 1; y++)
		{
		for (TInt x(0); x < iBitmapSize.iWidth - 1; x++)
			{
			// Check mask first
			maskbm.SetPos(TPoint(x, y));
			if (maskbm.GetPixel() == KRgbBlack.Internal())
				{
				bm.SetPos(TPoint(x, y));
				rgb = bm.GetPixel();
				iData[(iBitmapSize.iWidth * y) + x] = (rgb.Red() << 16)
						+ (rgb.Green() << 8) + rgb.Blue() | 0xff000000;
				}
			}
		}
	
	bm.End();
	maskbm.End();
	// bitmapContext, bitmapDevice, maskContext, maskDevice, mask, bitmap
	CleanupStack::PopAndDestroy(6, bitmap);
	RDebug::Print(_L("CFilterDraw::LoadFrameL - end"));
	}

// ---------------------------------------------------------------------------
// RDrawPath2PointArray
// ---------------------------------------------------------------------------
//
void CFilterDraw::RDrawPath2PointArray(const RDrawPath& aPath,
		CArrayFix<TPoint>*& aArrayPtr) const
	{
	// if allocation fails just do nothing. +1 if count is zero
	aArrayPtr = new CArrayFixFlat<TPoint> (aPath.Count() + 1);
	if (aArrayPtr)
		{
		for (TInt i(0); i < aPath.Count(); i++)
			{
			TPoint item = aPath[i];
			TRAP_IGNORE( aArrayPtr->AppendL( item ) );
			}
		}
	}

// ---------------------------------------------------------------------------
// RealToViewedFactories
// ---------------------------------------------------------------------------
//
void CFilterDraw::RealToViewedFactories(TReal& aWidth, TReal& aHeight)
	{
	// Factor between real and viewed image
	aWidth = TReal(ViewPortSize().iWidth) / TReal(iBitmapSize.iWidth);
	aHeight = TReal(ViewPortSize().iHeight) / TReal(iBitmapSize.iHeight);
	}

