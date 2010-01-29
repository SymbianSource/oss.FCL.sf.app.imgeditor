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


#ifndef __CJPEGSCALE_H__
#define __CJPEGSCALE_H__

#include <e32base.h>

class MJpegScaleCallBack
	{
	public:
		virtual ~MJpegScaleCallBack() {}
		virtual void JpegStatusCallBack( TInt aPercentReady );
	};
	
	
	
class CJpegScale
	: public CBase
	{
	public:
		/// Default constructor
		/// @param aCallBack status callback, tells conversion status percentage. Not mandatory
		CJpegScale( MJpegScaleCallBack* aCallBack = 0 );
		~CJpegScale();
	public:
	
		/// Scales jpeg iSourceName to iTargetName
		/// target size will be of iTargetScale
		/// if iTargetScale = 0 then iTargetSize is used
		/// can leave with common error codes
		/// Only downscaling supported
		void ScaleL();
		
	public:
		TFileName iSourceName;		/// source file name
		TFileName iTargetName;		/// target file name
		TReal iTargetScale;			/// target jpeg scale, 1.0 = 1:1 , if 0.0 then iTargetSize used instead
		TSize iTargetSize;			/// target jpeg size in pixels
		TInt iQuality;				/// target jpeg quality 0..100 default 95
		
	private:
		MJpegScaleCallBack* iCallBack;
		
		
	};

#endif
