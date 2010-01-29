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
* Active object to wait for asynchronous service completion.
*
*/


#ifndef JPEGROTATORHELPER_H
#define JPEGROTATORHELPER_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <aknwaitdialog.h>
#include "JpegRotator.h"
#include "JpegRotatorHelperObserver.h"

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DECLARATIONS

/**
* CJpegRotatorHelper
* An active object.
* 
*/
NONSHARABLE_CLASS(CJpegRotatorHelper) : public CActive, public MProgressDialogCallback
{
    public: // New functions

        /**
        * Constructor
        */
        CJpegRotatorHelper(RFs& aFsSession, MJpegRotatorHelperObserver& aObserver);


        /**
        * Destructor
        */
        ~CJpegRotatorHelper();

        /**
        * Start waiting
        */
		void StartRotateL (
			const CDesCArray* aSourceFileList, 
			CDesCArray* aTargetFileList, 
			CJpegRotator::TRotationMode aMode,
			TBool aPreserveOriginalFiles
			);

    public: // Functions from base classes

        /**
        * Callback method
        *   Get's called when a dialog is dismissed.
        */
        virtual void DialogDismissedL( TInt aButtonId );

    private: // Functions from base classes

        /**
        * From CActive
        */
        void RunL();
        
        /**
        * From CActive
        */
        void DoCancel();

        /**
        * 
        */
        void FinalizeRotatedFileL( TInt aCurrentFileIndex );

    private: // Data

        RFs&						iFsSession;
        MJpegRotatorHelperObserver& iObserver;
        CJpegRotator*				iJpegRotator;
        CAknWaitDialog*				iWaitDialog;
        const CDesCArray* 			iSourceFileList; // not owned
        CDesCArray*					iTargetFileList; // not owned
        TInt						iMode;
        TBool 						iPreserveOriginalFiles;
        TInt 						iCounter;
    };

#endif


// End of File

