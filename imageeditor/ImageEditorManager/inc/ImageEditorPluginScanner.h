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



#ifndef IMAGEEDITORPLUGINSCANNER_H
#define IMAGEEDITORPLUGINSCANNER_H


//	INCLUDES
#include <e32base.h>
#include <e32std.h> 
#include <badesca.h> 

//	FORWARD DECLARATIONS
class CPluginStorage;


/*  CLASS: CPluginScanner
*
*	CPluginScanner loads plug-ins of type CPluginType. The plug-in 
*	information is stored into plug-in storage.
*
*/ 
class CPluginScanner : public CBase
{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param -
	*	@return - pointer to created CPluginScanner object
	*/
	static CPluginScanner * NewL ();

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	virtual ~CPluginScanner ();

	/** ScanPluginsL
	*
	*	Scans all available plugins and stores plug-in information to
	*	plug-in storage. DOES NOT load the bitmaps of the plugin icons,
	*	this must be done separately by calling LoadIconBitmapsL.
	*	
	*	@param - aPluginStorageNeedsUpdate
	*	@return -
	*/
    void ScanPluginsL ( TBool& aPluginStorageNeedsUpdate );

	/** LoadIconBitmapsL
	*
	*	Scans all plugin in the plug-in storage and load the icon
	*	bitmaps, using the bitmap file path stored for each plugin.
	*
	*	@param -
	*	@return -
	*/
    void LoadIconBitmapsL();

	/** SetPluginStorage
	*
	*	Sets plug-in storage reference.
	*
	*	@param aStorage - pointer to plug-in storage
	*	@return -
	*/
    inline void SetPluginStorage (CPluginStorage * aStorage)
	{
		iStorage = aStorage;
	}

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
	CPluginScanner ();

	/** Second phase constructor, may leave
	*
	*	@param -
	*	@return -
	*/
	void ConstructL ();

	/** DoScanPluginsL
	*
	*	Scans all available plugins and stores plug-in information to
	*	plug-in storage. 
	*	
	*	@param - 
	*	@return -
	*/
    void DoScanPluginsL ();

	/** PluginStorageNeedsUpdateL
	*
	*	Perform "light" plug-in scan. Compare the plug-in dll file names
	*	from the plug-in storage and directory scan to determine
	*	if the information in the storage is valid.
	*	
	*	@param - 
	*	@return - TBool
	*/
    TBool PluginStorageNeedsUpdateL () const;

	/** Copy constructor, disabled
	*/
	CPluginScanner (const CPluginScanner & rhs);

	/** Assignment operator, disabled 
	*/
	CPluginScanner & operator= (const CPluginScanner & rhs);

//@}

/** @name Members:*/
//@{
	/// Plug-in storage
	CPluginStorage *	iStorage;
    /// Last used plug-in RID
    TInt                iLastRID;
//@}
};


#endif
