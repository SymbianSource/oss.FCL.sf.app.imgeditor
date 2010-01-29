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


#ifndef IMAGEEDITORDOCUMENT_H
#define IMAGEEDITORDOCUMENT_H


// INCLUDES
#include <akndoc.h>
   
// debug log writer
#include <imageeditordebugutils.h>

// CONSTANTS

// FORWARD DECLARATIONS
class  CEikAppUi;


// CLASS DECLARATION

/**
*  CIMAGEEDITORDocument application class.
*/

class CImageEditorDocument : public CAknDocument
{


public:

/** @name Methods:*/
//@{

	/*	CImageEditorDocument factory method
	*
	*   Creates new instance of CImageEditorDocument class.
	*
	*   @param aApp - reference to CEikApplication
	*   @return - pointer to the created instance of CImageEditorDocument
	*	@see CAknDocument
	*	@see CEikDocument
	*/
	static CImageEditorDocument * NewL (CEikApplication & aApp);

	/*	Destructor
	*
	*   @param -
	*   @return -
	*/
	virtual ~CImageEditorDocument();

	/*	OpenFileL
	*
	*   This method is called by UIKON framework to open a file.
	*
	*	@see CAknDocument
	*/
	void OpenFileL(CFileStore*& aFileStore, RFile& aFile);

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

	/*	Default constructor
	*
	*	Calls CEikDocument::CEikDocument(aApp), allocates new document,
	*	initializes its references to its associated application and managing 
	*	process.
	*
	*   @param aApp - reference to the application
	*   @return -
	*/
	CImageEditorDocument (CEikApplication & aApp);

	/*	Second phase constructor
	*
	*   @param -
	*   @return -
	*/
	void ConstructL();

	/*	CreateAppUiL
	*
	*	Allocates application UI. Application framework calls this to create 
	*	an instance of application UI for the document instance in question.
	*	Ownership of the returned CEikAppUi is transferred to UIKON.
	*
	*   @param -
	*   @return pointer to created application UI instance
	*	@see CEikDocument
	*/
    CEikAppUi * CreateAppUiL();
	
//@}

/** @name Members:*/
//@{
//@}
};

#endif

