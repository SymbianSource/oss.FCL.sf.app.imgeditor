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


#ifndef JPEGSCALER_H
#define JPEGSCALER_H


// INCLUDES
#include <e32base.h>

// CONSTANTS

// FORWARD DECLARATIONS
class CJpegScalerImpl;
class RFs;

/*  CLASS:  CJpegScaler
*
*
*   USAGE
*
*
*
*/
class CJpegScaler : public CBase
{
public:

    /** NewL factory method, pops cleanup stack
    *
    *   @param - aFsSession 
    *   @return pointer to created CJpegScaler object
    */
	IMPORT_C static CJpegScaler* NewL(RFs & aFsSession);

    /** Destructor
    *
    *   @param -
    *   @return -
    */	
	IMPORT_C ~CJpegScaler();
	
	/** Scale
    *
    *   @param aRequestStatus - request status object
    *   @param aSrcFileName - source file name
    *   @param aTrgFileName - target file name
    *   @param aDestinationSize - size of the target file
    *   @return -
    */	
	IMPORT_C void Scale(TRequestStatus* aRequestStatus, const TFileName& aSrcFileName, const TFileName& aTrgFileName, const TSize& aDestinationSize);
	
	/** Cancel
    *
    *   Cancel any outstanding activity
    *   
    *   @param - 
    *   @return -
    */	
	IMPORT_C void Cancel();

private:

	CJpegScaler();
	void ConstructL(RFs & aFsSession);

private:

	CJpegScalerImpl* iImplementation;

};    



#endif


// End of File
