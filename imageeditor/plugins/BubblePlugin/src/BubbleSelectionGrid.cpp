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
* Talk bubble grid implementation.
*
*/


//	INCLUDE
#include "BubbleSelectionGrid.h"
//=============================================================================
CBubbleSelectionGrid::CBubbleSelectionGrid () : 
CAknGrid()
{

}

//=============================================================================
CBubbleSelectionGrid::~CBubbleSelectionGrid ()
{
}

//=============================================================================
void CBubbleSelectionGrid::ConstructL (
    const CCoeControl *             aParent,  
    TInt                            aFlags)
{

	SetContainerWindowL(*aParent);
	CAknGrid::ConstructL(aParent, aFlags);
    ActivateL();
}

//=============================================================================
TSize CBubbleSelectionGrid::MinimumSize()
    {
	return Rect().Size();		
}


// End of File
