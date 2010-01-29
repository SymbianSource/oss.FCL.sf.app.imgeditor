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


#include "CFilterCrop.h"

EXPORT_C TInt CFilterCrop::Create()
	{
	CFilterCrop* ptr = NULL;
	TRAPD( error, ptr = NewL(); );
	if( error != KErrNone )
		{
		ptr = NULL;
		}
	return (TInt)((MImageFilter*)ptr);
	}



CFilterCrop* CFilterCrop::NewL()
	{
	CFilterCrop* self = new( ELeave )CFilterCrop();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterCrop::~CFilterCrop()
	{
	}



CFilterCrop::CFilterCrop()
	{

	}



void CFilterCrop::ConstructL()
	{

	}



TRect CFilterCrop::Rect()
	{
	
	if ( iRect == TRect(0,0,0,0) )
		{
		return iChild->Rect();
		}
	else
		{
	    TReal relscale = iChild->Scale();
		TRect rect;
	    rect.iTl.iX = (TInt)(iRect.iTl.iX * relscale + 0.5);
	    rect.iTl.iY = (TInt)(iRect.iTl.iY * relscale + 0.5);
	    rect.iBr.iX = (TInt)(iRect.iBr.iX * relscale + 0.5);
	    rect.iBr.iY = (TInt)(iRect.iBr.iY * relscale + 0.5);
		rect.Intersection( iChild->Rect() );
		return rect;
		}
	}

TReal CFilterCrop::Scale()
	{
	return iChild->Scale();
	}

TSize CFilterCrop::ViewPortSize()
	{
	if (iRect == TRect(0,0,0,0))
		{
		return iChild->ViewPortSize();	
		}
	else
		{
		return iRect.Size();	
		}
	}

TBlock * CFilterCrop::GetBlockL ( const TRect & aRect )
	{
    return iChild->GetBlockL (aRect);
	}


void CFilterCrop::SetParent( MImageFilter* aParent )
	{
	iParent = aParent;
	}




void CFilterCrop::SetChild( MImageFilter* aChild )
	{
	iChild = aChild;
	}



TInt CFilterCrop::CmdL( const TDesC16& aCmd )
	{
	TLex lex( aCmd );

	while( ! lex.Eos() )
		{
		TPtrC token = lex.NextToken();
		if( token.Compare( _L("x1") ) == 0 )
			{
            TReal relscale = iChild->Scale();
            TInt param = 0;
			lex.Inc ();
			lex.Val (param);
            iRect.iTl.iX = (TInt)((param / relscale) + 0.5);
			}
		else if( token.Compare( _L("y1") ) == 0 )
			{
            TReal relscale = iChild->Scale();
            TInt param = 0;
			lex.Inc ();
			lex.Val (param);
            iRect.iTl.iY = (TInt)((param / relscale) + 0.5);
			}
		else if( token.Compare( _L("x2") ) == 0 )
			{
            TReal relscale = iChild->Scale();
            TInt param = 0;
			lex.Inc ();
			lex.Val (param);
            iRect.iBr.iX = (TInt)((param / relscale) + 0.5);
			}
		else if( token.Compare( _L("y2") ) == 0 )
			{
            TReal relscale = iChild->Scale();
            TInt param = 0;
			lex.Inc ();
			lex.Val (param);
            iRect.iBr.iY = (TInt)((param / relscale) + 0.5);
			}
		}
	return 0;
	}



const char* CFilterCrop::Type()
	{
	return "crop";
	}
	
#if !defined(EKA2)
GLDEF_C TInt E32Dll( TDllReason )
    {
    return KErrNone;
    }	
#endif
