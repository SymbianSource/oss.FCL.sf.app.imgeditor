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


/*
*/

// INCLUDE FILES
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>
#include <aknconsts.h>					

#include <e32debug.h>


#include "cmgxremcontarget.h"

//MACROS

#ifdef _DEBUG
#define TRACES(x)    x
#else
#define TRACES(x)
#endif


// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CMGXRemConTarget::CMGXRemConTarget(MMGXMediakeyObserver* aObserver) : iObserver(aObserver)
    {
    }

// EPOC default constructor can leave.
void CMGXRemConTarget::ConstructL()
    {
    // Create interface selector.
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    // Create a new CRemConCoreApiTarget, owned by the interface selector.
    iCoreTarget = CRemConCoreApiTarget::NewL(*iInterfaceSelector, *this);
    // Start being a target.
    iInterfaceSelector->OpenTargetL();
    }

// Two-phased constructor.
CMGXRemConTarget* CMGXRemConTarget::NewL(MMGXMediakeyObserver* aObserver)
    {
    CMGXRemConTarget* self = new (ELeave) CMGXRemConTarget(aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CMGXRemConTarget::~CMGXRemConTarget()
    {
    delete iInterfaceSelector;
    // iCoreTarget was owned by iInterfaceSelector.
    iCoreTarget = NULL;
    }

// ---------------------------------------------------------
// CMGXRemConTarget::MrccatoCommand
// ---------------------------------------------------------
//
void CMGXRemConTarget::MrccatoCommand(TRemConCoreApiOperationId aOperationId, TRemConCoreApiButtonAction aButtonAct)
		{
		TRACES( RDebug::Print( _L( "CMGXRemConTarget::MrccatoCommand: buttonact:%d >>"), aButtonAct ) );
		switch (aOperationId)
		    {
		    case ERemConCoreApiVolumeUp:
		        iObserver->HandleVolumeUpL();
		        break;
		    case ERemConCoreApiVolumeDown:
		        iObserver->HandleVolumeDownL();
		        break;
		        
		    default:
		        break;
		    }
		TRACES( RDebug::Print( _L( "CMGXRemConTarget::MrccatoCommand << ") ));
		}

// ---------------------------------------------------------
// CDmhRemConTarget::MrccatoPlay
// ---------------------------------------------------------
//
void CMGXRemConTarget::MrccatoPlay(TRemConCoreApiPlaybackSpeed /*aSpeed*/, TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	TRACES( RDebug::Print( _L( "CMGXRemConTarget::MrccatoPlay <<") ));
	}
	
// ---------------------------------------------------------
// CDmhRemConTarget::MrccatoTuneFunction
// ---------------------------------------------------------
//
void CMGXRemConTarget::MrccatoTuneFunction(TBool /*aTwoPart*/, TUint /*aMajorChannel*/, TUint /*aMinorChannel*/, TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	
	}
	
// ---------------------------------------------------------
// CDmhRemConTarget::MrccatoSelectDiskFunction
// ---------------------------------------------------------
//
void CMGXRemConTarget::MrccatoSelectDiskFunction(TUint /*aDisk*/, TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	
	}
	
// ---------------------------------------------------------
// CMGXRemConTarget::MrccatoSelectAvInputFunction
// ---------------------------------------------------------
//
void CMGXRemConTarget::MrccatoSelectAvInputFunction(TUint8 /*aAvInputSignalNumber*/, TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	
	}
	
// ---------------------------------------------------------
// CMGXRemConTarget::MrccatoSelectAudioInputFunction
// ---------------------------------------------------------
//
void CMGXRemConTarget::MrccatoSelectAudioInputFunction(TUint8 /*aAudioInputSignalNumber*/, TRemConCoreApiButtonAction /*aButtonAct*/)
	{
	
	}
	
//  End of File  
