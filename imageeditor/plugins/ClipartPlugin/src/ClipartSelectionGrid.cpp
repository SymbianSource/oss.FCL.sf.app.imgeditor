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


//	INCLUDE
#include "ClipartSelectionGrid.h"


//=============================================================================
CClipartSelectionGrid::CClipartSelectionGrid () : 
CAknGrid()
{

}

//=============================================================================
CClipartSelectionGrid::~CClipartSelectionGrid ()
{

}

//=============================================================================
void CClipartSelectionGrid::ConstructL (
    const CCoeControl *             aParent,  
    TInt                            aFlags)
{

	SetContainerWindowL(*aParent);
	CAknGrid::ConstructL(aParent, aFlags);

    ActivateL();

}

//=============================================================================
TSize CClipartSelectionGrid::MinimumSize()
{	
	//return Rect().Size();
	return CEikonEnv::Static()->EikAppUi()->ClientRect().Size();
}


// End of File
