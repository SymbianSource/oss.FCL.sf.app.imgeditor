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



#ifndef IMAGEEDITORPLUGINSTORAGE_H
#define IMAGEEDITORPLUGINSTORAGE_H

//	INCLUDES
#include <e32base.h>
#include <e32std.h>

#include "imageeditordebugutils.h"

//	FORWARD DECLARATIONS
class CPluginInfo;
class RReadStream;
class RWriteStream;

/*   CLASS: CPluginStorage
*
*
*/ 
class CPluginStorage : public CBase
{

public:

/** @name Typedefs:*/
//@{
	/// Plug-in group array
	typedef RPointerArray<CPluginInfo> RPluginInfoArray;
//@}

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param -
	*	@return pointer to created CPluginStorage object
	*/
	static CPluginStorage * NewL ();

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	virtual ~CPluginStorage ();

    /** CountPlugins
	*
	*	Counts the number of found plug-ins.
	*
	*	@param -
	*	@return TInt - count of plug-ins
	*/
    TInt CountPlugins() const;

    /** ResetAndDestroy
	*
	*	Empty the storage and free allocated resources.
	*
	*	@param -
	*	@return 
	*/
    void ResetAndDestroy();

    /** FindPluginDll
	*
	*	Finds plug-in with DLL file name aPluginDll
	*
	*	@param aPluginDll - plug-in run-time ID
	*	@param aPos - index of the located element
	*	@return Zero, if a matching descriptor is found. Non-zero otherwise. 
	*/
    TInt FindPluginDll(const TDesC& aPluginDll, TInt& aPos ) const;

    /** GetPluginInfo
	*
	*	Returns plug-in information in group aGroup at position aPlugin.
	*
	*	@param aID - plug-in run-time ID
	*	@return plug-in information class
	*/
    CPluginInfo * GetPluginInfo (const TInt	aID);

	/** AddPluginInfoL
	*
	*	Add a new CPluginInfo instance to the storage.
	*
	*	@param aGroup - plug-in group index
	*	@param aPlugin - plug-in index
	*	@return -
	*/
	void AddPluginInfoL (const CPluginInfo * aPluginInfo);

	/** ExternalizeL
	*
	*	Externalizes object.
	*
	*	@param aStream - write stream
	*	@return -
	*/
	void ExternalizeL(RWriteStream& aStream) const;

	/** InternalizeL
	*
	*	Internalizes object.
	*
	*	@param aStream - read stream
	*	@return -
	*/
	void InternalizeL(RReadStream& aStream);

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
	CPluginStorage ();

	/** Second phase constructor, may leave
	*
	*	@param -
	*	@return -
	*/
	void ConstructL ();

	/** Copy constructor, disabled
	*/
	CPluginStorage (const CPluginStorage & rhs);

	/** Assignment operator, disabled 
	*/
	CPluginStorage & operator= (const CPluginStorage & rhs);

//@}

/** @name Members:*/
//@{
	/// Plug-in array
	RPluginInfoArray    iPlugins;
//@}

};


#endif
