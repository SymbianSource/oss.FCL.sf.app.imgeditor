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
* Forces an	asynchronous callback to be called.
*
*/


#ifndef CALLBACK_H
#define	CALLBACK_H

#include <e32base.h>
#include "callbackmethod.h"

/**
 *  Forces an	asynchronous callback to be called.
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib ImageEditorUtils.lib
 *  @since S60 v5.0
 */
class CObCallback : public CActive
{

public:

/** @name Methods:*/
//@{

    /**
     * Two-phased constructor.
     * @param aMethod - observer reference
     * @param ?arg2 ?description
     */
     IMPORT_C static CObCallback * NewL (MObCallbackMethod * aMethod);

    /**
    * Destructor.
    */
    IMPORT_C ~CObCallback();
		
    /**
     * Forces the active scheduler to do callback
     *
     * @since S60 ?S60_version
     * @param aParam - ID for callback, returned to observer
     * @param ?arg2 ?description
     * @return ?description
     */
     IMPORT_C void DoCallback (TInt aParam);

protected:

private:

	CObCallback (MObCallbackMethod * aMethod);
	
	void ConstructL();

// from base class CActive
	
    /**
     * From CActive.
     * See CActive
     *
     * @since S60 ?S60_version
     */
     virtual void RunL();


    /**
     * From CActive.
     * See CActive
     *
     * @since S60 ?S60_version
     */
     virtual void DoCancel();

	/** CompleteRequest
	*
	*	Force RunL
	*
	*	@param -
	*	@return -
	*/
    void CompleteRequest();

private: // data

    /**
     * Observer reference
     */
	  MObCallbackMethod *		iMethod;

    /**
     * Callback ID
     */	
	  TInt                    iParam;
};


#endif // CALLBACK_H
