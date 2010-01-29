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



#include <utf.h>

#include "pluginbasedefs.h"
#include "imageeditorpluginbasedefs.h"
#include "iepb.h"
#include "iepbint.h"
#include "iepbres.h"


//=============================================================================
EXPORT_C CImageEditorPluginBase * CImageEditorPluginBase::NewL (
    const TDesC & aResourcePath,
    const TDesC & aResourceFile
    )
{
	CImageEditorPluginBase * self = new (ELeave) CImageEditorPluginBase;
	CleanupStack::PushL (self);
	self->ConstructL (aResourcePath, aResourceFile);
	CleanupStack::Pop();
	return self;
}

//=============================================================================
EXPORT_C CImageEditorPluginBase::~CImageEditorPluginBase ()
{
    delete (CImageEditorPluginBaseInt *)iEditorPluginBaseInternalRepresentation;
}

//=============================================================================
EXPORT_C TInt CImageEditorPluginBase::SetProperty (
	TInt		/*aPropertyId*/, 
	TDesC &		/*aPropertyValue*/
	)
{
	return TPropRetErrNotSupported;
}

//=============================================================================
EXPORT_C TInt CImageEditorPluginBase::GetProperty (
	TInt		aPropertyId, 
	TDes &		aPropertyValue
	)
{

	aPropertyValue.Zero();
    CPluginResource * res = 
        ((CImageEditorPluginBaseInt *)iEditorPluginBaseInternalRepresentation)->GetPluginResource();
	
    switch (aPropertyId)
	{
		case KPluginCapabilityUID:
		{
			aPropertyValue.AppendNum ( res->PluginUID() );
			return KErrNone;
		}
		case KCapPluginUiType:
		{
			aPropertyValue.AppendNum ( (TInt)res->PluginUiType() );
			return KErrNone;
		}
		case KCapPluginFilterType:
		{
			aPropertyValue.AppendNum ( (TInt)res->PluginFilterType() );
			return KErrNone;
		}
		case KCapPluginScope:
		{
			aPropertyValue.AppendNum ( (TInt)res->PluginScope() );
			return KErrNone;
		}
		case KCapPluginDisplayOrder:
		{
			aPropertyValue.AppendNum ( (TInt)res->PluginDisplayOrder() );
			return KErrNone;
		}
		case KCapPluginName:
		{
            aPropertyValue.Copy ( res->PluginName() );
			return KErrNone;
		}
		case KCapIconName:
		{
            aPropertyValue.Copy ( res->IconFileName() );
			return KErrNone;
		}
		case KCapFilterName:
		{
            aPropertyValue.Copy ( res->FilterFileName() );
			return KErrNone;
		}
		case KCapPluginParamNames:
		{
			aPropertyValue.AppendNum ( (TInt)res->GetParameters() );
			return KErrNone;
		}
		case KPluginCapabilityPluginType:
		{
			aPropertyValue.AppendNum ( (TInt)KPluginTypeImageEditor );
			return KErrNone;
		}
		case KCapPluginSk1Cmd:
		{
			aPropertyValue.AppendNum ( (TInt)res->Sk1CommandIds() );
			return KErrNone;
		}
		case KCapPluginSk1Text:
		{
			aPropertyValue.AppendNum ( (TInt)res->Sk1Texts() );
			return KErrNone;
		}
		case KCapPluginSk2Cmd:
		{
			aPropertyValue.AppendNum ( (TInt)res->Sk2CommandIds() );
			return KErrNone;
		}
		case KCapPluginSk2Text:
		{
			aPropertyValue.AppendNum ( (TInt)res->Sk2Texts() );
			return KErrNone;
		}
		
		case KCapPluginMSKCmd:
		{
			aPropertyValue.AppendNum ( (TInt)res->MSKCommandIds() );
			return KErrNone;
		}
		case KCapPluginMSKText:
		{
			aPropertyValue.AppendNum ( (TInt)res->MSKTexts() );
			return KErrNone;
		}
		
		case KCapPluginMenuItems:
		{
			aPropertyValue.AppendNum ( (TInt)res->MenuItems() );
			return KErrNone;
		}
		case KCapGlobalZoomDisabled:
		{
			aPropertyValue.AppendNum ( (TInt)EFalse );
			return KErrNone;
		}
        case KPluginCapabilityAPIMajorVersion:
		{
			aPropertyValue.AppendNum ( KImageEditorPluginAPIMajor );
			return KErrNone;
		}
		case KPluginCapabilityAPIMinorVersion:
		{
			aPropertyValue.AppendNum ( KImageEditorPluginAPIMinor );
			return KErrNone;
		}
		case KPluginCapabilityAPIBuildVersion:
		{
			aPropertyValue.AppendNum ( KImageEditorPluginAPIBuild );
			return KErrNone;
		}
        case KCapReadyToRender:
        {
            aPropertyValue.AppendNum ( (TInt)ETrue );
			return KErrNone;
        }
        case KCapIsLandscapeEnabled:
        {
            aPropertyValue.AppendNum ( (TInt)ETrue );
			return KErrNone;
        }
        case KCapIsSlowPlugin:
		{
			aPropertyValue.AppendNum ((TInt)EFalse);
			return KErrNone;
		}
		default:
		{
			return CPluginBase::GetProperty (aPropertyId, aPropertyValue);
		}
	}
}

//=============================================================================
EXPORT_C void CImageEditorPluginBase::ShowPopupNote ()
{

}

//=============================================================================
EXPORT_C TInt CImageEditorPluginBase::InitPluginL (
	const TRect &		/*aRect*/,
	CCoeControl *		/*aParent*/,
	CCoeControl *&		aPluginControl
	)
{
	aPluginControl = 0;
	return KErrNone;
}

//=============================================================================
EXPORT_C void CImageEditorPluginBase::ProcessImageL (CEditorImage * /*aImage*/)
{

}

//=============================================================================
EXPORT_C void CImageEditorPluginBase::ReleasePlugin ()
{

}

//=============================================================================
EXPORT_C void CImageEditorPluginBase::ReservedImageEditorBaseMethod1 ()
{

}

//=============================================================================
EXPORT_C void CImageEditorPluginBase::ReservedImageEditorBaseMethod2 ()
{

}

//=============================================================================
EXPORT_C CImageEditorPluginBase::CImageEditorPluginBase () :
CPluginBase()
{

}

//=============================================================================
EXPORT_C void CImageEditorPluginBase::ConstructL (
    const TDesC & aResourcePath,
    const TDesC & aResourceFile
    )
{
    iEditorPluginBaseInternalRepresentation = (CImageEditorPluginBaseInt*)
        CImageEditorPluginBaseInt::NewL (aResourcePath, aResourceFile);
}

// End of File
