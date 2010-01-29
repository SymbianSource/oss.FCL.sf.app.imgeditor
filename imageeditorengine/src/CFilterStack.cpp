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


#include "CFilterStack.h"
#include <f32file.h>

//  Debug logger definitions
#include "imageeditordebugutils.h"
_LIT(KMIAWrapperLogFile,"EngineWrapper.log");

CFilterStack* CFilterStack::NewL()
	{
	CFilterStack* self = new( ELeave )CFilterStack();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}



CFilterStack::~CFilterStack()
	{
	iStack.ResetAndDestroy();
	}



CFilterStack::CFilterStack()
	{

	}



void CFilterStack::ConstructL()
	{
	/*
	TFileName fileName( _L("c:\\filterscript.txt") );

	RFs fs;
	User::LeaveIfError( fs.Connect() );
	CleanupClosePushL( fs );

	RFile file;
	User::LeaveIfError( file.Open( fs, fileName, EFileRead ) );
	CleanupClosePushL( file );

	TInt l = 0;
	file.Size( l );
	TUint8* t = new TUint8[ l ];
	CleanupStack::PushL( t );
	TPtr8 txt8o( t, l );
	User::LeaveIfError( file.Read( txt8o ) );
	TPtrC8 txt8( txt8o );
	TInt strl = txt8.Find( _L8(" ") );

	while( strl > 0 )
		{
		TPtrC8 filterName( txt8.Ptr(), strl );
		txt8.Set( txt8.Mid( strl+1 ) );
		
		strl = txt8.Find( _L8("\r") );
		TPtrC8 filterParam( txt8.Ptr(), strl );
		txt8.Set( txt8.Mid( strl+1 ) );

		if( txt8.Length() > 0 )
			{
			if( txt8[ 0 ] == '\n' )
				{
				txt8.Set( txt8.Mid( 1 ) );
				}
			}

		strl = txt8.Find( _L8(" ") );

		TFilter* f = new( ELeave )TFilter;
		TInt error = KErrNone;

		if( filterName.Compare( _L8("jpegsource") ) == 0 )
			{
			error = f->iLibrary.Load( _L("FilterJpegSource.dll" ) );
			}
		else if( filterName.Compare( _L8("jpegtarget") ) == 0 )
			{
			error = f->iLibrary.Load( _L("FilterJpegTarget.dll" ) );
			}
		else if( filterName.Compare( _L8("negate") ) == 0 )
			{
			error = f->iLibrary.Load( _L("FilterNegate.dll" ) );
			}
		else if( filterName.Compare( _L8("buffer") ) == 0 )
			{
			error = f->iLibrary.Load( _L("FilterBuffer.dll" ) );
			}
		
		User::LeaveIfError( error );

		MImageFilter*& filter = f->iFilter;
		filter = (MImageFilter*)(f->iLibrary.Lookup( 1 )());
		
		if( filter == NULL )
			{
			delete f;
			User::Panic( _L("Could not load filter"), 0 );
			}
		
		f->iFilter = filter;
		iStack.Append( f );
		InitStack();
		TBuf< 100 > cmd16;
		cmd16.Copy( filterParam );
		filter->CmdL( cmd16 );
		}
	
	
	CleanupStack::PopAndDestroy( 3 ); // t, file, fs
	*/
	}



void CFilterStack::InitStack()
	{
	TInt c = iStack.Count();
	TInt i;
	for( i=0; i<c; i++ )
		{
		TInt childn = i-1;
		TInt parentn = i+1;
		if( childn >= 0 )
			{
			iStack[ i ]->iFilter->SetChild( iStack[ childn ]->iFilter );
			}
		if( parentn < c )
			{
			iStack[ i ]->iFilter->SetParent( iStack[ parentn ]->iFilter );
			}
		}
	}



MImageFilter* CFilterStack::Filter( TInt aPosition )
	{
	return iStack[ aPosition ]->iFilter;
	}



void CFilterStack::AddFilterL( const TDesC& aName )
	{
	AddFilterL( aName, iStack.Count() );
	}



void CFilterStack::AddFilterL( const TDesC& aName, TInt aPosition )
	{
	LOG ( KMIAWrapperLogFile, "CFilterStack::AddFilterL" );

	TFilter* f = new( ELeave )TFilter;
	CleanupStack::PushL(f);
	
	LOG ( KMIAWrapperLogFile, "CFilterStack::AddFilterL: Loading" );
	User::LeaveIfError( f->iLibrary.Load( aName ) );
	MImageFilter*& filter = f->iFilter;

	LOG ( KMIAWrapperLogFile, "CFilterStack::AddFilterL: Lookup" );
	filter = (MImageFilter*)(f->iLibrary.Lookup( 1 )());
	if( filter == NULL )
		{
		LOG ( KMIAWrapperLogFile, "CFilterStack::AddFilterL: Filter null" );
		f->iLibrary.Close();
		User::Leave( KErrNoMemory );
		}
	f->iFilter = filter;

	if( iStack.Count() == aPosition )
		{
		iStack.Append( f );
		}
	else
		{
		/*
		iStack.Append( 0 );

		TInt s = aPosition;
		TInt e = iStack.Count()-2;
		TInt i;
		for( i=s; i<e; i++ )
			{
			iStack[ i+1 ] = iStack[ i ];
			}
			*/
		iStack.Insert( f, aPosition );
		}
	CleanupStack::Pop();
	LOG ( KMIAWrapperLogFile, "CFilterStack::AddFilterL: Initializing stack" );
		
	InitStack();
	}

void CFilterStack::AddFilterL( const TFilter * aFilter )
{
	AddFilterL( aFilter, iStack.Count() );
}

void CFilterStack::AddFilterL( const TFilter * aFilter, const TInt aPosition )
{
	LOG ( KMIAWrapperLogFile, "CFilterStack::AddFilterL" );
	if ( iStack.Count() == aPosition )
	{
		iStack.Append( aFilter );
	}
	else
	{
		iStack.Insert( aFilter, aPosition );
	}

	LOG ( KMIAWrapperLogFile, "CFilterStack::AddFilterL: Initializing stack" );
		
	InitStack();
}


void CFilterStack::RemoveFilter( TInt aPosition )
	{
	delete iStack[ aPosition ];
	iStack.Remove( aPosition );
	InitStack();
	}

void CFilterStack::RemoveFilter ( TInt aPosition, TFilter *& aFilter )
{
	aFilter = iStack[aPosition];
	iStack.Remove( aPosition );
	InitStack();
}

TInt CFilterStack::FunctionL( TInt aPosition, const TDesC& aCmd )
	{
	return iStack[ aPosition ]->iFilter->CmdL( aCmd );
	}




TInt CFilterStack::NumFilters()
	{
	return iStack.Count();
	}
