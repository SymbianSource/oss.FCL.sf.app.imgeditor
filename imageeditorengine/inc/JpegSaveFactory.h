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


#ifndef __JPEGSAVEFACTORY_H__
#define __JPEGSAVEFACTORY_H__

#include <f32file.h>

class MJpegSave;

class JpegSaveFactory
	{
	public:
		static MJpegSave* CreateJpegSaveL( RFs* aFs, RFile* aFile );
		static MJpegSave* CreateJpegSaveLC( RFs* aFs, RFile* aFile );
	};

#endif
