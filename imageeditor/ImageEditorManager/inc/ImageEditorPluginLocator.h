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



#ifndef IMAGEEDITORPLUGINLOCATOR_H
#define IMAGEEDITORPLUGINLOCATOR_H

// INCLUDES
#include <e32base.h>
#include <fbs.h>

#include "imageeditordebugutils.h"

// FORWARD DECLARATIONS
class CPluginScanner;
class CPluginStorage;
class CPluginInfo;

/*  CLASS: CPluginLocator
*
*	CPluginLocator represents plugin locator API for Image Editor software.
*	It implements a facade design pattern to accommodate pluginlocator and
*	pluginstorage classes and their functionality under the same API. Also,
*	the facade further disconnects the implementation of the plug-in locating
*	part and the application itself
*
*/ 
class CPluginLocator :	public CBase
{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param -
	*	@return pointer to created CPluginLocator object
	*/
	IMPORT_C static CPluginLocator * NewL ();

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C virtual ~CPluginLocator ();


	/*	
	*
	*	GENERAL PLUG-IN MANAGEMENT METHODS
	*
	*/


	/** ScanPlugins
	*
	*	Scans all available plug-ins
	*
	*	@return -
	*/
	IMPORT_C void ScanPluginsL ();

	/** CountPlugins
	*
	*	Counts the number of found plug-ins.
	*
	*	@param -
	*	@return TInt - count of plug-ins
	*/
    IMPORT_C TInt CountPlugins() const;


    /*	
	*
	*	PLUGIN QUERY METHODS 
	*
	*/


    /** GetPluginInfo
	*
	*	Returns a pointer to plug-in info object. 
	*
	*	@param aID - plug-in run-time ID [0, CountPlugins - 1]
	*	@return - plug-in DLL file name
	*/
	IMPORT_C CPluginInfo * GetPluginInfo (const TInt aID);

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

	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CPluginLocator ();

	/** Second phase constructor, may leave
	*
	*	@param -
	*	@return -
	*/
	void ConstructL ();

	/** Copy constructor, disabled
	*/
	CPluginLocator (const CPluginLocator & rhs);

	/** Assignment operator, disabled 
	*/
	CPluginLocator & operator= (const CPluginLocator & rhs);

//@}

/** @name Members:*/
//@{

	/// Plug-in locator 
	CPluginScanner *	iScanner;

	/// Plug-in storage
	CPluginStorage *	iStorage;

//@}

};


#endif
