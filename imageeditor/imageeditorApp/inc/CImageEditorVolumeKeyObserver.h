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
* Observes Remote Controller Framework for volume key presses.
* 
*/


#ifndef IMAGEEDITORVOLKEYOBSERVER_H
#define IMAGEEDITORVOLKEYOBSERVER_H

// INCLUDES
#include <RemConCoreApiTargetObserver.h>
  
// FORWARD DECLARATIONS
class CRemConInterfaceSelector;
class CRemConCoreApiTarget;
class CImageEditorRemConKeyResponse;

/**
* Observer interface for volume key events
* 
* since @3.1
*
*/
class MImageEditorVolumeKeyObserver
	{
public:
	/**
	* Report volume keys events to observer
	* @since 3.1
	* @param aOperationId Volume key up Volume key down event
	* @param aButtonAct button action (press, release, click)
	*/
	virtual void HandleVolumeKeyEvent( TRemConCoreApiOperationId aOperationId,
                            		   TRemConCoreApiButtonAction aButtonAct ) = 0;
	};

// CLASS DECLARATION

/**
*  Observes Remote Controller Framework
*  
*  @since 3.1
*/
class CImageEditorRemConObserver 
    : public CBase, public MRemConCoreApiTargetObserver
    {
    public:  // Constructors and destructor      
        
        /**
        * Two-phased constructor.
        * @param aZoomPane zoom pane instance
        * @return new instance.
        */
        static CImageEditorRemConObserver* NewL( MImageEditorVolumeKeyObserver& aObserver );

        /**
        * Destructor.
        */
        ~CImageEditorRemConObserver();

    public: // New functions
        
    protected:

        /**
        * from MRemConCoreApiTargetObserver interface class.
        * A command has been received. 
        * @param aOperationId The operation ID of the command.
        * @param aButtonAct The button action associated with the command.
        */
        void MrccatoCommand( TRemConCoreApiOperationId aOperationId, 
                             TRemConCoreApiButtonAction aButtonAct );

        /**
        * from MRemConCoreApiTargetObserver interface class.
        * not used
        */
        void MrccatoPlay(TRemConCoreApiPlaybackSpeed aSpeed, 
		                        TRemConCoreApiButtonAction aButtonAct);

        /**
        * from MRemConCoreApiTargetObserver interface class.
        * not used
        */
        void MrccatoTuneFunction(TBool aTwoPart, 
                        		TUint aMajorChannel, 
                        		TUint aMinorChannel,
                        		TRemConCoreApiButtonAction aButtonAct);

        /**
        * from MRemConCoreApiTargetObserver interface class.
        * not used
        */
        void MrccatoSelectDiskFunction(TUint aDisk,
                        		TRemConCoreApiButtonAction aButtonAct);
	
        /**
        * from MRemConCoreApiTargetObserver interface class.
        * not used
        */
        void MrccatoSelectAvInputFunction(TUint8 aAvInputSignalNumber,
                        		TRemConCoreApiButtonAction aButtonAct);

        /**
        * from MRemConCoreApiTargetObserver interface class.
        * not used
        */
        void MrccatoSelectAudioInputFunction(TUint8 aAudioInputSignalNumber,
                        		TRemConCoreApiButtonAction aButtonAct);

    private:

        /**
        * C++ default constructor.
        * @since 3.1
        * @param aZoomPane instance of zoom pane
        */
        CImageEditorRemConObserver( MImageEditorVolumeKeyObserver& aObserver );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:    // Data
        
        // Remote Controller
        CRemConInterfaceSelector* iInterfaceSelector;  
        
        // Remote Controller
        CRemConCoreApiTarget* iCoreTarget;
        
        // Key response
        CImageEditorRemConKeyResponse* iActiveRemCon;
        
        // zoom pane
        MImageEditorVolumeKeyObserver& iObserver;
    };

#endif      // IMAGEEDITORVOLKEYOBSERVER_H
            
// End of File
