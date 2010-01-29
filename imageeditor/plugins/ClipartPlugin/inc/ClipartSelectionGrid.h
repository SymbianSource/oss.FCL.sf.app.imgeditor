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
* Header for clipart selection grid.
*
*/


#ifndef CLIPARTINSELECTIONGRID_H
#define CLIPARTINSELECTIONGRID_H

//  INCLUDES
#include <akngrid.h>


//  FORWARD DECLARATIONS

/*	CLASS: CClipartSelectionGrid
*
*   CClipartSelectionGrid represents a clipart selection grid control in
*	Nokia Still Image Editor.
*/
class CClipartSelectionGrid : public CAknGrid
{
 
public:
	
/** @name Methods:*/
//@{
	
	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CClipartSelectionGrid ();

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	virtual ~CClipartSelectionGrid ();
	
    /** Second phase constructor
	*
	*	@param aParent - parent control
	*	@param aFlags - grid flags
	*	@param aItems - icon item array
	*	@return -
	*/
	virtual void ConstructL (
        const CCoeControl *             aParent,  
        TInt                            aFlags);


//@}
	
protected:
	
/** @name Methods:*/
//@{
	
	/*	MinimumSize
	*
	*   @see CCoeControl
	*/
	virtual TSize MinimumSize();
	
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


#endif // EOF ClipartSelectionGrid.h
