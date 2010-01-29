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


#ifndef IMAGEEDITORPLUGINBASEINTERNAL_HPP
#define IMAGEEDITORPLUGINBASEINTERNAL_HPP

//  INCLUDES
#include <e32base.h>

//  FORWARD DECLARATIONS
class CPluginResource;

/*	CLASS: CImageEditorPluginBaseInt
*
*   CImageEditorPluginBaseInt acts as internal representation of 
*   CImageEditorPluginBase class. All the generic functionality of
*   Image Editor plug-in base is implemented here.
*   
*/
class CImageEditorPluginBaseInt : public CBase
{

public:

/** @name Methods:*/
//@{

	/** NewL factory method, pops cleanupstack
	*
	*	@param aResourcePath - plug-in resource path
	*	@param aResourceFile - plug-in resource file
	*	@return pointer to created CImageEditorPluginBaseInt object
	*/
	static CImageEditorPluginBaseInt * NewL (
        const TDesC &   aResourcePath,
        const TDesC &   aResourceFile
        );

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	~CImageEditorPluginBaseInt ();

	/** GetPluginResource
	*
	*	@param -
	*	@return CPluginResource * - pointer to plug-in resource
	*/
	CPluginResource * GetPluginResource() const;

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
	CImageEditorPluginBaseInt ();

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
	CImageEditorPluginBaseInt (const CImageEditorPluginBaseInt & rhs);

	/** Assignment operator, disabled 
	*/
	CImageEditorPluginBaseInt & operator= (const CImageEditorPluginBaseInt & rhs);

//@}

/** @name Members:*/
//@{
    /// Plug-in resource
    CPluginResource *   iResource;
//@}

};



#endif
