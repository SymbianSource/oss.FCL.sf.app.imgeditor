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



#ifndef IMAGEEDITORPLUGINRESOURCE_H
#define IMAGEEDITORPLUGINRESOURCE_H

// INCLUDES
#include <e32base.h>
#include <badesca.h>
#include <eikmenup.h>

#include "ImageEditorPluginBaseDefs.h"
#include "ImageEditorPluginBase.hrh"

// TYPEDEFS
typedef CArrayFixSeg<CEikMenuPaneItem::SData> CMenuItemArray;

/*  CLASS: CPluginResource
*
*   CPluginResource acts as constructable container object for Image Editor
*   plug-in.
*	    
*/ 
class CPluginResource   : public CBase
{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
    *
	*	@param aResourcePath - plug-in resource path
	*	@param aResourceFile - plug-in resource file
	*	@return CPluginResource *, pointer to created CPluginResource object
	*/
	static CPluginResource * NewL (
        const TDesC &   aResourcePath,
        const TDesC &   aResourceFile
        );

	/** Destructor
	*	@param -
	*	@return -
	*/
	virtual ~CPluginResource ();

	/** PluginUID - const
    *
    *   Getter for plug-in UID
    *
	*	@param -
	*	@return const reference to plug-in UID
	*/
	const TPluginInt & PluginUID () const;

	/** PluginUiType - const
    *
    *   Getter for plug-in UI type
    *
	*	@param -
	*	@return const reference to plug-in UI type
	*/
	const TPluginUiType & PluginUiType() const;

	/** PluginFilterType - const
    *
    *   Getter for plug-in filter type
    *
	*	@param -
	*	@return const reference to plug-in filter type
	*/
	const TPluginFilterType & PluginFilterType() const;

	/** PluginScope - const
    *
    *   Getter for plug-in scope
    *
	*	@param -
	*	@return const reference to plug-in scope
	*/
	const TPluginScope & PluginScope() const;

	/** PluginDisplayOrder - const
    *
    *   Getter for plug-in display order
    *
	*	@param -
	*	@return const reference to plug-in display order
	*/
	const TPluginInt & PluginDisplayOrder() const;

    /** PluginName - const
    *
    *   Getter for plug-in name
    *
	*	@param -
	*	@return const reference to plug-in name descriptor
	*/
	const TPtrC PluginName () const;

	/** IconFileName  - const
    *
    *   Getter for plug-in icon file name
    *
	*	@param -
	*	@return const reference to plug-in icon file name
	*/
	const TPtrC IconFileName () const;

	/** FilterFileName - const
    *
    *   Getter for plug-in filter DLL name
    *
	*	@param -
	*	@return const reference to plug-in filter DLL name
	*/
	const TPtrC FilterFileName () const;

	/** GetParameters
    *
    *   Gets parameter descriptor array.
    *
	*	@param -
	*	@return the parameter name descriptor array
	*/
	const CDesCArray * GetParameters () const;

	/** GetSk1Text
    *
    *   Gets soft key 1 text.
    *
	*	@param -
	*	@return the soft key name
	*/
	const CDesCArray * Sk1Texts () const;

	/** GetSk1CommandId
    *
    *   Gets soft key 1 command ID.
    *
	*	@param -
	*	@return the soft key name
	*/
	const CArrayFix<TInt> * Sk1CommandIds() const;

    /** GetSk2Text
    *
    *   Gets soft key 2 text.
    *
	*	@param -
	*	@return the soft key command ID
	*/
	const CDesCArray * Sk2Texts() const;

	/** GetSk2CommandId
    * 
    *   Gets soft key 2 command ID.
    *
	*	@param -
	*	@return the soft key command ID
	*/
	const CArrayFix<TInt> * Sk2CommandIds() const;
    
    /** GetMSKText
    *
    *   Gets Middle softkey text.
    *
	*	@param -
	*	@return the soft key command ID
	*/
	const CDesCArray * MSKTexts() const;

	/** GetMSKCommandId
    * 
    *   Gets Middle soft key command ID.
    *
	*	@param -
	*	@return the soft key command ID
	*/
	const CArrayFix<TInt> * MSKCommandIds() const;
    
    /** GetMenuItems
    *
    *   Gets menu items.
    *
	*	@param -
	*	@return - pointer to menu item array
	*/
	const CMenuItemArray * MenuItems() const;

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
	CPluginResource ();

	/** Second phase constructor, may leave
    *
	*	@param aResourcePath - plug-in resource path
	*	@param aResourceFile - plug-in resource file
	*	@return -
	*/
	void ConstructL (
        const TDesC &   aResourcePath,
        const TDesC &   aResourceFile
        );

	/** Copy constructor, disabled
	*/
	CPluginResource (const CPluginResource & rhs);

	/** Assignment operator, disabled 
	*/
	CPluginResource & operator= (const CPluginResource & rhs);

//@}

/** @name Members:*/
//@{
	/// Plug-in UID
	TPluginInt	            iPluginUID;
    /// Plug-in UI type
    TPluginUiType           iPluginUIType;
    /// Plug-in filter type
    TPluginFilterType       iPluginFilterType;
    /// Plug-in scope
    TPluginScope            iPluginScope;
	/// Plug-in display order
	TPluginInt	            iPluginDisplayOrder;
	/// Plug-in name
	HBufC *                 iPluginName;
	/// Plug-in icon name
	HBufC *                 iPluginIconFileName;
	/// Plug-in filter DLL name
	HBufC *                 iPluginFilterDllName;
	/// Plug-in parameters
	CDesCArray *	        iParameters;
	/// Soft key 1 text
	CDesCArray *            iSk1Texts;
	/// Soft key 1 command id
	CArrayFix<TInt> *       iSk1Cmds;
	/// Soft key 2 text
	CDesCArray *            iSk2Texts;
	/// Soft key 2 command id
	CArrayFix<TInt> *       iSk2Cmds;
	/// Middle softkey text
	CDesCArray *            iMSKTexts;
	/// Middle softkey command id
	CArrayFix<TInt> *       iMSKCmds;
	/// Menu items
    CMenuItemArray *	    iMenuItems;
//@}

};


#endif
