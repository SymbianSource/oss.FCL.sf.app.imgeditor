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


#ifndef COLORSELECTIONGRID_H
#define COLORSELECTIONGRID_H

//  INCLUDES
#include <akngrid.h>


/*	CLASS: CColorSelectionGrid
*
*   CColorSelectionGrid represents a color selection grid control in
*	Nokia Still Image Editor.
*/
class CColorSelectionGrid : public CAknGrid
{

public:
	
/** @name Methods:*/
//@{
	
	/** Default constructor
	*
	*	@param aParent - parent control
	*	@param aFlags - grid flags
	*	@return -
	*/
	IMPORT_C CColorSelectionGrid ();

	/** Destructor
	*
	*	@param -
	*	@return -
	*/
	IMPORT_C virtual ~CColorSelectionGrid ();
	
    /** Second phase constructor
	*
	*	@param aParent - parent control
	*	@param aFlags - grid flags
	*	@return -
	*/
	IMPORT_C virtual void ConstructL (
        const CCoeControl *     aParent,  
        TInt                    aFlags 
        );

	
//@}
	
protected:
	
/** @name Methods:*/
//@{
	
	/*	SizeChanged
	*
	*   @see CImageEditorControlBase
	*/
	IMPORT_C virtual void SizeChanged();
	
//@}
	
/** @name Members:*/
//@{

//@}
	
private:
	
/** @name Methods:*/
//@{

	/*	SetupGridIconsL
	*
	*	Sets up grid icons.
	*
	*   @param -
	*   @return -
	*/
	void SetupGridIconsL ();

	/*	SetupGrid
	*
	*	Sets up grid every time the grid size changes.
	*
	*   @param -
	*   @return -
	*/
	void SetupGrid();

	/*	HSLtoRGB
	*
	*	HSL to RGB color conversion
	*
	*   @param aH - hue				[0.0 - 360.0]
	*   @param aS - saturation		[0.0 - 1.0]
	*   @param aL - luminance		[0.0 - 1.0]
	*   @return TRgb
	*/
	TRgb HSLtoRGB (
		const TReal aH, 
		const TReal aS, 
		const TReal aL
		) const;

	/*	ToRGB
	*
	*	Helper function in color conversion.
	*
	*   @param aRM1 - helper variable
	*   @param aRM2 - helper variable
	*   @param aRH - helper variable
	*   @return TInt
	*/
	TInt ToRGB (
		TReal aRM1, 
		TReal aRM2, 
		TReal aRH
		) const;

	/** Copy constructor, disabled
	*/
	CColorSelectionGrid (const CColorSelectionGrid & rhs);
	
	/** Assignment operator, disabled 
	*/
	CColorSelectionGrid & operator= (const CColorSelectionGrid & rhs);
	
//@}

private:

/** @name Members:*/
//@{
    TSize iGridSize;
    TSize iIconSize;
//@}

};


#endif // EOF ColorSelectionGrid.h
