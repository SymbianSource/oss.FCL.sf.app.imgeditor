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



#ifndef ClipartScaler_h
#define ClipartScaler_h

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

class CBitmapScaler;
class CFbsBitmap;

/*	CLASS: MClipartScalerNotifier
*
*   Notifier class for CClipartScaler
*
*/
class MClipartScalerNotifier
    {
    public:
        virtual void ClipartScalerOperationReadyL(TInt aError) = 0;
    };

/*	CLASS: CClipartScaler
*
*   Class is used for scaling bitmaps
*
*/
class CClipartScaler : public CActive
    {
    
public:

    /** Default constructor, cannot leave.
	*
	*	@param -
	*	@return -
	*/
	CClipartScaler (MClipartScalerNotifier* aNotifier);

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	~CClipartScaler ();

	/*	Second phase constructor	
	*
	*	@param -
	*	@return -
	*/
    void ConstructL ();

    /*	Second phase constructor	
	*
	*	@param aBitmap - bitmap to be scaled 
	*	@return -
	*/
    void ScaleBitmapL(CFbsBitmap* aBitmap, TSize aNewSize);

protected:

  	/*	DoCancel
	*
	*   @see CActive
	*/
    void DoCancel();
 
  	/*	RunL
	*
	*   @see CActive
	*/
    void RunL();

private:

    CBitmapScaler*  iBitmapScaler;
    MClipartScalerNotifier* iNotifier;

};

#endif

// End of File
