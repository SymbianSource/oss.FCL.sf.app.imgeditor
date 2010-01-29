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



#ifndef IMAGECONTROLLEROBSERVER_H
#define IMAGECONTROLLEROBSERVER_H

//  INCLUDES

//  FORWARD DECLARATIONS


/*	CLASS: MImageControllerObserver
*
*	MImageControllerObserver is an mixing class providing a callback API
*	for the observer of CImageController class.
*
*/
class MImageControllerObserver
{

public:

/** @name Methods:*/
//@{
	enum TOperationCode
	{
		/// Invalid
		EOperationCodeMin = 0,
		/// Loading images
		EOperationCodeLoad,
		/// Block loading images
		EOperationCodeBlockLoad,
		/// Saving images
		EOperationCodeSave,
		/// Block saving images
		EOperationCodeBlockSave,
		/// Deleting images
		EOperationCodeDelete,
		/// Renaming images
		EOperationCodeRename,
		/// Searching images
		EOperationCodeSearch,
		/// Invalid
		EOperationCodeMax
	};
//@}

/** @name Methods:*/
//@{
	
	/*	OperationReadyL
	*
	*	This method is called when asynchronous loading or saving operation
	*	is completed or when an error has occurred.
	*
	*	@param aOpId - operation ID
	*	@param aError - error code
	*	@return -
	*/
	virtual void OperationReadyL (
		TOperationCode		aOpId,
		TInt				aError 
		) = 0;

	/*	IncrementProgressBarL
	*
	*	This method is called to increment the progress bar when
	*	one step is completed.
	*
	*	@param - 
	*	@return -
	*/
	virtual void IncrementProgressBarL (const TInt aIncrement) = 0;
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
