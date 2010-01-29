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



#ifndef PLUGINBASE_H
#define PLUGINBASE_H

#include <e32base.h>
#include <eikimage.h>


/*   CLASS: CPluginBase
*
*
*/ 
class CPluginBase : public CBase
{

public:


/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param -
	*	@return CPluginBase *, pointer to created CPluginBase object
	*/
	IMPORT_C static CPluginBase * NewL ();

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C CPluginBase ();

	/** Second phase constructor, may leave
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C void ConstructL ();

	/*	Destructor
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C virtual ~CPluginBase ();

	/*	SetProperty
	*
	*   Sets plug-in property.
	*
	*   @param aPropertyId - ID number of the property
	*   @param aPropertyValue - new property value as descriptor
	*   @return TInt - KErrNone if everything ok, one of the system wide
	*	error codes otherwise
	*/
	IMPORT_C virtual TInt SetProperty (
		TInt		aPropertyId, 
		TDesC &		aPropertyValue
		);

	/*	GetProperty
	*
	*   Gets plug-in property.
	*
	*   @param aPropertyId - ID number of the queried property
	*   @param aPropertyValue - queried property value as descriptor
	*   @return TInt - KErrNone if everything ok, one of the system wide
	*	error codes otherwise
	*/
	IMPORT_C virtual TInt GetProperty (
		TInt		aPropertyId, 
		TDes &		aPropertyValue
		);

	/*	ReservedBaseMethod1
	*
	*   Reserved method to export table for future extensions.
	*
	*   @param -
	*   @return -
	*/
	IMPORT_C virtual void ReservedBaseMethod1 ();

	/*	ReservedBaseMethod2
	*
	*   Reserved method to export table for future extensions.
	*
	*   @param -
	*   @return -
	*/
	IMPORT_C virtual void ReservedBaseMethod2 ();

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

	/*	
	*	Internal representation pointer.
	*
	*	DO NOT add any other private members here, so that the size 
	*	of the class stays constant.
	*/
    void * iPluginBaseInternalRepresentation;

//@}

};


#endif

// End of File
