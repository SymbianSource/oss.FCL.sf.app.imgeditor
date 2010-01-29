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



#ifndef IMAGEEDITORUIITEM_HPP
#define IMAGEEDITORUIITEM_HPP

//  INCLUDES
#include <e32base.h>

//  FORWARD DECLARATIONS
class CFbsBitmap;


/*	CLASS: CUiItem
*
*	CUiItem represents an UI information item for a single Image Editor
*	plug-in.
*/
class CUiItem : public CBase
{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param -
	*	@return pointer to created CUiItem object
	*/
	IMPORT_C static CUiItem * NewL ();

	/** NewLC factory method, does not pop cleanupstack
	*
	*	@param -
	*	@return pointer to created CUiItem object
	*/
	IMPORT_C static CUiItem * NewLC ();

	/** ComparePluginOrder
	*
    *   Comparison method for CUiItem instances.
    *
	*	@param aItem1 - item 1
	*	@param aItem2 - item 2
	*	@return - -1 if aItem1 is first in plug-in order
	*	@return - 1 if aItem2 is first in plug-in order
	*	@return - 0 otherwise
	*/
	IMPORT_C static TInt ComparePluginOrder (
        const CUiItem &     aItem1,
        const CUiItem &     aItem2
        );

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C virtual ~CUiItem ();

	/** Setter/getter, PluginName, non-const
	*
	*	@param -
	*	@return - plug-in name
	*/
	IMPORT_C TDes & PluginName();

	/** Getter, PluginName, const
	*
	*	@param -
	*	@return - plug-in name
	*/
	IMPORT_C const TDesC & PluginName() const;

    /** Setter/getter, PluginRID, non-const
	*
	*	@param -
	*	@return - plug-in order inside group
	*/
	IMPORT_C TInt & PluginRID();

    /** Getter, PluginRID, const
	*
	*	@param -
	*	@return - plug-in order inside group
	*/
	IMPORT_C const TInt & PluginRID() const;

    /** Setter/getter, PluginOrder, non-const
	*
	*	@param -
	*	@return - plug-in order inside group
	*/
	IMPORT_C TInt & PluginOrder();

    /** Getter, PluginOrder, const
	*
	*	@param -
	*	@return - plug-in order inside group
	*/
	IMPORT_C const TInt & PluginOrder() const;

    /** Setter/getter, GroupOrder, non-const
	*
	*	@param -
	*	@return - plug-in order inside group
	*/
	IMPORT_C TInt & GroupOrder();

    /** Getter, GroupOrder, const
	*
	*	@param -
	*	@return - plug-in order inside group
	*/
	IMPORT_C const TInt & GroupOrder() const;

	/** Setter/getter, Icon, non-const
	*
	*	@param -
	*	@return - icon bitmap
	*/
	IMPORT_C CFbsBitmap *& Icon();

	/** Getter, Icon, const
	*
	*	@param -
	*	@return - icon bitmap
	*/
	IMPORT_C const CFbsBitmap * Icon() const;

    /** Setter/getter, Mask, non-const
	*
	*	@param -
	*	@return - icon bitmap
	*/
	IMPORT_C CFbsBitmap *& Mask();

    /** Getter, Mask, const
	*
	*	@param -
	*	@return - icon bitmap
	*/
	IMPORT_C const CFbsBitmap * Mask() const;

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
	IMPORT_C CUiItem ();

	/** Second phase constructor, may leave
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C void ConstructL ();    

    /** Copy constructor, disabled
	*/
	CUiItem (const CUiItem & rhs);

	/** Assignment operator, disabled 
	*/
	CUiItem & operator= (const CUiItem & rhs);

//@}

/** @name Members:*/
//@{
	/// Plug-in name
	TBuf<64>		iPluginName;
	/// Plug-in run-time ID
	TInt			iPluginRID;
	/// Plug-in order inside group
	TInt			iPluginOrder;
	/// Group order
	TInt			iGroupOrder;
	/// Icon bitmap
	CFbsBitmap *    iIcon;
	/// Icon mask
	CFbsBitmap *    iMask;

//@}

};

#endif

// End of File
