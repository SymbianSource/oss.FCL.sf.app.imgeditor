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


#include "JpegSaveFactory.h"
#include "CJpegSave.h"


MJpegSave* JpegSaveFactory::CreateJpegSaveL( RFs* aFs, RFile* aFile )
	{
	return CJpegSave::NewL( aFs, aFile );
	}
	
	

MJpegSave* JpegSaveFactory::CreateJpegSaveLC( RFs* aFs, RFile* aFile )
	{
	return CJpegSave::NewLC( aFs, aFile );
	}
