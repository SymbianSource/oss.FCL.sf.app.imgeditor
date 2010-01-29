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
* Receives buton presses from volume keys.
* 
*/


// INCLUDE FILES

#include "CImageEditorVolumeKeyObserver.h"
#include <RemConCoreApiTarget.h>
#include <remconinterfaceselector.h>
#include <akndef.h>
#include <aknconsts.h>

/**
*  Helper class for sending response back to 
*  Remote Controller Framework
*
*  @since 3.1
*/
class CImageEditorRemConKeyResponse : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        * aparam Remote Controller instance
        * @return new instance.
        */
        static CImageEditorRemConKeyResponse* NewL( CRemConCoreApiTarget& 
        								   			aRemConCoreApiTarget );
            
        /**
        * Destructor.
        */
        virtual ~CImageEditorRemConKeyResponse();
       
    public: // new function

		/**
        * Send the any key response back to Remcon server
        * @since 3.1
        * @param aOperationId Remcon operation
        */
        void CompleteAnyKeyL( TRemConCoreApiOperationId aOperationId );
        
    private: //from base class
    
        /**
        * From CActive
        * Called on completion of a request
        * @since 3.1
        */
        void RunL();
        
        /**
        * From CActive
        * Cancels an outstanding request
        * @since 3.1
        */
        void DoCancel();

    private:

        /**
        * C++ default constructor.
        * @since 3.1
        * aparam Remote Controller instance
        */
        CImageEditorRemConKeyResponse( CRemConCoreApiTarget& aRemConCoreApiTarget );
   
    private:
    	
    	// Response array
    	RArray<TRemConCoreApiOperationId> iResponseArray;
    	
    	// Remote controller
    	CRemConCoreApiTarget& iRemConCoreApiTarget;
    };


// -----------------------------------------------------------------------------
// CImageEditorRemConKeyResponse::CImageEditorRemConKeyResponse
// default C++ constructor
// -----------------------------------------------------------------------------
//
CImageEditorRemConKeyResponse::CImageEditorRemConKeyResponse( CRemConCoreApiTarget& 
															  aRemConCoreApiTarget )
      : CActive ( EPriorityNormal ),
      	iRemConCoreApiTarget ( aRemConCoreApiTarget )

    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CImageEditorRemConKeyResponse::NewL
// -----------------------------------------------------------------------------
//
CImageEditorRemConKeyResponse* CImageEditorRemConKeyResponse::NewL(CRemConCoreApiTarget& 
														           aRemConCoreApiTarget)
    {
    CImageEditorRemConKeyResponse* self = 
        new (ELeave) CImageEditorRemConKeyResponse( aRemConCoreApiTarget );

    return self;
    }

// -----------------------------------------------------------------------------
// CImageEditorRemConKeyResponse::~CImageEditorRemConKeyResponse
// -----------------------------------------------------------------------------
//
CImageEditorRemConKeyResponse::~CImageEditorRemConKeyResponse()
  {
  Cancel();
  iResponseArray.Reset();
  iResponseArray.Close();
  }
    
// -----------------------------------------------------------------------------
// CImageEditorRemConKeyResponse::DoCancel
// -----------------------------------------------------------------------------
//
void CImageEditorRemConKeyResponse::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CImageEditorRemConKeyResponse::RunL
// -----------------------------------------------------------------------------
//
void CImageEditorRemConKeyResponse::RunL()
    {
    // if any existing -> Send response
    if ( iResponseArray.Count() )
        {
        CompleteAnyKeyL( iResponseArray[0] );
        // Remove already completed key
        iResponseArray.Remove(0);
        iResponseArray.Compress();
        }
    }

// -----------------------------------------------------------------------------
// CCamConKeyResponse::CompleteAnyKeyL
// -----------------------------------------------------------------------------
//
void CImageEditorRemConKeyResponse::CompleteAnyKeyL( TRemConCoreApiOperationId 
																aOperationId )
  {
  if ( !IsActive() )
    {
    switch ( aOperationId )
        {
        case ERemConCoreApiVolumeUp:
            {
            iRemConCoreApiTarget.VolumeUpResponse( iStatus, KErrNone );
            SetActive();
            }
            break;
        case ERemConCoreApiVolumeDown:
            {
            iRemConCoreApiTarget.VolumeDownResponse( iStatus, KErrNone );	
            SetActive();
            }
            break;
        default:
            {
            // Send general response for 'other' keys
            iRemConCoreApiTarget.SendResponse( iStatus, aOperationId, KErrNone );
            SetActive();
            }
            break;
        }
    }
  // already active. Append to array and complete later.
  else
    {
        User::LeaveIfError( iResponseArray.Append( aOperationId ) );
    }
  }


