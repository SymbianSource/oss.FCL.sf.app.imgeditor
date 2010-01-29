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


#ifndef JPEGSCALERIMPL_H
#define JPEGSCALERIMPL_H


// INCLUDES
#include <f32file.h>
#include <e32base.h>

#include "CJpegScale.h"

// debug log writer
//#include "imageeditordebugutils.h"

// CONSTANTS

// FORWARD DECLARATIONS

/*	CLASS: 	CJpegScalerImpl
*
*	CJpegScalerImpl
*	Internal representation of CJpegScaler
*
*/
class CJpegScalerImpl : public CAsyncOneShot//, public MJpegScaleCallBack
{
    
public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanup stack
	*
	*	@param - aFsSession
	*	@return pointer to created CJpegScalerImpl object
	*/
	static CJpegScalerImpl * NewL (RFs& aFsSession);

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	~CJpegScalerImpl ();

    /** RunL
	*
	*	@param -
	*	@return -
	*/
	virtual void RunL();

    /** Scale
	*
	*	@param -
	*	@return -
	*/
    void Scale(TRequestStatus* aRequestStatus, const TFileName& aSrcFileName, const TFileName& aTrgFileName, const TSize& aDestinationSize);

private:

	/** Default constructor
	*
	*	@param - aFsSession
	*	@return -
	*/
	CJpegScalerImpl (RFs& aFsSession);

	/** Second phase constructor, may leave
	*
	*	@param -
	*	@return -
	*/
	void ConstructL ();

	/** Callback function for CJpegScale
	*
	*	@param -
	*	@return -
	*/
    //void JpegStatusCallBack( TInt aPercentReady );

private:

    /// File Server session handle
    RFs& 						iFsSession;

    /// Source and target files
    TFileName					iSourceFileName;
    TFileName					iTargetFileName;

    /// Scaler
    CJpegScale*                 iScale;

    /// Caller request status
    TRequestStatus*             iCallerReqStatus;

#if defined (LOG_TIMING)
    TTime                       iStartTime;
    TTime                       iSavingStartTime;
    TTime                       iFinishTime;
#endif


};

#endif


// End of File
