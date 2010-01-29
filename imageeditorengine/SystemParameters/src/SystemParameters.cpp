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


#include "SystemParameters.h"


//=============================================================================
EXPORT_C CSystemParameters::~CSystemParameters()
{

}

//=============================================================================
EXPORT_C TSize & CSystemParameters::SourceSize()
{
	return iSourceSize;
}

//=============================================================================
EXPORT_C const TSize & CSystemParameters::SourceSize() const
{
	return iSourceSize;
}

//=============================================================================
EXPORT_C TRect & CSystemParameters::ViewPortRect()
{
    return iViewPortRect;
}

//=============================================================================
EXPORT_C const TRect & CSystemParameters::ViewPortRect() const
{
    return iViewPortRect;
}

//=============================================================================
EXPORT_C TRect & CSystemParameters::VisibleImageRect()
{
	return iVisibleImageRect;
}

//=============================================================================
EXPORT_C const TRect & CSystemParameters::VisibleImageRect() const
{
	return iVisibleImageRect;
}

//=============================================================================
EXPORT_C TRect & CSystemParameters::VisibleImageRectPrev()
{
	return iVisibleImageRectPrev;
}

//=============================================================================
EXPORT_C const TRect & CSystemParameters::VisibleImageRectPrev() const
{
	return iVisibleImageRectPrev;
}

//=============================================================================
EXPORT_C TReal & CSystemParameters::Scale()
{
	return iScale;
}

//=============================================================================
EXPORT_C const TReal & CSystemParameters::Scale() const
{
	return iScale;
}

//=============================================================================
EXPORT_C TReal & CSystemParameters::RelScale()
{
	return iRelScale;
}

//=============================================================================
EXPORT_C const TReal & CSystemParameters::RelScale() const
{
	return iRelScale;
}

// End of File