// -----------------------------------------------------------------------------
// ImageEditorRemConObserver::NewL
// -----------------------------------------------------------------------------
//
CImageEditorRemConObserver* CImageEditorRemConObserver::NewL( MImageEditorVolumeKeyObserver &aObserver )
    {
    CImageEditorRemConObserver* self = new ( ELeave ) CImageEditorRemConObserver( aObserver );
    CleanupStack::PushL( self );
    self->ConstructL(  );
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// CPhoneRemConObserver::~CPhoneRemConObserver
// destructor
// -----------------------------------------------------------------------------
//
CImageEditorRemConObserver::~CImageEditorRemConObserver()
    {
    delete iActiveRemCon;
    delete iInterfaceSelector;
    }
 
// -----------------------------------------------------------------------------
// CPhoneRemConObserver::CPhoneRemConObserver
// C++ default constructor
// -----------------------------------------------------------------------------
//
CImageEditorRemConObserver::CImageEditorRemConObserver( MImageEditorVolumeKeyObserver& aObserver ):
	iObserver ( aObserver )
    {
    }

// -----------------------------------------------------------------------------
// CPhoneRemConObserver::ConstructL
// Symbian 2nd phase constructor
// -----------------------------------------------------------------------------
//
void CImageEditorRemConObserver::ConstructL()
    {
    iInterfaceSelector = CRemConInterfaceSelector::NewL();

	// owned by CRemConInterfaceSelector instance  
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );
    iActiveRemCon = CImageEditorRemConKeyResponse::NewL( *iCoreTarget );
    
    iInterfaceSelector->OpenTargetL();  
    }
  
// -----------------------------------------------------------------------------	
// CPhoneRemConObserver::MrccatoCommand
// A command has been received. 
// -----------------------------------------------------------------------------
//
void CImageEditorRemConObserver::MrccatoCommand(
        TRemConCoreApiOperationId aOperationId, 
        TRemConCoreApiButtonAction aButtonAct )
    {
    switch ( aOperationId )
        {
        case ERemConCoreApiVolumeUp:
            {
            // send the response back to Remcon server
			TRAP_IGNORE( iActiveRemCon->CompleteAnyKeyL( aOperationId ) );
			
			// send button press to zoom pane
			iObserver.HandleVolumeKeyEvent( aOperationId, aButtonAct );
            }
            break;
        case ERemConCoreApiVolumeDown:
           	{
			TRAP_IGNORE( iActiveRemCon->CompleteAnyKeyL( aOperationId ) );
			iObserver.HandleVolumeKeyEvent( aOperationId, aButtonAct );
           	}
            break;
        default:
        	{
            // Complete any other operation id
            TRAP_IGNORE( iActiveRemCon->CompleteAnyKeyL( aOperationId ) );
        	}
            break;
        }
    }

// -----------------------------------------------------------------------------	
// CPhoneRemConObserver::MrccatoPlay
// not used
// -----------------------------------------------------------------------------
//
void CImageEditorRemConObserver::MrccatoPlay( TRemConCoreApiPlaybackSpeed /*aSpeed*/, 
		TRemConCoreApiButtonAction /*aButtonAct*/ )
	{
	// not used
 	}

// -----------------------------------------------------------------------------	
// CPhoneRemConObserver::MrccatoTuneFunction
// not used
// -----------------------------------------------------------------------------
//
void CImageEditorRemConObserver::MrccatoTuneFunction( TBool /*aTwoPart*/, 
		TUint /*aMajorChannel*/, 
		TUint /*aMinorChannel*/,
		TRemConCoreApiButtonAction /*aButtonAct*/ )
	{
    // not used
 	}

// -----------------------------------------------------------------------------	
// CPhoneRemConObserver::MrccatoSelectDiskFunction
// not used
// -----------------------------------------------------------------------------
//
void CImageEditorRemConObserver::MrccatoSelectDiskFunction( TUint /*aDisk*/,
		TRemConCoreApiButtonAction /*aButtonAct*/ )
	{
	// not used
 	}
	
// -----------------------------------------------------------------------------	
// CPhoneRemConObserver::MrccatoSelectAvInputFunction
// not used
// -----------------------------------------------------------------------------
//
void CImageEditorRemConObserver::MrccatoSelectAvInputFunction( TUint8 /*aAvInputSignalNumber*/,
		TRemConCoreApiButtonAction /*aButtonAct*/ )
	{
    // not used
 	}

// -----------------------------------------------------------------------------	
// CPhoneRemConObserver::MrccatoSelectAudioInputFunction
// not used
// -----------------------------------------------------------------------------
//
void CImageEditorRemConObserver::MrccatoSelectAudioInputFunction( TUint8 /*aAudioInputSignalNumber*/,
		TRemConCoreApiButtonAction /*aButtonAct*/ )
	{
	// not used
 	}

//  End of File
