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



#ifndef SINGLEPARAMCONTROLOBSERVER_H
#define SINGLEPARAMCONTROLOBSERVER_H

//  INCLUDES

//  FORWARD DECLARATIONS


/*	CLASS: MSingleParControlObserver
*
*   MSingleParControlObserver is a mixin class for updating the parameter
*
*/
class MSingleParControlObserver
{
public:
/** @name Methods:*/
//@{
    enum TParamOperation
    {
        //  Invalid
        EParamOperationMin,
        //  Set default parameter
        EParamOperationDefault,
        //  Add step to parameter
        EParamOperationAdd,
        //  Subtract step from parameter
        EParamOperationSubtract,
        //  Invalid
        EParamOperationMax,
    };
//@}

/** @name Methods:*/
//@{
	/** ParamOperation 
    *
    *   Performs an operation to the filter parameter.
	*
	*	@param aOperation - filter parameter operation
	*	@return -
	*/
	virtual void ParamOperation (const TParamOperation aOperation) = 0;

	/** GetParam
    *
    *   Gets filter parameter.
	*
	*	@param -
	*	@return TReal - filter parameter value
	*/
	virtual TReal GetParam () const = 0;
//@}

protected:

/** @name Methods:*/
//@{

//@}

/** @name Members:*/
//@{

//@}

private:

/** @name Methods:*/
//@{


//@}

/** @name Members:*/
//@{

//@}

};

#endif

// End of File
