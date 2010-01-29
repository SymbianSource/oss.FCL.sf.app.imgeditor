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
* Observer API for CObCallback class.
*
*/


#ifndef CALLBACKMETHOD_H
#define CALLBACKMETHOD_H

/**
 *  Observer API for CObCallback class
 *
 *  @code
 *   ?good_class_usage_example(s)
 *  @endcode
 *
 *  @lib -
 *  @since S60 v5.0
 */
class MObCallbackMethod
{

public:

    /**
     * Returns ETrue if callback is called again, otherwise EFalse
     *
     * @since S60 v5.0
     * @param aParam Callback ID
     * @return TBool - ETrue if callback called again, EFalse otherwise
     */
     virtual TBool CallbackMethodL ( TInt aParam ) = 0;
};

#endif // CALLBACKMETHOD_H
