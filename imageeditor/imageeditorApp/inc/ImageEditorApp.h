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


#ifndef IMAGEEDITORAPP_H
#define IMAGEEDITORAPP_H


// INCLUDES
#include <aknapp.h>

// debug log writer
#include "imageeditordebugutils.h"

// CONSTANTS

// FORWARD DECLARATIONS


/*	CLASS: 	CImageEditorApp
*
*	CImageEditorApp application class. Provides factory to create concrete
*	document object.   
*
*/
class CImageEditorApp : public CAknApplication
{
    
public:

/** @name Methods:*/
//@{

	/*	OpenIniFileLC
	*
	*	Opens .ini file associated with the application. Constructs 
	*	dictionary store object, puts the pointer to it to CleanupStack
	*	and returns it.
	*
	*	The .ini file is created, if it does not exist.
	*
	*	Implementation of this method is provided by UI framework.
	*
	*   @param aFs - file server session
	*   @return pointer to the created dictionary store
	*	@see CApaApplication
	*/
    virtual CDictionaryStore * OpenIniFileLC (RFs & aFs) const;

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

	/*	CreateDocumentL
	*
	*	Creates a document object. Called by the application process when
	*	a new document is required. Application process adds the document
	*	its list of documents.
	*
	*	Implementation of this method is provided by UI framework.
	*
	*   @param -
	*   @return pointer to the created CApaDocument
	*	@see CApaApplication
	*/
    virtual CApaDocument * CreateDocumentL();
        
	/*	AppDllUid
	*
	*	Gets application specific UID. The UID is used to differentiate 
	*	between UI applications. 
	*
	*	Implementation of this method must be provided by UI application.
	*
	*   @param -
	*   @return TUid - application's UID
	*	@see CApaApplication
	*/
    virtual TUid AppDllUid() const;

//@}

/** @name Members:*/
//@{
//@}

};

#endif


