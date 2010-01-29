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
* Header file for bubble grid.
*
*/



#ifndef BUBBLEINSELECTIONGRID_H
#define BUBBLEINSELECTIONGRID_H

//  INCLUDES
#include <akngrid.h>

//  FORWARD DECLARATIONS

/*	CLASS: CBubbleSelectionGrid
*
*   CBubbleSelectionGrid represents a Bubble selection grid control in
*	Nokia Still Image Editor.
*/
class CBubbleSelectionGrid : public CAknGrid 
{
 
public:
	
/** @name Methods:*/
//@{
	
	/** Default constructor
	*
	*	@param -
	*	@return -
	*/
	CBubbleSelectionGrid ();

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	virtual ~CBubbleSelectionGrid ();
	
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


#endif // EOF BubbleSelectionGrid.h
