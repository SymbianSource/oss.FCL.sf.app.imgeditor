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



#ifndef PLUGINLOADER_HPP
#define PLUGINLOADER_HPP

//	INCLUDES
#include <e32base.h>

#include "imageeditordebugutils.h"

//	PLUGIN TYPE DEFINITIONS
#include "plugintypedef.h"


/*	CLASS: CPluginLoader
*
*	CPluginLoader is a class loading a polymorphic DLL plug-in. 
*/ 
class CPluginLoader : public CBase
{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param aFileName - plug-in name
	*	@param aUID2 - pointer to UID2, if NULL not checked
	*	@param aUID3 - pointer to UID3, if NULL not checked
	*	@return pointer to created object
	*/
	static CPluginLoader * NewL (
		const TDesC	&	aFileName,
		const TUid *	aUID2,
		const TUid *	aUID3
		);

	/** NewL factory method, does not pop cleanupstack
	*
	*	@param aFileName - plug-in name
	*	@param aUID2 - pointer to UID2, if NULL not checked
	*	@param aUID3 - pointer to UID3, if NULL not checked
	*	@return pointer to created object
	*/
	static CPluginLoader * NewLC (
		const TDesC	&	aFileName,
		const TUid *	aUID2,
		const TUid *	aUID3
		);

		/** Destructor
	*	@param -
	*	@return -
	*/
	virtual ~CPluginLoader ();

	/** GetPlugin
	*
	*	Returns plug-in.
	*
	*	@param -
	*	@return - the loaded and created plug-in
	*/
	CPluginType * GetPlugin () const;

	/** GetPluginDll
	*
	*	Returns reference to the RLibrary object that has loaded the plug-in.
	*
	*	@param -
	*	@return - RLibrary 
	*/
	const RLibrary & GetPluginDll () const;


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

	/** Default constructor, cannot leave.
	*
	*	@param -
	*	@return -
	*/
	CPluginLoader ();

	/** Second phase constructor, may leave
	*
	*	@param aFileName - plug-in name
	*	@param aUID2 - pointer to UID2, if NULL not checked
	*	@param aUID3 - pointer to UID3, if NULL not checked
	*/
	void ConstructL (
		const TDesC	&	aFileName,
		const TUid *	aUID2,
		const TUid *	aUID3
		);

	/** Copy constructor, disabled
	*/
	CPluginLoader (const CPluginLoader & rhs);

	/** Assignment operator, disabled 
	*/
	CPluginLoader & operator= (const CPluginLoader & rhs);

//@}

/** @name Members:*/
//@{
	/// DLL loader
	RLibrary			iLibrary;
	/// Plugin 
	CPluginType *		iPlugin;
//@}

};


#endif
