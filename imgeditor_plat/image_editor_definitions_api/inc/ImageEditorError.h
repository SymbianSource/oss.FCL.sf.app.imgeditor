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
* Image editor related error codes.
*
*/


#ifndef IMAGEEDITORERROR_H
#define IMAGEEDITORERROR_H

const TInt KSIEENone = KErrNone;
const TInt KSIEEErrorBase = -60000;
const TInt KSIEENotEnoughDiskSpace = KSIEEErrorBase - 2;
const TInt KSIEEOpenFile = KSIEEErrorBase - 3;
const TInt KSIEESaveFile = KSIEEErrorBase - 4;
const TInt KSIEEProcessFile	= KSIEEErrorBase - 5;
const TInt KSIEEEngine = KSIEEErrorBase - 6;
const TInt KSIEEInternal = KSIEEErrorBase - 7;
const TInt KSIEEInternalNonRecoverable = KSIEEErrorBase - 8;
const TInt KSIEEProtectedFile = KSIEEErrorBase - 9;
const TInt KSIEEExifRead = KSIEEErrorBase - 10; // should be handled exactly like KSIEEOpenFile
const TInt KSIEEExifUpdate = KSIEEErrorBase - 11;
const TInt KSIEEIncompatibleImage = KSIEEErrorBase - 12;
const TInt KSIEEFileExists = KSIEEErrorBase - 13; 
const TInt KSIEEErrorOkToExit = KSIEEErrorBase - 98;
const TInt KSIEEErrorMax = KSIEEErrorBase - 99;

#endif // IMAGEEDITORERROR_H
