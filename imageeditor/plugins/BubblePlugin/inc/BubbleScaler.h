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


#ifndef BUBBLESCALER_H
#define BUBBLESCALER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS

class CBitmapScaler;
class CFbsBitmap;

/*	CLASS: MBubbleScalerNotifier
*
*   Notifier class for CBubbleScaler
*
*/
class MBubbleScalerNotifier
    {
    public:
        virtual void BubbleScalerOperationReadyL(TInt aError) = 0;
    };

/*	CLASS: CBubbleScaler
*
*   Class is used for scaling bitmaps
*
*/
class CBubbleScaler : public CActive
    {
    
public:

    /** Default constructor, cannot leave.
	*
	*	@param -
	*	@return -
	*/
	CBubbleScaler (MBubbleScalerNotifier* aNotifier);

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	~CBubbleScaler ();

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
    MBubbleScalerNotifier* iNotifier;

};

#endif

// End of File
